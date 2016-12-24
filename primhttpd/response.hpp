//
//  response.hpp
//  primhttpd
//
//  Created by Denis Abakumov on 2016-12-23.
//  Copyright © 2016 Denis Abakumov. All rights reserved.
//

#ifndef response_hpp
#define response_hpp

#include <stdio.h>
#include <string.h>
#include <vector>
#include <string>
#include <assert.h>
#include <map>


class http_response
{
public:
    http_response ();
    http_response (int const status_code);
    void add_status(int const status_code, std::string status_message = "");
    void add_body (std::string body);
    void add_header (std::string name, std::string value);
    std::string text();
    std::string status_line;
    std::map<std::string, std::string> headers;
    std::string body;
} ;



#endif /* response_hpp */
