// #include "pqueue.h"

// // dependency inversion; put implementation in private method to prevent using multiple locks in 1 method (creates an exploitable gap)
// template <typename T>
// void persistent_queue<T>::reserveImpl(size_type newAlloc) {
//     if (newAlloc <= this->space) return;

//     storage_base<T> b {this->alloc, newAlloc};

//     for (int i = head; i < head + this->sz; ++i) 
//         new (b.elem+i-head) int(this->elem[i % this->space]);
//     b.sz = this->sz;

//     for (int i = 0; i < this->sz; ++i) this->alloc.destroy(this->elem+i);
//     this->sz = 0;

//     std::swap<storage_base<T>>(static_cast<storage_base<T>&>(*this), static_cast<storage_base<T>&>(b));
//     this->head = 0;
//     this->tail = head+this->sz;
// }

// template <typename T>
// void persistent_queue<T>::reserve(size_type newAlloc)
// {
//     std::lock_guard<std::shared_mutex> lockGuard {this->mutex}; // use shared_lock for reading
//     // call private impl once lock has been acquired
//     reserveImpl(newAlloc);
// }

// template <typename T>
// int persistent_queue<T>::size() {
//     return this->sz;
// }

// template <typename T>
// void persistent_queue<T>::resize(size_type newSize)
// {
//     std::lock_guard<std::shared_mutex> lockGuard {this->mutex};

//     reserveImpl(newSize);
//     for (int i = 0; i < newSize; ++i) this->alloc.construct(&this->elem[i], i);
//     for (int i = newSize; i < this->sz; ++i) this->alloc.destroy(&this->elem[i]);
//     this->sz = newSize;
// }

// template <typename T>
// void persistent_queue<T>::enqueue(T i) {
//     std::lock_guard<std::shared_mutex> lockGuard {this->mutex}; 
//     if (this->space == 0)
//         reserveImpl(8);

//     if (this->sz >= this->space)
//         reserveImpl(2*this->space);

//     new (this->elem+tail) int(i);
//     tail = (tail+1)%this->space;
//     this->sz++;
// }

// template <typename T>
// T persistent_queue<T>::dequeue() {
//     std::lock_guard<std::shared_mutex> lockGuard {this->mutex};
//     if (this->sz == 0)
//         return -1;
//     int rt = *(this->elem+head);
//     std::destroy_at(this->elem+head);
//     head = (head+1)%this->space;
//     this->sz--;
//     return rt;
// }

// template <typename T>
// T persistent_queue<T>::peek() {
//     if (this->sz == 0)
//         return -1;
//     int rt = *(this->elem+head);
//     return rt;
// }
