/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "PassthroughDir.hpp"

#include "FileSystem.hpp"
#include "PassthroughFile.hpp"
#include "FilePath.hpp"
#include "../utils/debug.hpp"

using namespace std;

static unique_ptr<FileObject> makePassthrough( wstring parent, FolderContent info ){
	auto new_path = parent + L"\\" + info.name;
	if( info.is_dir )
		return make_unique<PassthroughDir>( new_path );
	else
		return make_unique<PassthroughFile>( new_path );
}

PassthroughDir::PassthroughDir( std::wstring filepath )
	:	FakeDir( FilePath( filepath.c_str() ).filename().toBasicString() ), filepath(filepath) {
	
	auto files = getFolderContents( filepath );
	reserve( files.size() );
	for( auto file : files )
		addChild( makePassthrough( filepath, file ) );
}

std::unique_ptr<FileObject> PassthroughDir::copy() const
	{ return std::make_unique<PassthroughDir>( filepath ); }
