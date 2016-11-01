/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef LZ4_FILE_HPP
#define LZ4_FILE_HPP

#include "CompressedFile.hpp"

#include <string>

class Lz4File : public CompressedFile{
	public:
		Lz4File( std::wstring filepath ) : CompressedFile( filepath, L"[LZ4] " ) { }
		
		std::unique_ptr<FileHandle> openRead() const override;
		FileObjectId type() const override{ return 20; }
		
		static bool compressFile( std::wstring filepath, ConstByteView data );
};

#endif
