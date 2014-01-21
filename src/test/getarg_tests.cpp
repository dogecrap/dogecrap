#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-APL");
    BOOST_CHECK(GetBoolArg("-APL"));
    BOOST_CHECK(GetBoolArg("-APL", false));
    BOOST_CHECK(GetBoolArg("-APL", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-APLo"));
    BOOST_CHECK(!GetBoolArg("-APLo", false));
    BOOST_CHECK(GetBoolArg("-APLo", true));

    ResetArgs("-APL=0");
    BOOST_CHECK(!GetBoolArg("-APL"));
    BOOST_CHECK(!GetBoolArg("-APL", false));
    BOOST_CHECK(!GetBoolArg("-APL", true));

    ResetArgs("-APL=1");
    BOOST_CHECK(GetBoolArg("-APL"));
    BOOST_CHECK(GetBoolArg("-APL", false));
    BOOST_CHECK(GetBoolArg("-APL", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noAPL");
    BOOST_CHECK(!GetBoolArg("-APL"));
    BOOST_CHECK(!GetBoolArg("-APL", false));
    BOOST_CHECK(!GetBoolArg("-APL", true));

    ResetArgs("-noAPL=1");
    BOOST_CHECK(!GetBoolArg("-APL"));
    BOOST_CHECK(!GetBoolArg("-APL", false));
    BOOST_CHECK(!GetBoolArg("-APL", true));

    ResetArgs("-APL -noAPL");  // -APL should win
    BOOST_CHECK(GetBoolArg("-APL"));
    BOOST_CHECK(GetBoolArg("-APL", false));
    BOOST_CHECK(GetBoolArg("-APL", true));

    ResetArgs("-APL=1 -noAPL=1");  // -APL should win
    BOOST_CHECK(GetBoolArg("-APL"));
    BOOST_CHECK(GetBoolArg("-APL", false));
    BOOST_CHECK(GetBoolArg("-APL", true));

    ResetArgs("-APL=0 -noAPL=0");  // -APL should win
    BOOST_CHECK(!GetBoolArg("-APL"));
    BOOST_CHECK(!GetBoolArg("-APL", false));
    BOOST_CHECK(!GetBoolArg("-APL", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--APL=1");
    BOOST_CHECK(GetBoolArg("-APL"));
    BOOST_CHECK(GetBoolArg("-APL", false));
    BOOST_CHECK(GetBoolArg("-APL", true));

    ResetArgs("--noAPL=1");
    BOOST_CHECK(!GetBoolArg("-APL"));
    BOOST_CHECK(!GetBoolArg("-APL", false));
    BOOST_CHECK(!GetBoolArg("-APL", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-APL", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-APL", "eleven"), "eleven");

    ResetArgs("-APL -bar");
    BOOST_CHECK_EQUAL(GetArg("-APL", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-APL", "eleven"), "");

    ResetArgs("-APL=");
    BOOST_CHECK_EQUAL(GetArg("-APL", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-APL", "eleven"), "");

    ResetArgs("-APL=11");
    BOOST_CHECK_EQUAL(GetArg("-APL", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-APL", "eleven"), "11");

    ResetArgs("-APL=eleven");
    BOOST_CHECK_EQUAL(GetArg("-APL", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-APL", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-APL", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-APL", 0), 0);

    ResetArgs("-APL -bar");
    BOOST_CHECK_EQUAL(GetArg("-APL", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-APL=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-APL", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-APL=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-APL", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--APL");
    BOOST_CHECK_EQUAL(GetBoolArg("-APL"), true);

    ResetArgs("--APL=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-APL", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noAPL");
    BOOST_CHECK(!GetBoolArg("-APL"));
    BOOST_CHECK(!GetBoolArg("-APL", true));
    BOOST_CHECK(!GetBoolArg("-APL", false));

    ResetArgs("-noAPL=1");
    BOOST_CHECK(!GetBoolArg("-APL"));
    BOOST_CHECK(!GetBoolArg("-APL", true));
    BOOST_CHECK(!GetBoolArg("-APL", false));

    ResetArgs("-noAPL=0");
    BOOST_CHECK(GetBoolArg("-APL"));
    BOOST_CHECK(GetBoolArg("-APL", true));
    BOOST_CHECK(GetBoolArg("-APL", false));

    ResetArgs("-APL --noAPL");
    BOOST_CHECK(GetBoolArg("-APL"));

    ResetArgs("-noAPL -APL"); // APL always wins:
    BOOST_CHECK(GetBoolArg("-APL"));
}

BOOST_AUTO_TEST_SUITE_END()
