/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef FILE_SYSTEM_HPP
#define FILE_SYSTEM_HPP

#include "../utils/ArrayView.hpp"

#include <string>
#include <vector>

struct FolderContent{
	bool is_dir; ///True if this is a directory
	std::wstring name; ///The name of this file/dir
	uint64_t filesize; ///The filesize, likely only set if !is_dir
	uint64_t creation_time; ///unix timestamp of file creation
	uint64_t access_time; ///unix timestamp of last read access
	uint64_t modified_time; ///unix timestamp of last write access
	
	/** @return The entire file path of this object
	 *  @param parent The file path of the parent directory without an ending "\\" */
	std::wstring path( const std::wstring& parent )
		{ return parent + L"\\" + name; }
};
std::vector<FolderContent> getFolderContents( std::wstring path );
bool makeFolder( std::wstring path, std::wstring name );


std::wstring fromJapPath( const char* str, size_t lenght );

inline std::wstring fromJapPath( const std::string& str )
	{ return fromJapPath( str.c_str(), str.size() ); }
	
std::wstring fromJapPath( ConstByteView view );
std::wstring fromJapPath( ByteView view );


#endif
