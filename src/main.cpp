/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "filesystem-win.hpp"
#include "filesystem/FilePath.hpp"
#include "filesystem/VirtualDataDir.hpp"
#include "utils/File.hpp"
#include "utils/StringView.hpp"

#include <iostream>

static int error( const char* const msg, int code = -1 ){
	std::cerr << msg << std::endl;
	return code;
}

static int copy_file( const wchar_t* data_dir_path, const wchar_t* filepath, const wchar_t* to ){
	VirtualDataDir dir( data_dir_path );
	auto file = dir.getFromPath( { filepath } );
	if( !file )
		return error( "File not found" );
	
	auto handle = file->openRead();
	
	File out( to, L"wb" );
	
	const auto buffer_size = 256*4;
	Buffer read_buffer( buffer_size );
	
	uint64_t bytes_read = 0;
	uint64_t offset = 0;
	std::cout << "Starting to read\n";
	while( (bytes_read = handle->read( read_buffer.view(), offset )) > 0 ){
	//	std::cout << "read: " << bytes_read << std::endl;
		out.write( read_buffer.view().left( bytes_read ) );
		offset += bytes_read;
	}
	
	return 0;
}

int wmain( int argc, wchar_t* argv[] ){
	if( argc < 2 )
		return error( "VirtualAA2 <path-to-aa2-data-dir> [mount-point]\n" );
	
	auto param1 = makeView( argv[1] );
	if( param1.startsWith( makeView( L"--" ) ) ){
		if( param1 == makeView( L"--copy" ) ){
			if( argc == 5 )
				return copy_file( argv[2], argv[3], argv[4] );
			else
				return error( "VirtualAA2 --copy <path-to-aa2-data-dir> <file-to-copy> <output-path>" );
		}
		else
			return error( "Unknown parameter" );
	}
	else{
		auto data_dir_path = argv[1];
		auto mount_point = argc < 3 ? L"v" : argv[2];
		
		return startFilesystem( data_dir_path, mount_point );
	}
}


