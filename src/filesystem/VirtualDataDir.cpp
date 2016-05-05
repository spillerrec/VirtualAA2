/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "VirtualDataDir.hpp"

#include "FilePath.hpp"
#include "PassthroughDir.hpp"
#include "../utils/debug.hpp"

VirtualDataDir::VirtualDataDir( std::wstring filepath )
	:	basedir( std::make_unique<PassthroughDir>( filepath ) )
{
	
}

const FileObject* VirtualDataDir::getFromPath( FilePath path ){
	require( path.hasRoot() );
	if( path.isRoot() )
		return basedir.get();
	
	const FileObject* current = basedir.get();
	for( unsigned i=1; i<path.path.size(); i++ ){
		auto currentpath = path.path[i];
		
		const FileObject* found = nullptr;
		for( unsigned j=0; j<current->children(); j++ )
			if( (*current)[j].name() == currentpath ){
				found = &(*current)[j];
				break;
			}
		
		if( !found )
			return nullptr;
		current = found;
	}
	
	return current;
}
