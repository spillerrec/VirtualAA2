/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef ARRAY_VIEW_HPP
#define ARRAY_VIEW_HPP

#include <stdint.h>
#include <string>

template<typename T>
class ArrayView{
	protected:
		T* data;
		size_t lenght;
		
	public:
		ArrayView() : data(nullptr), lenght(0) {} //TODO: remove?
		ArrayView( T* data, size_t lenght ) : data(data), lenght(lenght) { }
		
		auto size() const{ return lenght; }
		T& operator[]( int index )       { return data[index]; } //TODO: Check in debug mode
		T  operator[]( int index ) const { return data[index]; } //TODO: Check in debug mode
		
		T* begin() { return data         ; }
		T* end()   { return data + lenght; }
};

using ByteView = ArrayView<uint8_t>;

struct NotByteView : public ByteView {
	NotByteView() {}
	NotByteView( ByteView view ) : ByteView( view ) { }
	uint8_t  operator[]( int index ) const { return ~(data[index]); } //TODO: Check in debug mode
	
	std::string toString() const;
};

#endif
