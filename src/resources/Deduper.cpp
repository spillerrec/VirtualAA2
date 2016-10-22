/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "Deduper.hpp"

#include <boost/crc.hpp>

Deduper::Hash Deduper::calculateHash( ByteView data ){
	boost::crc_32_type crc;
	crc.process_bytes( data.begin(), data.size() );
	return crc.checksum();
}

Deduper::Resource* Deduper::findExisting( Deduper::Hash hash ){
	for( auto& item : items )
		if( item.hash == hash )
			return &item;
	
	return nullptr;
}

const Deduper::Resource* Deduper::findExisting( Deduper::Hash hash ) const{
	for( auto& item : items )
		if( item.hash == hash )
			return &item;
	
	return nullptr;
}

void Deduper::add( ByteView data ){
	auto hash = calculateHash( data );
	auto resource = findExisting( hash );
	if( resource )
		resource->sources.push_back( data );
	else
		items.emplace_back( hash, data );
}

unsigned Deduper::duplicated( ByteView view ) const{
	//TODO: look after view without accessing data?
	auto resource = findExisting( calculateHash( view ) );
	return resource ? resource->sources.size() : 0;
}