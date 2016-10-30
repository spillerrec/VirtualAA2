/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "lz4.hpp"

#include <lz4.h>
#include <lz4hc.h>

#include "../utils/Buffer.hpp"

uint64_t lz4::maxSize( uint64_t uncompressed_size )
	{ return LZ4_compressBound( uncompressed_size ); }

/** Note that out.size() should be at least lz4::maxSize() */
ByteView lz4::compress( ConstByteView data, ByteView out ){
	//Compress
	auto result = LZ4_compress_HC(
			(const char*)data.begin(), (char*)out.begin()
		,	data.size(), out.size()
		,	LZ4HC_DEFAULT_CLEVEL //TODO: Higher?
		);
	//TODO: investigate streaming API
	
	//Check for error
	if( result < 0 )
		return {};
	//TODO: error?
	
	//Return only the range actually needed
	return out.left( result );
}

Buffer lz4::compress( ConstByteView data ){
	Buffer out( maxSize( data.size() ) );
	auto compressed = compress( data, out.view() );
	return Buffer( compressed ); //TODO: resize buffer instead?
}

/** Note that out.size() must be exactly the size of the uncompressed data */
bool lz4::decompress( ConstByteView data, ByteView out ){
	LZ4_decompress_safe( (const char*)data.begin(), (char*)out.begin(), data.size(), out.size() );
	return true; //TODO: check for error
}

