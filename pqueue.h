#include <memory>

// write unit tests
// template<typename T> should be a template eventually, start with 1 datatype

class queue_base {
    public:
        using size_type = unsigned long int;
    
        std::allocator<int> alloc;
        std::atomic<int*> elem;
        size_type sz;
        size_type space;
        std::atomic<int*> head;
        std::atomic<int*> tail;
    
        queue_base(std::allocator<int> allocator, size_type sz, size_type space): 
            alloc{allocator}, 
            sz{sz}, space{space}
        {
            elem.store(alloc.allocate(space));
            head = elem.load();
            tail = elem.load();
        }
    
        ~queue_base() {
            if (elem.load()) {
                alloc.deallocate(elem.load(), space);
                elem.store(nullptr);
                head.store(nullptr);
                tail.store(nullptr);
                sz = 0;
                space = 0;
            }
        }
    
        queue_base(queue_base&&);
    
        queue_base& operator=(queue_base&&);
    };

class persistent_queue: private queue_base
{};