//
// Created by pbeerkens on 4/26/20.
//
#include <gtest/gtest.h>
#include <ILINK3Messages/MessageBuffer.h>

TEST(ILINK3MessagesTests, Resize) {
    MessageBuffer buf(4);
    EXPECT_EQ(buf.getBufSize(),4);

    buf.expandIfRequired(12);
    EXPECT_EQ(buf.getBufSize(),12);

    buf.expandIfRequired(10);
    EXPECT_EQ(buf.getBufSize(),12);
}

TEST(ILINK3MessagesTests, Write1) {
    MessageBuffer buf(10);
    EXPECT_FALSE (buf.bad ());
    EXPECT_EQ (buf.getWrtBufFree (),10);
    char c [10] {'H','e','l','l','o',' ','t','e','s','t'};
    buf.write(c,10);
    EXPECT_FALSE (buf.bad ());
    EXPECT_EQ (buf.getWrtBufFree (),0);
}

TEST(ILINK3MessagesTests, Write2) {
    MessageBuffer buf(20);
    EXPECT_FALSE (buf.bad ());
    char c [10] {'H','e','l','l','o',' ','t','e','s','t'};
    buf.write(c,10);
    EXPECT_FALSE (buf.bad ());
    EXPECT_EQ (buf.getWrtBufFree (),10);
}
