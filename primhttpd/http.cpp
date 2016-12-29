//
//  httpd_.cpp
//  primhttpd
//
//  Created by Denis Abakumov on 2016-12-23.
//  Copyright © 2016 Denis Abakumov. All rights reserved.
//

#include "http.hpp"

static std::map<std::string, std::string> mime_types;
static config *conf;

bool is_valid_ip (char *ip_str)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ip_str, &(sa.sin_addr));
    return result != 0;
}


int start_server (config * _conf)
{
    int result = 0;
    int socket_desc;
    int client_sock;
    int c;
    int *new_sock;
    struct sockaddr_in server, client;
    in_addr_t in_addr = 0;
    
    assert (_conf);
    if (_conf)
    {
        conf = _conf;
        std::string mime_types_path = path_join (conf->config_root, "mime.types");
        mime_types = parse_mime_types(mime_types_path);

        assert (mime_types.size() > 10);
        
        //Create socket
        socket_desc = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_desc == -1)
        {
            printf("Could not create socket");
            return false;
        }
        
        // Get correct IP address
        if (conf->ip_address == "any")
            in_addr = INADDR_ANY;
        else
            in_addr = inet_addr( conf->ip_address.c_str() );
        
        //Prepare the sockaddr_in structure
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = in_addr;
        server.sin_port = htons( conf->tcp_port );
        
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
        sprintf(port, "PORT: %d\n", conf->tcp_port);
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
                return 1;
            }
            
            //Now join the thread, so that we dont terminate before the thread
            pthread_join( sniffer_thread, NULL);
        }
        
        if (client_sock < 0)
        {
            perror("accept failed");
            return 2;
        }
    }
    return result;
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
    std::string request_file_dir;
    std::string file_path;
    http_request request = http_request();
    http_response response = http_response();
    cgi_engine cgi = cgi_engine(conf->script_alias);
    std::string first_dir;
    
    //Receive a message from client
    bool quit = false;
    while ( (sock > 0) && (! quit) && ((read_size = recv(sock, client_message, 2000, 0)) > 0 ) )
    {
        if (strlen(client_message) == read_size)
        {
            //assert(strlen(client_message) == read_size);
            request.process(client_message, read_size);
            if (request.request_method == "GET")
            {
                // if first element of path in request URI corresponds to cgi dir, use script alias and handle CGI request
                first_dir = path_split(request.resource_path)[0];
                if ( first_dir.length() && (first_dir == path_trim_delim(conf->cgi_dir)) )
                    cgi.process (&request, &response);
                else
                {
                    file_path = path_join(conf->doc_root, request.resource_path);
                    if ( test_path(file_path) == IS_FILE )
                    {
                        get (&request, &response);
                    }
                    else if ( test_path(file_path) == IS_DIR )
                    {
                        ;
                    }
                    else
                    {
                        response.set_status(404);
                        response.set_body("404 Not found\n" + request.resource_path);
                    }
                }
                send_response(sock, &response);
                if ( (request.get_header("Connection") == "close") || (response.get_header("Connection") == "close") )
                    quit = 1;
            }
        }
    }
    
    shutdown( sock, SHUT_WR);
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


int get (http_request * request, http_response * response)
{
    int result = -1;
    assert(request);
    assert(response);
    std::string file_path = path_join(conf->doc_root, request->resource_path);
    std::string file_extension;
    std::string mime_type;
    
    if ( test_path(file_path) == IS_FILE )
    {
        file_extension = file_ext(file_path);
        if (mime_types.find(file_extension) != mime_types.end())
            mime_type = mime_types[file_extension];
        assert (mime_type.length());
        response->set_status(200);
        response->set_header("Content-type", mime_type);
        std::ifstream file(file_path);
        std::string body((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
        response->set_body(body);
        result = 0;
    }
    return result;
}


void send_response(int const socket, http_response * response)
{
    std::string text;
    
    assert (socket);
    assert (response);
    
    if (socket && response)
    {
        text = response->text();
        //std::cout << text;
        write (socket, text.c_str(), text.length());
    }
}
