/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef DEDUPER_HPP
#define DEDUPER_HPP

#include "../utils/ArrayView.hpp"
#include "../utils/Buffer.hpp"

class Deduper{
	public:
		using Hash = uint32_t;
		
		struct Resource{
			Hash hash;
			Buffer data; //A local copy of the resource
			std::vector<ByteView> sources; //All the objects which points to it
			
			Resource() {}
			Resource( Hash hash, ByteView view ) : hash(hash), data(view), sources{ view } {}
		};
	
	private:
		std::vector<Resource> items;
		
		static Hash calculateHash( ByteView data );
		Resource* findExisting( Hash hash );
		const Resource* findExisting( Hash hash ) const;
		
	public:
		void add( ByteView resource );
		
		unsigned duplicated( ByteView view ) const;
		const auto& resources() const{ return items; }
		
		unsigned total_size() const;
		unsigned savings() const;
};

#endif
