//
//  utils.cpp
//  primhttpd
//
//  Created by Denis Abakumov on 2016-12-23.
//  Copyright © 2016 Denis Abakumov. All rights reserved.
//

#include "utils.hpp"

bool file_exists (std::string file_path)
{
    assert (file_path.length());
    if (file_path.length())
        return ( access( file_path.c_str(), F_OK ) != -1 );
    else
        return false;
}


char * strip (char * const str)
{
    unsigned long num = 0;
    
    if (str)
    {
        if (str[0] == ' ')
            while (str[num] != '\0')
            {
                str[num] = str[num+1];
                num++;
            }
        num = strlen(str);
        if (num)
        {
            num--;
            while (num && str[num] == ' ')
                num--;
            str[num+1] = '\0';
        }
    }
    return str;
}


void set_env_value(char const * const name, char const * const value)
{
    char n[1024];
    char v[1024];
    
    if ( value && (strlen(value)) && name && (strlen(name)) )
    {
        strcpy (n, name);
        strcpy (v, value);
        strip(n);
        strip(v);
        
        if (strlen(v))
            setenv(n, v, 1);
    }
    puts(getenv("REQUEST_METHOD"));

}

std::string exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    puts(getenv("REQUEST_METHOD"));
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
    }
    return result;
}


std::vector<std::string> split(const std::string& str, const std::string& delim)
{
    std::vector<std::string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == std::string::npos)
            pos = str.length();
        std::string token = str.substr(prev, pos-prev);
//        if (!token.empty())
            tokens.push_back(token);
        prev = pos + delim.length();
    }
    while (pos < str.length() && prev < str.length());
    return tokens;
}
