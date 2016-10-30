/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "Lz4File.hpp"
#include "FilePath.hpp"
#include "mergers/PassthroughMerger.hpp"
#include "../utils/ByteViewReader.hpp"
#include "../utils/debug.hpp"
#include "../utils/File.hpp"

#include <algorithm>
#include <stdexcept>
#include <iostream>

#include <lz4.h>
#include <lz4hc.h>

Lz4File::Lz4File( std::wstring filepath ) : filepath(filepath) {
	FilePath path( filepath.c_str() );
	require( path.path.size() > 0 );
	filename = path.path.back();
	
	// remove "[LZ4] " prefix
	//TODO: assert that it exists
	filename = filename.right( filename.size()-6 );
	
	//Get resulting filesize
	File f( filepath.c_str(), L"rb" );
	auto buf_size = f.read(4);
	file_size = ByteViewReader( buf_size ).read32u();
}

std::unique_ptr<AMergingObject> Lz4File::createMerger() const
	{ return std::make_unique<PassthroughMerger>( *this ); }

class BufferHandle : public FileHandle{
	protected:
		Buffer data;
		ByteViewReader reader;
		
		void init( Buffer data ){
			this->data = std::move(data);
			reader = ByteViewReader( this->data );
		}
		BufferHandle() : reader(data) {}
	public:
		uint64_t read( ByteView to_read, uint64_t offset ) override{
			reader.seek( offset );
			auto data = reader.read( std::min(to_read.size(), reader.left()) );
			data.copyTo( to_read );
			return reader.tell() - offset;
		}

		uint64_t write( ConstByteView to_write, uint64_t offset ) override{
			//TODO: write
			reader.seek( offset + to_write.size() );
			return 0;
		}
};

class Lz4FileHandle : public BufferHandle{
	public:
		Lz4FileHandle( const std::wstring& path, uint64_t filesize ) {
			File f( path.c_str(), L"rb" );
			f.read(4); //Already known
			auto compressed = f.readAll();
			
			//TODO: decode
			Buffer out( filesize );
			LZ4_decompress_safe( (char*)compressed.begin(), (char*)out.begin(), compressed.size(), out.size() );
			
			init( std::move(out) );
		}
};

std::unique_ptr<FileHandle> Lz4File::openRead() const
	{ return std::make_unique<Lz4FileHandle>( filepath, filesize() ); }

bool Lz4File::compressFile( std::wstring filepath, ConstByteView data ){
	File f( filepath.c_str(), L"wb" );
	f.write32u( data.size() );
	
	Buffer out( LZ4_compressBound( data.size() ) );
	auto result = LZ4_compress_HC( (const char*)data.begin(), (char*)out.begin(), data.size(), out.size(), LZ4HC_DEFAULT_CLEVEL );
	if( result < 0 )
		return false;
		
	f.write( out.view().left( result ) );
	return true;
}