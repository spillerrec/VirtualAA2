/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "LzmaFile.hpp"
#include "FilePath.hpp"
#include "BufferHandle.hpp"
#include "mergers/PassthroughMerger.hpp"
#include "../compressors/lzma.hpp"
#include "../utils/ByteViewReader.hpp"
#include "../utils/debug.hpp"
#include "../utils/File.hpp"

#include <algorithm>
#include <stdexcept>
#include <iostream>

LzmaFile::LzmaFile( std::wstring filepath ) : filepath(filepath) {
	//TODO: may as well have this as input, the factory have it anyway
	FilePath path( this->filepath.c_str() );
	require( path.path.size() > 0 );
	filename = path.path.back();
	
	//Remove "[LZMA] " prefix
	//TODO: assert that it exists
	filename = filename.right( filename.size()-7 );
	
	//Get resulting filesize
	File f( filepath.c_str(), L"rb" );
	auto buf_size = f.read(4);
	file_size = ByteViewReader( buf_size ).read32u();
}

std::unique_ptr<AMergingObject> LzmaFile::createMerger() const
	{ return std::make_unique<PassthroughMerger>( *this ); }

class LzmaFileHandle : public BufferReadHandle{
	public:
		LzmaFileHandle( const std::wstring& path, uint64_t filesize ) : BufferReadHandle( filesize ) {
			File f( path.c_str(), L"rb" );
			f.read(4); //Already known //TODO: avoid alloc?
			lzma::decompress( f.readAll().constView(), data.view() );
		}
};

std::unique_ptr<FileHandle> LzmaFile::openRead() const
	{ return std::make_unique<LzmaFileHandle>( filepath, filesize() ); }

bool LzmaFile::compressFile( std::wstring filepath, ConstByteView data ){
	File f( filepath.c_str(), L"wb" );
	f.write32u( data.size() );
	
	Buffer out( lzma::maxSize( data.size() ) );
	auto compressed = lzma::compress( data, out.view() );
	f.write( compressed );
	
	return compressed.size() > 0;
}