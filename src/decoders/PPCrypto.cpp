/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "PPCrypto.hpp"
#include "../utils/Buffer.hpp"

using namespace PP;

const uint8_t HeaderDecrypter::offset[] = {0x3A, 0xE3, 0x87, 0xC2, 0xBD, 0x1E, 0xA6, 0xFE};

const uint8_t FileDecrypter::mask[] = {
	0x4D, 0x2D, 0xBF, 0x6A, 0x5B, 0x4A, 0xCE, 0x9D,
	0xF4, 0xA5, 0x16, 0x87, 0x92, 0x9B, 0x13, 0x03,
	0x8F, 0x92, 0x3C, 0xF0, 0x98, 0x81, 0xDB, 0x8E,
	0x5F, 0xB4, 0x1D, 0x2B, 0x90, 0xC9, 0x65, 0x00
};

void HeaderDecrypter::setPosition( uint32_t position ){
	if( index != position )
		throw std::runtime_error( "HeaderDecrypter::setPosition does not yet support random seeking" ); //TODO:
}

void HeaderDecrypter::decrypt( ByteView buffer ){
	for( auto& val : buffer ){
		auto var28 = index % 8;
		mask[var28] += offset[var28];
		val ^= mask[var28];
		index++;
	}
}


void FileDecrypter::decrypt( ByteView buffer ){
	for( auto& val : buffer ){
		val ^= mask[index % 32];
		index++;
	}
}
