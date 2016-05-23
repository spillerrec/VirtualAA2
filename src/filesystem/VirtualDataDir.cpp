/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "VirtualDataDir.hpp"

#include "FileSystem.hpp"
#include "FilePath.hpp"
#include "../utils/debug.hpp"

#include <iostream>

using namespace std;

Mod::Mod( std::wstring path )
	: name( FilePath( makeView( path ) ).filename().toBasicString() ), root( path ) { }

VirtualDataDir::VirtualDataDir( std::wstring filepath ) {
	auto package_dir = filepath + L"\\packages";
	auto mod_folders = getFolderContents( package_dir  );
	mods.reserve( mod_folders.size() );
	for( auto mod_folder : mod_folders )
		if( mod_folder.is_dir )
			mods.emplace_back( mod_folder.path( package_dir ) );
	
	for( auto& mod : mods ){
		//TODO: Support some form for overwrite rules
		root.combine( mod.root );
		wcout << "Loaded: " << mod.name << "\n";
	}
}

const FileObject* VirtualDataDir::getFolder( const std::wstring& name )
	{ return root.find( makeView( name ) ); }

const FileObject* VirtualDataDir::getFromPath( FilePath path ){
	require( path.hasRoot() );
	
	const FileObject* current = &root;
	for( unsigned i=1; i<path.path.size(); i++ ){
		current = current->find( path.path[i] );
		if( !current )
			return nullptr;
	}
	
	return current;
}
