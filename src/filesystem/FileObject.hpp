/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef FILE_OBJECT_HPP
#define FILE_OBJECT_HPP

#include "../utils/StringView.hpp"

#include <memory>
#include <vector>

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

template<class T>
auto findChild( T& object, WStringView child_name ) -> decltype(&object[0]){
	for( unsigned j=0; j<object.children(); j++ )
		if( compareInsensitive( object[j].name(), child_name ) )
			return &object[j];
	return nullptr;
}

class AMergingObject;

/** Virtual file which may contain children */
class FileObject{
	public:
		/** Name of the file/dir as used in the path */
		virtual WStringView name() const = 0;
		
		/** @return true if this is a directory */
		virtual bool isDir() const = 0;
		
		/** @return the Time this file was created */
		virtual Time created()  const{ return {}; }
		
		/** @return the Time this file was last modified */
		virtual Time modified() const{ return {}; }
		
		/** @return true if it is possible to write to the file */
		virtual bool canWrite() const = 0;
		
		/** @return the amount of bytes which can be read from the file */
		virtual uint64_t filesize() const{ return 0; }
		
		virtual std::unique_ptr<FileHandle> openRead()   const;
		virtual std::unique_ptr<FileHandle> openWrite()  const;
		virtual std::unique_ptr<FileHandle> openAppend() const;
		
		virtual FileObjectId type() const = 0;
		
		/** @return the amount of children for this directory */
		virtual uint64_t children() const{ return 0; }
		
		/** Accessors to child FileObjects
		 *  @param index the index of the child to access
		 *  @return The FileObject */
		virtual const FileObject& operator[]( int index ) const;
		
		/** @return AMerginObject were this object was already added to it.
		 *  The lifetime of the returned object is required to be shorter than this */
		virtual std::unique_ptr<AMergingObject> createMerger() const = 0;
		
		/** Find a child object with the specified name
		 *  @param child_name A string with matching contents to the name() of the wanted object
		 *  @return Pointer to the object, or nullptr if none were found */
		const FileObject* find( WStringView child_name ) const{ return findChild( *this, child_name ); }
		
		virtual ~FileObject() { }
};

template<typename FileType>
class FileVectorObject : public FileType{
	protected:
		std::vector<std::unique_ptr<FileType>> objects;
		
	public:
		FileType& addChild( std::unique_ptr<FileType> child ){
			objects.emplace_back( std::move(child) );
			return *objects.back();
		}
		void reserve( size_t amount ){ objects.reserve( amount ); }
		
		uint64_t children() const override { return objects.size(); }
		const FileObject& operator[]( int index ) const override { return *objects[index]; }
};

using FileObjectWithChildren = FileVectorObject<FileObject>;

#endif
