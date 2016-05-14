/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef PASSTHROUGH_DIR_HPP
#define PASSTHROUGH_DIR_HPP

#include "FileObject.hpp"

#include <string>
#include <vector>
#include <memory>

class PassthroughDir : public FileObject{
	private:
		std::wstring filepath;
		WStringView filename;
		
		std::vector<std::unique_ptr<FileObject>> objects;
		std::vector<const FileObject*> object_refs;
		bool contains( WStringView name );
		
	public:
		PassthroughDir( std::wstring filepath );
		
		WStringView name() const override{ return filename; }
		bool isDir() const override{ return true; }
		bool canWrite() const override{ return false; }
		uint64_t filesize() const override{ return 0; }
		
		uint64_t children() const override{ return objects.size() + object_refs.size(); }
		const FileObject& operator[]( int index ) const override;
		
		void combine( FileObject& with ) override;
		
		FileObjectId type() const override{ return 2; }
};

#endif
