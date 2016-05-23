/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef VIRTUAL_DATA_DIR_HPP
#define VIRTUAL_DATA_DIR_HPP

#include "PassthroughDir.hpp"
#include "mergers/FakeDir.hpp"

#include <string>
#include <vector>

class FilePath;

class Mod{
	public:
		std::wstring name;
		PassthroughDir root;
		
		Mod( std::wstring name, std::wstring path ) : name(name), root( path ) { }
		Mod( std::wstring path );
};

class VirtualDataDir{
	private:
		FakeDir root;
		std::vector<Mod> mods; //The available mods
		
		const FileObject* getFolder( const std::wstring& name );
		
	public:
		VirtualDataDir( std::wstring filepath );
		
		const FileObject* getFromPath( FilePath path );
};

#endif
