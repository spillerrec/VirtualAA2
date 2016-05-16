/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "PPFile.hpp"
#include "FilePath.hpp"
#include "../utils/debug.hpp"

#include <algorithm>

PPFile::PPFile( std::wstring filepath ) : filepath(filepath) {
	FilePath path( this->filepath.c_str() );
	auto folder = path.filename();
	//require( path.path.size() > 0 );
	filename = folder.right(folder.size()-5);
	
	//TODO: initialize files
}

std::unique_ptr<FileObject> PPFile::copy() const
	{ return std::make_unique<PPFile>( filepath ); }

	
uint64_t PPFile::filesize() const{
	return std::accumulate( files.begin(), files.end(), 0u
		,	[](uint64_t acc, const PPSubFile& file){ return acc + file.filesize; } );
}

class PPFileHandle : public FileHandle{
	private:
		const PPFile& pp;
		
	public:
		PPFileHandle( const PPFile& pp )
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

std::unique_ptr<FileHandle> PPFile::openRead() const
	{ return std::make_unique<PPFileHandle>( *this ); }

