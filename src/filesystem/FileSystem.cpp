/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "FileSystem.hpp"

#include <Windows.h>

#include <iostream>

using namespace std;

std::vector<std::wstring> getFolders( std::wstring path ){
	WIN32_FIND_DATA find_data;
	auto handle = FindFirstFile( (path + L"\\*").c_str(), &find_data );
	
	std::vector<std::wstring> children;
	if( handle != INVALID_HANDLE_VALUE ){
		do{
			if( wstring{ find_data.cFileName } != L"." && wstring{ find_data.cFileName } != L".." )
				children.emplace_back( find_data.cFileName );
		} while( FindNextFile( handle, &find_data ) );
		
		FindClose( handle );
	}
	
	return children;
}

bool makeFolder( std::wstring path, std::wstring name ){
	wcout << "Creating dir: " << name << " in " << path << "\n";
	return CreateDirectory( (path + L"\\" + name).c_str(), nullptr ) != 0;
}
