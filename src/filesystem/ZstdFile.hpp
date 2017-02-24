/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef ZSTD_FILE_HPP
#define ZSTD_FILE_HPP

#include "CompressedFile.hpp"
#include "TypeIDs.hpp"

#include <string>

class ZstdFile : public CompressedFile{
	public:
		ZstdFile( std::wstring filepath ) : CompressedFile( filepath, L"[ZSTD] " ) { }
		
		std::unique_ptr<FileHandle> openRead() const override;
		FileObjectId type() const override{ return ID::ZSTD; }
		
		static bool compressFile( std::wstring filepath, ConstByteView data );
};

#endif
