/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef PASSTHROUGH_DIR_HPP
#define PASSTHROUGH_DIR_HPP

#include "FakeDir.hpp"

#include <string>
#include <vector>
#include <memory>

class PassthroughDir : public FakeDir{
	private:
		std::wstring filepath;
		
	public:
		PassthroughDir( std::wstring filepath );
		
		FileObjectId type() const override{ return 2; }
		std::unique_ptr<FileObject> copy() const override;
};

#endif
