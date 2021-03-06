/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "FileFactory.hpp"
#include "FileSystem.hpp"
#include "FilePath.hpp"
#include "DeflateFile.hpp"
#include "Lz4File.hpp"
#include "LzmaFile.hpp"
#include "ZstdFile.hpp"
#include "PassthroughDir.hpp"
#include "PassthroughFile.hpp"
#include "PPFile.hpp"
#include "PPFolder.hpp"

#include <cstring>
#include <iostream>
#include <stdexcept>

using namespace std;

std::unique_ptr<FileObject> FileFactory::makeFileObject( wstring parent, FolderContent info ){
	auto path = parent + L"\\" + info.name;
	
	//Prefix checker
	auto filename = FilePath( makeView( path ) ).filename();
	auto check = [&]( auto prefix )
		{ return filename.startsWith( WStringView( prefix, wcslen(prefix) ) ); };
	
	try{
		//Check for custom handlers
		//TODO: different checks for files and folders?
		if( check( L"[PP] "      ) )  return make_unique<   PPFolder>( path );
		if( check( L"[LZ4] "     ) )  return make_unique<    Lz4File>( path );
		if( check( L"[LZMA] "    ) )  return make_unique<   LzmaFile>( path );
		if( check( L"[DEFLATE] " ) )  return make_unique<DeflateFile>( path );
		if( check( L"[ZSTD] "    ) )  return make_unique<   ZstdFile>( path );
		
		if( filename.size() > 3 )
		if( filename.right(3) == WStringView(L".pp", 3) ) return make_unique<  PPFile>( path );
	}
	catch( std::exception& exception ){
		//Fall back to Passthrough
		std::wcout << L"Warning, failed to initialize handler for: " << path.c_str() << L"\n";
		std::cout << "\t" << exception.what() << "\n";
	}
	
	//Passthrough
	if( info.is_dir )
		return make_unique<PassthroughDir>( path );
	else
		return make_unique<PassthroughFile>( parent, info );
}