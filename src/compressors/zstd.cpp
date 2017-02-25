/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "zstd.hpp"

#include <zstd.h>

#include "../utils/Buffer.hpp"


uint64_t zstd::maxSize( uint64_t uncompressed_size )
	{ return ZSTD_compressBound( uncompressed_size ); }

ByteView zstd::compress( ConstByteView data, ByteView out ){
	auto result = ZSTD_compress( out.begin(), out.size(), data.begin(), data.size(), 19 );
	if( ZSTD_isError(result) )
		return {};
	return out.left( result );
}

Buffer zstd::compress( ConstByteView data ){
	Buffer out( maxSize( data.size() ) );
	auto compressed = zstd::compress( data, out.view() );
	return Buffer( compressed ); //TODO: resize buffer instead?
}

bool zstd::decompress( ConstByteView data, ByteView out ){
	//Fail if decoded_size specified, and not the same
	auto decompressed_size = ZSTD_getDecompressedSize( data.begin(), data.size() );
	if( decompressed_size != 0 )
		if( decompressed_size != out.size() )
			return false;
	
	auto decoded_size = ZSTD_decompress( out.begin(), out.size(), data.begin(), data.size() );
	
	return decoded_size == out.size();
}
