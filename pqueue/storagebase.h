// write unit tests
// template<typename T> should be a template eventually, start with 1 datatype
#include <memory>

template <typename T, typename A = std::allocator<T>>
class storage_base {
public:
    using size_type = unsigned long int;

    A alloc;
    T* elem;
    size_type sz;
    size_type space;

    storage_base(size_type n):
        sz{0}, space{n}
    {
        elem = alloc.allocate(space);
    }

    ~storage_base() {
        for (int i = 0; i < sz; ++i)
            std::destroy_at(elem+i);

        alloc.deallocate(elem, space);
        elem = 0;
        sz = 0;
        space = 0;
    }

    storage_base(storage_base&&);

    storage_base& operator=(storage_base&&);
};

template <typename T, typename A>
storage_base<T, A>::storage_base(storage_base<T, A>&& sb):
    alloc{sb.alloc},
    elem{sb.elem},
    sz{sb.sz},
    space{sb.space}
{
    sb.sz = 0;
    sb.space = 0;
    sb.elem = nullptr;
}

template <typename T, typename A>
storage_base<T, A>& storage_base<T,A>::operator=(storage_base<T, A>&& sb) 
{
    /* move data from qb to *this */

    // delete existing data 
    for (int i = 0; i < sz; ++i) std::destroy_at(elem+i);
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
