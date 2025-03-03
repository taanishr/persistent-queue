// In memory, persistent, (eventually distributed) queue
#include "pqueue.h"

queue_base::queue_base(queue_base&& qb):
    alloc{qb.alloc},
    elem{qb.elem.load()},
    sz{qb.sz},
    space{qb.space},
    head{qb.tail.load()},
    tail{qb.head.load()}
{
    qb.alloc.deallocate(qb.elem.load(), qb.space);
    qb.sz = 0;
    qb.space = 0;
    qb.elem.store(nullptr);
    qb.head.store(nullptr);
    qb.tail.store(nullptr);
}

queue_base& queue_base::operator=(queue_base&& qb) 
{
    // move data from qb to *this
    alloc.deallocate(elem.load(), space);
    elem.store(qb.elem.load());
    tail.store(qb.head.load());
    tail.store(qb.tail.load());
    sz = qb.sz;
    space = qb.space;

    // destroy qb's elements
    qb.alloc.deallocate(qb.elem, qb.space);
    qb.sz = 0;
    qb.space = 0;
    qb.elem.store(nullptr);
    qb.head.store(nullptr);
    qb.tail.store(nullptr);
}