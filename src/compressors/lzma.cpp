/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "lzma.hpp"

#include <lzma.h>

#include "../utils/Buffer.hpp"

uint64_t lzma::maxSize( uint64_t uncompressed_size )
	//TODO: I had simply used 2*size before, find out if there is a better way
	{ return uncompressed_size * 2; }

/** Note that out.size() should be at least lzma::maxSize() */
ByteView lzma::compress( ConstByteView data, ByteView out ){
	//Initialize LZMA
	lzma_stream strm = LZMA_STREAM_INIT;
	if( lzma_easy_encoder( &strm, 9 | LZMA_PRESET_EXTREME, LZMA_CHECK_CRC64 ) != LZMA_OK )
		return {};
	
	//Setup in/out buffers
	strm.next_in  = data.begin();
	strm.avail_in = data.size();
	strm.next_out  = out.begin();
	strm.avail_out = out.size();
	
	//Compress
	lzma_ret ret = lzma_code( &strm, LZMA_FINISH );
	if( ret != LZMA_STREAM_END ){
		lzma_end( &strm ); //TODO: Use RAII
		return {};
	}
	
	lzma_end( &strm );
	
	//Return only the range actually needed
	return out.left( out.size() - strm.avail_out );
}

Buffer lzma::compress( ConstByteView data ){
	Buffer out( maxSize( data.size() ) );
	auto compressed = compress( data, out.view() );
	return Buffer( compressed ); //TODO: resize buffer instead?
}

/** Note that out.size() must be exactly the size of the uncompressed data */
bool lzma::decompress( ConstByteView data, ByteView out ){
	//Initialize decoder
	lzma_stream strm = LZMA_STREAM_INIT;
	if( lzma_stream_decoder( &strm, UINT64_MAX, 0 ) != LZMA_OK )
		return false;
	
	//Setup in/out buffers
	strm.next_in   = data.begin();
	strm.avail_in  = data.size();
	strm.next_out  = out.begin();
	strm.avail_out = out.size();
	
	//Decompress
	if( lzma_code( &strm, LZMA_FINISH ) != LZMA_STREAM_END )
		return false;
	
	lzma_end(&strm);
	return true;
}

