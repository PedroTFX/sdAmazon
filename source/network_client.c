#include "client_stub.h"
#include "sdmessage.pb-c.h"
#include "network_client-private.h"
#include "message-private.h"
#include "client_stub-private.h"
#include "table.h"
#include "table-private.h"
#include "entry.h"
#include "data.h"
#include "inet.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>


/* Esta função deve:
* - Obter o endereço do servidor (struct sockaddr_in) com base na
* informação guardada na estrutura rtable;
* - Estabelecer a ligação com o servidor;
* - Guardar toda a informação necessária (e.g., descritor do socket)
* na estrutura rtable;
* - Retornar 0 (OK) ou -1 (erro).
*/
int network_connect(struct rtable_t *rtable){
    // socket info
    struct sockaddr_in server_info = {0};
    server_info.sin_family = AF_INET;
    
    // Store the IP address and port in struct sockaddr_in
    inet_pton(AF_INET, rtable->server_address, &(server_info.sin_addr));
    server_info.sin_port = htons(rtable->server_port);
    socklen_t server_info_len = sizeof(server_info);

    // Ignore SIGPIPE signal so the client doesn't crash if the socket closes unexpectedly
    struct sigaction new_actn;
    new_actn.sa_handler = SIG_IGN;
    sigemptyset(&new_actn.sa_mask);
    new_actn.sa_flags = 0;
    sigaction(SIGPIPE, &new_actn, NULL);

    // Create a socket
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd < 0) {
        perror("socket");
        return -1;
    }
    rtable->sockfd = sfd;

    // Connect to the server
    int con = connect(sfd, (struct sockaddr*)&server_info, server_info_len);
    if (con == -1) {
        perror("connect");
        return -1;
    }

    return 0;
}

/* Esta função deve:
* - Obter o descritor da ligação (socket) da estrutura rtable_t;
* - Serializar a mensagem contida em msg;
* - Enviar a mensagem serializada para o servidor;
* - Esperar a resposta do servidor;
* - De-serializar a mensagem de resposta;
* - Tratar de forma apropriada erros de comunicação;
* - Retornar a mensagem de-serializada ou NULL em caso de erro.
Tal como do lado do cliente, também o servidor tem de receber/enviar um short (2 bytes)
antes receber/enviar as respetivas mensagens, para se saber a dimensão das mensagens (e dos
buffers onde serão recebidas)
*/
MessageT *network_send_receive(struct rtable_t *rtable, MessageT *msg){
    // Serialize the message
    int msg_size = message__get_packed_size(msg);
    char *msg_buf = (char*)malloc(msg_size);
    message__pack(msg, (uint8_t*)msg_buf);

    // Send the size of the message
    int write_size = write_all(rtable->sockfd, (char*)&msg_size, sizeof(short));
    if (write_size < 0) {
        perror("write_all");
        return NULL;
    }

    // Send the message
    write_size = write_all(rtable->sockfd, msg_buf, msg_size);
    if (write_size < 0) {
        perror("write_all");
        return NULL;
    }

    // MIGHT NEED TO WAIT FOR THE SERVER TO SEND THE RESPONSE BEFORE READING IT

    // Receive the size of the message
    short msg_size2;
    int read_size = read_all(rtable->sockfd, (char*)&msg_size2, sizeof(short));
    if (read_size < 0) {
        perror("read_all");
        return NULL;
    }

    // Receive the message
    char *msg_buf2 = (char*)malloc(msg_size2);
    read_size = read_all(rtable->sockfd, msg_buf2, msg_size2);
    if (read_size < 0) {
        perror("read_all");
        return NULL;
    }

    // Deserialize the message
    MessageT *msg2 = message__unpack(NULL, msg_size2, (uint8_t*)msg_buf2);
    if (msg2 == NULL) {
        perror("message__unpack");
        return NULL;
    }

    return msg2;
}


/* Fecha a ligação estabelecida por network_connect().
* Retorna 0 (OK) ou -1 (erro).
*/
int network_close(struct rtable_t *rtable){
    return close(rtable->sockfd);
}
