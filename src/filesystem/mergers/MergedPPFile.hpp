/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef MERGED_PP_FILE_HPP
#define MERGED_PP_FILE_HPP

#include "AMergingObject.hpp"
#include "../TypeIDs.hpp"
#include "../PPFolder.hpp"

#include <string>
#include <vector>

struct PPSubFileReference{
	const PPSubFile* parent;
	uint64_t offset{ 0u };
	
	PPSubFileReference( const PPSubFile& parent ) : parent(&parent) { }
	
	auto filesize() const{ return parent->file->filesize(); }
	bool operator<( const PPSubFileReference& other ) const{ return this->offset < other.offset; }
};

class MergedPPFile : public AMergingObject{
	private:
		std::wstring filename;
		std::vector<PPSubFileReference> files;
		
		bool contains( const PPSubFile& file ) const;
		
	public:
		MergedPPFile( std::wstring filename ) : filename(filename) { }
		MergedPPFile( std::wstring filename, const std::vector<PPSubFile>& files )
			: MergedPPFile(filename) { addPP( files ); }
		
		void addPP( const std::vector<PPSubFile>& files );
		uint64_t headerSize() const;
		
		const std::vector<PPSubFileReference>& subfiles() const{ return files; }
		
		using FileIterator = std::vector<PPSubFileReference>::const_iterator;
		FileIterator firstSubFile( uint64_t offset ) const;
		FileIterator  lastSubFile( uint64_t offset ) const;
		
		WStringView name() const override{ return makeView( filename ); }
		bool isDir() const override{ return false; }
		bool canWrite() const override{ return false; }
		uint64_t filesize() const override;
		
		std::unique_ptr<FileHandle> openRead()   const override;
		
		std::unique_ptr<AMergingObject> createMerger() const override;
		
		FileObjectId type() const override{ return ID::PP_MERGER; }
		
		void combine( const FileObject& with ) override;
		void combine( const class PPFile& with );
		void combine( const MergedPPFile& with );
};

#endif
