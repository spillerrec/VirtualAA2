/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "Deduper.hpp"

#include <boost/crc.hpp>
#include <numeric>

//TODO: Check const correctness of ByteViews

Deduper::Hash Deduper::calculateHash( ConstByteView data ){
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
	auto hash = calculateHash( makeConst(data) );
	auto resource = findExisting( hash );
	if( resource )
		resource->sources.push_back( data );
	else
		items.emplace_back( hash, data );
}

unsigned Deduper::duplicated( ByteView view ) const{
	//TODO: look after view without accessing data?
	auto resource = findExisting( calculateHash( makeConst(view) ) );
	return resource ? resource->sources.size() : 0;
}

unsigned Deduper::total_size() const{
	return std::accumulate( items.begin(), items.end(), 0u
		,	[]( unsigned acc, auto& item )
			{ return acc + item.data.size() * item.sources.size(); }
		);
}

unsigned Deduper::savings() const{
	return total_size() - std::accumulate( items.begin(), items.end(), 0u
		,	[]( unsigned acc, auto& item ){ return acc + item.data.size(); }
		);
}

unsigned Deduper::addedCount() const{
	return std::accumulate( items.begin(), items.end(), 0u
		,	[]( unsigned acc, auto& item ){ return acc + item.sources.size(); }
		);
}

unsigned Deduper::savedCount() const{ return items.size(); }
