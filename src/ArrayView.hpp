/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef ARRAY_VIEW_HPP
#define ARRAY_VIEW_HPP

#include <stdint.h>
#include <string>

class ArrayView{
	protected:
		uint8_t* data;
		size_t lenght;
		
	public:
		ArrayView() : data(nullptr), lenght(0) {} //TODO: remove?
		ArrayView( uint8_t* data, size_t lenght ) : data(data), lenght(lenght) { }
		
		auto size() const{ return lenght; }
		uint8_t& operator[]( int index )       { return data[index]; } //TODO: Check in debug mode
		uint8_t  operator[]( int index ) const { return data[index]; } //TODO: Check in debug mode
		
		uint8_t* begin() { return data         ; }
		uint8_t* end()   { return data + lenght; }
};

struct NotArrayView : public ArrayView {
	NotArrayView() {}
	NotArrayView( ArrayView view ) : ArrayView( view ) { }
	uint8_t  operator[]( int index ) const { return ~(data[index]); } //TODO: Check in debug mode
	
	std::string toString() const;
};

#endif
