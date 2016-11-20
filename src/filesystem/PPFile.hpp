/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef PP_FILE_HPP
#define PP_FILE_HPP

#include "PPFolder.hpp"
#include "TypeIDs.hpp"

#include "../decoders/PPArchive.hpp"

class PPFile : public FileObject{
	private:
		std::wstring filepath;
		WStringView filename;
		
		class Reader : public FileObject{
			private:
				const PPFile* parent;
				unsigned index;
				
			public:
				Reader( const PPFile& parent, unsigned index )
					:	parent(&parent), index(index) {}
				
				WStringView name() const override;
				bool isDir() const override { return false; }
				bool canWrite() const override { return false; }
				
				uint64_t filesize() const override;
				std::unique_ptr<FileHandle> openRead() const override;
				std::unique_ptr<AMergingObject> createMerger() const override;
				FileObjectId type() const override{ return ID::PP_FILE_READER; }
		};
		
		PP::Header header;
		std::vector<PPSubFile> files;
		std::vector<Reader> objects;
		
	public:
		PPFile( std::wstring filepath );
		
		WStringView name() const override{ return filename; }
		bool isDir() const override{ return false; }
		bool canWrite() const override{ return false; }
		
		std::unique_ptr<AMergingObject> createMerger() const override;
		
		FileObjectId type() const override{ return ID::PP_FILE; }
		
		PP::Header::SubFile get( unsigned index ) const;
};

#endif
