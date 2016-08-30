/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef PP_CRYPTO_HPP
#define PP_CRYPTO_HPP

#include <array>
#include <cstdint>

template<typename T> class ArrayView;

namespace PP{
	
class HeaderDecrypter{
	private:
		static const uint8_t offset[];
		std::array<uint8_t, 8> mask{ { 0xFA, 0x49, 0x7B, 0x1C, 0xF9, 0x4D, 0x83, 0x0A } };
		uint32_t index = { 0 };
	
	public:
		void setPosition( uint32_t position );
		void decrypt( ArrayView<uint8_t> buffer );
		
		bool operator==( const HeaderDecrypter& other ) const
			{ return index==other.index && mask==other.mask; }
};

class FileDecrypter{
	private:
		static const uint8_t mask[];
		uint32_t index = { 0 };
	
	public:
		void setPosition( uint32_t position ){ index = position; }
		void decrypt( ArrayView<uint8_t> buffer );
};

}

#endif
