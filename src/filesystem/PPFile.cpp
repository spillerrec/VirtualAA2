/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "PPFile.hpp"
#include "FileSystem.hpp"
#include "FilePath.hpp"
#include "FileFactory.hpp"
#include "mergers/PassthroughMerger.hpp"
#include "../utils/ByteViewReader.hpp"
#include "../utils/debug.hpp"
#include "../utils/File.hpp"

#include <algorithm>
#include <stdexcept>
#include <iostream>

const FolderContent* shiftJisBinarySearch( const std::vector<FolderContent>& array, ByteView comp ){
	//See if we can get away with just comparing directly
	if( shiftJisOnlySingleBytes( comp ) ){
		auto it = std::lower_bound( array.begin(), array.end(), comp
			,	[](auto& folder, auto& comp){ return makeView( folder.name ).lexicographical_less( comp ); }
			);
		return ( makeView(it->name) == comp ) ? &*it : nullptr;
	}
	else{
		std::cout << "Cannot quick search: " << comp.toBasicString().c_str() << "\n";
		//Else convert it properly
		auto converted = fromJapPath( comp );
		auto it = std::lower_bound( array.begin(), array.end(), converted
			,	[](auto& folder, auto& comp){ return folder.name < comp; }
			);
		return ( it->name == converted ) ? &*it : nullptr;
	}
}

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
		subfile.filename = reader.readName();
		subfile.metadata = reader.read( 20 );
		
		files.emplace_back( std::move(subfile) );
	}
	
	//Lookup all the file sizes
	auto folder_files = getFolderContents( filepath );
	std::wcerr << "Found " << folder_files.size() << " files in " << filepath << "\n";
	auto folder_compare = []( auto& a, auto& b ){ return a.name < b.name; };
	std::sort( folder_files.begin(), folder_files.end(), folder_compare );
	for( auto& file : files ){
		//Lookup in folder_files
		auto folder = shiftJisBinarySearch( folder_files, file.filename );
		if( folder )
			file.file = FileFactory::makeFileObject( filepath, *folder );
		else{
			std::cerr << "Could not find file: \"" << file.filename.toBasicString().c_str() << "\"\n";
			throw std::runtime_error( "Could not find file!" );
		}
	}
}

std::unique_ptr<AMergingObject> PPFile::createMerger() const
	{ return std::make_unique<PassthroughMerger>( *this ); } //TODO:

	
uint64_t PPFile::filesize() const{
	//TODO: calculate header size
	auto header_size = 0u;
	return header_size + std::accumulate( files.begin(), files.end(), 0u
		,	[](uint64_t acc, const PPSubFile& file){ return acc + file.file->filesize(); } );
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

