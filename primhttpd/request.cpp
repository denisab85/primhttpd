//
//  request.cpp
//  primhttpd
//
//  Created by Denis Abakumov on 2016-12-23.
//  Copyright © 2016 Denis Abakumov. All rights reserved.
//

#include "request.hpp"


http_request::http_request()
{
    this->request_method.clear();
    this->request_uri.clear();;
    this->request_scheme.clear();;
    this->server_protocol.clear();;
    this->query_string.clear();;
    this->resource_path.clear();;
    this->headers.clear();
    this->body.clear();
}


bool http_request::process(char * const buffer, size_t const len)
{
    int i = 0;
    int n = 0;
    char line[2000];
    int num = 0;
    char * name;
    char * value;
    char request_uri[8000];
    
    while (i < len)
    {
        if ( (buffer[i] == '\r') && (buffer[i+1] == '\n') )
        {
            line[n] = '\0';
            if (strlen(line))
            {
                if (num == 0)   // parse request line
                {
                    if (! strlen (line))
                        return false;
                    value = strtok(line, " ");
                    this->request_method = value;
                    
                    value = strtok(NULL, " ");
                    strcpy(request_uri, value);
                    this->request_uri = value;
                    
                    value = strtok(NULL, " ");
                    this->server_protocol = value;
                    
                    value = strtok(value, "/");
                    this->request_scheme = value;
                    
                    if (strchr(request_uri, '?'))
                    {
                        value = strtok(request_uri, "?");
                        this->resource_path = value;
                        value = strtok(NULL, "");
                        this->query_string = value;
                    }
                    else
                        this->resource_path = request_uri;
                }
                else  // add to headers
                {
                    name = strtok(line, ":");
                    if (! strcmp(name, "Host"))
                    {
                        value = strtok(NULL, ":");
                        this->headers.insert(std::pair<std::string, std::string>("Post", value));
                        
                        value = strtok(NULL, ":");
                        this->headers["Port"] = value;
                    }
                    else
                    {
                        if (! strcmp(name, "Connection"))
                        {
                            value = strtok(NULL, ":");
                        }
                        else if (! strcmp(name, "Upgrade-Insecure-Requests"))
                        {
                            value = strtok(NULL, ":");
                        }
                        else if (! strcmp(name, "User-Agent"))
                        {
                            value = strtok(NULL, ":");
                        }
                        else if (! strcmp(name, "Accept"))
                        {
                            value = strtok(NULL, ":");
                        }
                        else if (! strcmp(name, "Accept-Encoding"))
                        {
                            value = strtok(NULL, ":");
                        }
                        else if (! strcmp(name, "Accept-Language"))
                        {
                            value = strtok(NULL, ":");
                        }
                        else if (! strcmp(name, "Authorization"))
                        {
                            value = strtok(NULL, ":");
                        }
                        this->headers[name] = value;
                    }
                }
            }
            
            n = 0;
            i++;
            num++;
        }
        else
        {
            line[n] = buffer[i];
            n++;
        }
        i++;
    }
    return true;
}
