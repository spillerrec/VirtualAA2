/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef FILE_OBJECT_HPP
#define FILE_OBJECT_HPP

#include "../utils/StringView.hpp"

#include <stdexcept>
#include <cstdio>

using FileHandle = FILE*;

struct Time{
	uint64_t unix{ 0 };
};

class FileObject{
	public:
		virtual WStringView name() const = 0;
		virtual bool isDir() const = 0;
		virtual Time created()  const{ return {}; }
		virtual Time modified() const{ return {}; }
		virtual bool canWrite() const = 0;
		virtual uint64_t filesize() const{ return 0; }
		
		virtual FileHandle openRead() const = 0;
		virtual FileHandle openWrite() const = 0;
		virtual uint64_t read( FileHandle handle, uint8_t* buffer, uint64_t amount, uint64_t offset ) const = 0;
		virtual void close( FileHandle handle ) const = 0;
		
		virtual uint64_t children() const{ return 0; }
		virtual const FileObject& operator[]( int index ) const{ throw std::domain_error( "No children defined!" ); }
};

#endif
