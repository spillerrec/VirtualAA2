/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "PPCompactor.hpp"

#include "../filesystem/FilePath.hpp"
#include "../filesystem/FileSystem.hpp"
#include "../decoders/PPArchive.hpp"
#include "../utils/File.hpp"

#include <iostream>

using namespace std;


std::wstring toWString( const std::string& str ){
	setlocale( LC_ALL, "ja-JP" ); //TODO: "(except isdigit, isxdigit, mbstowcs, and mbtowc, which are unaffected)." ???
	// https://msdn.microsoft.com/en-us/library/x99tb11d.aspx
	
	//mingw have not implemented _create_locale / _free_locale
	//auto locale = _create_locale( LC_CTYPE, "ja-JP" );
	
	auto buf_size = str.size()*2; //Should be excessive
	auto buf = std::make_unique<wchar_t[]>( buf_size );
	
	size_t char_converted = 0;
	mbstowcs_s( &char_converted, buf.get(), buf_size, str.c_str(), buf_size-1 );
	//_mbstowcs_s_l( &char_converted, buf.get(), buf_size, str.c_str(), buf_size-1, locale );
	
	//_free_locale( locale );
	
	return { buf.get(), char_converted };
}


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
		fheader.write( f.filename );
		fheader.write( f.metadata );
	}
	
	
	//Write sub-files
	for( auto& f : pp.files ){
		auto output = f.getFile( file );
		auto wide_name = toWString( string( (char*)f.filename.data() ) );
		
		auto fout = _wfopen( (prefix + wide_name).c_str(), L"wb" );
		fwrite( output.data(), 1, output.size(), fout );
		fclose( fout );
	}
}
