//
//  main.cpp
//  primhttpd
//
//  Created by Denis Abakumov on 2016-12-23.
//  Copyright © 2016 Denis Abakumov. All rights reserved.
//

#include <iostream>
#include "http.hpp"


static int port_min = 8001;
static int port_max = 9000;


int main(int argc, char *argv[])
{
    int tcp_port = port_min;
    char ip_address[15];
    
    if (argc > 1)
        setenv("CGI_ROOT", argv[1], 1);
    else
        setenv("CGI_ROOT", "./", 1);
    
    strcpy (ip_address, "any");
    
    while ( (tcp_port < port_max) & ( ! start_server(ip_address, tcp_port) ))
        tcp_port++;
    return 0;
}

