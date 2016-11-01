/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "deflate.hpp"

#include <zlib.h>

#include "../utils/Buffer.hpp"

uint64_t DEFLATE::maxSize( uint64_t uncompressed_size )
	{ return compressBound( uncompressed_size ); }

/** Note that out.size() should be at least DEFLATE::maxSize() */
ByteView DEFLATE::compress2( ConstByteView data, ByteView out ){
	uLongf out_size = out.size();
	if( compress( (Bytef*)out.begin(), &out_size, (const Bytef*)data.begin(), (uLong)data.size() ) != Z_OK )
		return {};
	
	return out.left( out_size );
}

Buffer DEFLATE::compress2( ConstByteView data ){
	Buffer out( maxSize( data.size() ) );
	auto compressed = compress2( data, out.view() );
	return Buffer( compressed ); //TODO: resize buffer instead?
}

/** Note that out.size() must be exactly the size of the uncompressed data */
bool DEFLATE::decompress( ConstByteView data, ByteView out ){
	uLongf uncompressed = out.size();
	auto result = uncompress( (Bytef*)out.begin(), &uncompressed, (const Bytef*)out.begin(), out.size() );
	
	//TODO: check size of uncompressed data?
	return result == Z_OK;
}

