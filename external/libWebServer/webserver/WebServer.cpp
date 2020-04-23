#include <iostream>
#include "WebServer.h"
#include <assert.h>
#include <sys/socket.h>
#include <cstring>
#include <netinet/in.h>
#include <future>
#include <unistd.h>
//#include <sstream>
#include <arpa/inet.h>
#include <optional>
#include <sstream>

std::string webserver::WebServer::version() const {
    return LIBWEBSERVERVERSION;
}

IUrlRequestSetting& webserver::WebServer::registerURL(const std::string &url, const webserver::WebServer::WebServerCB &cb) {
    //We convert all urls to lower case as they are supposed to be case insensitive.
    std::string urlCopy (url);
    std::transform (urlCopy.begin (), urlCopy.end (), urlCopy.begin (), [] (char c) {return std::tolower(c);});

    //std::tie (std::ignore, inserted)=registeredURLs_.emplace (std::piecewise_construct, std::forward_as_tuple (urlCopy),std::forward_as_tuple (cb));
    auto [itr, inserted]=registeredURLs_.emplace (urlCopy,cb);

    if (!inserted) {
        log ("Url "+urlCopy+" was already registered");
    };

    auto startItr=url.find_first_not_of('/');
    static_cast <IUrlRequestSetting &> (itr->second).displayName(url.substr(startItr)); //cast to IUrlRequest to access the set function displayName.

    //TODO return a setter object that in it's destructor does the actual insert
    //this will allow us to generate the html for the index when a new url is registered
    //and avoid showing urls that will be marked as hidden a little while later (because typically done at start-up maybe not a huge concern).
    return itr->second; //return the setting interface to the registered url object. Allows us to set setting like this: WebServer::get ().registerUrl ("/MyUrl",myFunc).description ("A Url").hidden (false).showIndex (true);
}

void webserver::WebServer::log(std::string_view s) {
    std::cout<<s<<std::endl;
}

bool webserver::WebServer::start(std::int32_t port) {
    assert (port_==-1&&"Do not call start more than once.");
    port_=port;

    socket_=::socket (AF_INET, SOCK_STREAM, 0);
    if (socket_==-1) {
        log ("WebServer::start failed to create socket with error code "+std::to_string(errno)+" ("+strerror (errno)+")");
        port_=-1; //reset so that we could call start again possibly with a different port
        return false;
    }

    int enable = 1;
    if (setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        log ("setsockopt(SO_REUSEADDR) failed");
    }

    struct sockaddr_in sockAddr;
    bzero (&sockAddr, sizeof (sockAddr));
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = htonl (INADDR_ANY);
    sockAddr.sin_port = htons (port_);

    auto result=::bind (socket_, reinterpret_cast<const struct sockaddr*>(&sockAddr),sizeof (sockAddr));
    if (result!=0) {
        log ("WebServer::start failed to bind socket on port "+std::to_string (port_)+" with error code "+std::to_string(errno)+" ("+strerror (errno)+")");
        port_=-1; //reset so that we could call start again possibly with a different port
        return false;
    }

    result = ::listen (socket_, SOMAXCONN); //allow the maximum number of connections to be queued up waiting for an accept call
    if (result!=0) {
        log ("WebServer::start failed to listen socket on port "+std::to_string (port_)+" with error code "+std::to_string(errno)+" ("+strerror (errno)+")");
        port_=-1; //reset so that we could call start again possibly with a different port
        return false;
    }

    assert (threadPtr_==nullptr);

    std::promise <void> threadStartedPromise_;
    threadPtr_=std::make_unique <std::thread> ([this, &threadStartedPromise_] () {
        threadStartedPromise_.set_value();
        this->doWork ();});

    threadStartedPromise_.get_future().wait(); //wait till thread has been started;

    return true;
}

void webserver::WebServer::doWork() {
    log ("Webserver listening to port "+std::to_string (port_));

    while (!requestedToTerminate_.load(std::memory_order_relaxed)) {
        struct sockaddr_in clientAddr;
        socklen_t len=sizeof (clientAddr);

        auto newSocket=::accept (socket_, reinterpret_cast<struct sockaddr*>(&clientAddr), &len);
        //If accept fails it could be a temporary thing. For example there could be a handle leak somewhere in the app
        //and we can force increase the number of handles for the application or some connections disconnect which frees
        //up some more handles. Better to keep trying than to give up.
        if (newSocket==-1) {
            log ("WebServer::doWork accept call failed on port "+std::to_string (port_)+" with error code "+std::to_string(errno)+" ("+strerror (errno)+"). Going to try again in one minute");
            for (std::int32_t loopCount=60;!requestedToTerminate_&&loopCount>0;--loopCount) std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }

        //Get the remote ip address. Remote port is less interesting as it is temporary in most cases.
        char remoteIP [20] = {0};
        if (::inet_ntop (AF_INET, reinterpret_cast <const void*>(&clientAddr.sin_addr), remoteIP, sizeof (remoteIP))==nullptr) {
            log ("Failed to get remote ip address");
        }

        //read and parse the request from the newly created socket
        auto request= readRequest(newSocket, remoteIP);

        //now process the request. This is mainly executing the callback function or loading the requested page.
        std::stringstream output;
        if (request.has_value()) {
            processRequest (*request,output);
            sendBackResponse (newSocket, output.str ()); //C++20 allows stringview here which would be really nice but trying to not be too cutting edge
        } else {
            //TODO sendBackError (newSocket);
        }

        ::shutdown(newSocket, SHUT_WR); //send the FIN to let the client know that we are done
        ::close (newSocket);
    }
}

bool webserver::WebServer::stop() {
    log ("WebServer::stop called");
    requestedToTerminate_.store(true);

    if (socket_!=-1) {
        ::shutdown(socket_,SHUT_RD); //this forces the accept loop to terminate
        ::close (socket_);
        socket_=-1;
    }

    if (threadPtr_ && threadPtr_->joinable()) {
        threadPtr_->join ();
    }

    return false;
}

std::optional<webserver::UrlRequest> webserver::WebServer::readRequest(int socket, const std::string& remoteIP) {
    //A typical request looks like this:
    //GET /hello?test=1 HTTP/1.1
    //Host: vmserver03:25000
    //Upgrade-Insecure-Requests: 1
    //Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
    //User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/13.0.5 Safari/605.1.15
    //Accept-Language: en-us
    //Accept-Encoding: gzip, deflate
    //Connection: keep-alive

    assert (socket!=-1 && "WebServer::readRequest called with an invalid socket. Small chance that this during shutdown.");
    if (socket==-1) return std::nullopt;

    auto retValue=std::make_optional<UrlRequest> (remoteIP);

    bool requestCompleted {false};
    std::string currentLine;

    while (!requestCompleted && !requestedToTerminate_) {

        //todo add timeout
        char c;
        auto n = ::read(socket, &c, 1);
        if (n != 1) {
             if (n==0) {
                 log ("EOF while reading from socket in WebServer::readRequest with error code "+std::to_string(errno)+" ("+strerror (errno)+")");
                 return std::nullopt; //connection was closed
             }
             if (errno==EAGAIN||errno==EINTR) continue; //these are acceptable reasons why read failed. Try again.
             log ("Failed to read from socket in WebServer::readRequest with error code "+std::to_string(errno)+" ("+strerror (errno)+")");
             return std::nullopt;
        }

        if (c==13) //end of line
        {
            if (!currentLine.empty()) {
                std::cout<<currentLine<<std::endl;
                if (!retValue->processHttpRequestLine(currentLine)) {
                    log ("Failed to process http request. Line: "+currentLine);
                    return std::nullopt;
                }
            } else {
                requestCompleted=true;
            }
            currentLine.clear (); //start afresh for the next line
        } else {
            if (c!=10) currentLine.push_back(c);} //ignore new line. Add all other characters
    }

    return retValue;
}

void webserver::WebServer::processRequest(const webserver::UrlRequest &urlRequest, std::ostream& os) const {
    //see if this url is registered as a callback
    auto itr=registeredURLs_.find (urlRequest.getBaseUrl());
    if (itr!=registeredURLs_.end ()) {
        //found this URL as a callback
        try {
            if (itr->second.getShowShortcuts()) os<<getShortCutsHTML ();
            //make the callback
            itr->second.cb_ (urlRequest, os);
            return; //done
        }
        catch (const std::exception& e) {
            os<<"Error: Exception thrown by callback function: "<<e.what ();
        }
        catch (...) {
            os<<"Error: Exception thrown by callback function";
        }
    }

    //not found so send an index page
    indexPageCB_ (urlRequest, os);
}

bool webserver::WebServer::generateDefaultIndexPage(const webserver::UrlRequest &, std::ostream & os) const {
    os<<getShortCutsHTML ();

    os <<"<TABLE>";
    for (const auto& itr: registeredURLs_) {
        if (!itr.second.getVisible()) continue;
        std::stringstream url;
        auto displayName=itr.second.getDisplayName (); //show url or displayName if set
        url<<"<a href='"<<itr.first<<"'>"<<displayName<<"</a>";
        os<<"<TR><TD>"<<url.str ();
        os<<"<TD>"<<itr.second.getDescription().value_or("");
    }
    os <<"</TABLE>";
    return false;
}

bool webserver::WebServer::sendBackResponse(int socket, std::string_view sv) const {

    static const std::string_view baseHeader ("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
    if (!sendN (socket, baseHeader.data (), baseHeader.size ())) return false;

    //If we want to send the content length we need to remove the extra \r\n from the base header
    //std::string headerContentLength {"Content-Length: "};
    //headerContentLength+=std::to_string (sv.size ());
    //headerContentLength.push_back(13);
    //headerContentLength.push_back(10);
    //headerContentLength.push_back(13);
    //headerContentLength.push_back(10);
    //if (!sendN (socket, headerContentLength.c_str (), headerContentLength.size ())) return false;

    return sendN (socket, sv.data (), sv.size ());
}

bool webserver::WebServer::sendN (int socket, const char* buf, std::size_t size) const {
    const char* wptr=buf;
    std::size_t dataLeft=size;

    while (dataLeft&&!requestedToTerminate_) {
        auto written=::write (socket, wptr, dataLeft);
        if (written<0) {
            if (errno!=EINTR && errno !=EWOULDBLOCK && errno!=EAGAIN) return false;
        } else {
            dataLeft -= written;
            wptr += written;
        }
    }
    return true;
}

std::string webserver::WebServer::getShortCutsHTML() const {
    std::stringstream ss;
    ss<<"<section><nav>";
    bool needToaddSeparator {false};

    for (const auto& registeredURL: registeredURLs_) {
        if (registeredURL.second.getVisible()) {
            auto displayName=registeredURL.second.getDisplayName (); //show url or displayName if set
            if (needToaddSeparator) ss<<" | ";
            ss<<R"(<a href=")"<<registeredURL.first<<R"(">)"<<displayName<<R"(</a>)";
            needToaddSeparator=true;
        }
    }
    ss<<"</nav></section><hr>";
    return ss.str ();
}
