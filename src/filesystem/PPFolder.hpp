/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef PP_FOLDER_HPP
#define PP_FOLDER_HPP

#include "FileObject.hpp"
#include "../utils/Buffer.hpp"

#include <string>
#include <vector>

struct PPSubFile{
	ByteView filename;
	ByteView metadata;
	FileObject* file;
};

class PPFolder : public FileObject{
	private:
		std::wstring filepath;
		WStringView filename;
		
		Buffer header;
		std::vector<PPSubFile> files;
		std::vector<std::unique_ptr<FileObject>> objects;
		
	public:
		PPFolder( std::wstring filepath );
		
		WStringView name() const override{ return filename; }
		bool isDir() const override{ return false; }
		bool canWrite() const override{ return false; }
		
		std::unique_ptr<AMergingObject> createMerger() const override;
		
		FileObjectId type() const override{ return 4; }
};

#endif
