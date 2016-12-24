//
//  response.cpp
//  primhttpd
//
//  Created by Denis Abakumov on 2016-12-23.
//  Copyright © 2016 Denis Abakumov. All rights reserved.
//

#include "response.hpp"

std::map<int, std::string> status_codes = { {100, "Continue"}, {101, "Switching Protocols"}, {102, "Processing (WebDAV)"}, {200, "OK"}, {201, "Created"}, {202, "Accepted"}, {203, "Non-Authoritative Information"}, {204, "No Content"}, {205, "Reset Content"}, {206, "Partial Content"}, {207, "Multi-Status (WebDAV)"}, {208, "Already Reported (WebDAV)"}, {226, "IM Used"}, {300, "Multiple Choices"}, {301, "Moved Permanently"}, {302, "Found"}, {303, "See Other"}, {304, "Not Modified"}, {305, "Use Proxy"}, {306, "(Unused)"}, {307, "Temporary Redirect"}, {308, "Permanent Redirect (experiemental)"}, {400, "Bad Request"}, {401, "Unauthorized"}, {402, "Payment Required"}, {403, "Forbidden"}, {404, "Not Found"}, {405, "Method Not Allowed"}, {406, "Not Acceptable"}, {407, "Proxy Authentication Required"}, {408, "Request Timeout"}, {409, "Conflict"}, {410, "Gone"}, {411, "Length Required"}, {412, "Precondition Failed"}, {413, "Request Entity Too Large"}, {414, "Request-URI Too Long"}, {415, "Unsupported Media Type"}, {416, "Requested Range Not Satisfiable"}, {417, "Expectation Failed"}, {418, "I'm a teapot (RFC 2324)"}, {420, "Enhance Your Calm (Twitter)"}, {422, "Unprocessable Entity (WebDAV)"}, {423, "Locked (WebDAV)"}, {424, "Failed Dependency (WebDAV)"}, {425, "Reserved for WebDAV"}, {426, "Upgrade Required"}, {428, "Precondition Required"}, {429, "Too Many Requests"}, {431, "Request Header Fields Too Large"}, {444, "No Response (Nginx)"}, {449, "Retry With (Microsoft)"}, {450, "Blocked by Windows Parental Controls (Microsoft)"}, {451, "Unavailable For Legal Reasons"}, {499, "Client Closed Request (Nginx)"}, {500, "Internal Server Error"}, {501, "Not Implemented"}, {502, "Bad Gateway"}, {503, "Service Unavailable"}, {504, "Gateway Timeout"}, {505, "HTTP Version Not Supported"}, {506, "Variant Also Negotiates (Experimental)"}, {507, "Insufficient Storage (WebDAV)"}, {508, "Loop Detected (WebDAV)"}, {509, "Bandwidth Limit Exceeded (Apache)"}, {510, "Not Extended"}, {511, "Network Authentication Required"}, {598, "Network read timeout error"}, {599, "Network connect timeout error"} };

std::string http_response::text()
{
    std::string text;
    text.append (this->status_line);
    text.append("\r\n");
    for (std::map<std::string, std::string>::iterator item = this->headers.begin(); item != this->headers.end(); ++item)
    {
        text.append(item->first + ": " + item->second);
        text.append("\r\n");
    }
    text.append("\r\n");
    text.append(this->body);
    text.append("\r\n\r\n");

    return text;
}


http_response::http_response ()
{
    
}


http_response::http_response (int const status_code)
{
    
}

void http_response::add_status(int const status_code, std::string status_message)
{
    this->status_line.clear();
    
    assert ( (status_code >= 100) && (status_code < 600) );
    if ( (status_code >= 100) && (status_code < 600) )
    {
        this->status_line.append("HTTP/1.1 ");
        this->status_line.append(std::to_string(status_code));
        this->status_line.append(" ");
        
        if (! status_message.length())
        {
            if (status_codes.find(status_code) != status_codes.end())
                status_message = status_codes[status_code];
            else
            {
                if (status_code <= 199)
                    status_message = "Continue";
                else if (status_code <= 299)
                    status_message = "Success";
                else if (status_code <= 399)
                    status_message = "Redirection";
                else if (status_code <= 499)
                    status_message = "Client Error";
                else if (status_code <= 599)
                    status_message = "Server Error";
            }
        }
        assert (status_message.length());
        this->status_line.append(status_message);
    }
}


void http_response::add_body (std::string body)
{
    this->body = body;
}


void http_response::add_header (std::string name, std::string value)
{
    this->headers[name] = value;
}
