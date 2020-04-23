#ifndef LIBWEBSERVER_URLREQUEST_H
#define LIBWEBSERVER_URLREQUEST_H

#include <string_view>
#include <unordered_map>

namespace webserver {

class UrlRequest {
public:
    explicit UrlRequest (const std::string& remoteIP);
    bool processHttpRequestLine (const std::string& line);

    const std::string& getMethod () const {return method_;}
    const std::string& getRequestTarget () const {return requestTarget_;}
    const std::string& getHost () const {return host_;} //returns host and port (if present)
    const std::string& getBaseUrl () const {return baseUrl_;}
    const std::string& getRemoteIP () const {return remoteIP_;}

    std::size_t getParamCount () const {return params_.size ();}
    bool hasParam (const std::string& name) const {return params_.count (name)>0;}
    std::string getParam (const std::string& name ) const {
        auto itr=params_.find (name);
        if (itr==params_.end ()) return "";
        return itr->second;
    }

protected:
    bool readRequestLine {false}; //First line is the request line (GET, POST etc..). This indicates whether we have already seen this
    std::unordered_map <std::string, std::string> params_;
    std::string remoteIP_;

    bool processRequestLine (const std::string& line);
    bool processHeaderField (const std::string& line);
    bool extractParameters (); //populates params_ from requestTarget
    std::string method_; //GET, POST etc
    std::string requestTarget_; // none host and none port part of the url. eg /index.html
    std::string host_; //contains host and port (if port is present)
    std::string baseUrl_; //the url without parameters
};

}

#endif //LIBWEBSERVER_URLREQUEST_H
