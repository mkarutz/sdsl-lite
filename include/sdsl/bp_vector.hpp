/* sdsl - succinct data structures library
    Copyright (C) 2008 Simon Gog

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see http://www.gnu.org/licenses/ .
*/
/*! \file bp_vector.hpp
   \brief bp_vector.hpp contains the sdsl::bp_vector class.
   \author Simon Gog
*/
#ifndef SDSL_BP_VECTOR
#define SDSL_BP_VECTOR

#include <limits>

#include "int_vector.hpp"
#include "iterators.hpp"

#include "codecs.h"
#include "codecfactory.h"
#include "bitpacking.h"
#include "simdfastpfor.h"
#include "simdbinarypacking.h"
#include "deltautil.h"

#define sdsllikely(x)       __builtin_expect((x),1)
#define sdslunlikely(x)       __builtin_expect((x),0)

//! Namespace for the succinct data structure library.
namespace sdsl
{

//! A generic immutable space-saving vector class for unsigned integers.
/*! A vector v is stored more space-efficiently by self-delimiting coding
 *  the deltas v[i+1]-v[i] (v[-1]:=0). Space of the structure and random
 *  access time to it can be controlled by a sampling parameter sample_dens.
 *
 *  \tparam t_coder  Self-delimiting coder.
 *  \tparam sample_dens   Every sample_dens-th element of v is sampled.
 *  \tparam t_width  Width of the int_vector used to store the samples and pointers.
 *  This class is a parameter of csa_sada.
 * @ingroup int_vector
 */
template<uint32_t t_dens = 128>
class bp_vector
{
    public:
        using comp_codec = FastPForLib::FastBinaryPacking<t_dens/4>;
        typedef uint32_t                                 value_type;
        typedef random_access_const_iterator<bp_vector>  iterator;
        typedef iterator                                 const_iterator;
        typedef const value_type                         reference;
        typedef const value_type                         const_reference;
        typedef const value_type*                        const_pointer;
        typedef ptrdiff_t                                difference_type;
        typedef int_vector<>::size_type                  size_type;
        typedef iv_tag                                   index_category;
        static  const uint32_t                           sample_dens    = t_dens;

        mutable comp_codec                               coder;
        int_vector<0>   	m_sample_vals_and_pointer; // samples and pointers
        size_type         	m_size = 0;                // number of vector elements
        std::vector<uint32_t,FastPForLib::cacheallocator>  m_z; // storage for encoded deltas
    private:
        void clear()
        {
            m_z.resize(0);
            m_size = 0;
            m_sample_vals_and_pointer.resize(0);
        }

    public:
        bp_vector() = default;
        bp_vector(const bp_vector&) = default;
        bp_vector(bp_vector&&) = default;
        bp_vector& operator=(const bp_vector&) = default;
        bp_vector& operator=(bp_vector&&) = default;

        //! Constructor for a Container of unsigned integers.
        /*! \param c A container of unsigned integers.
          */
        template<class Container>
        bp_vector(const Container& c);

        //! Constructor for an int_vector_buffer of unsigned integers.
        /*
            \param v_buf A int_vector_buf.
        */
        template<uint8_t int_width>
        bp_vector(int_vector_buffer<int_width>& v_buf);

        //! Default Destructor
        ~bp_vector() { }

        //! The number of elements in the bp_vector.
        size_type size()const
        {
            return m_size;
        }

        //! Return the largest size that this container can ever have.
        static size_type max_size()
        {
            return int_vector<>::max_size()/2;
        }

        //!    Returns if the bp_vector is empty.
        bool empty() const
        {
            return 0==m_size;
        }

        //! Swap method for bp_vector
        void swap(bp_vector& v);

        //! Iterator that points to the first element of the bp_vector.
        const const_iterator begin()const
        {
            return const_iterator(this, 0);
        }

        //! Iterator that points to the position after the last element of the bp_vector.
        const const_iterator end() const
        {
            return const_iterator(this, this->m_size);
        }

        //! operator[]
        /*! \param i Index. \f$ i \in [0..size()-1]\f$.
         */
        auto operator[](size_type i) const -> value_type;

        //! Serialize the bp_vector to a stream.
        /*! \param out Out stream to write the data structure.
            \return The number of written bytes.
         */
        size_type serialize(std::ostream& out, structure_tree_node* v=nullptr, std::string name="")const;

        //! Load the bp_vector from a stream.
        void load(std::istream& in);

        //! Returns the i-th sample of bp_vector
        /*! \param i The index of the sample. 0 <= i < size()/get_sample_dens()
         *  \return The value of the i-th sample.
         */
        auto sample(const size_type i) const -> value_type;

        uint32_t get_sample_dens() const
        {
            return sample_dens;
        }

        /*!
         * \param i The index of the sample for which all values till the next sample should be decoded. 0 <= i < size()/get_sample_dens()
         * \param it A pointer to a uint64_t vector, whereto the values should be written
         */
        void get_inter_sampled_values(const size_type block_id, uint64_t* it) const
        {
            static uint32_t tmp[sample_dens];
            auto data_start = m_sample_vals_and_pointer[(block_id<<1)+1];
            size_t decoded_ids = 0;
            __builtin_prefetch(&m_z[data_start]);
            coder.decodeBlock(&m_z[data_start],&tmp[0],decoded_ids);
            uint64_t sum = 0;
            for (size_t j=0; j<sample_dens; j++) {
                sum += tmp[j];
                *(it++) = sum;
            }
        };
};

template<uint32_t t_dens>
inline auto bp_vector<t_dens>::operator[](const size_type i) const -> value_type
{
    assert(i+1 != 0);
    assert(i < m_size);
    size_type block_id = i/sample_dens;
    size_type offset = i%sample_dens;
    auto sample = m_sample_vals_and_pointer[(block_id<<1)];
    auto data_start = m_sample_vals_and_pointer[(block_id<<1)+1];
    if (sdslunlikely(!offset)) return sample + offset;
    static uint32_t tmp[sample_dens];
    size_t decoded_ids = 0;
    __builtin_prefetch(&m_z[data_start]);
    coder.decodeBlock(&m_z[data_start],&tmp[0],decoded_ids);
    tmp[0] = sample;
    for (size_t j=1; j<=offset; j++) tmp[j] += tmp[j-1];
    return tmp[offset];
}

template<uint32_t t_dens>
inline auto bp_vector<t_dens>::sample(const size_type i) const -> value_type
{
    assert(i*get_sample_dens()+1 != 0);
    assert(i*get_sample_dens() < m_size);
    return m_sample_vals_and_pointer[i<<1];
}

template<uint32_t t_dens>
void bp_vector<t_dens>::swap(bp_vector& v)
{
    if (this != &v) { // if v and _this_ are not the same object
        m_z.swap(v.m_z);
        m_sample_vals_and_pointer.swap(v.m_sample_vals_and_pointer);
        std::swap(m_size, v.m_size);
    }
}

template<uint32_t t_dens>
template<class Container>
bp_vector<t_dens>::bp_vector(const Container& c)
{
    if (c.size() == 0)  // if c is empty there is nothing to do...
        return;

    // check if values fit in 32bit uint
    if (std::any_of(c.begin(), c.end(), [](typename Container::value_type i) { return i > std::numeric_limits<uint32_t>::max(); })) {
        throw std::runtime_error("bp_vector: only supports 32bit integers.");
    }

    // alloc space
    size_type samples = c.size()/sample_dens;
    if (c.size()%sample_dens!=0) samples++;
    m_sample_vals_and_pointer.resize(2*samples+2); // add 2 for last entry
    m_z.resize(2*c.size()+1024); // from FastPfor Docs...
    uint32_t* out = &m_z[0];
    size_type z_size=0;
    while (FastPForLib::needPaddingTo128Bits(out)) {
        out++;
        z_size++;
    }
    // encode blocks
    auto sample_itr = m_sample_vals_and_pointer.begin();
    auto n = c.size();
    size_type left = n;
    static uint32_t tmp[sample_dens];
    const size_type block_size = sample_dens;
    for (auto it = c.begin(); left >= block_size; it+=block_size) {
        auto sample_val = *it;
        std::copy(it,it+sample_dens,std::begin(tmp));
        // sample first value and write pos of next block
        *(sample_itr++) = sample_val;
        *(sample_itr++) = z_size;
        /* encode in block after the sample value */
        FastPForLib::Delta::fastDelta(&tmp[0],sample_dens);
        tmp[0] = 0; // delta the first value with the sample
        size_t encoded_size_u32 = 0;
        coder.encodeBlock(&tmp[0],out,encoded_size_u32);
        out += encoded_size_u32;
        z_size += encoded_size_u32;
        left -= block_size;
    }
    // last block
    if (left) {
        auto sample_val = c[n-left];
        *(sample_itr++) = sample_val;
        *(sample_itr++) = z_size;
        for (size_t i=0; i<left; i++) tmp[i] = c[n-left+i];;
        for (size_t i=left; i<sample_dens; i++) tmp[i] = c[n-1];
        FastPForLib::Delta::fastDelta(&tmp[0],sample_dens);
        tmp[0] = 0; // delta the first value with the sample
        size_t encoded_size_u32 = 0;
        coder.encodeBlock(&tmp[0],out,encoded_size_u32);
        out += encoded_size_u32;
        z_size += encoded_size_u32;
    }
    m_z.resize(z_size);
    util::bit_compress(m_sample_vals_and_pointer);
    m_size = c.size();
}

template<uint32_t t_dens>
template<uint8_t int_width>
bp_vector<t_dens>::bp_vector(int_vector_buffer<int_width>& c)
{
    if (c.size() == 0)  // if c is empty there is nothing to do...
        return;

    // check if values fit in 32bit uint
    if (std::any_of(c.begin(), c.end(), [](typename int_vector_buffer<int_width>::value_type i) { return i > std::numeric_limits<uint32_t>::max(); })) {
        throw std::runtime_error("bp_vector: only supports 32bit integers.");
    }

    // alloc space
    size_type samples = c.size()/sample_dens;
    if (c.size()%sample_dens!=0) samples++;
    m_sample_vals_and_pointer.resize(2*samples+2); // add 2 for last entry
    m_z.resize(2*c.size()+1024); // from FastPfor Docs...
    uint32_t* out = &m_z[0];
    size_type z_size=0;
    while (FastPForLib::needPaddingTo128Bits(out)) {
        out++;
        z_size++;
    }
    // encode blocks
    auto sample_itr = m_sample_vals_and_pointer.begin();
    auto n = c.size();
    size_type left = n;
    static uint32_t tmp[sample_dens];
    const size_type block_size = sample_dens;
    for (auto it = c.begin(); left >= block_size; it+=block_size) {
        auto sample_val = *it;
        std::copy(it,it+sample_dens,std::begin(tmp));
        // sample first value and write pos of next block
        *(sample_itr++) = sample_val;
        *(sample_itr++) = z_size;
        /* encode in block after the sample value */
        FastPForLib::Delta::fastDelta(&tmp[0],sample_dens);
        tmp[0] = 0; // delta the first value with the sample
        size_t encoded_size_u32 = 0;
        coder.encodeBlock(&tmp[0],out,encoded_size_u32);
        out += encoded_size_u32;
        z_size += encoded_size_u32;
        left -= block_size;
    }
    // last block
    if (left) {
        auto sample_val = c[n-left];
        *(sample_itr++) = sample_val;
        *(sample_itr++) = z_size;
        for (size_t i=0; i<left; i++) tmp[i] = c[n-left+i];;
        for (size_t i=left; i<sample_dens; i++) tmp[i] = c[n-1];
        FastPForLib::Delta::fastDelta(&tmp[0],sample_dens);
        tmp[0] = 0; // delta the first value with the sample
        size_t encoded_size_u32 = 0;
        coder.encodeBlock(&tmp[0],out,encoded_size_u32);
        out += encoded_size_u32;
        z_size += encoded_size_u32;
    }
    // terminate samples to match enc_vector
    *(sample_itr++) = 0;
    *(sample_itr++) = 0;

    m_z.resize(z_size);
    util::bit_compress(m_sample_vals_and_pointer);
    m_size = c.size();
}


template<uint32_t t_dens>
typename bp_vector<t_dens>::size_type bp_vector<t_dens>::serialize(std::ostream& out, structure_tree_node* v, std::string name)const
{
    using namespace sdsl;
    structure_tree_node* child = structure_tree::add_child(v, name, util::class_name(*this));
    size_type written_bytes = 0;
    written_bytes += write_member(m_size, out, child, "size");

    auto* datac = sdsl::structure_tree::add_child(child, "encoded data","encoded data");
    size_type data_size = m_z.size();
    written_bytes += sdsl::write_member(data_size,out,datac,"data_size");
    out.write((const char*)m_z.data(), m_z.size()*sizeof(uint32_t));
    written_bytes += m_z.size()*sizeof(uint32_t);
    sdsl::structure_tree::add_size(datac, m_z.size()*sizeof(uint32_t));
    sdsl::structure_tree::add_size(datac, sizeof(size_type));

    written_bytes += m_sample_vals_and_pointer.serialize(out, child, "samples_and_pointers");
    structure_tree::add_size(child, written_bytes);
    return written_bytes;
}

template<uint32_t t_dens>
void bp_vector<t_dens>::load(std::istream& in)
{
    read_member(m_size, in);
    size_type data_size;
    read_member(data_size, in);
    m_z.resize(data_size);
    in.read((char*)m_z.data(),data_size*sizeof(uint32_t));
    m_sample_vals_and_pointer.load(in);
}

} // end namespace sdsl
#endif
