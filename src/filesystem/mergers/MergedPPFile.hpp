/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef MERGED_PP_FILE_HPP
#define MERGED_PP_FILE_HPP

#include "AMergingObject.hpp"
#include "../PPFile.hpp"

#include <string>
#include <vector>

struct PPSubFileReference{
	const PPSubFile& parent;
	uint64_t offset{ 0u };
	
	PPSubFileReference( const PPSubFile& parent ) : parent(parent) { }
	
	auto filesize() const{ return parent.file->filesize(); }
	
	bool operator<( uint64_t offset ) const{ return filesize() < offset; }
};

class MergedPPFile : public AMergingObject{
	private:
		std::wstring filename;
		
		std::vector<PPSubFileReference> files;
		
		uint64_t header_size() const;
		
		PPSubFileReference fileFromOffset( uint64_t offset ) const
			{ return *std::lower_bound( files.begin(), files.end(), offset ); }
		
	public:
		MergedPPFile( std::wstring filename ) : filename(filename) { }
		MergedPPFile( std::wstring filename, const std::vector<PPSubFile>& files )
			: MergedPPFile(filename) { addPP( files ); }
		
		void addPP( const std::vector<PPSubFile>& files );
		
		WStringView name() const override{ return makeView( filename ); }
		bool isDir() const override{ return false; }
		bool canWrite() const override{ return false; }
		uint64_t filesize() const override;
		
		std::unique_ptr<FileHandle> openRead()   const override;
		
		std::unique_ptr<AMergingObject> createMerger() const override;
		
		FileObjectId type() const override{ return 6; }
};

#endif
