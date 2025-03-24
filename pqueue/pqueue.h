#include <memory>
#include <shared_mutex>
#include <mutex>
#include "storagebase.h"

// eventually want this to be templated instead of ints. Concrete implementation should store bitsreams use conversion methods
template <typename T>
class persistent_queue: private storage_base<T>
{
public:
    using size_type = typename storage_base<T>::size_type;

    persistent_queue():
        storage_base<T>{0}
    {
        head = 0;
        tail = 0;
    }

    int capacity() const;
    T* data();

    int size() const;

    void enqueue(T i);
    std::optional<T> dequeue();
    std::optional<T> peek() const;

    // void enqueueBatch(int n);
    // void dequeueBatch(int n);
    
    void reserve(size_type newAlloc);
    void resize(size_type newSize, T val = T());

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

    storage_base<T> b {newAlloc};

    for (int i = head; i < head + this->sz; ++i) 
        new (b.elem+i-head) int(this->elem[i % this->space]);
    b.sz = this->sz;

    for (int i = 0; i < this->sz; ++i) std::destroy_at(this->elem+i);
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
int persistent_queue<T>::size() const {
    return this->sz;
}

template <typename T>
int persistent_queue<T>::capacity() const {
    return this->space;
}

template <typename T>
T* persistent_queue<T>::data() {
    return this->elem;
}

template <typename T>
void persistent_queue<T>::resize(size_type newSize, T val)
{
    std::lock_guard<std::shared_mutex> lockGuard {this->mutex};

    reserveImpl(newSize);
    for (int i = this->sz; i < newSize; ++i) new (&this->elem[i]) T(val);

    for (int i = newSize; i < this->sz; ++i) std::destroy_at(&this->elem[i]);
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
std::optional<T> persistent_queue<T>::dequeue() {
    std::lock_guard<std::shared_mutex> lockGuard {this->mutex};
    if (this->sz == 0)
        return std::nullopt;
    int rt = *(this->elem+head);
    std::destroy_at(this->elem+head);
    head = (head+1)%this->space;
    this->sz--;
    return rt;
}

template <typename T>
std::optional<T> persistent_queue<T>::peek() const{
    if (this->sz == 0)
        return std::nullopt;
    int rt = *(this->elem+head);
    return rt;
}