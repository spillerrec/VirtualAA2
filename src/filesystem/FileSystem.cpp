/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "FileSystem.hpp"

#include <Windows.h>

#include <iostream>

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
