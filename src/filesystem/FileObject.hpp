/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef FILE_OBJECT_HPP
#define FILE_OBJECT_HPP

#include "../utils/StringView.hpp"

#include <memory>

class FileHandle{
	public:
		virtual uint64_t read(       ByteView to_read,  uint64_t offset ) = 0;
		virtual uint64_t write( ConstByteView to_write, uint64_t offset ) = 0;
		virtual ~FileHandle() { }
};

struct Time{
	uint64_t unix{ 0 };
};

using FileObjectId = uint64_t;

class FileObject{
	public:
		virtual WStringView name() const = 0;
		virtual bool isDir() const = 0;
		virtual Time created()  const{ return {}; }
		virtual Time modified() const{ return {}; }
		virtual bool canWrite() const = 0;
		virtual uint64_t filesize() const{ return 0; }
		
		virtual std::unique_ptr<FileHandle> openRead()   const;
		virtual std::unique_ptr<FileHandle> openWrite()  const;
		virtual std::unique_ptr<FileHandle> openAppend() const;
		
		virtual FileObjectId type() const = 0;
		virtual uint64_t children() const{ return 0; }
		virtual const FileObject& operator[]( int index ) const;
		virtual       FileObject& operator[]( int index )      ; //Overwrite this if you need different behaviour from const version!
		
		virtual std::unique_ptr<FileObject> copy() const = 0;
		
		virtual void combine( const FileObject& with );
		
		      FileObject* find( WStringView child_name );
		const FileObject* find( WStringView child_name ) const;
		
		virtual ~FileObject() { }
};

#endif
