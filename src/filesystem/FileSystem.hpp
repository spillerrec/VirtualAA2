/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef FILE_SYSTEM_HPP
#define FILE_SYSTEM_HPP

#include <string>
#include <vector>

struct FolderContent{
	bool is_dir;
	std::wstring name;
	uint64_t filesize;
	uint64_t creation_time;
	uint64_t access_time;
	uint64_t modified_time;
};
std::vector<FolderContent> getFolderContents( std::wstring path );
bool makeFolder( std::wstring path, std::wstring name );

#endif
