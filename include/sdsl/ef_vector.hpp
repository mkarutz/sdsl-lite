#pragma once

#include "bit_streams.hpp"
#include "eliasfano_list.hpp"
#include "bitvector_list.hpp"

namespace sdsl
{



template<uint32_t t_block_size = 128>
class ef_vector
{
    private:
        static_assert(t_block_size > 1 , "ef_vector: block size must be larger than `1`");
    public:
        typedef uint64_t                                 value_type;
        typedef random_access_const_iterator<ef_vector> iterator;
        typedef iterator                                 const_iterator;
        typedef const value_type                         reference;
        typedef const value_type                         const_reference;
        typedef const value_type*                        const_pointer;
        typedef ptrdiff_t                                difference_type;
        typedef int_vector<>::size_type                  size_type;
        static  const uint64_t                           block_size    = t_block_size;
        static  const uint64_t                           sample_dens   = t_block_size;
        typedef ef_vector                               enc_vec_type;

    private:
        size_type                                        m_size = 0;
        bit_vector                                       m_low_data;
        int_vector<>                                     m_block_data;
        bit_istream                                      m_lis;
    private:
        enum class efv_blocktype
        {
            EF, BV, FULL
        };

        efv_blocktype
        determine_block_type(size_t m,size_t n) const
        {
            if (n==m) return efv_blocktype::FULL;
            auto ef_bits = eliasfano_list<true,true>::estimate_size(m,n);
            auto bv_bits = bitvector_list<true>::estimate_size(m,n);
            if (bv_bits < ef_bits) return efv_blocktype::BV;
            return efv_blocktype::EF;
        }

        size_type encode_block(bit_ostream& os,std::vector<uint64_t>& data,size_t cur_block,
                               const std::vector<uint64_t>& top,bool last = false)
        {
            size_type offset = os.tellp();
            if (std::is_sorted(data.begin(),data.end()) && top[cur_block] < data[0] && !last) {

                auto block_value_offset = top[cur_block];
                std::for_each(data.begin(),data.end(),[&block_value_offset](uint64_t& x) { x-=block_value_offset; });
                auto block_universe = top[cur_block+1] - block_value_offset;
                auto block_type = determine_block_type(data.size(),block_universe);
                if (block_type == efv_blocktype::BV) {
                    os.put_int(1,2);
                    bitvector_list<true>::create(os,data.begin(),data.end(),block_universe);
                } else if (block_type == efv_blocktype::EF) {
                    os.put_int(2,2);
                    eliasfano_list<true,true>::create(os,data.begin(),data.end(),data.size(),block_universe);
                } else {
                    os.put_int(3,2);
                }
            } else {
                os.put_int(0,2);
                eliasfano_list<false,false>::create(os,data.begin(),data.end());
            }
            return offset;
        }

    public:
        ef_vector() : m_lis(m_low_data) {};
        ef_vector(const ef_vector&) = default;
        ef_vector(ef_vector&&) = default;
        ef_vector& operator=(const ef_vector&) = default;
        ef_vector& operator=(ef_vector&&) = default;
        ~ef_vector() = default;

        template<class Container>
        ef_vector(const Container& c) : m_size(c.size()), m_lis(m_low_data)
        {
            // (1) construct top
            std::vector<uint64_t> top;
            auto itr = c.begin()+block_size-1;
            auto end = c.end();
            size_t block_items = 0;
            top.push_back(0); // first block
            while (std::distance(itr,end) > (int64_t) block_size) {
                top.push_back(*itr);
                itr += block_size;
            }
            if (m_size%block_size != 0) top.push_back(*(end-1)); // last block incomplete
            // (2) construct lower lvl blocks
            {
                bit_ostream os(m_low_data);
                std::vector<uint64_t> tmp(block_size);
                itr = c.begin();
                m_block_data.resize(2*top.size());
                size_t cur_block = 0;
                block_items = 0;
                while (itr != end) {
                    tmp[block_items++] = *itr++;
                    if (block_items == block_size) {
                        m_block_data[2*cur_block] = encode_block(os,tmp,cur_block,top);
                        block_items = 0;
                        cur_block++;
                    }
                }
                if (block_items != 0) {
                    tmp.resize(block_items);
                    m_block_data[2*cur_block] = encode_block(os,tmp,cur_block,top,true);
                }
            }
            // (3) encode top
            for (size_t i=0; i<top.size(); i++) m_block_data[2*i+1] = top[i];

            // (4) bit compress stuff
            util::bit_compress(m_block_data);
        }

        value_type sample(const size_type i) const
        {
            return m_block_data[2*i+1];
        }

        uint32_t get_sample_dens() const
        {
            return block_size;
        }

        void get_inter_sampled_values(const size_type i, uint64_t* it)const
        {
            static_assert(block_size == 0,"this does not work yet.");
            *(it++) = 0;
        };

        size_type size() const
        {
            return m_size;
        }

        bool empty() const
        {
            return 0==m_size;
        }

        const const_iterator begin()const
        {
            return const_iterator(this, 0);
        }

        const const_iterator end()const
        {
            return const_iterator(this, this->m_size);
        }

        value_type operator[](size_type i) const
        {
            auto block_num = i/block_size;
            auto in_block_offset = i%block_size;
            auto data_offset = m_block_data[2*block_num];
            m_lis.seek(data_offset);
            auto block_type = m_lis.peek_int(2);
            if (block_type == 1) {
                auto value_offset = m_block_data[2*block_num+1];
                auto block_universe = m_block_data[2*(block_num+1)+1] - value_offset;
                auto list = bitvector_list<true>::materialize(m_lis,data_offset+2,block_size,block_universe);
                auto itr = list.begin();
                itr+=in_block_offset;
                return value_offset + *itr;
            }
            if (block_type == 2) {
                auto value_offset = m_block_data[2*block_num+1];
                auto block_universe = m_block_data[2*(block_num+1)+1] - value_offset;
                auto list = eliasfano_list<true,true>::materialize(m_lis,data_offset+2,block_size,block_universe);
                auto itr = list.begin();
                itr+=in_block_offset;
                return value_offset + *itr;
            }
            if (block_type == 3) {
                auto value_offset = m_block_data[2*block_num+1];
                return value_offset + in_block_offset + 1;
            }
            auto list = eliasfano_list<false,false>::materialize(m_lis,data_offset+2);
            auto itr = list.begin();
            return *(itr+in_block_offset);
        }
        size_type serialize(std::ostream& out, structure_tree_node* v=nullptr, std::string name="") const
        {
            structure_tree_node* child = structure_tree::add_child(v, name, util::class_name(*this));
            size_type written_bytes = 0;
            written_bytes += write_member(m_size, out, child, "size");
            written_bytes += m_low_data.serialize(out, child, "low");
            written_bytes += m_block_data.serialize(out, child, "block data");
            structure_tree::add_size(child, written_bytes);
            return written_bytes;
        }

        void load(std::istream& in)
        {
            read_member(m_size, in);
            m_low_data.load(in);
            m_block_data.load(in);
        }

        void swap(ef_vector& v)
        {
            if (this != &v) {
                std::swap(m_block_data,v.m_block_data);
                std::swap(m_size, v.m_size);
                std::swap(m_low_data,v.m_low_data);
            }
        }
};



}
