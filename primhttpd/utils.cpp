//
//  utils.cpp
//  primhttpd
//
//  Created by Denis Abakumov on 2016-12-23.
//  Copyright © 2016 Denis Abakumov. All rights reserved.
//

#include "utils.hpp"


#if defined(WIN32) || defined(_WIN32)
#define PATH_DELIMITER "\\"
#else
#define PATH_DELIMITER "/"
#endif


std::map<std::string, std::string> parse_mime_types (std::string file_path)
{
    std::map<std::string, std::string> result;
    std::vector<std::string> tokens;
    std::string mime_type;
    std::ifstream mime_types_file(file_path);
    
    assert (test_path(file_path) == IS_FILE);
    if(test_path(file_path) == IS_FILE)
    {
        for (std::string line; std::getline(mime_types_file, line);)
        {
            if (line.length() && line[0] != '#')
            {
                tokens = split (line, "\t", true);
                if (tokens.size() > 1)
                {
                    mime_type = tokens[0];
                    tokens = split (tokens[1], " ", true);
                    for (std::string ext : tokens)
                        result[ext] = mime_type;
                }
            }
        }
    }
    return result;
}


std::string file_ext (std::string file_path)
{
    std::string result;
    size_t last_dot_pos = 0;
    size_t last_delim_pos = 0;
    
    last_dot_pos = file_path.find_last_of(".");
    last_delim_pos = file_path.find_last_of(PATH_DELIMITER);
    
    if (last_delim_pos < last_dot_pos)
    {
        result = file_path.substr(last_dot_pos + 1);
    }
    
    return result;
}


std::vector<std::string> path_split (std::string s)
{
    std::vector<std::string> result = split(s, PATH_DELIMITER, true);
    return result;
}


std::string path_trim_delim (std::string s, TRIM_KIND kind)
{
    std::string result = s;
    
    if (kind == TRIM_LEFT || kind == TRIM_BOTH) {
        // trim leading path delimiters if any
        while ( result.length() && (! result.compare(0, 1, PATH_DELIMITER)) )
            result.erase(result.begin());
    }
    
    if (kind == TRIM_RIGHT || kind == TRIM_BOTH) {
        // trim trailing path delimiters if any
        while ( result.length() && (! result.compare(result.length()-1, 1, PATH_DELIMITER)) )
            result.pop_back();
    }
    return result;
}

std::string path_join (std::string s1, std::string s2)
{
    std::string result = path_trim_delim (s1, TRIM_RIGHT);
    if (s2.length())
        result += PATH_DELIMITER + path_trim_delim (s2);
    return result;
}


PATH_TYPE test_path (std::string file_path)
{
    PATH_TYPE result = IS_NONE;
    struct stat s;

    assert (file_path.length());
    if (file_path.length())
    {
        if( stat(file_path.c_str(), &s) == 0 )
        {
            if( s.st_mode & S_IFDIR )
                result = IS_DIR;
            else if( s.st_mode & S_IFREG )
                result = IS_FILE;
        }
    }
    return result;
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
}

std::string exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
    }
    return result;
}


std::vector<std::string> split(const std::string& str, const std::string& delim, bool no_empty)
{
    std::vector<std::string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == std::string::npos)
            pos = str.length();
        std::string token = str.substr(prev, pos-prev);
        if (!token.empty() || ! no_empty)
            tokens.push_back(token);
        prev = pos + delim.length();
    }
    while (pos < str.length() && prev < str.length());
    return tokens;
}

std::string path_parent(std::string path)
{
    std::string result = path_trim_delim(path, TRIM_RIGHT);
    size_t last_delim = result.find_last_of(PATH_DELIMITER);
    result = result.substr(0, last_delim+1);
    
    if (result == "" && ! path.compare(0, 1, PATH_DELIMITER))
        result = PATH_DELIMITER;
    return result;
}


config::config(std::string config_file)
{
    assert((test_path(config_file) == IS_FILE));
    if (test_path(config_file) == IS_FILE)
    {
        this->config_file = config_file;
        this->config_root = path_parent(config_file);
    }
}


bool is_valid_ip (std::string ip_str)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ip_str.c_str(), &(sa.sin_addr));
    return result != 0;
}


int config::read ()
{
    int result = 0;
    std::vector<std::string> tokens;
    std::string name;
    std::string value;
    
    std::ifstream config_file(this->config_file);
    
    for (std::string line; std::getline(config_file, line);)
    {
        if (line.length() && line[0] != '#')
        {
            tokens = split (line, " ", true);
            if (tokens.size() > 1)
            {
                name = tokens[0];
                if (name == "ServerRoot") {
                    value = tokens[1];
                    this->server_root = value;
                }
                else if (name == "ScriptAlias")
                {
                    if (tokens.size() > 2)
                    {
                        this->cgi_dir = tokens[1];
                        this->script_alias = tokens[2];
                    }
                }
                else if (name == "DocumentRoot") {
                    value = tokens[1];
                    this->doc_root = value;
                }
                else if (name == "Listen")
                {
                    tokens = split(tokens[1], ":");
                    if (tokens.size() > 1)
                    {
                        this->tcp_port = std::stoi(tokens[1]);

                        if (is_valid_ip (tokens[0]))
                            this->ip_address = tokens[0];
                        else
                            this->ip_address = "any";
                    }
                    else
                    {
                        this->ip_address = "any";
                        this->tcp_port = std::stoi(tokens[0]);
                    }
                }
                else if (name == "ErrorLog") {
                    value = tokens[1];
                    this->error_log = value;
                }
                else if (name == "LogLevel") {
                    value = tokens[1];
                    this->log_level = value;
                }
            }
        }
    }
    return result;
}
