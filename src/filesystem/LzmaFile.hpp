/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef LZMA_FILE_HPP
#define LZMA_FILE_HPP

#include "CompressedFile.hpp"
#include "TypeIDs.hpp"

#include <string>

class LzmaFile : public CompressedFile{
	public:
		LzmaFile( std::wstring filepath ) : CompressedFile( filepath, L"[LZMA] " ) { }
		
		std::unique_ptr<FileHandle> openRead() const override;
		FileObjectId type() const override{ return ID::LZMA; }
		
		static bool compressFile( std::wstring filepath, ConstByteView data );
};

#endif
