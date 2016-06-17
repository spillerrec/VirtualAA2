/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "MergedPPFile.hpp"
#include "../../decoders/PPArchive.hpp"

#include <iostream>

uint64_t MergedPPFile::headerSize() const{
	auto magic_size = 8 + 4 + 1; //magic, version, unknown1
	return magic_size + ( 260 + 4 + 4 + 20 ) * files.size(); // ( name, offset, size, meta )
}

void MergedPPFile::addPP( const std::vector<PPSubFile>& subfiles ){
	//Add new files
	for( auto& subfile : subfiles )
		files.emplace_back( subfile ); //TODO: check for collisions
	
	//Update offsets
	auto offset = headerSize();
	for( auto& file : files ){
		file.offset = offset;
		offset += file.filesize();
	}
}

uint64_t MergedPPFile::filesize() const{
	//Sum of all sub-files
	auto data_size = std::accumulate( files.begin(), files.end(), 0u
		,	[](uint64_t acc, const PPSubFileReference& file)
				{ return acc + file.filesize(); }
		);
	
	return headerSize() + data_size;
}

struct OffsetView{
	ByteView view;
	uint64_t offset;
	
	OffsetView( ByteView view, uint64_t offset ) : view(view), offset(offset) { }
	
	auto splitPos( uint64_t position ) const
		{ return uint64_t(std::max( int64_t(position) - int64_t(offset), int64_t(0) )); }
		
	OffsetView leftAt( uint64_t position )
		{ return { view.left( std::min( splitPos(position), view.size() ) ), offset }; }
	
	OffsetView rightAt( uint64_t position )
		{ return { view.right( std::min(view.size() - splitPos(position), view.size()) ), offset }; }
	
	void copyFrom( ConstByteView data ){
		for( unsigned i=0; i<std::min(view.size(), data.size()); i++ )
			view[i] = data[i];
	}
	
	OffsetView debug( const char* name ) const{
		//std::cout << name << ", offset: " << offset << " with size: " << view.size() << "\n";
		return *this;
	}
};

class PPFileHandle : public FileHandle{
	private:
		const MergedPPFile& pp;
		
		uint64_t readHeaderHeader( OffsetView view ){
			view.debug( "header-header" );
			auto magic = view.leftAt( PPArchive::magic_length );
			magic.copyFrom( { PPArchive::magic, PPArchive::magic_length } );
			//TODO: magic 8, version 4, unknown 1
			return magic.view.size();
		}
		uint64_t readHeaderFile( OffsetView view ){
			//TODO:
			return 0;
		}
		uint64_t readHeaderFiles( OffsetView view ){
			//TODO: read files
			//TODO: encrypt
			return 0;
		}
		
		uint64_t readHeader( OffsetView view ){
			view.debug( "header" );
			auto magic_size = 8 + 4 + 1;
			
			return readHeaderHeader( view.leftAt( magic_size ) )
			   +   readHeaderFiles( view.rightAt( magic_size ) )
			   ;
		}
		
		
		uint64_t readFile( OffsetView view ){
			//TODO:
			return 0;
		}
		
		uint64_t readData( OffsetView view ){
			//TODO:
			return 0;
		}
		
	public:
		PPFileHandle( const MergedPPFile& pp )
			:	pp( pp ) { }
		
		uint64_t read( ByteView to_read, uint64_t offset ) override{
			std::cout << "read( " << to_read.size() << ", " << offset << " )\n";
			OffsetView view( to_read, offset );
			view.debug( "read" );
			auto header_size = pp.headerSize();
			return readHeader( view.leftAt( header_size ) )
			   +   readData( view.rightAt( header_size ) )
			   ;
		}

		uint64_t write( ConstByteView to_write, uint64_t offset ) override
			{ throw std::logic_error( "Trying to write on a no-write PPFileHandle" ); }
};

std::unique_ptr<FileHandle> MergedPPFile::openRead() const
	{ return std::make_unique<PPFileHandle>( *this ); }

std::unique_ptr<AMergingObject> MergedPPFile::createMerger() const
	{ return std::make_unique<MergedPPFile>( *this ); }
