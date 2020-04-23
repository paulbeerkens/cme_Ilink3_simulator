#include <gtest/gtest.h>
#include <webserver/WebServer.h>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>

class LibWebServerFixture: public ::testing::Test {
protected:
    void TearDown () override {
        webserver::WebServer::get ().cleanupForUnitTest ();
    }
};

TEST_F (LibWebServerFixture, normal) {
    ASSERT_TRUE (webserver::WebServer::get ().start (25000));
    std::this_thread::sleep_for (std::chrono::seconds(1));
    webserver::WebServer::get ().stop ();
}

TEST_F (LibWebServerFixture, normalAgain) {
    //This is more for testing the testing framework to see if we can
    //recreate the singleton.
    ASSERT_TRUE (webserver::WebServer::get ().start (25000));
    std::this_thread::sleep_for (std::chrono::seconds(1));
    webserver::WebServer::get ().stop ();
}

TEST_F (LibWebServerFixture, doubleStop) {
    ASSERT_TRUE (webserver::WebServer::get ().start (25000));
    std::this_thread::sleep_for (std::chrono::seconds(1));
    webserver::WebServer::get ().stop ();
    webserver::WebServer::get ().stop ();
}

TEST_F (LibWebServerFixture, immediateStop) {
    ASSERT_TRUE (webserver::WebServer::get ().start (25000));
    webserver::WebServer::get ().stop ();
}

TEST_F (LibWebServerFixture, stopWithoutStart) {
    webserver::WebServer::get ().stop ();
}

TEST_F (LibWebServerFixture, invalidPort) {
    ASSERT_FALSE (webserver::WebServer::get ().start (22));
}

TEST_F (LibWebServerFixture, invalidPortThenValidPort) {
    ASSERT_FALSE (webserver::WebServer::get ().start (22));
    ASSERT_TRUE (webserver::WebServer::get ().start (25000));
}

TEST_F (LibWebServerFixture, portInUse) {
    //block the port for now and see if can call start sucesfully after we unblock the port
    auto socket = ::socket(AF_INET, SOCK_STREAM, 0);

    int enable = 1;
    setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

    struct sockaddr_in sockAddr;
    bzero(&sockAddr, sizeof(sockAddr));
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    sockAddr.sin_port = htons(25000);

    ::bind(socket, reinterpret_cast<const struct sockaddr *>(&sockAddr), sizeof(sockAddr));
    ::listen (socket, SOMAXCONN);

    ASSERT_FALSE (webserver::WebServer::get().start(25000));

    ::close (socket);

    ASSERT_TRUE (webserver::WebServer::get().start(25000));
}