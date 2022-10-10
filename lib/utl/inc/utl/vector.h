#ifndef UTL_VECTOR_H
#define UTL_VECTOR_H

#include "utl/storage.h"

#define VECTOR_ILIST_COPY       false   // Enable std::initializer_list assignment overloads. NOTE: This will hide C array&& overloads
#define VECTOR_ARRAY_COPY       true    // Enable const std::array& assignment overloads 
#define VECTOR_ARRAY_MOVE       true    // Enable C array&& assignment overloads    

namespace utl {

/**
 * @brief Resizable static vector with pre-allocated memory. 
 * Uses utl::storage, which doesn't default construct 
 * elements and can be used in constexpr context.
 * 
 * @tparam T Type of elements
 * @tparam N Maximum number of elements
 */
template<class T, size_t N>
struct vector {

    // ANCHOR Member types

    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using universal_reference = value_type&&;
    using iterator = pointer;
    using const_iterator = const_pointer;

    // ANCHOR Constructors

    constexpr vector() noexcept = default;
    constexpr explicit vector(size_type n) : len{n}
    {
        assert(capacity() >= len);
        ctor_n(begin(), len);
    }
    constexpr explicit vector(size_type n, const_reference x) : len{n}
    {
        assert(capacity() >= len);
        ctor_n(begin(), len, x);
    }
    template <std::input_iterator It>
    constexpr vector(It first, It last) : len{size_type(last - first)}
    {
        assert(capacity() >= len);
        copy_create_forward(first, last, begin());
    }
    constexpr vector(const vector& other) noexcept(std::is_nothrow_copy_constructible_v<value_type>) : len{other.size()}
    {
        copy_create_forward(other.begin(), other.end(), begin());
    }
    constexpr vector(vector&& other) noexcept(std::is_nothrow_move_constructible_v<value_type>) : len{other.size()}
    {
        move_create_forward(other.begin(), other.end(), begin());
    }
#if (VECTOR_ILIST_COPY)
    constexpr vector(std::initializer_list<value_type> ilist) : len{ilist.size()}
    {
        assert(capacity() >= len);
        copy_create_forward(ilist.begin(), ilist.end(), begin());
    }
#endif
#if (VECTOR_ARRAY_COPY)
    template<size_type Size>
    constexpr vector(const std::array<value_type, Size>& array) : len{Size}
    {
        static_assert(N >= Size);
        copy_create_forward(array.begin(), array.end(), begin());
    }
#endif
#if (VECTOR_ARRAY_MOVE)
    template<size_type Size>
    constexpr vector(value_type(&&array)[Size]) : len{Size}
    {
        static_assert(N >= Size);
        move_create_forward(array, array + Size, begin());
    }
#endif
    constexpr ~vector() noexcept
    {
        clear();
    }

    // ANCHOR Assingment operator

    constexpr vector& operator=(const vector& other) noexcept(std::is_nothrow_copy_assignable_v<value_type>)
    {
        if (&other != this) {
            assign(other.begin(), other.end());
        }
        return *this;
    }
    constexpr vector& operator=(vector&& other) noexcept(std::is_nothrow_move_assignable_v<value_type>)
    {
        if (&other != this) {
            impl_assign_prepare(other.len);
            move_create_forward(other.begin(), other.end(), begin());
        }
        return *this;
    }
#if (VECTOR_ILIST_COPY)
    constexpr vector& operator=(std::initializer_list<value_type> ilist)
    {
        assign(ilist);
        return *this;
    }
#endif
#if (VECTOR_ARRAY_COPY)
    template<size_type Size>
    constexpr vector& operator=(const std::array<value_type, Size>& array)
    {
        assign(array);
        return *this;
    }
#endif
#if (VECTOR_ARRAY_MOVE)
    template<size_type Size>
    constexpr vector& operator=(value_type(&&array)[Size])
    {
        assign(std::move(array));
        return *this;
    }
#endif

    // ANCHOR Assign

    constexpr void assign(size_type n, const_reference x)
    {
        impl_assign_prepare(n);
        ctor_n(begin(), n, x);
    }
    template <std::input_iterator It>
    constexpr void assign(It first, It last)
    {
        impl_assign_prepare(last - first);
        copy_create_forward(first, last, begin());
    }
#if (VECTOR_ILIST_COPY)
    constexpr void assign(std::initializer_list<value_type> ilist)
    {
        assign(ilist.begin(), ilist.end());
    }
#endif
#if (VECTOR_ARRAY_COPY)
    template<size_type Size>
    constexpr void assign(const std::array<value_type, Size>& array)
    {
        static_assert(N >= Size);
        impl_assign_prepare(Size);
        copy_create_forward(array.begin(), array.end(), begin());
    }
#endif
#if (VECTOR_ARRAY_MOVE)
    template<size_type Size>
    constexpr void assign(value_type(&&array)[Size])
    {
        static_assert(N >= Size);
        impl_assign_prepare(Size);
        move_create_forward(array, array + Size, begin());
    }
#endif

    // ANCHOR Capacity

    constexpr static size_type capacity()       { return N; }
    constexpr static size_type max_size()       { return N; }
    constexpr size_type size() const noexcept   { return len; }
    constexpr bool empty() const noexcept       { return len == 0; }
    constexpr bool full() const noexcept        { return len == capacity(); }

    // ANCHOR Iterators

    constexpr iterator begin() noexcept                 { return buf; }
    constexpr const_iterator begin() const noexcept     { return buf; }
    constexpr const_iterator cbegin() const noexcept    { return begin(); }
    constexpr iterator end() noexcept                   { return buf + len; }
    constexpr const_iterator end() const noexcept       { return buf + len; }
    constexpr const_iterator cend() const noexcept      { return end(); }

    // ANCHOR Access

    constexpr reference operator[](size_type i)             { return buf[i]; }
    constexpr const_reference operator[](size_type i) const { return buf[i]; }
    constexpr reference front()                             { return buf[0]; }
    constexpr const_reference front() const                 { return buf[0]; }
    constexpr reference back()                              { return buf[len - 1]; }
    constexpr const_reference back() const                  { return buf[len - 1]; }
    constexpr pointer data() noexcept                       { return buf; }
    constexpr const_pointer data() const noexcept           { return buf; }

    // ANCHOR Modifiers

    constexpr void resize(size_type n)
    {
        impl_resize(n);
    }
    
    constexpr void resize(size_type n, const_reference x)
    {
        impl_resize(n, x);
    }

    constexpr void clear() noexcept
    {
        dtor_n(begin(), size());
        len = 0;
    }

    constexpr void push_back(const_reference x)
    {
        emplace_back(x);
    }
    
    constexpr void push_back(universal_reference x)
    {
        emplace_back(std::move(x));
    }

    template<class... Args>
    constexpr reference emplace_back(Args&&... args)
    {
        assert(full() == false);
        return *ctor(begin() + len++, std::forward<Args>(args)...);
    }
    
    template<class... Args>
    constexpr iterator emplace(const_iterator pos, Args&&... args)
    {
        assert(full() == false);
        assert(begin() <= pos && pos <= end());
        auto ret = cast_itr(pos);
        if (ret != end()) {
            ctor(end(), std::move(back()));
            move_assign_backward(ret, end() - 1, end());
            dtor(ret);
        }
        ++len;
        return ctor(ret, std::forward<Args>(args)...);
    }

    constexpr iterator insert(const_iterator pos, const_reference x)
    {
        return emplace(pos, x);
    }

    constexpr iterator insert(const_iterator pos, universal_reference x)
    {
        return emplace(pos, std::move(x));
    }

    constexpr iterator insert(const_iterator pos, size_type n, const_reference x)
    {
        auto ret = impl_insert_move(pos, n);
        ctor_n(ret, n, x);
        return ret;
    }

    template<class InputIt>
    constexpr iterator insert(const_iterator pos, InputIt first, InputIt last)
    {
        auto ret = impl_insert_move(pos, last - first);
        copy_create_forward(first, last, ret);
        return ret;
    }

    constexpr iterator insert(const_iterator pos, std::initializer_list<value_type> ilist)
    {
        return insert(pos, ilist.begin(), ilist.end());
    }

    constexpr void pop_back()
    {
        assert(empty() == false);
        dtor(begin() + --len);
    }

    constexpr void pop_idx(size_type pos)
    {
        assert(size() > pos);
        dtor(begin() + pos);
        if (pos != size() - 1)
            buf[pos] = std::move(back());
        --len;
    }

    constexpr void pop(const_iterator pos)
    {
        pop_idx(pos - begin());
    }

    constexpr void pop(const_iterator first, const_iterator last)
    {
        assert(empty() == false);
        assert(begin() <= first && last <= end());

        size_type n = last - first;
        size_type to_move = std::min(size_type(end() - last), n);

        dtor_n(first, n);
        move_assign_forward(end() - to_move, end(), cast_itr(first));
        len -= n;
    }

    constexpr iterator erase(const_iterator pos)
    {
        return erase(pos, pos + 1);
    }

    constexpr iterator erase(const_iterator first, const_iterator last)
    {
        assert(empty() == false);
        assert(begin() <= first && last <= end());
        dtor(first, last);
        move_assign_forward(cast_itr(last), end(), cast_itr(first));
        len -= last - first;
        return cast_itr(first);
    }

    constexpr void swap(vector& other) noexcept(std::is_nothrow_swappable_v<value_type>)
    {
        auto tmp    = std::move(other);
        other       = std::move(*this);
        *this       = std::move(tmp);
    }
private:
    static constexpr iterator cast_itr(const_iterator itr)
    {
        return const_cast<iterator>(itr);
    }

    template<class... Args>
    constexpr void impl_resize(size_type n, Args&&... args)
    {
        assert(n <= capacity());
        if (n > len)
            ctor_n(begin() + len, n - len, std::forward<Args>(args)...);
        else
            dtor_n(begin() + n, len - n);
        len = n;
    }

    constexpr iterator impl_insert_move(const_iterator pos, size_type n)
    {
        assert(size() + n <= capacity());
        assert(begin() <= pos && pos <= end());

        if (!n)
            return cast_itr(pos);

        auto insert_head = cast_itr(pos);
        auto insert_tail = insert_head + n;

        size_type till_end = end() - pos;
        size_type to_move_create;
        size_type to_move_assign;
        iterator create_dst_head;

        if (insert_tail > end()) {
            to_move_assign = 0;
            to_move_create = till_end;
            create_dst_head = insert_tail;
        } else {
            to_move_assign = till_end - n;
            to_move_create = n;
            create_dst_head = end();
        }
        move_create_forward(
            end() - to_move_create, 
            end(), 
            create_dst_head);
        move_assign_backward(
            insert_head, 
            insert_head + to_move_assign, 
            insert_tail + to_move_assign);
        dtor_n(insert_head, to_move_create);
        len += n;
        return insert_head;
    }

    constexpr void impl_assign_prepare(size_type n)
    {
        assert(capacity() >= n);
        dtor_n(begin(), size());
        len = n;
    }
private:
    storage<T, N> buf;
    size_type len = 0;
};

}

#undef VECTOR_ILIST_COPY
#undef VECTOR_ARRAY_COPY
#undef VECTOR_ARRAY_MOVE

#endif