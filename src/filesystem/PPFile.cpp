/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "PPFile.hpp"
#include "FileSystem.hpp"
#include "FilePath.hpp"
#include "mergers/PassthroughMerger.hpp"
#include "../utils/ByteViewReader.hpp"
#include "../utils/debug.hpp"
#include "../utils/File.hpp"

#include <algorithm>
#include <stdexcept>
#include <iostream>

PPFile::PPFile( std::wstring filepath ) : filepath(filepath) {
	FilePath path( this->filepath.c_str() );
	auto folder = path.filename();
	//require( path.path.size() > 0 );
	filename = folder.right(folder.size()-5);
	
	//Read header file
	auto header_data = File( (filepath + L"\\__HEADER").c_str(), L"rb" ).readAll();
	ByteViewReader reader( header_data );
	
	//Parse __HEADER contents
	//NOTE: We should change the format so it contains the amount of subfiles
	while( reader.left() > 0 ){
		PPSubFile subfile;
		subfile.filename = Buffer( reader.readName() );
		subfile.metadata = Buffer( reader.read( 20 ) );
		
		files.emplace_back( std::move(subfile) );
	}
	
	//Lookup all the file sizes
	auto folder_files = getFolderContents( filepath );
	std::wcerr << "Found " << folder_files.size() << " files in " << filepath << "\n";
	auto folder_compare = []( auto& a, auto& b ){ return a.name < b.name; };
	std::sort( folder_files.begin(), folder_files.end(), folder_compare );
	for( auto& file : files ){
		//Convert file name
		FolderContent wanted;
		wanted.name = fromJapPath( file.filename.view() );
		file.filepath = wanted.path( filepath );
		
		//Lookup in folder_files
		auto it = std::lower_bound( folder_files.begin(), folder_files.end(), wanted, folder_compare );
		if( it->name == wanted.name ){
			file.filesize = it->filesize;
		}
		else{
			std::cerr << "Could not find file: \"" << file.filename.view().toBasicString().c_str() << "\"\n";
			std::wcerr << "Could not find file: \"" << wanted.name << "\"\n";
			std::wcerr << "Lower bound is: \"" << it->name << "\"\n";
			throw std::runtime_error( "Could not find file!" );
		}
	}
	
	//TODO: calculate offsets
}

PPFile::PPFile( const PPFile& other ) : filepath( other.filepath ), files( other.files ) {
	filename = FilePath( filepath.c_str() ).filename();
}

std::unique_ptr<AMergingObject> PPFile::copy() const
	{ return std::make_unique<PassthroughMerger>( *this ); } //TODO:

	
uint64_t PPFile::filesize() const{
	//TODO: calculate header size
	auto header_size = 0u;
	return header_size + std::accumulate( files.begin(), files.end(), 0u
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

