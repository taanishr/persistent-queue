#include <memory>
#include <shared_mutex>
#include <mutex>

// write unit tests
// template<typename T> should be a template eventually, start with 1 datatype

class storage_base {
    public:
        using size_type = unsigned long int;
    
        std::allocator<int> alloc;
        int* elem;
        size_type sz;
        size_type space;
    
        storage_base(std::allocator<int> allocator, size_type n): 
            alloc{allocator}, 
            sz{0}, space{n}
        {
            elem = alloc.allocate(space);
        }
    
        ~storage_base() {
            for (int i = 0; i < sz; ++i)
                alloc.destroy(elem+i);

            alloc.deallocate(elem, space);
            elem = 0;
            sz = 0;
            space = 0;
        }
    
        storage_base(storage_base&&);
    
        storage_base& operator=(storage_base&&);
    };


// eventually want this to be templated instead of ints. Concrete implementation should store bitsreams use conversion methods
class persistent_queue: private storage_base
{
private:
    void reserveImpl(size_type newAlloc);
    std::shared_mutex mutex;
    int head;
    int tail;
public:
    persistent_queue():
        storage_base{std::allocator<int>(), 0}
    {
        head = 0;
        tail = 0;
    }

    int size();

    void enqueue(int i);
    int dequeue();
    int peek();

    // void enqueueBatch(int n);
    // void dequeueBatch(int n);
    
    void reserve(size_type newAlloc);
    void resize(size_type newSize);

    // define iterators
};