/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef VIRTUAL_DATA_DIR_HPP
#define VIRTUAL_DATA_DIR_HPP

#include "FileObject.hpp"

#include <string>
#include <memory>
#include <vector>

class FilePath;

struct FolderImport{
	std::wstring name;
	std::unique_ptr<FileObject> folder;
	FolderImport( std::wstring name, std::unique_ptr<FileObject> folder )
		:	name(std::move(name)), folder(std::move(folder)) { }
};

class Mod{
	public:
		std::wstring name;
		std::vector<FolderImport> folders;
};

class VirtualDataDir{
	private:
		std::vector<FolderImport> folders; //The folders which can be emulated
		std::vector<Mod> mods; //The available mods
		
		FileObject* getFolder( const std::wstring& name );
		
	public:
		VirtualDataDir( std::wstring filepath );
		
		const FileObject* getFromPath( FilePath path );
};

#endif
