/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef FAKE_DIR_HPP
#define FAKE_DIR_HPP

#include "AMergingObject.hpp"

#include <string>
#include <vector>

class FakeDir : public AMergingObjectWithChildren{
	protected:
		std::wstring filename;
		
	public:
		FakeDir() { }
		FakeDir( std::wstring filename ) : filename( std::move(filename) ) { }
		
		WStringView name() const override{ return makeView( filename ); }
		
		bool isDir() const override{ return true; }
		bool canWrite() const override{ return false; }
		uint64_t filesize() const override{ return 0; }
		
		void combine( const FileObject& with ) override;
		
		std::unique_ptr<AMergingObject> createMerger() const override;
		
		FileObjectId type() const override{ return 3; }
};

#endif
