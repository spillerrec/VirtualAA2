/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef VIRTUAL_DATA_DIR_HPP
#define VIRTUAL_DATA_DIR_HPP

#include "FileObject.hpp"

#include <string>
#include <memory>

class FilePath;

class VirtualDataDir{
	private:
		std::unique_ptr<FileObject> basedir;
		
	public:
		VirtualDataDir( std::wstring filepath );
		
		const FileObject* getFromPath( FilePath path );
};

#endif
