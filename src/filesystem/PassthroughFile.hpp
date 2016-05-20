/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef PASSTHROUGH_FILE_HPP
#define PASSTHROUGH_FILE_HPP

#include "FileObject.hpp"

#include <string>

class PassthroughFile : public FileObject{
	private:
		std::wstring filepath;
		WStringView filename;
		
	public:
		PassthroughFile( std::wstring filepath );
		
		WStringView name() const override{ return filename; }
		bool isDir() const override{ return false; }
		bool canWrite() const override{ return true; }
		uint64_t filesize() const override;
		
		std::unique_ptr<FileHandle> openRead()   const override;
		std::unique_ptr<FileHandle> openWrite()  const override;
		std::unique_ptr<FileHandle> openAppend() const override;
		
		std::unique_ptr<AMergingObject> copy() const override;
		
		FileObjectId type() const override{ return 1; }
};

#endif
