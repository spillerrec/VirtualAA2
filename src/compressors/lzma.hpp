/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef LZMA_HPP
#define LZMA_HPP

#include "../utils/ArrayView.hpp"

class Buffer;

namespace lzma{
	uint64_t maxSize( uint64_t uncompressed_size );
	ByteView compress( ConstByteView data, ByteView out );
	Buffer compress( ConstByteView data );
	bool decompress( ConstByteView data, ByteView out );
}

#endif
