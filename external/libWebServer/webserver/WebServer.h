#ifndef LIBWEBSERVER_WEBSERVER_H
#define LIBWEBSERVER_WEBSERVER_H

#include <string>
#include <unordered_map>
#include "UrlRequest.h"
#include <functional>
#include <thread>
#include <atomic>
#include "IUrlRequestSetting.h"


namespace webserver {

class WebServer {
public:
    typedef std::function <bool (const UrlRequest&, std::ostream&)> WebServerCB;

    static WebServer &get() {
        static WebServer instance;
        return instance;
    }

    IUrlRequestSetting& registerURL(const std::string &url, const WebServerCB& cb);

    void onUnrecognizedURL(std::ostream &os);

    std::string version() const;

    void log (std::string_view s);

    bool start (std::int32_t port);

    bool stop ();

    void cleanupForUnitTest () {
        stop ();
        threadPtr_=nullptr;
        port_=-1;
    }
protected:
    WebServer(WebServer const &) = delete;             // Copy construct
    WebServer(WebServer &&) = delete;                  // Move construct
    WebServer &operator=(WebServer const &) = delete;  // Copy assign
    WebServer &operator=(WebServer &&) = delete;      // Move assign
    WebServer ()
    :indexPageCB_ { [this] (const UrlRequest& urlRequest, std::ostream& os)->bool {generateDefaultIndexPage (urlRequest,os); return true;}} //set the callback to generateDefaultIndexPage
    {

    }

    ~WebServer () {stop ();}

    void doWork (); //the thread that does the actual work for this library
                    //in the future can be changed to a thread pool or
                    //even an external thread.

    void processRequest (const UrlRequest& urlRequest, std::ostream& os) const;
    bool sendBackResponse (int socket, std::string_view sv) const;
    bool sendN (int socket, const char* buf, std::size_t size) const;

    struct RegisteredURLInfo: public IUrlRequestSetting {
        explicit RegisteredURLInfo (const WebServerCB& cb)
        :cb_ (cb) {};
        WebServerCB cb_;
    private: //we don't want these to be called other than through the interface IUrlRequestSetting from the registerURL call.
        IUrlRequestSetting& description(const std::string &d) override {description_=d; return *this;}
        IUrlRequestSetting& visible(bool v) override {visible_=v; return *this;}
        IUrlRequestSetting& showShortcuts(bool s) override {showShortcuts_=s; return *this;}
        IUrlRequestSetting& displayName(const std::string &n) override {displayName_=n; return *this;}
    public:
        [[nodiscard]] const std::optional<std::string>& getDescription() const {return description_;}
        [[nodiscard]] bool getVisible() const {return visible_;}
        [[nodiscard]] bool getShowShortcuts() const {return showShortcuts_;}
        [[nodiscard]] const std::string& getDisplayName () const {return displayName_;};

    protected:
        std::optional <std::string> description_;
        bool visible_ {true};
        bool showShortcuts_ {true};
        std::string displayName_;
    };

    std::unordered_map<std::string, RegisteredURLInfo> registeredURLs_;
    WebServerCB indexPageCB_; //this is called when the requested url was not found
    bool generateDefaultIndexPage (const UrlRequest&, std::ostream&) const;

    std::int32_t port_ {-1};
    int socket_ {-1};

    std::unique_ptr <std::thread> threadPtr_;
    std::atomic <bool> requestedToTerminate_ {false};

    std::optional <UrlRequest> readRequest (int socket, const std::string& remoteIP);

    std::string getShortCutsHTML () const;
    //void generateShortCutsHTML (); //populates shortCutsHTML
    //std::string shortCutsHTML_; //the html that goes to the top a web pages when enabled
};

}

#endif //LIBWEBSERVER_WEBSERVER_H
