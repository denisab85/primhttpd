//
//  main.cpp
//  primhttpd
//
//  Created by Denis Abakumov on 2016-12-23.
//  Copyright © 2016 Denis Abakumov. All rights reserved.
//

#include <iostream>
#include "http.hpp"


int main(int argc, char *argv[])
{
    int result = 0;
    config conf = config("/Users/denisab85/Documents/primhttpd/primhttpd/etc/primhttpd/primhttpd.conf");
    result = conf.read();
    if (! result)
        result = start_server(&conf);
    return result;
}


//    setenv("DOC_ROOT", "/Users/denisab85/Downloads/rar/w_ob2005", 1);
//
//    setenv("CGI_ROOT", "/cgi-bin", 1);
//
//    setenv("CONFIG_ROOT", "/Users/denisab85/Documents/primhttpd/primhttpd/etc/primhttpd", 1);
//
//    while ( (tcp_port < port_max) & ( ! start_server(ip_address, tcp_port) ))
//        tcp_port++;
