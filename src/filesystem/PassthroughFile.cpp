/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "PassthroughFile.hpp"
#include "FilePath.hpp"
#include "../utils/debug.hpp"

PassthroughFile::PassthroughFile( std::wstring filepath ) : filepath(filepath) {
	FilePath path( this->filepath.c_str() );
	require( path.path.size() > 0 );
	filename = path.path.back();
}

WStringView PassthroughFile::name() const{
	return filename;
}

	
uint64_t PassthroughFile::filesize() const{
	__stat64 stat = { 0 };
	if( _wstat64( filepath.c_str(), &stat ) == 0 )
		return stat.st_size;
	else
		return 0;
}


FileHandle PassthroughFile::openRead() const{
	return _wfopen( filepath.c_str(), L"rb" );
}
FileHandle PassthroughFile::openWrite() const{
	return _wfopen( filepath.c_str(), L"wb" );
}

uint64_t PassthroughFile::read( FileHandle handle, uint8_t* buffer, uint64_t amount, uint64_t offset ) const{
	fseek( handle, offset, 0 );
	return fread( buffer, 1, amount, handle );
}

void PassthroughFile::close( FileHandle handle ) const{
	fclose( handle );
}

