//
//  utils.hpp
//  primhttpd
//
//  Created by Denis Abakumov on 2016-12-23.
//  Copyright © 2016 Denis Abakumov. All rights reserved.
//

#ifndef utils_hpp
#define utils_hpp

#include <stdio.h>
#include <string.h>    //strlen
#include <stdlib.h>    //strlen
#include <unistd.h>    //access
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <assert.h>
#include <vector>
#include <map>
#include <sys/stat.h>

typedef enum
{
    TRIM_LEFT,
    TRIM_RIGHT,
    TRIM_BOTH,
} TRIM_KIND;

typedef enum
{
    IS_NONE,
    IS_FILE,
    IS_DIR
} PATH_TYPE;

void set_env_value(char const * const name, char const * const value);
char * strip (char * const str);
PATH_TYPE test_path (std::string file_path);
std::string exec(const char* cmd);
std::vector<std::string> split(const std::string& str, const std::string& delim, bool no_empty = false);
std::vector<std::string> path_split (std::string s);
std::string path_join (std::string s1, std::string s2);
std::string path_trim_delim (std::string s, TRIM_KIND trim = TRIM_BOTH);
std::string path_parent(std::string path);
std::string file_ext (std::string file_path);
std::map<std::string, std::string> parse_mime_types (std::string file_path);
bool is_valid_ip (std::string ip_str);

class config
{
public:
    int tcp_port;
    std::string ip_address;
    
    std::string config_root;
    std::string config_file;
    std::string server_root;
    std::string doc_root;
    std::string cgi_dir;
    std::string script_alias;
    std::string error_log;
    std::string access_log;
    std::string log_level;

    config(std::string config_file);
    int read ();

};

#endif /* utils_hpp */
