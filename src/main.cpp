/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "filesystem-win.hpp"

#include <iostream>

int wmain( int argc, wchar_t* argv[] ){
	if( argc < 2 ){
		std::cout << "VirtualAA2 <path-to-aa2-data-dir> [mount-point]\n";
		return -1;
	}
	auto data_dir_path = argv[1];
	auto mount_point = argc < 3 ? L"v" : argv[2];
	
	return startFilesystem( data_dir_path, mount_point );
}


