//
//  cgi.hpp
//  primhttpd
//
//  Created by Denis Abakumov on 2016-12-23.
//  Copyright © 2016 Denis Abakumov. All rights reserved.
//

#ifndef cgi_hpp
#define cgi_hpp


#include <stdio.h>
#include <map>
#include "request.hpp"
#include "response.hpp"
#include "utils.hpp"
#include <assert.h>
#include <vector>


class cgi_engine
{
private:
    std::string cgi_root;
    bool parse_request (http_request * request);
    bool parse_response (std::string text, http_response * response);
    
public:
    cgi_engine (std::string cgi_root);
    bool process (http_request * request, http_response * response);
    
};


#endif /* cgi_hpp */
