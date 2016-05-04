/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "PassthroughDir.hpp"

#include "PassthroughFile.hpp"
#include "FilePath.hpp"
#include "../utils/debug.hpp"

#include <Windows.h>

using namespace std;

static unique_ptr<FileObject> makePassthrough( wstring parent, WIN32_FIND_DATA info ){
	auto new_path = parent + L"\\" + wstring{ info.cFileName };
	if( info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		return make_unique<PassthroughDir>( new_path );
	else
		return make_unique<PassthroughFile>( new_path );
}

PassthroughDir::PassthroughDir( std::wstring filepath ) : filepath(filepath) {
	FilePath path( this->filepath.c_str() );
	require( path.path.size() > 0 );
	filename = path.path.back();
	
	WIN32_FIND_DATA find_data;
	auto handle = FindFirstFile( (filepath + L"\\*").c_str(), &find_data );
	
	if( handle != INVALID_HANDLE_VALUE ){
		do{
			if( wstring{ find_data.cFileName } != L"." && wstring{ find_data.cFileName } != L".." )
				objects.emplace_back( makePassthrough( filepath, find_data ) );
		} while( FindNextFile( handle, &find_data ) );
		
		FindClose( handle );
	}
}
