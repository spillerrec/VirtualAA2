/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef DEFLATE_HPP
#define DEFLATE_HPP

#include "../utils/ArrayView.hpp"

class Buffer;

namespace DEFLATE{
	uint64_t maxSize( uint64_t uncompressed_size );
	ByteView compress2( ConstByteView data, ByteView out );
	Buffer compress2( ConstByteView data );
	bool decompress( ConstByteView data, ByteView out );
}

#endif
