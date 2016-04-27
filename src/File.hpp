/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef FILE_HPP
#define FILE_HPP

#include "Buffer.hpp"

#include <cstdio>

class File{
	private:
		FILE* handle;
	
	public:
		File( const char* filepath ) : handle( std::fopen( filepath, "rb" ) ) {
			//TODO: throw on (handle == nullptr)
		}
		~File(){ std::fclose( handle ); }
		
		void read( Buffer& buf ){
			fread( buf.data(), 1, buf.size(), handle );
		}
		
		int seek( long int offset, int origin )
			{ return fseek( handle, offset, origin ); }
		
		Buffer read( size_t bytes ){
			Buffer buf( bytes );
			read( buf );
			return buf;
		}
};

#endif
