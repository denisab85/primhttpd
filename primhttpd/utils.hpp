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
#include <string>
#include <sstream>
#include <iostream>
#include <assert.h>
#include <vector>


void set_env_value(char const * const name, char const * const value);
char * strip (char * const str);
bool file_exists (std::string file_path);
std::string exec(const char* cmd);
std::vector<std::string> split(const std::string& str, const std::string& delim);


#endif /* utils_hpp */
