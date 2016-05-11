/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "VirtualDataDir.hpp"

#include "FileSystem.hpp"
#include "FilePath.hpp"
#include "PassthroughDir.hpp"
#include "../utils/debug.hpp"
#include "../encoders/PPCompactor.hpp"

#include <algorithm>
#include <cwctype>

using namespace std;


VirtualDataDir::VirtualDataDir( std::wstring filepath )
	:	basedir( std::make_unique<PassthroughDir>( filepath ) )
{
	auto imports = getFolderContents( filepath + L"\\import" );
	WStringView pp_ext( L".pp", 3 );
	for( auto import : imports ){
		if( !import.is_dir && makeView(import.name).endsWith( pp_ext ) )
			PPCompactor::importPP( filepath + L"\\import\\" + import.name );
	}
}

static bool compareInsensitive( WStringView a, WStringView b ){
	if( a.size() != b.size() )
		return false;
	return equal( a.begin(), a.end(), b.begin()
		,	[]( auto a, auto b ){ return towlower(a) == towlower(b); }
		);
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
			if( compareInsensitive( (*current)[j].name(), currentpath ) ){
				found = &(*current)[j];
				break;
			}
		
		if( !found )
			return nullptr;
		current = found;
	}
	
	return current;
}
