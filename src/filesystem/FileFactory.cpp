/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "FileFactory.hpp"
#include "FileSystem.hpp"
#include "FilePath.hpp"
#include "Lz4File.hpp"
#include "PassthroughDir.hpp"
#include "PassthroughFile.hpp"
#include "PPFile.hpp"

using namespace std;

std::unique_ptr<FileObject> FileFactory::makeFileObject( wstring parent, FolderContent info ){
	auto path = parent + L"\\" + info.name;
	auto filename = FilePath( makeView( path ) ).filename();
	
	WStringView pp_prefix( L"[PP] ", 5 );
	if( filename.startsWith(pp_prefix) )
		return make_unique<PPFile>( path );
	
	WStringView lz4_prefix( L"[LZ4] ", 6 );
	if( filename.startsWith(lz4_prefix) )
		return make_unique<Lz4File>( path );
	
	if( info.is_dir )
		return make_unique<PassthroughDir>( path );
	else
		return make_unique<PassthroughFile>( parent, info );
}