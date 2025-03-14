#include "pqueue.h"

// dependency inversion; put implementation in private method to prevent using multiple locks in 1 method (creates an exploitable gap)
void persistent_queue::reserveImpl(size_type newAlloc) {
    if (newAlloc <= this->space) {
        return;
    }

    queue_base b {this->alloc, newAlloc};

    int* startPtr = this->elem;
    int* endPtr = startPtr + space;
    std::uninitialized_copy(startPtr, endPtr, b.elem);

    startPtr = this->elem;
    for (int i = 0; i < sz; ++i) {
        alloc.destroy(startPtr+i);
    }

    alloc.deallocate(this->elem, space);

    startPtr = b.elem;
    this->space = newAlloc;
    this->head = startPtr;
    this->tail = startPtr + sz;
}

void persistent_queue::reserve(size_type newAlloc)
{
    std::lock_guard<std::shared_mutex> lockGuard {this->mutex}; // use shared_lock for reading
    // call private impl once lock has been acquired
    reserveImpl(newAlloc);
}

void persistent_queue::resize(size_type newSize)
{
    std::lock_guard<std::shared_mutex> lockGuard {this->mutex};

    reserveImpl(newSize);
    for (int i = 0; i < newSize; ++i) alloc.construct(&elem[i], i);
    for (int i = newSize; i < sz; ++i) alloc.destroy(&elem[i]);
    this->sz = newSize;
}
