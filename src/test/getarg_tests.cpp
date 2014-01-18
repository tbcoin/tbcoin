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
    ResetArgs("-TBC");
    BOOST_CHECK(GetBoolArg("-TBC"));
    BOOST_CHECK(GetBoolArg("-TBC", false));
    BOOST_CHECK(GetBoolArg("-TBC", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-TBCo"));
    BOOST_CHECK(!GetBoolArg("-TBCo", false));
    BOOST_CHECK(GetBoolArg("-TBCo", true));

    ResetArgs("-TBC=0");
    BOOST_CHECK(!GetBoolArg("-TBC"));
    BOOST_CHECK(!GetBoolArg("-TBC", false));
    BOOST_CHECK(!GetBoolArg("-TBC", true));

    ResetArgs("-TBC=1");
    BOOST_CHECK(GetBoolArg("-TBC"));
    BOOST_CHECK(GetBoolArg("-TBC", false));
    BOOST_CHECK(GetBoolArg("-TBC", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noTBC");
    BOOST_CHECK(!GetBoolArg("-TBC"));
    BOOST_CHECK(!GetBoolArg("-TBC", false));
    BOOST_CHECK(!GetBoolArg("-TBC", true));

    ResetArgs("-noTBC=1");
    BOOST_CHECK(!GetBoolArg("-TBC"));
    BOOST_CHECK(!GetBoolArg("-TBC", false));
    BOOST_CHECK(!GetBoolArg("-TBC", true));

    ResetArgs("-TBC -noTBC");  // -TBC should win
    BOOST_CHECK(GetBoolArg("-TBC"));
    BOOST_CHECK(GetBoolArg("-TBC", false));
    BOOST_CHECK(GetBoolArg("-TBC", true));

    ResetArgs("-TBC=1 -noTBC=1");  // -TBC should win
    BOOST_CHECK(GetBoolArg("-TBC"));
    BOOST_CHECK(GetBoolArg("-TBC", false));
    BOOST_CHECK(GetBoolArg("-TBC", true));

    ResetArgs("-TBC=0 -noTBC=0");  // -TBC should win
    BOOST_CHECK(!GetBoolArg("-TBC"));
    BOOST_CHECK(!GetBoolArg("-TBC", false));
    BOOST_CHECK(!GetBoolArg("-TBC", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--TBC=1");
    BOOST_CHECK(GetBoolArg("-TBC"));
    BOOST_CHECK(GetBoolArg("-TBC", false));
    BOOST_CHECK(GetBoolArg("-TBC", true));

    ResetArgs("--noTBC=1");
    BOOST_CHECK(!GetBoolArg("-TBC"));
    BOOST_CHECK(!GetBoolArg("-TBC", false));
    BOOST_CHECK(!GetBoolArg("-TBC", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-TBC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-TBC", "eleven"), "eleven");

    ResetArgs("-TBC -bar");
    BOOST_CHECK_EQUAL(GetArg("-TBC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-TBC", "eleven"), "");

    ResetArgs("-TBC=");
    BOOST_CHECK_EQUAL(GetArg("-TBC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-TBC", "eleven"), "");

    ResetArgs("-TBC=11");
    BOOST_CHECK_EQUAL(GetArg("-TBC", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-TBC", "eleven"), "11");

    ResetArgs("-TBC=eleven");
    BOOST_CHECK_EQUAL(GetArg("-TBC", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-TBC", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-TBC", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-TBC", 0), 0);

    ResetArgs("-TBC -bar");
    BOOST_CHECK_EQUAL(GetArg("-TBC", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-TBC=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-TBC", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-TBC=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-TBC", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--TBC");
    BOOST_CHECK_EQUAL(GetBoolArg("-TBC"), true);

    ResetArgs("--TBC=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-TBC", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noTBC");
    BOOST_CHECK(!GetBoolArg("-TBC"));
    BOOST_CHECK(!GetBoolArg("-TBC", true));
    BOOST_CHECK(!GetBoolArg("-TBC", false));

    ResetArgs("-noTBC=1");
    BOOST_CHECK(!GetBoolArg("-TBC"));
    BOOST_CHECK(!GetBoolArg("-TBC", true));
    BOOST_CHECK(!GetBoolArg("-TBC", false));

    ResetArgs("-noTBC=0");
    BOOST_CHECK(GetBoolArg("-TBC"));
    BOOST_CHECK(GetBoolArg("-TBC", true));
    BOOST_CHECK(GetBoolArg("-TBC", false));

    ResetArgs("-TBC --noTBC");
    BOOST_CHECK(GetBoolArg("-TBC"));

    ResetArgs("-noTBC -TBC"); // TBC always wins:
    BOOST_CHECK(GetBoolArg("-TBC"));
}

BOOST_AUTO_TEST_SUITE_END()
