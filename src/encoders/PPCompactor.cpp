/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "PPCompactor.hpp"

#include "../filesystem/FilePath.hpp"
#include "../filesystem/FileSystem.hpp"

#include <iostream>

using namespace std;

void PPCompactor::importPP( std::wstring path ){
	FilePath file_path( path.c_str() );
	makeFolder( file_path.folderPath() + L"\\result", file_path.filename().toBasicString() );
	
	
}
