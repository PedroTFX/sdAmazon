#include "table.h"
#include "sdmessage.pb-c.h"

/* Função para preparar um socket de receção de pedidos de ligação
* num determinado porto.
* Retorna o descritor do socket ou -1 em caso de erro.
*/
int network_server_init(short port){
    // socket info
    struct sockaddr_in server_info = {0};
    server_info.sin_family = AF_INET;
    server_info.sin_addr.s_addr = htonl(INADDR_ANY);
    server_info.sin_port = htons(port);
    socklen_t server_info_len = sizeof(server_info);

    // Ignore SIGPIPE signal so the server doesn't crash if the socket closes unexpectedly
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

    // Bind the socket to the address and port
    int bind = bind(sfd, (struct sockaddr*)&server_info, server_info_len);
    if (bind < 0) {
        perror("bind");
        return -1;
    }

    // Listen for incoming connections
    int listen = listen(sfd, 0);
    if (listen < 0) {
        perror("listen");
        return -1;
    }

    return sfd;

}


/* A função network_main_loop() deve:
* - Aceitar uma conexão de um cliente;
* - Receber uma mensagem usando a função network_receive;
* - Entregar a mensagem de-serializada ao skeleton para ser processada
 na tabela table;
* - Esperar a resposta do skeleton;
* - Enviar a resposta ao cliente usando a função network_send.
* A função não deve retornar, a menos que ocorra algum erro. Nesse
* caso retorna -1.
*/
int network_main_loop(int listening_socket, struct table_t *table){
    // Accept a connection
    int client_socket = accept(listening_socket, NULL, NULL);
    if (client_socket < 0) {
        perror("accept");
        return -1;
    }

    // Receive a message
    MessageT *msg = network_receive(client_socket);
    if (msg == NULL) {
        perror("network_receive");
        return -1;
    }

    // Invoke the message
    int invoke = invoke(msg, table);
    if (invoke < 0) {
        perror("invoke");
        return -1;
    }

    // Send the response
    int send = network_send(client_socket, msg);
    if (send < 0) {
        perror("network_send");
        return -1;
    }

    return 0;

}
/* A função network_receive() deve:
* - Ler os bytes da rede, a partir do client_socket indicado;
* - De-serializar estes bytes e construir a mensagem com o pedido,
* reservando a memória necessária para a estrutura MessageT.
* Retorna a mensagem com o pedido ou NULL em caso de erro.
Tal como do lado do cliente, também o servidor tem de receber/enviar um short (2 bytes)
antes receber/enviar as respetivas mensagens, para se saber a dimensão das mensagens (e dos
buffers onde serão recebidas)
*/
MessageT *network_receive(int client_socket){
    // Receive the size of the message
    short msg_size;
    int read_size = read_all(client_socket, (char*)&msg_size, sizeof(short));
    if (read_size < 0) {
        perror("read_all");
        return NULL;
    }

    // Receive the message
    char *msg_buf = (char*)malloc(msg_size);
    read_size = read_all(client_socket, msg_buf, msg_size);
    if (read_size < 0) {
        perror("read_all");
        return NULL;
    }

    // Deserialize the message
    MessageT *msg = message__unpack(NULL, msg_size, (uint8_t*)msg_buf);
    if (msg == NULL) {
        perror("message__unpack");
        return NULL;
    }

    return msg;

}

/* A função network_send() deve:
* - Serializar a mensagem de resposta contida em msg;
* - Enviar a mensagem serializada, através do client_socket.
* Retorna 0 (OK) ou -1 em caso de erro.
*/
int network_send(int client_socket, MessageT *msg){
    // Serialize the message
    int msg_size = message__get_packed_size(msg);
    char *msg_buf = (char*)malloc(msg_size);
    message__pack(msg, (uint8_t*)msg_buf);

    // Send the size of the message
    int write_size = write_all(client_socket, (char*)&msg_size, sizeof(short));
    if (write_size < 0) {
        perror("write_all");
        return -1;
    }

    // Send the message
    write_size = write_all(client_socket, msg_buf, msg_size);
    if (write_size < 0) {
        perror("write_all");
        return -1;
    }

    return 0;
}
/* Liberta os recursos alocados por network_server_init(), nomeadamente
* fechando o socket passado como argumento.
* Retorna 0 (OK) ou -1 em caso de erro.
*/
int network_server_close(int socket){
    return close(socket);
}
