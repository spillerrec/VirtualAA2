/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef COMPRESSED_FILE_HPP
#define COMPRESSED_FILE_HPP

#include "FileObject.hpp"

#include <string>

class Buffer;

class CompressedFile : public FileObject{
	protected:
		std::wstring filepath;
		WStringView filename;
		uint64_t file_size;
		
		Buffer compressedData() const;
		static bool writeCompressed( const wchar_t* path, uint64_t size, ByteView data );
		
	public:
		CompressedFile( std::wstring filepath, const wchar_t* prefix );
		
		WStringView name()  const override{ return filename; }
		bool isDir()        const override{ return false; }
		bool canWrite()     const override{ return false; }
		uint64_t filesize() const override{ return file_size; }
		
		std::unique_ptr<AMergingObject> createMerger() const override;
};

#endif
