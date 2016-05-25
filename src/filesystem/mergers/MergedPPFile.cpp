/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "MergedPPFile.hpp"

void MergedPPFile::addPP( const std::vector<PPSubFile>& subfiles ){
	for( auto& subfile : subfiles )
		files.emplace_back( subfile ); //TODO: check for collisions
	//TODO: update offsets
}

uint64_t MergedPPFile::filesize() const{
	//TODO: calculate header size
	auto header_size = 0u;
	return header_size + std::accumulate( files.begin(), files.end(), 0u
		,	[](uint64_t acc, const PPSubFileReference& file)
				{ return acc + file.parent.file->filesize(); } );
}

class PPFileHandle : public FileHandle{
	private:
		const MergedPPFile& pp;
		
	public:
		PPFileHandle( const MergedPPFile& pp )
			:	pp( pp ) { }
		
		uint64_t read( ByteView to_read, uint64_t offset ) override{
			//TODO:
			return 0;
		}

		uint64_t write( ConstByteView to_write, uint64_t offset ) override{
			//TODO: throw exception
			return 0;
		}
};

std::unique_ptr<FileHandle> MergedPPFile::openRead() const
	{ return std::make_unique<PPFileHandle>( *this ); }

std::unique_ptr<AMergingObject> MergedPPFile::createMerger() const
	{ return std::make_unique<MergedPPFile>( *this ); }
