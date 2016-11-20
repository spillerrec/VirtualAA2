/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef DEFLATE_FILE_HPP
#define DEFLATE_FILE_HPP

#include "CompressedFile.hpp"
#include "TypeIDs.hpp"

#include <string>

class DeflateFile : public CompressedFile{
	public:
		DeflateFile( std::wstring filepath ) : CompressedFile( filepath, L"[DEFLATE] " ) { }
		
		std::unique_ptr<FileHandle> openRead() const override;
		FileObjectId type() const override{ return ID::DEFLATE; }
		
		static bool compressFile( std::wstring filepath, ConstByteView data );
};

#endif
