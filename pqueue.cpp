#include "pqueue.h"

void persistent_queue::reserve(size_type newAlloc)
{
    std::lock_guard<std::shared_mutex> lockGuard {this->mutex}; // use shared_lock for reading

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

void persistent_queue::resize()
{

}
