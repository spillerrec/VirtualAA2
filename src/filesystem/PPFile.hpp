/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef PP_FILE_HPP
#define PP_FILE_HPP

#include "FileObject.hpp"

#include <string>
#include <vector>

struct PPSubFile{
	std::string filename;
	uint64_t offset;
	uint64_t filesize;
};

class PPFile : public FileObject{
	private:
		std::wstring filepath;
		WStringView filename;
		
		std::vector<PPSubFile> files;
		
	public:
		PPFile( std::wstring filepath );
		
		WStringView name() const override{ return filename; }
		bool isDir() const override{ return false; }
		bool canWrite() const override{ return false; }
		uint64_t filesize() const override;
		
		std::unique_ptr<FileHandle> openRead()   const override;
		
		std::unique_ptr<FileObject> copy() const override;
		
		FileObjectId type() const override{ return 4; }
};

#endif