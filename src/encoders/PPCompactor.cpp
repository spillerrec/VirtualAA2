/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "PPCompactor.hpp"

#include "../filesystem/FilePath.hpp"
#include "../filesystem/FileSystem.hpp"
#include "../decoders/PPArchive.hpp"
#include "../utils/File.hpp"

#include <iostream>

using namespace std;


void PPCompactor::importPP( std::wstring path ){
	FilePath file_path( path.c_str() );
	auto folder_name = file_path.folderPath();
	auto file_name = L"[PP] " + file_path.filename().toBasicString();
	makeFolder( folder_name, file_name );
	auto prefix = folder_name + L"\\" + file_name + L"\\";
	
	File file( path.c_str() );
	PPArchive pp( file );
	
	//Write header
	File fheader( (prefix + L"__HEADER").c_str(), L"wb" );
	for( auto& f : pp.files ){
		auto name = f.filename.view().left( f.filename.view().findIndex( '\0' ) );
		fheader.write32u( name.size() );
		fheader.write( name );
		fheader.write( f.metadata );
	}
	
	
	//Write sub-files
	for( auto& f : pp.files ){
		auto output = f.getFile( file );
		auto wide_name = fromJapPath( f.filename.view() );
		
		File fout( (prefix + wide_name).c_str(), L"wb" );
		fout.write( output.view() );
	}
}
