// In memory, persistent, (eventually distributed) queue
#include "pqueue.h"

storage_base::storage_base(storage_base&& sb):
    alloc{sb.alloc},
    elem{sb.elem},
    sz{sb.sz},
    space{sb.space}
{
    sb.sz = 0;
    sb.space = 0;
    sb.elem = nullptr;
}

storage_base& storage_base::operator=(storage_base&& sb) 
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