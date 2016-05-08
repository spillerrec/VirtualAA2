/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef ARRAY_VIEW_HPP
#define ARRAY_VIEW_HPP

#include <algorithm>
#include <memory>
#include <type_traits>
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
		
		using type = typename std::remove_cv<T>::type;
		
		auto size() const{ return lenght; }
		T& operator[]( int index )       { return data[index]; } //TODO: Check in debug mode
		T  operator[]( int index ) const { return data[index]; } //TODO: Check in debug mode
		
		      T* begin()       { return data         ; }
		const T* begin() const { return data         ; }
		      T* end()         { return data + lenght; }
		const T* end()   const { return data + lenght; }
		
		bool operator==( ArrayView<T> other ) const{
			if( size() != other.size() )
				return false;
			
			return std::equal( begin(), end(), other.begin() );
		}
		
		std::basic_string<type> toBasicString() const{
			auto buf = std::make_unique<type[]>( size() );
			for( unsigned i=0; i<size(); i++ )
				buf[i] = (*this)[i];
			return std::basic_string<type>( buf.get(), size() );
		}
};

using      ByteView = ArrayView<      uint8_t>;
using ConstByteView = ArrayView<const uint8_t>;

struct NotByteView : public ByteView {
	NotByteView() {}
	NotByteView( ByteView view ) : ByteView( view ) { }
	uint8_t  operator[]( int index ) const { return ~(data[index]); } //TODO: Check in debug mode
	
	std::string toString() const;
};



#endif
