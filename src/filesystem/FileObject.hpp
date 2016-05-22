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
		
		virtual std::unique_ptr<AMergingObject> createMerger() const = 0;
		
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
