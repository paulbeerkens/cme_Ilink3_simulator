#include "UrlRequest.h"
#include <iostream>
#include <algorithm>
#include "WebUtils.h"

//HTTP spec: https://tools.ietf.org/html/rfc7230#section-3.1

webserver::UrlRequest::UrlRequest(const std::string& remoteIP)
:remoteIP_ (remoteIP) {

}

bool webserver::UrlRequest::processHttpRequestLine(const std::string &line) {

    if (!readRequestLine) {
        readRequestLine=true;
        //The Request-Line is the first line in the http request
        //and is of the format like:
        //GET /info?pageSize=20 HTTP/1.1
        return processRequestLine (line);
    }
    return processHeaderField (line);
}

bool webserver::UrlRequest::processRequestLine(const std::string &line) {
    //Per the spec this is the format:
    //request-line   = method SP request-target SP HTTP-version CRLF

    //find the first space
    auto methodEnd=line.find (' ',0);
    if (methodEnd==std::string::npos) return false;
    if (methodEnd==0) return false; //If the first character is a space. This is invalid http.
    method_=line.substr(0,methodEnd);

    //find the second space which indicated the end of the request-target
    auto requestTargetEnd=line.find (' ',methodEnd+1);
    if (requestTargetEnd==std::string::npos) return false; //no second space
    requestTarget_=line.substr(methodEnd+1,requestTargetEnd-methodEnd-1);
    if (requestTarget_.empty ()) return false; //happens when there two consecutive spaces

    //A correctly formatted http request line should end with HTTP/1.1
    //We don't support any other versions of the http protocol
    if (line.find ("HTTP/1.1", requestTargetEnd+1)!=requestTargetEnd+1) return false;

    return extractParameters (); //gets the parameters if any out of the requestTarget.
}

bool webserver::UrlRequest::processHeaderField(const std::string &line) {
    //header-field   = field-name ":" OWS field-value OWS
    //OWS=Optional White Space
    //There are many fields here. We are only really interested in the host for now
    //so we don't implement a generic version
    constexpr char HOST [] = "Host:";
    constexpr auto HOST_LEN = length (HOST); //length is defined in WebHost it is an constexpr way to get the length

    if (line.rfind (HOST,0) == 0) { //basically check if starts with "Host:".
        //now need to extract the field-value part.
        //Remember there can be an optional space at the beginning and end according to the spec.
        if (line.size ()<=HOST_LEN) { //line is "Host:" no data. Debatable whether we want to fail on this but better to go for robustness than pedantic error checking.
            host_="";
            return true;
        }
        size_t start=HOST_LEN;
        if (line [start]==' ') start++; //remove optional space
        size_t end=line.size ()-1;
        if (line [end]==' ') end--;
        host_=line.substr (start, end-start+1);
    }

    return true;
}

bool webserver::UrlRequest::extractParameters() {
    //the request target is in the form of /url?params
    //example /hello?world=1&&intro=true

    auto paramBegin=requestTarget_.find ('?');
    baseUrl_=requestTarget_.substr(0,paramBegin);
    //convert to lower case because the spec says the url is not case sensitive
    std::transform (baseUrl_.begin (), baseUrl_.end (), baseUrl_.begin (), [] (unsigned char c) {return std::tolower(c);});
    if (paramBegin==std::string::npos) return true; //no parameters in this request target so we are done.

    auto paramChunks=webserver::split (requestTarget_.substr(paramBegin+1, std::string::npos),'&');
    for (const auto& chunk: paramChunks) { //these are param settings like debugLevel=3
        //see if there is a value in here
        auto equalPos=chunk.find ('=');
        if (equalPos==std::string::npos) {
            params_.emplace(chunk,"");
        } else {
            std::string value=chunk.substr(equalPos+1, std::string::npos);
            std::string name=chunk.substr(0,equalPos);
            if (!deUrlify(name)) return false;
            if (!deUrlify(value)) return false;
            params_.emplace (name,value);
        }
    }

    return true;
}
