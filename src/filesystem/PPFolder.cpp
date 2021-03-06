/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "PPFolder.hpp"
#include "FileSystem.hpp"
#include "FilePath.hpp"
#include "FileFactory.hpp"
#include "mergers/MergedPPFile.hpp"
#include "../utils/ByteViewReader.hpp"
#include "../utils/debug.hpp"
#include "../utils/File.hpp"

#include <algorithm>
#include <stdexcept>
#include <iostream>

const FolderContent* shiftJisBinarySearch( const std::vector<FolderContent>& array, ConstByteView comp ){
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

PPFolder::PPFolder( std::wstring filepath ) : filepath(filepath) {
	FilePath path( this->filepath.c_str() );
	auto folder = path.filename();
	//require( path.path.size() > 0 );
	filename = folder.right(folder.size()-5);
	
	//Read header file
	header = File( (filepath + L"\\__HEADER").c_str(), L"rb" ).readAll();
	ByteViewReader reader( header );
	
	//Parse __HEADER contents
	//NOTE: We should change the format so it contains the amount of subfiles
	while( reader.left() > 0 ){
		PPSubFile subfile;
		subfile.filename = makeConst(reader.readName());
		subfile.metadata = makeConst(reader.read( 20 ));
		subfile.file = nullptr;
		
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
		if( folder ){
			objects.push_back( FileFactory::makeFileObject( filepath, *folder ) );
			file.file = objects.back().get();
		}
		else{
			std::cerr << "Could not find file: \"" << file.filename.toBasicString().c_str() << "\"\n";
			throw std::runtime_error( "Could not find file!" );
		}
	}
}

std::unique_ptr<AMergingObject> PPFolder::createMerger() const {
	return std::make_unique<MergedPPFile>( filename.toBasicString(), files );
}

