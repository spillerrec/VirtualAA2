/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "PassthroughDir.hpp"
#include "mergers/FakeDir.hpp"

#include "FileSystem.hpp"
#include "FilePath.hpp"
#include "FileFactory.hpp"

using namespace std;

PassthroughDir::PassthroughDir( std::wstring filepath )
	:	 filepath(filepath) {
	filename = FilePath( makeView(this->filepath) ).filename();
	auto files = getFolderContents( filepath );
	reserve( files.size() );
	for( auto file : files )
		addChild( FileFactory::makeFileObject( filepath, file ) );
}

std::unique_ptr<AMergingObject> PassthroughDir::copy() const {
	auto dir = std::make_unique<FakeDir>( filename.toBasicString() );
	for( unsigned i=0; i<objects.size(); i++ )
		dir->addChild( objects[i]->copy() );
	return std::move( dir );
}
