#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
// #include "table_server-private.h"
#include "network_server.h"
#include "table_skel.h"

int main(int argc, char const *argv[]){
    if(argc != 2){
        printf("Usage: %s <port>\n", argv[0]);
        exit(0);
    }

    short port = (short)atoi(argv[1]);
    if(port == 0){
        printf("Invalid port\n");
        exit(0);
    }

    int listening_socket = network_server_init(port);
    if(listening_socket == -1){
        printf("Error creating listening socket\n");
        exit(0);
    }

    struct table_t *table = table_skel_init(10);
    if(!table){
        printf("Error creating table\n");
        exit(0);
    }

    //listen to interrupt signal
    signal(SIGINT, table_server_close);

    while(1){
        int connection_socket = network_main_loop(listening_socket, table);
        if(connection_socket == -1){
            printf("Error in main loop\n");
            exit(0);
        }
    }

    table_skel_destroy(table);
    close(listening_socket);
    return 0;
}

void table_server_close(int signum){
    if(network_server_close() == -1){
        printf("Error closing server\n");
        exit(0);
    }

    table_skel_destroy();
}