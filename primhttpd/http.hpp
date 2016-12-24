//
//  httpd_.hpp
//  primhttpd
//
//  Created by Denis Abakumov on 2016-12-23.
//  Copyright © 2016 Denis Abakumov. All rights reserved.
//

#ifndef http_hpp
#define http_hpp

#include <stdio.h>
#include <string.h>    //strlen
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <pthread.h>   //for threading, link with lpthread
#include "utils.hpp"
#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include "request.hpp"
#include "response.hpp"
#include "cgi.hpp"


bool start_server (char ip_address[15], int tcp_port);
void *run_http(void *);

void parse_line(int num, char * line);

void send_response(int const socket, http_response * response);

bool make_status(char line[1024]);

#endif /* http_hpp */
