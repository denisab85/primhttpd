//
//  httpd_.cpp
//  primhttpd
//
//  Created by Denis Abakumov on 2016-12-23.
//  Copyright © 2016 Denis Abakumov. All rights reserved.
//

#include "http.hpp"


bool is_valid_ip (char *ip_str)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ip_str, &(sa.sin_addr));
    return result != 0;
}


bool start_server (char ip_address[15], int tcp_port)
{
    int socket_desc;
    int client_sock;
    int c;
    int *new_sock;
    struct sockaddr_in server, client;
    in_addr_t in_addr = 0;
    
    //Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
        return false;
    }
    
    // Get correct IP address
    if (! strcmp(ip_address, "any"))
        in_addr = INADDR_ANY;
    else
    {
        if (is_valid_ip (ip_address))
            in_addr = inet_addr( ip_address );
        else
            in_addr = INADDR_ANY;
    }
    
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = in_addr;
    server.sin_port = htons( tcp_port );
    
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server, sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return false;
    }
    
    //Listen
    listen(socket_desc, 3);
    
    //Accept an incoming connection
    char port[15];
    sprintf(port, "PORT: %d\n", tcp_port);
    puts(port);
    c = sizeof(struct sockaddr_in);
    
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        pthread_t sniffer_thread;
        new_sock = (int*) malloc(1);
        *new_sock = client_sock;
        
        if( pthread_create( &sniffer_thread, NULL, run_http, (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return false;
        }
        
        //Now join the thread, so that we dont terminate before the thread
        pthread_join( sniffer_thread, NULL);
    }
    
    if (client_sock < 0)
    {
        perror("accept failed");
        return false;
    }
    return true;
}



/*
 * This will handle connection for each client
 * */
void * run_http(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    size_t read_size = 0;
    char client_message[2000];
    char file_path[1024];
    http_request request = http_request();
    http_response response = http_response();
    char * cgi_root = getenv("CGI_ROOT");
    cgi_engine cgi = cgi_engine(cgi_root);
    
    
    //Receive a message from client
    bool quit = false;
    while ( (sock > 0) && (! quit) && ((read_size = recv(sock, client_message, 2000, 0)) > 0 ) )
    {
        
        request.process(client_message, read_size);
        //parse_line(num, line); // parse each request line and setup environment
        
        strcpy(file_path, cgi_root);
        strcat(file_path, request.resource_path.c_str());
        if ( file_exists (file_path) )
        {
            cgi.process (&request, &response);
        }
        else
        {
            response.add_status(404);
            response.add_body(request.resource_path);
        }
        send_response(sock, &response);
        quit = 1;
    }
    
    close(sock);
    sock = 0;
    
    if(read_size == 0)
        fflush(stdout);
    else if(read_size == -1)
        perror("recv failed");
    
    //Free the socket pointer
    free(socket_desc);
    
    return 0;
}


void send_response(int const socket, http_response * response)
{
    std::string text;
    
    assert (socket);
    assert (response);
    
    if (socket && response)
    {
        text = response->text();
        std::cout << text;
        write (socket, text.c_str(), text.length());
    }
}
