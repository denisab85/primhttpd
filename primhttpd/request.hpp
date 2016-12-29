//
//  request.hpp
//  primhttpd
//
//  Created by Denis Abakumov on 2016-12-23.
//  Copyright © 2016 Denis Abakumov. All rights reserved.
//

#ifndef request_hpp
#define request_hpp

#include <stdio.h>
#include <map>
#include <string>
#include <assert.h>


class http_request
{
public:
    http_request();
    bool process(char * const buffer, size_t const len);
    std::string get_header(std::string header_name);
    
    std::string request_method;
    std::string request_uri;
    std::string request_scheme;
    std::string server_protocol;
    std::string query_string;
    std::string resource_path;
    std::map<std::string,std::string> headers;
    std::string body;
} ;


#endif /* request_hpp */
