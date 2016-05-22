/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef PASSTHROUGH_MERGER_HPP
#define PASSTHROUGH_MERGER_HPP

#include "AMergingObject.hpp"

class PassthroughMerger : public AMergingObject{
	protected:
		const FileObject& parent;
	
	public:
		PassthroughMerger( const FileObject& parent ) : parent(parent) { }
		
		WStringView name()  const override { return parent.name()    ; }
		bool isDir()        const override { return parent.isDir()   ; }
		Time created()      const override { return parent.created() ; }
		Time modified()     const override { return parent.modified(); }
		bool canWrite()     const override { return parent.canWrite(); }
		uint64_t filesize() const override { return parent.filesize(); }
		
		std::unique_ptr<FileHandle> openRead()   const override { return parent.openRead(); }
		std::unique_ptr<FileHandle> openWrite()  const override { return parent.openWrite(); }
		std::unique_ptr<FileHandle> openAppend() const override { return parent.openAppend(); }
		
		FileObjectId type() const override { return parent.type(); } //TODO: no no...
		uint64_t children() const override { return parent.children(); }
		const FileObject& operator[]( int index ) const override { return parent[index]; }
		
		std::unique_ptr<AMergingObject> createMerger() const override
			{ return std::make_unique<PassthroughMerger>( parent ); }
};

#endif
