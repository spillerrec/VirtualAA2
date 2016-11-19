/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef PP_ARCHIVE_HPP
#define PP_ARCHIVE_HPP

#include "../utils/Buffer.hpp"

class File;

namespace PP{
class HeaderDecrypter;

class Header{
	private:
		Buffer header;
		uint32_t file_count;
	
	public:
		Header( File& file );
		
		static const uint8_t magic[];
		static const int magic_length = 8;
		
		struct SubFile{
			ConstByteView file;
			
			ConstByteView filename() const;
			ConstByteView metadata() const;
			uint32_t size() const;
			uint32_t offset() const;
			
			SubFile( ConstByteView data, uint32_t offset );
			SubFile& operator++();
			bool operator!=( const SubFile& other ) const
				{ return file.begin() != other.file.begin(); }
			SubFile& operator*(){ return *this; }
		};
		
		SubFile begin();
		SubFile end();
};

Buffer readFile( File& file, uint32_t offset, uint32_t size );

}

#endif
