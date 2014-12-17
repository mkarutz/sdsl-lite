#pragma once

#include "sdsl/int_vector.hpp"

#include  <algorithm>

namespace sdsl
{

template<class t_itr>
struct list_dummy {
    using size_type = sdsl::int_vector<>::size_type;
    using const_iterator = t_itr;
    t_itr m_begin;
    t_itr m_end;
    size_type m_size;
    list_dummy(const t_itr& b,const t_itr& e,size_type s) : m_begin(b), m_end(e), m_size(s) {}
    list_dummy(const t_itr& b,const t_itr& e) : m_begin(b), m_end(e)
    {
        m_size = m_begin.size();
    }
    t_itr begin() const
    {
        return m_begin;
    }
    t_itr end() const
    {
        return m_end;
    }
    size_type size() const
    {
        return m_size;
    }
    bool operator<(const list_dummy<t_itr>& b) const
    {
        return size() < b.size();
    }
};

}