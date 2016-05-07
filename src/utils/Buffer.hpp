/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef BUFFER_HPP
#define BUFFER_HPP

#include "ArrayView.hpp"
#include <memory>


class Buffer{
	private:
		std::unique_ptr<uint8_t[]> buffer;
		size_t lenght;
		
	public:
		Buffer() = default;
		Buffer( size_t lenght ) : buffer( std::make_unique<uint8_t[]>( lenght ) ), lenght(lenght) { }
		//Buffer( Buffer&& other ) : buffer(std::move(other.buffer)), lenght(other.lenght) {}
		/*
		Buffer( const Buffer& copy ) : Buffer( copy.lenght ) {
			for( size_t i=0; i<lenght; i++ )
				buffer[i] = copy.buffer[i];
		}
		*/
		      auto data()      { return buffer.get(); }
		const auto data() const{ return buffer.get(); }
		auto size() const{ return lenght; }
		
		auto begin(){ return buffer.get(); }
		auto end(){ return buffer.get() + lenght; }
		
		uint8_t& operator[]( int index )       { return buffer[index]; }
		uint8_t& operator[]( int index ) const { return buffer[index]; }
		
		ArrayView<      uint8_t> view()      { return { data(), size() }; }
		ArrayView<const uint8_t> view() const{ return { data(), size() }; }
};

#endif
