//
//  cgi.cpp
//  primhttpd
//
//  Created by Denis Abakumov on 2016-12-23.
//  Copyright © 2016 Denis Abakumov. All rights reserved.
//

#include "cgi.hpp"


std::map<std::string,std::string> env_headers = {{"Host", "HTTP_HOST"}, {"Port", "SERVER_PORT"}, {"Connection", "HTTP_CONNECTION"}, {"Upgrade-Insecure-Requests", "HTTP_UPGRADE_INSECURE_REQUESTS"}, {"User-Agent", "HTTP_USER_AGENT"}, {"Accept", "HTTP_ACCEPT"}, {"Accept-Encoding", "HTTP_ACCEPT_ENCODING"}, {"Accept-Language", "HTTP_ACCEPT_LANGUAGE"}, {"Authorization", "HTTP_AUTHORIZATION"} };


cgi_engine::cgi_engine (std::string cgi_root)
{
    assert (cgi_root.length());
    this->cgi_root = cgi_root;
}


bool cgi_engine::parse_request (http_request * request)
{
    
    std::string name;
    std::string value;
    std::map<std::string, std::string>::iterator it;
    //    std::map<std::string, std::string>::iterator item;
    
    set_env_value("REQUEST_METHOD", request->request_method.c_str());

    set_env_value("REQUEST_URI", request->request_uri.c_str());
    set_env_value("SERVER_PROTOCOL", request->server_protocol.c_str());
    set_env_value("REQUEST_SCHEME", request->request_scheme.c_str());
    set_env_value("SCRIPT_NAME", request->resource_path.c_str());
    set_env_value("QUERY_STRING", request->query_string.c_str());
    
    for (std::map<std::string, std::string>::iterator item = request->headers.begin(); item != request->headers.end(); ++item)
    {
        name = item->first;
        value = item->second;
        it = env_headers.find(name);
        if (it != env_headers.end())
        {
            name = it->second;
            set_env_value(name.c_str(), value.c_str());
        }
    }
    return true;
}


bool cgi_engine::parse_response (std::string text, http_response * response)
{
    std::vector<std::string> lines;
    std::vector<std::string> elements;
    std::string name;
    std::string value;
    std::string line;
    std::string body;
    int empty_lines = 0;
    int status = 0;
    
    assert (text.length());
    assert (response);
    
    if ( (text.length()) && (response) )
    {
        lines = split (text, "\n");
        if (lines[0].substr(0, 7) == "Status:")
        {
            elements = split (lines[0], " ");
            assert (elements.size() > 1);
            if (elements.size() > 1)
                status = stoi (elements[1]);
            else return false;
        }
        else
            status = 200;
        response->set_status(status);
        for (int i = 1; i < lines.size(); i++)
            //std::vector<std::string>::iterator line = lines.begin(); line != lines.end(); ++line)
        {
            line = lines[i];
            if (line.length())
            {
                if (empty_lines == 0)
                {
                    elements = split (line, ": ");
                    if (elements.size() > 1)
                    {
                        name = elements[0];
                        value = elements[1];
                        response->set_header(name, value);
                    }
                }
                else if (empty_lines == 1)
                {
                    body.append(line + "\n");
                }
            }
            else
                empty_lines++;
        }
        response->set_body(body);
    }
    return true;
}


bool cgi_engine::process (http_request * request, http_response * response)
{
    bool result = false;
    std::string file_path;
    std::string output;
    
    assert (request);
    assert (response);
    
    if (request && response)
    {
        file_path = path_join (path_parent(this->cgi_root), request->resource_path);
        if ( test_path(file_path) == IS_FILE )
        {
            if (this->parse_request(request))
            {
                output = exec (file_path.c_str());
                result = this->parse_response(output, response);
            }
        }
        else result = false;
    }
    return result;
}

