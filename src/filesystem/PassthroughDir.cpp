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

PassthroughDir::PassthroughDir( std::wstring filepath ) : filepath(filepath) {
	FilePath path( this->filepath.c_str() );
	require( path.path.size() > 0 );
	filename = path.filename();
	
	auto files = getFolderContents( filepath );
	objects.reserve( files.size() );
	for( auto file : files )
		objects.emplace_back( makePassthrough( filepath, file ) );
}

const FileObject& PassthroughDir::operator[]( int index ) const{
	if( index < objects.size() )
		return *objects[index];
	else
		return *(object_refs[ index-objects.size() ]);
}

bool PassthroughDir::contains( WStringView name ){
	for( unsigned i=0; i<children(); i++ )
		if( (*this)[i].name() == name )
			return true;
	return false;
}

void PassthroughDir::combine( FileObject& with ){
	for( unsigned i=0; i<with.children(); i++ ){
		if( contains( with[i].name() ) )
			throw std::runtime_error( "File already exists!" );
		else
			object_refs.push_back( &with[i] );
	}
}
