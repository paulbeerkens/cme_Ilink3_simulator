#include <gtest/gtest.h>
#include <webserver/WebUtils.h>

TEST(WebUtilsTest, split) {
    auto retVal=webserver::split("hello world how is it going?",' ');
    ASSERT_EQ (retVal.size (),6);
    ASSERT_EQ (retVal [0],"hello");
    ASSERT_EQ (retVal [5],"going?");
}

TEST(WebUtilsTest, split_no_seperator) {
    auto retVal=webserver::split("hello",' ');
    ASSERT_EQ (retVal.size (),1);
    ASSERT_EQ (retVal [0],"hello");
}

TEST(WebUtilsTest, split_double_seperator) {
    auto retVal=webserver::split("hello  world",' ');
    ASSERT_EQ (retVal.size (),2);
    ASSERT_EQ (retVal [0],"hello");
    ASSERT_EQ (retVal [1],"world");
}

TEST (WebUtilsTest, deUrlify_no_modification) {
    std::string s ("/hello?world=1&another=2");
    ASSERT_TRUE (webserver::deUrlify(s));
    ASSERT_EQ (s,"/hello?world=1&another=2");
}

TEST (WebUtilsTest, deUrlify_some_modification) {
    std::string s ("/hello?world=1&another=bites%20the%20dust");
    ASSERT_TRUE (webserver::deUrlify(s));
    ASSERT_EQ (s,"/hello?world=1&another=bites the dust");
}

TEST (WebUtilsTest, deUrlify_modification_at_end) {
    std::string s ("/hello?world=1&another=one%24");
    ASSERT_TRUE (webserver::deUrlify(s));
    ASSERT_EQ (s,"/hello?world=1&another=one$");
}

TEST (WebUtilsTest, deUrlify_double_modification) {
    std::string s ("/hello?world=1&another=bites%20%20dust");
    ASSERT_TRUE (webserver::deUrlify(s));
    ASSERT_EQ (s,"/hello?world=1&another=bites  dust");
}

TEST (WebUtilsTest, deUrlify_badspecialcharacter) {
    std::string s ("/hello?world=1&another=bites%dust");
    ASSERT_FALSE (webserver::deUrlify(s));
}

TEST (WebUtilsTest, deUrlify_badspecialcharacters) {
    std::string s ("/hello?world=1&another=bites%2the");
    ASSERT_FALSE (webserver::deUrlify(s));
}

TEST (WebUtilsTest, deUrlify_badspecialcharacter_at_end) {
    std::string s ("/hello?world=1&another=bites%");
    ASSERT_FALSE (webserver::deUrlify(s));
}

TEST (WebUtilsTest, deUrlify_badspecialcharacters_at_end) {
    std::string s ("/hello?world=1&another=bites%2");
    ASSERT_FALSE (webserver::deUrlify(s));
}

