#include <memory>
#include <shared_mutex>
#include <mutex>
#include <list>

// write unit tests
// template<typename T> should be a template eventually, start with 1 datatype

class queue_base {
    public:
        using size_type = unsigned long int;
    
        std::allocator<int> alloc;
        std::shared_mutex mutex;
        int* elem;
        size_type sz;
        size_type space;
        int* head;
        int* tail;
    
        queue_base(std::allocator<int> allocator, size_type n): 
            alloc{allocator}, 
            sz{n}, space{n}
        {
            elem = alloc.allocate(space);
            head = elem;
            tail = elem;
        }
    
        ~queue_base() {
            for (int i = 0; i < sz; ++i)
                alloc.destroy(elem+i);

            alloc.deallocate(elem, space);
            elem = nullptr;
            head = nullptr;
            sz = 0;
            space = 0;
        }
    
        queue_base(queue_base&&);
    
        queue_base& operator=(queue_base&&);
    };


// eventually want this to be templated instead of ints. Concrete implementation should store bitsreams use conversion methods
class persistent_queue: private queue_base
{
private:
    void reserveImpl(size_type newAlloc);
public:
    persistent_queue():
        queue_base{std::allocator<int>(), 0}
    {}

    void enqueue(int elem);
    void peek();
    int dequeue();

    void enqueueBatch(std::list<int>::iterator begin, std::list<int>::iterator end);
    void peekBatch(size_type count);
    int* dequeueBatch(size_type count);
    
    void reserve(size_type newAlloc);
    void resize(size_type newSize);

    // define iterators
};