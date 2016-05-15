/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef BUFFER_READER_HPP
#define BUFFER_READER_HPP

#include "ArrayView.hpp"
#include "Buffer.hpp"
#include "debug.hpp"
#include <iostream>

inline uint32_t convert32unsigned( uint8_t a, uint8_t b, uint8_t c, uint8_t d )
	{ return a + (b<<8) + (c<<16) + (d<<24); }


class ByteViewReader{
	private:
		ByteView buffer;
		size_t position{ 0 };
		
	public:
		ByteViewReader( ByteView view ) : buffer(view) { }
		ByteViewReader( Buffer& buf ) : buffer(buf.view()) { }
		
		auto tell() const{ return position; }
		auto left() const{ return buffer.size() - position; }
		
		ByteView makeView( unsigned from, unsigned amount ){
			require( from+amount <= buffer.size() );
			return ByteView( buffer.begin() + from, amount );
		}
		
		ByteView read( unsigned amount ){
			position += amount;
			return makeView( position - amount, amount );
		}
		
		uint8_t read8u(){
			require( left() >= 1 );
			return buffer[position++];
		}
		
		uint16_t read16u(){
			auto b = read( 2 );
			return (b[1] << 8) + b[0];
		}
		
		uint32_t read32u(){
			auto b = read( 4 );
			return convert32unsigned( b[0], b[1], b[2], b[3] );
		}
		
		ByteView readName(){
			auto lenght = read32u();
			return { read( lenght ) };
		}
};

#endif
