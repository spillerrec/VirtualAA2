/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef LZMA_FILE_HPP
#define LZMA_FILE_HPP

#include "FileObject.hpp"
#include "../utils/Buffer.hpp"

#include <string>

class LzmaFile : public FileObject{
	private:
		std::wstring filepath;
		WStringView filename;
		uint64_t file_size;
		
	public:
		LzmaFile( std::wstring filepath );
		
		WStringView name() const override{ return filename; }
		bool isDir()    const override{ return false; }
		bool canWrite() const override{ return false; }
		uint64_t filesize() const override{ return file_size; }
		
		std::unique_ptr<FileHandle> openRead() const override;
		
		std::unique_ptr<AMergingObject> createMerger() const override;
		
		FileObjectId type() const override{ return 21; }
		
		static bool compressFile( std::wstring filepath, ConstByteView data );
};

#endif
