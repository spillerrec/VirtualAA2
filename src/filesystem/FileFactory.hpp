/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef FILE_FACTORY_HPP
#define FILE_FACTORY_HPP

#include "FileObject.hpp"

#include <string>


class FileFactory{
	public:
		static std::unique_ptr<FileObject> makeFileObject( std::wstring parent, struct FolderContent info );
};

#endif
