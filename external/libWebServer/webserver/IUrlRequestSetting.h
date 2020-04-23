#ifndef LIBWEBSERVER_IURLREQUESTSETTING_H
#define LIBWEBSERVER_IURLREQUESTSETTING_H

#include <string>

class IUrlRequestSetting {
public:
    ~IUrlRequestSetting()=default;
    virtual IUrlRequestSetting& description (const std::string& d)=0;
    virtual IUrlRequestSetting& visible (bool v)=0; //if true then will show in index and header. Default value is true.
    virtual IUrlRequestSetting& showShortcuts (bool s)=0; //if true then will show a list of short cuts at the top of the generated web page. Default value is true.
    virtual IUrlRequestSetting& displayName (const std::string& n)=0; //the name shown on the shortcut page and default index page
};


#endif //LIBWEBSERVER_IURLREQUESTSETTING_H
