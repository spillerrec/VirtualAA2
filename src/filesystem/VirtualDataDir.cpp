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

Mod::Mod( std::wstring name, std::wstring path ) : name(name), root( path ) {
	wcout << "Filepath is: " << path << "\n";
	
}

static Mod loadMod( std::wstring filepath ){
	return Mod( FilePath( makeView( filepath ) ).filename().toBasicString(), filepath );
}

VirtualDataDir::VirtualDataDir( std::wstring filepath ) {
	auto package_dir = filepath + L"\\packages";
	auto mod_folders = getFolderContents( package_dir  );
	mods.reserve( mod_folders.size() );
	for( auto mod_folder : mod_folders )
		if( mod_folder.is_dir )
			mods.emplace_back( loadMod( mod_folder.path( package_dir ) ) );
	
	for( auto& mod : mods ){
		wcout << "Mod: " << mod.name << "\n";
			root.combine( mod.root );
			/* TODO: only add directories
		for( unsigned i=0; i<mod.root.children(); i++ ){
			wcout << "combining: " << mod.root[i].name().toBasicString() << "\n";
			root.combine( mod.root[i] );
		}*/
	}
	cout << "root-children: " << root.children() << "\n";
}

const FileObject* VirtualDataDir::getFolder( const std::wstring& name )
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
