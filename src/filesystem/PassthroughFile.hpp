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
		
		WStringView name() const override;
		bool isDir() const override{ return false; }
		bool canWrite() const override{ return true; }
		uint64_t filesize() const override;
		
		FileHandle openRead() const override;
		FileHandle openWrite() const override;
		FileHandle openAppend() const override;
		uint64_t read(  FileHandle handle,       uint8_t* buffer, uint64_t amount, uint64_t offset ) const override;
		uint64_t write( FileHandle handle, const uint8_t* buffer, uint64_t amount, uint64_t offset ) const override;
		void close( FileHandle handle ) const override;
};

#endif
