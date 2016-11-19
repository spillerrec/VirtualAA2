/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "PPFile.hpp"
#include "FileSystem.hpp"
#include "FilePath.hpp"
#include "mergers/MergedPPFile.hpp"
#include "mergers/PassthroughMerger.hpp"
#include "../utils/debug.hpp"
#include "../utils/File.hpp"


PPFile::PPFile( std::wstring filepath )
	:	filepath(filepath)
	,	header( this->filepath.c_str() ) //Read header
{
	FilePath path( this->filepath.c_str() );
	filename = path.filename();
	
	
	//TODO: Read header header
	
	
	//TODO: Read entire header into buffer
	
	//TODO: parse header and construct 'files'
	objects.reserve( header.count() );
	files.reserve( header.count() );
	for( uint32_t i=0; i<header.count(); i++ ){
		objects.emplace_back( *this, i );
		
		auto file = header[i];
		PPSubFile sub;
		sub.filename = file.filename();
		sub.metadata = file.metadata();
		sub.file = &objects.back();
		files.push_back( sub );
	}
	//TODO: create file objects and set them in files
}

PP::Header::SubFile PPFile::get( unsigned index ) const{ return header[index]; }

WStringView PPFile::Reader::name() const{
	return {}; //TODO: Check if valid
}

uint64_t PPFile::Reader::filesize() const{
	return parent->get( index ).size();
}

class FileOffsetHandle : public FileHandle{
	private:
		File file; //TODO: Share handle somehow?
		uint64_t base_offset{ 0 };
		uint64_t current_pos{ 0 };
		
	public:
		FileOffsetHandle( const std::wstring& str, uint64_t offset )
			: file(str.c_str(), L"rb"), base_offset(offset) { }
		
		uint64_t read( ByteView to_read,  uint64_t offset ) override{
			if( current_pos != offset )
				file.seek( offset, base_offset );
			current_pos += file.read( to_read );
			return current_pos - offset;
		}
		uint64_t write( ConstByteView to_write, uint64_t offset ) override{ return 0; }
};

std::unique_ptr<FileHandle> PPFile::Reader::openRead() const{
	return std::make_unique<FileOffsetHandle>( parent->filepath, parent->get(index).offset() );
}

std::unique_ptr<AMergingObject> PPFile::Reader::createMerger() const
	{ return std::make_unique<PassthroughMerger>( *this ); }

std::unique_ptr<AMergingObject> PPFile::createMerger() const {
	return std::make_unique<MergedPPFile>( filename.toBasicString(), files );
}

