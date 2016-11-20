/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef TYPE_IDS_HPP
#define TYPE_IDS_HPP

namespace ID{
	constexpr uint64_t DEFLATE        =  1;
	constexpr uint64_t LZ4            =  2;
	constexpr uint64_t LZMA           =  3;
	constexpr uint64_t PASS_DIR       =  4;
	constexpr uint64_t PASS_FILE      =  5;
	constexpr uint64_t PP_FILE        =  6;
	constexpr uint64_t PP_FOLDER      =  7;
	constexpr uint64_t PP_FILE_READER =  8;
	constexpr uint64_t PP_MERGER      =  9;
	constexpr uint64_t PASS_MERGER    = 10;
	constexpr uint64_t FAKE_DIR       = 11;
}

#endif
