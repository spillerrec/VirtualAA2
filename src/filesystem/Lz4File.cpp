/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "Lz4File.hpp"
#include "FilePath.hpp"
#include "mergers/PassthroughMerger.hpp"
#include "../compressors/lz4.hpp"
#include "../utils/ByteViewReader.hpp"
#include "../utils/debug.hpp"
#include "../utils/File.hpp"

#include <algorithm>
#include <stdexcept>
#include <iostream>

Lz4File::Lz4File( std::wstring filepath ) : filepath(filepath) {
	FilePath path( filepath.c_str() );
	require( path.path.size() > 0 );
	filename = path.path.back();
	
	//Remove "[LZ4] " prefix
	//TODO: assert that it exists
	filename = filename.right( filename.size()-6 );
	
	//Get resulting filesize
	File f( filepath.c_str(), L"rb" );
	auto buf_size = f.read(4);
	file_size = ByteViewReader( buf_size ).read32u();
}

std::unique_ptr<AMergingObject> Lz4File::createMerger() const
	{ return std::make_unique<PassthroughMerger>( *this ); }

class BufferReadHandle : public FileHandle{
	protected:
		Buffer data;
		ByteViewReader reader;
		
		BufferReadHandle( uint64_t size ) : data( size ), reader( data ) { }
		
	public:
		uint64_t read( ByteView to_read, uint64_t offset ) override{
			reader.seek( offset );
			auto data = reader.read( std::min(to_read.size(), reader.left()) );
			data.copyTo( to_read );
			return data.size();
		}

		uint64_t write( ConstByteView, uint64_t ) override { return 0; }
};

class Lz4FileHandle : public BufferReadHandle{
	public:
		Lz4FileHandle( const std::wstring& path, uint64_t filesize ) : BufferReadHandle( filesize ) {
			File f( path.c_str(), L"rb" );
			f.read(4); //Already known //TODO: avoid alloc?
			lz4::decompress( f.readAll().constView(), data.view() );
		}
};

std::unique_ptr<FileHandle> Lz4File::openRead() const
	{ return std::make_unique<Lz4FileHandle>( filepath, filesize() ); }

bool Lz4File::compressFile( std::wstring filepath, ConstByteView data ){
	File f( filepath.c_str(), L"wb" );
	f.write32u( data.size() );
	
	Buffer out( lz4::maxSize( data.size() ) );
	auto compressed = lz4::compress( data, out.view() );
	f.write( compressed );
	
	return compressed.size() > 0;
}