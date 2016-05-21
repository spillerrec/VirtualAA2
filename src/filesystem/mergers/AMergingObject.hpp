/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef A_MERGING_OBJECT_HPP
#define A_MERGING_OBJECT_HPP

#include "../FileObject.hpp"

class AMergingObject : public FileObject{
	public:
		virtual void combine( const FileObject& with );
};

class AMergingObjectWithChildren : public FileVectorObject<AMergingObject>{
	public:
		AMergingObject& operator[]( int index ){ return *objects[index]; }
};

#endif
