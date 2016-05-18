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
#include <iostream>

using namespace std;

static Mod loadMod( std::wstring filepath ){
	Mod mod;
	mod.name = FilePath( makeView( filepath ) ).filename().toBasicString();
	
	//Load folders
	for( auto game_folder : getFolderContents( filepath ) )
		if( game_folder.is_dir ){
			auto file_object = std::make_unique<PassthroughDir>( filepath + L"\\" + game_folder.name ); //TODO: more intelligent
			mod.folders.addChild( std::move(file_object) );
		}
	
	return mod;
}

VirtualDataDir::VirtualDataDir( std::wstring filepath ) {
	auto imports = getFolderContents( filepath + L"\\import" );
	WStringView pp_ext( L".pp", 3 );
	for( auto import : imports ){
//		if( !import.is_dir && makeView(import.name).endsWith( pp_ext ) )
		//	PPCompactor::importPP( filepath + L"\\import\\" + import.name );
	}
	
	auto package_dir = filepath + L"\\packages";
	auto mod_folders = getFolderContents( package_dir  );
	mods.reserve( mod_folders.size() );
	for( auto mod_folder : mod_folders )
		if( mod_folder.is_dir )
			mods.emplace_back( loadMod( mod_folder.path( package_dir ) ) );
	
	for( auto& mod : mods ){
		wcout << "Mod: " << mod.name << "\n";
		root.combine( mod.folders );
	}
}

FileObject* VirtualDataDir::getFolder( const std::wstring& name )
	{ return root.find( makeView( name ) ); }

const FileObject* VirtualDataDir::getFromPath( FilePath path ){
	require( path.hasRoot() );
	if( path.isRoot() )
		return &root;
	
	const FileObject* current = &root;
	for( unsigned i=1; i<path.path.size(); i++ ){
		current = current->find( path.path[i] );
		if( !current )
			return nullptr;
	}
	
	return current;
}
