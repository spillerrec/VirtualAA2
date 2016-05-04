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
		File( const char* filepath, const char* flags="rb" );
		File( const wchar_t* filepath, const wchar_t* flags=L"rb" );
		~File(){ std::fclose( handle ); }
		
		void read( Buffer& buf ){
			fread( buf.data(), 1, buf.size(), handle );
		}
		
		auto seek( long int offset, int origin )
			{ return fseek( handle, offset, origin ); }
		
		auto tell(){ return ftell( handle ); }
		
		Buffer read( size_t bytes ){
			Buffer buf( bytes );
			read( buf );
			return buf;
		}
		
		Buffer readAll() {
			auto current = tell();
			seek( 0, SEEK_END );
			auto end = tell();
			seek( current, 0 );
			return read( end - current );
		}
};

#endif
