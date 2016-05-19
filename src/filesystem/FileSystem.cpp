/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "FileSystem.hpp"

#include <Windows.h>

#include <iostream>
#include <memory>

using namespace std;


static uint64_t convertFileSize( DWORD high, DWORD low ){
	ULARGE_INTEGER large;
	large.HighPart = high;
	large.LowPart  = low;
	return large.QuadPart;
}

static FolderContent fromFindData( const WIN32_FIND_DATA& data ){
	FolderContent file;
	
	file.is_dir = data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
	file.name = wstring{ data.cFileName };
	file.filesize = convertFileSize( data.nFileSizeHigh, data.nFileSizeLow );
	
	//TODO:
	file.creation_time = 0;
	file.access_time   = 0;
	file.modified_time = 0;
	
	return file;
}

std::vector<FolderContent> getFolderContents( std::wstring path ){
	WIN32_FIND_DATA find_data;
	auto handle = FindFirstFile( (path + L"\\*").c_str(), &find_data );
	
	std::vector<FolderContent> children;
	if( handle != INVALID_HANDLE_VALUE ){
		do{
			FolderContent file = fromFindData( find_data );
			if( file.name != L"." && file.name != L".." )
				children.emplace_back( file );
		} while( FindNextFile( handle, &find_data ) );
		
		FindClose( handle );
	}
	
	return children;
}

bool makeFolder( std::wstring path, std::wstring name ){
	wcout << "Creating dir: " << name << " in " << path << "\n";
	return CreateDirectory( (path + L"\\" + name).c_str(), nullptr ) != 0;
}

std::wstring fromJapPath( const char* str, size_t lenght ){
	//TODO: Slow as hell, fix...
	setlocale( LC_ALL, "ja-JP" ); //TODO: "(except isdigit, isxdigit, mbstowcs, and mbtowc, which are unaffected)." ???
	// https://msdn.microsoft.com/en-us/library/x99tb11d.aspx
	
	//mingw have not implemented _create_locale / _free_locale
	//auto locale = _create_locale( LC_CTYPE, "ja-JP" );
	
	auto buf_size = lenght*2; //Should be excessive
	auto buf = std::make_unique<wchar_t[]>( buf_size );
	
	size_t char_converted = 0;
	mbstowcs_s( &char_converted, buf.get(), buf_size, str, buf_size-1 );
	//_mbstowcs_s_l( &char_converted, buf.get(), buf_size, str, buf_size-1, locale );
	
	//_free_locale( locale );
	
	return { buf.get(), char_converted-1 }; //NOTE: Remove terminating zero
}

std::wstring fromJapPath( ConstByteView view ) {
	auto str = view.toBasicString();
	return fromJapPath( reinterpret_cast<const char*>(str.c_str()), str.size() );
}
	
std::wstring fromJapPath( ByteView view ) {
	auto str = view.toBasicString();
	return fromJapPath( reinterpret_cast<const char*>(str.c_str()), str.size() );
}
