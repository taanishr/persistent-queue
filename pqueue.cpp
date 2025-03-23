#include "pqueue.h"

// dependency inversion; put implementation in private method to prevent using multiple locks in 1 method (creates an exploitable gap)
void persistent_queue::reserveImpl(size_type newAlloc) {
    if (newAlloc <= this->space) return;

    storage_base b {this->alloc, newAlloc};

    for (int i = head; i < head + sz; ++i) 
        new (b.elem+i) int(this->elem[i % space]);

    for (int i = 0; i < sz; ++i) this->alloc.destroy(this->elem+i);
    std::swap<storage_base>(static_cast<storage_base&>(*this), static_cast<storage_base&>(b));
    this->head = 0;
    this->tail = head+sz;
}

void persistent_queue::reserve(size_type newAlloc)
{
    std::lock_guard<std::shared_mutex> lockGuard {this->mutex}; // use shared_lock for reading
    // call private impl once lock has been acquired
    reserveImpl(newAlloc);
}

int persistent_queue::size() {
    return this->sz;
}

void persistent_queue::resize(size_type newSize)
{
    std::lock_guard<std::shared_mutex> lockGuard {this->mutex};

    reserveImpl(newSize);
    for (int i = 0; i < newSize; ++i) this->alloc.construct(&this->elem[i], i);
    for (int i = newSize; i < sz; ++i) this->alloc.destroy(&this->elem[i]);
    this->sz = newSize;
}

void persistent_queue::enqueue(int i) {
    std::lock_guard<std::shared_mutex> lockGuard {this->mutex}; 
    if (space == 0)
        reserveImpl(8);

    if (sz >= space)
        reserveImpl(2*space);

    new (this->elem+tail) int(i);
    tail = (tail+1)%space;
    sz++;
}

int persistent_queue::dequeue() {
    std::lock_guard<std::shared_mutex> lockGuard {this->mutex};
    if (sz == 0)
        return -1;
    int rt = *(this->elem+head);
    std::destroy_at(this->elem+head);
    head = (head+1)%space;
    sz--;
    return rt;
}

int persistent_queue::peek() {
    if (sz == 0)
        return -1;
    int rt = *(this->elem+head);
    return rt;
}
