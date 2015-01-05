#include "sdsl/uint128_t.hpp"

//! Namespace for the succinct data structure library
namespace sdsl
{

#ifndef MSVC_COMPILER
std::ostream& operator<<(std::ostream& os, const uint128_t& x)
{
    uint64_t X[2] = {(uint64_t)(x >> 64), (uint64_t)x};
    for (int j=0; j < 2; ++j) {
        for (int i=0; i < 16; ++i) {
            os << std::hex << ((X[j]>>60)&0xFULL);
            X[j] <<= 4;
        }
    }
    return os;
}
#else
std::ostream& operator<<(std::ostream& os, const uint128_t& x)
{
	uint64_t X[2] = { x.m_high, x.m_lo };
	for (int j = 0; j < 2; ++j) {
		for (int i = 0; i < 16; ++i) {
			os << std::hex << ((X[j] >> 60) & 0xFULL) << std::dec;
			X[j] <<= 4;
		}
	}
	return os;
}

#endif

} // end namespace
