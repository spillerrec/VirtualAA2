/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef FAKE_DIR_HPP
#define FAKE_DIR_HPP

#include "FileObject.hpp"

#include <string>
#include <vector>

class FakeDir : public FileObject{
	protected:
		std::wstring filename;
		std::vector<std::unique_ptr<FileObject>> objects;
		
	public:
		FakeDir() { }
		FakeDir( std::wstring filename ) : filename( std::move(filename) ) { }
		
		FileObject& addChild( std::unique_ptr<FileObject> child );
		void reserve( size_t amount ){ objects.reserve( amount ); }
		
		WStringView name() const override{ return makeView( filename ); }
		
		bool isDir() const override{ return true; }
		bool canWrite() const override{ return false; }
		uint64_t filesize() const override{ return 0; }
		
		uint64_t children() const override{ return objects.size(); }
		const FileObject& operator[]( int index ) const override;
		
		void combine( const FileObject& with ) override;
		
		std::unique_ptr<FileObject> copy() const override;
		
		FileObjectId type() const override{ return 3; }
};

#endif
