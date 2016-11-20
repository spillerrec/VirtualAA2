/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef PASSTHROUGH_DIR_HPP
#define PASSTHROUGH_DIR_HPP

#include "FileObject.hpp"
#include "TypeIDs.hpp"

#include <string>
#include <vector>
#include <memory>

class PassthroughDir : public FileObjectWithChildren{
	private:
		std::wstring filepath;
		WStringView filename;
		
	public:
		PassthroughDir( std::wstring filepath );
		
		WStringView name() const override{ return filename; }
		
		bool isDir() const override{ return true; }
		bool canWrite() const override{ return false; }
		uint64_t filesize() const override{ return 0; }
		
		FileObjectId type() const override{ return ID::PASS_DIR; }
		std::unique_ptr<AMergingObject> createMerger() const override;
};

#endif
