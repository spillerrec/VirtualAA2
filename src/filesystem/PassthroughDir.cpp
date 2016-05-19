/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "PassthroughDir.hpp"

#include "FileSystem.hpp"
#include "FilePath.hpp"
#include "FileFactory.hpp"

using namespace std;

PassthroughDir::PassthroughDir( std::wstring filepath )
	:	FakeDir( FilePath( filepath.c_str() ).filename().toBasicString() ), filepath(filepath) {
	
	auto files = getFolderContents( filepath );
	reserve( files.size() );
	for( auto file : files )
		addChild( FileFactory::makeFileObject( filepath, file ) );
}

std::unique_ptr<FileObject> PassthroughDir::copy() const
	{ return std::make_unique<PassthroughDir>( filepath ); }
