/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef PASSTHROUGH_FILE_HPP
#define PASSTHROUGH_FILE_HPP

#include "FileObject.hpp"
#include "FileSystem.hpp"

#include <string>

class PassthroughFile : public FileObject{
	private:
		FolderContent fileinfo;
		std::wstring filepath;
		WStringView filename;
		
	public:
		PassthroughFile( const std::wstring& parent, FolderContent fileinfo );
		
		WStringView name() const override{ return filename; }
		bool isDir() const override{ return false; }
		bool canWrite() const override{ return true; }
		uint64_t filesize() const override{ return fileinfo.filesize; }
		
		Time created()  const override { return {fileinfo.creation_time}; }
		Time modified() const override { return {fileinfo.modified_time}; }
		
		std::unique_ptr<FileHandle> openRead()   const override;
		std::unique_ptr<FileHandle> openWrite()  const override;
		std::unique_ptr<FileHandle> openAppend() const override;
		
		std::unique_ptr<AMergingObject> createMerger() const override;
		
		FileObjectId type() const override{ return 1; }
};

#endif
