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

std::unique_ptr<FileObject> PassthroughFile::copy() const
	{ return std::make_unique<PassthroughFile>( filepath ); }

	
uint64_t PassthroughFile::filesize() const{
	__stat64 stat = { 0 };
	if( _wstat64( filepath.c_str(), &stat ) == 0 )
		return stat.st_size;
	else
		return 0;
}

class PassthroughFileHandle : public FileHandle{
	private:
		File file;
		//TODO: Try to optimize away the seeks?
		
	public:
		PassthroughFileHandle( const std::wstring& path, const wchar_t* mode )
			:	file( path.c_str(), mode ) { }
		
		uint64_t read( ByteView to_read, uint64_t offset ) override{
			file.seek( offset, 0 );
			return file.read( to_read );
		}

		uint64_t write( ConstByteView to_write, uint64_t offset ) override{
			file.seek( offset, 0 );
			return file.write( to_write );
		}
};

std::unique_ptr<FileHandle> PassthroughFile::openRead() const
	{ return std::make_unique<PassthroughFileHandle>( filepath.c_str(), L"rb" ); }

std::unique_ptr<FileHandle> PassthroughFile::openWrite() const
	{ return std::make_unique<PassthroughFileHandle>( filepath.c_str(), L"wb" ); }

std::unique_ptr<FileHandle> PassthroughFile::openAppend() const
	{ return std::make_unique<PassthroughFileHandle>( filepath.c_str(), L"ab" ); }

