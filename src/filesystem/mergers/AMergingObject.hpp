/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef A_MERGING_OBJECT_HPP
#define A_MERGING_OBJECT_HPP

#include "../FileObject.hpp"

/** FileObject which can be used to combine several FileObjects which otherwise
 *  would be conflicting */
class AMergingObject : public FileObject{
	public:
		/** Adds the children to this merged object, or throws if not possible
		 *  The file object must have a lifetime which extends this one
		 * @todo create or specify a exception
		 * @param with The FileObject to combine with  */
		virtual void combine( const FileObject& with );
};

/** AMergingObject which contains children using a standard vector */
class AMergingObjectWithChildren : public FileVectorObject<AMergingObject>{
	public:
		AMergingObject& operator[]( int index ){ return *objects[index]; }
};

#endif
