#include <memory>
#include <shared_mutex>
#include <mutex>

// write unit tests
// template<typename T> should be a template eventually, start with 1 datatype

template <typename T>
class storage_base {
public:
    using size_type = unsigned long int;

    std::allocator<T> alloc;
    int* elem;
    size_type sz;
    size_type space;

    storage_base(std::allocator<T> allocator, size_type n): 
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

template <typename T>
storage_base<T>::storage_base(storage_base<T>&& sb):
    alloc{sb.alloc},
    elem{sb.elem},
    sz{sb.sz},
    space{sb.space}
{
    sb.sz = 0;
    sb.space = 0;
    sb.elem = nullptr;
}

template <typename T>
storage_base<T>& storage_base<T>::operator=(storage_base<T>&& sb) 
{
    /* move data from qb to *this */

    // delete existing data 
    for (int i = 0; i < sz; ++i) alloc.destroy(elem+i);
    alloc.deallocate(elem, space);

    // complete move
    elem = sb.elem;
    sz = sb.sz;
    space = sb.space;

    // reset sb
    sb.sz = 0;
    sb.space = 0;
    sb.elem = nullptr;

    return *this; // forgot to return *this, which is fairly important
}

// eventually want this to be templated instead of ints. Concrete implementation should store bitsreams use conversion methods
template <typename T>
class persistent_queue: private storage_base<T>
{
public:
    using size_type = typename storage_base<T>::size_type;

    persistent_queue():
        storage_base<T>{std::allocator<T>(), 0}
    {
        head = 0;
        tail = 0;
    }

    int size();

    void enqueue(T i);
    T dequeue();
    T peek();

    // void enqueueBatch(int n);
    // void dequeueBatch(int n);
    
    void reserve(size_type newAlloc);
    void resize(size_type newSize);

    // define iterators
private:
    void reserveImpl(size_type newAlloc);
    std::shared_mutex mutex;
    int head;
    int tail;
};

template <typename T>
void persistent_queue<T>::reserveImpl(size_type newAlloc) {
    if (newAlloc <= this->space) return;

    storage_base<T> b {this->alloc, newAlloc};

    for (int i = head; i < head + this->sz; ++i) 
        new (b.elem+i-head) int(this->elem[i % this->space]);
    b.sz = this->sz;

    for (int i = 0; i < this->sz; ++i) this->alloc.destroy(this->elem+i);
    this->sz = 0;

    std::swap<storage_base<T>>(static_cast<storage_base<T>&>(*this), static_cast<storage_base<T>&>(b));
    this->head = 0;
    this->tail = head+this->sz;
}

template <typename T>
void persistent_queue<T>::reserve(size_type newAlloc)
{
    std::lock_guard<std::shared_mutex> lockGuard {this->mutex}; // use shared_lock for reading
    // call private impl once lock has been acquired
    reserveImpl(newAlloc);
}

template <typename T>
int persistent_queue<T>::size() {
    return this->sz;
}

template <typename T>
void persistent_queue<T>::resize(size_type newSize)
{
    std::lock_guard<std::shared_mutex> lockGuard {this->mutex};

    reserveImpl(newSize);
    for (int i = 0; i < newSize; ++i) this->alloc.construct(&this->elem[i], i);
    for (int i = newSize; i < this->sz; ++i) this->alloc.destroy(&this->elem[i]);
    this->sz = newSize;
}

template <typename T>
void persistent_queue<T>::enqueue(T i) {
    std::lock_guard<std::shared_mutex> lockGuard {this->mutex}; 
    if (this->space == 0)
        reserveImpl(8);

    if (this->sz >= this->space)
        reserveImpl(2*this->space);

    new (this->elem+tail) int(i);
    tail = (tail+1)%this->space;
    this->sz++;
}

template <typename T>
T persistent_queue<T>::dequeue() {
    std::lock_guard<std::shared_mutex> lockGuard {this->mutex};
    if (this->sz == 0)
        return -1;
    int rt = *(this->elem+head);
    std::destroy_at(this->elem+head);
    head = (head+1)%this->space;
    this->sz--;
    return rt;
}

template <typename T>
T persistent_queue<T>::peek() {
    if (this->sz == 0)
        return -1;
    int rt = *(this->elem+head);
    return rt;
}
