/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "PassthroughFile.hpp"
#include "FilePath.hpp"
#include "FileSystem.hpp"
#include "mergers/PassthroughMerger.hpp"
#include "../utils/debug.hpp"
#include "../utils/File.hpp"

PassthroughFile::PassthroughFile( const std::wstring& parent, FolderContent fileinfo )
	:	fileinfo(fileinfo), filepath(fileinfo.path(parent)) {
	FilePath path( filepath.c_str() );
	require( path.path.size() > 0 );
	filename = path.path.back();
}

std::unique_ptr<AMergingObject> PassthroughFile::createMerger() const
	{ return std::make_unique<PassthroughMerger>( *this ); }

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

