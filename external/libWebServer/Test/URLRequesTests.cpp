#include <gtest/gtest.h>
#include <webserver/UrlRequest.h>

//helper class to give unit tests access to some private functions
//used for testing parameter extraction
class TestUrlRequest: public webserver::UrlRequest
{
public:
    TestUrlRequest (const std::string& rt)
    :webserver::UrlRequest ("1.1.1.1")
    {
        requestTarget_=rt;
    }
    bool callExtractParameters () {return extractParameters();}
    bool callProcessRequestLine (const std::string& line) {return processRequestLine (line);}
};


TEST (URLRequestTest, normal_chrome_simple) {
    std::vector <std::string> request =
            {{"GET / HTTP/1.1"},
             {"Host: vmserver03:25000"},
             {"Connection: keep-alive"},
             {"Cache-Control: max-age=0"},
             {"Upgrade-Insecure-Requests: 1"},
             {"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.163 Safari/537.36"},
             {"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9"},
             {"Accept-Encoding: gzip, deflate"},
             {"Accept-Language: en-US,en;q=0.9,nl;q=0.8)"}
            };

    webserver::UrlRequest urlRequest ("1.1.1.1");
    for (const auto&line: request) {
        ASSERT_TRUE (urlRequest.processHttpRequestLine (line));
    }

    ASSERT_EQ (urlRequest.getMethod(),"GET");
    ASSERT_EQ (urlRequest.getRequestTarget(),"/");
    EXPECT_EQ ( urlRequest.getHost(),"vmserver03:25000");
    EXPECT_EQ ( urlRequest.getBaseUrl(), "/");
}

TEST (URLRequestTest, normal_chrome_with_paramaters) {
    std::vector <std::string> request =
            {{"GET /clock?tz=cst&&setting=show%20date HTTP/1.1"},
             {"Host: vmserver03:25000"},
             {"Connection: keep-alive"},
             {"Cache-Control: max-age=0"},
             {"Upgrade-Insecure-Requests: 1"},
             {"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.163 Safari/537.36"},
             {"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9"},
             {"Accept-Encoding: gzip, deflate"},
             {"Accept-Language: en-US,en;q=0.9,nl;q=0.8)"}
            };

    webserver::UrlRequest urlRequest ("1.1.1.1");
    for (const auto&line: request) {
        ASSERT_TRUE (urlRequest.processHttpRequestLine (line));
    }
    ASSERT_EQ (urlRequest.getMethod(),"GET");
    ASSERT_EQ (urlRequest.getRequestTarget(),"/clock?tz=cst&&setting=show%20date");
    ASSERT_EQ ( urlRequest.getHost(),"vmserver03:25000");
    EXPECT_EQ(urlRequest.getParamCount(),2);
    EXPECT_TRUE (urlRequest.hasParam("tz"));
    EXPECT_EQ (urlRequest.getParam("tz"),"cst");
    EXPECT_TRUE (urlRequest.hasParam("setting"));
    EXPECT_EQ (urlRequest.getParam("setting"),"show date");
    EXPECT_EQ ( urlRequest.getBaseUrl(), "/clock");
}

TEST (URLRequestTest, normal_chrome_with_paramaters_check_casing) {
    std::vector <std::string> request =
            {{"GET /Clock?TZ=cst&&Setting=Show%20Date HTTP/1.1"},
             {"Host: vmserver03:25000"},
             {"Connection: keep-alive"},
             {"Cache-Control: max-age=0"},
             {"Upgrade-Insecure-Requests: 1"},
             {"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.163 Safari/537.36"},
             {"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9"},
             {"Accept-Encoding: gzip, deflate"},
             {"Accept-Language: en-US,en;q=0.9,nl;q=0.8)"}
            };

    webserver::UrlRequest urlRequest ("1.1.1.1");
    for (const auto&line: request) {
        ASSERT_TRUE (urlRequest.processHttpRequestLine (line));
    }
    ASSERT_EQ (urlRequest.getMethod(),"GET");
    ASSERT_EQ (urlRequest.getRequestTarget(),"/Clock?TZ=cst&&Setting=Show%20Date");
    ASSERT_EQ ( urlRequest.getHost(),"vmserver03:25000");
    EXPECT_EQ(urlRequest.getParamCount(),2);
    EXPECT_TRUE (urlRequest.hasParam("TZ"));
    EXPECT_EQ (urlRequest.getParam("TZ"),"cst");
    EXPECT_TRUE (urlRequest.hasParam("Setting"));
    EXPECT_EQ (urlRequest.getParam("Setting"),"Show Date");
    EXPECT_EQ ( urlRequest.getBaseUrl(), "/clock");
}

TEST (URLRequestTest, leading_space_in_request_line) {
    std::vector <std::string> request =
            {{" GET / HTTP/1.1"},
             {"Host: vmserver03:25000"},
             {"Connection: keep-alive"},
             {"Cache-Control: max-age=0"},
             {"Upgrade-Insecure-Requests: 1"},
             {"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.163 Safari/537.36"},
             {"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9"},
             {"Accept-Encoding: gzip, deflate"},
             {"Accept-Language: en-US,en;q=0.9,nl;q=0.8)"}
            };

    webserver::UrlRequest urlRequest ("1.1.1.1");
    ASSERT_FALSE (urlRequest.processHttpRequestLine (request [0]));
}

TEST (URLRequestTest, no_space_in_request_line) {
    std::vector <std::string> request =
            {{" GET/HTTP/1.1"},
             {"Host: vmserver03:25000"},
             {"Connection: keep-alive"},
             {"Cache-Control: max-age=0"},
             {"Upgrade-Insecure-Requests: 1"},
             {"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.163 Safari/537.36"},
             {"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9"},
             {"Accept-Encoding: gzip, deflate"},
             {"Accept-Language: en-US,en;q=0.9,nl;q=0.8)"}
            };

    webserver::UrlRequest urlRequest ("1.1.1.1");
    ASSERT_FALSE (urlRequest.processHttpRequestLine (request [0]));
}

TEST (URLRequestTest, trailing_space_in_request_line) {
    std::vector <std::string> request =
            {{"GET/HTTP/1.1 "},
             {"Host: vmserver03:25000"},
             {"Connection: keep-alive"},
             {"Cache-Control: max-age=0"},
             {"Upgrade-Insecure-Requests: 1"},
             {"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.163 Safari/537.36"},
             {"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9"},
             {"Accept-Encoding: gzip, deflate"},
             {"Accept-Language: en-US,en;q=0.9,nl;q=0.8)"}
            };

    webserver::UrlRequest urlRequest ("1.1.1.1");
    ASSERT_FALSE (urlRequest.processHttpRequestLine (request [0]));
}

TEST (URLRequestTest, two_spaces_in_request_line) {
    std::vector <std::string> request =
            {{"GET  / HTTP/1.1"},
             {"Host: vmserver03:25000"},
             {"Connection: keep-alive"},
             {"Cache-Control: max-age=0"},
             {"Upgrade-Insecure-Requests: 1"},
             {"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.163 Safari/537.36"},
             {"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9"},
             {"Accept-Encoding: gzip, deflate"},
             {"Accept-Language: en-US,en;q=0.9,nl;q=0.8)"}
            };

    webserver::UrlRequest urlRequest ("1.1.1.1");
    ASSERT_FALSE (urlRequest.processHttpRequestLine (request [0]));
}

TEST (URLRequestTest, host_no_optional_spaces) {
    std::vector <std::string> request =
            {{"GET / HTTP/1.1"},
             {"Host:vmserver03:25000"},
             {"Connection: keep-alive"},
             {"Cache-Control: max-age=0"},
             {"Upgrade-Insecure-Requests: 1"},
             {"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.163 Safari/537.36"},
             {"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9"},
             {"Accept-Encoding: gzip, deflate"},
             {"Accept-Language: en-US,en;q=0.9,nl;q=0.8)"}
            };

    webserver::UrlRequest urlRequest ("1.1.1.1");
    for (const auto&line: request) {
        ASSERT_TRUE (urlRequest.processHttpRequestLine (line));
    }

    ASSERT_EQ (urlRequest.getMethod(),"GET");
    ASSERT_EQ (urlRequest.getRequestTarget(),"/");
    ASSERT_EQ ( urlRequest.getHost(),"vmserver03:25000");
    EXPECT_EQ ( urlRequest.getBaseUrl(), "/");
}

TEST (URLRequestTest, host_optional_space_at_end) {
    std::vector <std::string> request =
            {{"GET / HTTP/1.1"},
             {"Host:vmserver03:25000 "},
             {"Connection: keep-alive"},
             {"Cache-Control: max-age=0"},
             {"Upgrade-Insecure-Requests: 1"},
             {"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.163 Safari/537.36"},
             {"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9"},
             {"Accept-Encoding: gzip, deflate"},
             {"Accept-Language: en-US,en;q=0.9,nl;q=0.8)"}
            };

    webserver::UrlRequest urlRequest ("1.1.1.1");
    for (const auto&line: request) {
        ASSERT_TRUE (urlRequest.processHttpRequestLine (line));
    }

    ASSERT_EQ (urlRequest.getMethod(),"GET");
    ASSERT_EQ (urlRequest.getRequestTarget(),"/");
    ASSERT_EQ ( urlRequest.getHost(),"vmserver03:25000");
    EXPECT_EQ ( urlRequest.getBaseUrl(), "/");
}

TEST (URLRequestTest, host_both_optional_spaces) {
    std::vector <std::string> request =
            {{"GET / HTTP/1.1"},
             {"Host: vmserver03:25000 "},
             {"Connection: keep-alive"},
             {"Cache-Control: max-age=0"},
             {"Upgrade-Insecure-Requests: 1"},
             {"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.163 Safari/537.36"},
             {"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9"},
             {"Accept-Encoding: gzip, deflate"},
             {"Accept-Language: en-US,en;q=0.9,nl;q=0.8)"}
            };

    webserver::UrlRequest urlRequest ("1.1.1.1");
    for (const auto&line: request) {
        ASSERT_TRUE (urlRequest.processHttpRequestLine (line));
    }

    ASSERT_EQ (urlRequest.getMethod(),"GET");
    ASSERT_EQ (urlRequest.getRequestTarget(),"/");
    ASSERT_EQ ( urlRequest.getHost(),"vmserver03:25000");
    EXPECT_EQ ( urlRequest.getBaseUrl(), "/");
}

TEST (URLRequestTest, host_only_spaces) {
    std::vector <std::string> request =
            {{"GET / HTTP/1.1"},
             {"Host:  "},
             {"Connection: keep-alive"},
             {"Cache-Control: max-age=0"},
             {"Upgrade-Insecure-Requests: 1"},
             {"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.163 Safari/537.36"},
             {"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9"},
             {"Accept-Encoding: gzip, deflate"},
             {"Accept-Language: en-US,en;q=0.9,nl;q=0.8)"}
            };

    webserver::UrlRequest urlRequest ("1.1.1.1");
    for (const auto&line: request) {
        ASSERT_TRUE (urlRequest.processHttpRequestLine (line));
    }

    ASSERT_EQ (urlRequest.getMethod(),"GET");
    ASSERT_EQ (urlRequest.getRequestTarget(),"/");
    ASSERT_EQ ( urlRequest.getHost(),"");
    EXPECT_EQ ( urlRequest.getBaseUrl(), "/");
}

TEST (URLRequestTest, host_one_space) {
    std::vector <std::string> request =
            {{"GET / HTTP/1.1"},
             {"Host: "},
             {"Connection: keep-alive"},
             {"Cache-Control: max-age=0"},
             {"Upgrade-Insecure-Requests: 1"},
             {"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.163 Safari/537.36"},
             {"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9"},
             {"Accept-Encoding: gzip, deflate"},
             {"Accept-Language: en-US,en;q=0.9,nl;q=0.8)"}
            };

    webserver::UrlRequest urlRequest ("1.1.1.1");
    for (const auto&line: request) {
        ASSERT_TRUE (urlRequest.processHttpRequestLine (line));
    }

    ASSERT_EQ (urlRequest.getMethod(),"GET");
    ASSERT_EQ (urlRequest.getRequestTarget(),"/");
    ASSERT_EQ ( urlRequest.getHost(),"");
    EXPECT_EQ ( urlRequest.getBaseUrl(), "/");
}

TEST (URLRequestTest, host_no_data) {
    std::vector <std::string> request =
            {{"GET / HTTP/1.1"},
             {"Host:"},
             {"Connection: keep-alive"},
             {"Cache-Control: max-age=0"},
             {"Upgrade-Insecure-Requests: 1"},
             {"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.163 Safari/537.36"},
             {"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9"},
             {"Accept-Encoding: gzip, deflate"},
             {"Accept-Language: en-US,en;q=0.9,nl;q=0.8)"}
            };

    webserver::UrlRequest urlRequest ("1.1.1.1");
    for (const auto&line: request) {
        ASSERT_TRUE (urlRequest.processHttpRequestLine (line));
    }

    ASSERT_EQ (urlRequest.getMethod(),"GET");
    ASSERT_EQ (urlRequest.getRequestTarget(),"/");
    ASSERT_EQ ( urlRequest.getHost(),"");
    EXPECT_EQ ( urlRequest.getBaseUrl(), "/");
}

TEST (URLRequestTestParamaterExtraction, basic) {
    TestUrlRequest urlRequest ("/");
    ASSERT_TRUE (urlRequest.callExtractParameters());
    ASSERT_EQ (urlRequest.getParamCount(),0);
    EXPECT_EQ ( urlRequest.getBaseUrl(), "/");
}

TEST (URLRequestTestParamaterExtraction, basic2) {
    TestUrlRequest urlRequest ("/hello");
    ASSERT_TRUE (urlRequest.callExtractParameters());
    ASSERT_EQ (urlRequest.getParamCount(),0);
    EXPECT_EQ ( urlRequest.getBaseUrl(), "/hello");
}

TEST (URLRequestTestParamaterExtraction, one_param_simple) {
    TestUrlRequest urlRequest ("/hello?world=1");
    ASSERT_TRUE (urlRequest.callExtractParameters());
    EXPECT_EQ (urlRequest.getParamCount(),1);
    EXPECT_TRUE (urlRequest.hasParam("world"));
    EXPECT_EQ (urlRequest.getParam("world"),"1");
    EXPECT_EQ ( urlRequest.getBaseUrl(), "/hello");
}

TEST (URLRequestTestParamaterExtraction, two_params_simple) {
    TestUrlRequest urlRequest ("/hello?world=1&another=2");
    ASSERT_TRUE (urlRequest.callExtractParameters());
    EXPECT_EQ (urlRequest.getParamCount(),2);
    EXPECT_TRUE (urlRequest.hasParam("world"));
    EXPECT_EQ (urlRequest.getParam("world"),"1");
    EXPECT_TRUE (urlRequest.hasParam("another"));
    EXPECT_EQ (urlRequest.getParam("another"),"2");
    EXPECT_EQ ( urlRequest.getBaseUrl(), "/hello");
}

TEST (URLRequestTestParamaterExtraction, one_param_without_value_simple) {
    TestUrlRequest urlRequest ("/hello?world");
    ASSERT_TRUE (urlRequest.callExtractParameters());
    EXPECT_EQ (urlRequest.getParamCount(),1);
    EXPECT_TRUE (urlRequest.hasParam("world"));
    EXPECT_EQ (urlRequest.getParam("world"),"");
    EXPECT_EQ ( urlRequest.getBaseUrl(), "/hello");
}

TEST (URLRequestTestParamaterExtraction, two_params_without_value_simple) {
    TestUrlRequest urlRequest ("/tests/hello?world&another");
    ASSERT_TRUE (urlRequest.callExtractParameters());
    EXPECT_EQ (urlRequest.getParamCount(),2);
    EXPECT_TRUE (urlRequest.hasParam("world"));
    EXPECT_EQ (urlRequest.getParam("world"),"");
    EXPECT_TRUE (urlRequest.hasParam("another"));
    EXPECT_EQ (urlRequest.getParam("another"),"");
    EXPECT_EQ ( urlRequest.getBaseUrl(), "/tests/hello");
}

TEST (URLRequestTestParamaterExtraction, missing_param) {
    TestUrlRequest urlRequest ("/hello?");
    ASSERT_TRUE (urlRequest.callExtractParameters());
    EXPECT_EQ (urlRequest.getParamCount(),0);
    EXPECT_EQ ( urlRequest.getBaseUrl(), "/hello");
}

TEST (URLRequestTestParamaterExtraction, missing_value) {
    TestUrlRequest urlRequest ("/hello?world=");
    ASSERT_TRUE (urlRequest.callExtractParameters());
    EXPECT_EQ (urlRequest.getParamCount(),1);
    EXPECT_TRUE (urlRequest.hasParam("world"));
    EXPECT_EQ (urlRequest.getParam("world"),"");
    EXPECT_EQ ( urlRequest.getBaseUrl(), "/hello");
}

TEST (URLRequestTestParamaterExtraction, check_param_name_deurlifying) {
    TestUrlRequest urlRequest ("/settings?maxCPU%25=20");
    ASSERT_TRUE (urlRequest.callExtractParameters());
    EXPECT_EQ (urlRequest.getParamCount(),1);
    EXPECT_TRUE (urlRequest.hasParam("maxCPU%"));
    EXPECT_EQ (urlRequest.getParam("maxCPU%"),"20");
    EXPECT_EQ ( urlRequest.getBaseUrl(), "/settings");
}



