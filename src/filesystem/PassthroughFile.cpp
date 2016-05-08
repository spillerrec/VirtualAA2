/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "PassthroughFile.hpp"
#include "FilePath.hpp"
#include "../utils/debug.hpp"
#include "../utils/File.hpp"

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
	return File( filepath.c_str(), L"rb" ).stealHandle();
}
FileHandle PassthroughFile::openWrite() const{
	return File( filepath.c_str(), L"wb" ).stealHandle();
}
FileHandle PassthroughFile::openAppend() const{
	return File( filepath.c_str(), L"ab" ).stealHandle();
}

uint64_t PassthroughFile::read( FileHandle handle, ByteView to_read, uint64_t offset ) const{
	FileOperations file( handle );
	file.seek( offset, 0 );
	return file.read( to_read );
}

uint64_t PassthroughFile::write( FileHandle handle, ConstByteView to_write, uint64_t offset ) const{
	FileOperations file( handle );
	file.seek( offset, 0 );
	return file.write( to_write );
}

void PassthroughFile::close( FileHandle handle ) const{
	fclose( handle );
}

