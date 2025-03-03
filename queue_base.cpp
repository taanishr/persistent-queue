// In memory, persistent, (eventually distributed) queue
#include "pqueue.h"

queue_base::queue_base(queue_base&& qb):
    alloc{qb.alloc},
    elem{qb.elem},
    sz{qb.sz},
    space{qb.space},
    head{qb.tail},
    tail{qb.head}
{
    
    qb.alloc.deallocate(qb.elem, qb.space);
    qb.sz = 0;
    qb.space = 0;
    qb.elem = nullptr;
    qb.head = nullptr;
    qb.tail = nullptr;
}

queue_base& queue_base::operator=(queue_base&& qb) 
{
    // move data from qb to *this
    alloc.deallocate(elem, space);
    elem = qb.elem;
    tail = qb.head;
    tail = qb.tail;
    sz = qb.sz;
    space = qb.space;

    // destroy qb's elements
    qb.alloc.deallocate(qb.elem, qb.space);
    qb.sz = 0;
    qb.space = 0;
    qb.elem = nullptr;
    qb.head = nullptr;
    qb.tail = nullptr;
}
