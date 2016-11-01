/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "CompressedFile.hpp"
#include "FilePath.hpp"
#include "mergers/PassthroughMerger.hpp"
#include "../utils/Buffer.hpp"
#include "../utils/ByteViewReader.hpp"
#include "../utils/debug.hpp"
#include "../utils/File.hpp"


CompressedFile::CompressedFile( std::wstring filepath, const wchar_t* prefix ) : filepath(filepath) {
	FilePath path( this->filepath.c_str() );
	require( path.path.size() > 0 );
	filename = path.path.back();
	
	//Remove prefix
	//TODO: assert that it exists
	filename = filename.right( filename.size()-std::wcslen(prefix) );
	
	//Get resulting filesize
	File f( filepath.c_str(), L"rb" );
	auto buf_size = f.read(4);
	file_size = ByteViewReader( buf_size ).read32u();
}

std::unique_ptr<AMergingObject> CompressedFile::createMerger() const
	{ return std::make_unique<PassthroughMerger>( *this ); }
	
Buffer CompressedFile::compressedData() const{
	File f( filepath.c_str(), L"rb" );
	f.read(4); //Already known //TODO: avoid alloc?
	return f.readAll();
}

bool CompressedFile::writeCompressed( const wchar_t* path, uint64_t size, ByteView data ){
	if( data.size() == 0 )
		return false;
	
	//TODO: check if file was opened and written properly?
	File f( path, L"wb" );
	f.write32u( size );
	f.write( data );
	
	return true;
}
