/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#define BOOST_TEST_MODULE Test framework
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include "../src/decoders/PPCrypto.hpp"
#include "../src/utils/Buffer.hpp"

using namespace boost::unit_test;

BOOST_DATA_TEST_CASE( HeaderDecrypter_setPosition, data::xrange(0, 2000, 100 ) ){
	//Advance the HeaderDecrypter normally
	PP::HeaderDecrypter normal;
	normal.decrypt( Buffer( sample ).view() );
	
	//Advance the HeaderDecrypter by skipping with setPosition
	PP::HeaderDecrypter fast;
	fast.setPosition( sample );
	
	//Their state should still be exactly the same
	BOOST_CHECK( normal == fast );
}
