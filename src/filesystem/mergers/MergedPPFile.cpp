/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "MergedPPFile.hpp"

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
		{ return std::max( int64_t(offset - position), int64_t(0) ); }
		
	OffsetView leftAt( uint64_t position )
		{ return { view.left( splitPos(position) ), offset }; }
	
	OffsetView rightAt( uint64_t position )
		{ return { view.right( view.size() - splitPos(position) ), offset }; }
};

class PPFileHandle : public FileHandle{
	private:
		const MergedPPFile& pp;
		
		uint64_t readHeaderHeader( OffsetView view ){
			//TODO: magic 8, version 4, unknown 1
			return 0;
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
			OffsetView view( to_read, offset );
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
