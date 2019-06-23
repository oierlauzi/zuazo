#define BOOST_TEST_MODULE PixelFormat
#include <boost/test/included/unit_test.hpp>
#include <PixelFormat.h>

using PixelFormat = Zuazo::PixelFormat;

BOOST_AUTO_TEST_CASE(default_constructor){
    PixelFormat pixFmt;
}
