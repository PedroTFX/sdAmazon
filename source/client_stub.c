#include "data.h"
#include "entry.h"

/* Remote table, que deve conter as informações necessárias para comunicar
* com o servidor. A definir pelo grupo em client_stub-private.h
*/
// struct rtable_t;

/* Função para estabelecer uma associação entre o cliente e o servidor,
* em que address_port é uma string no formato <hostname>:<port>.
* Retorna a estrutura rtable preenchida, ou NULL em caso de erro.
*/
struct rtable_t *rtable_connect(char *address_port){
    char* adrsport = (char*)malloc(strlen(address_port) + 1);
	strcpy(adrsport, address_port);
	char* hostname = strdup(strtok(adrsport, ":"));
	char* port = strdup(strtok(NULL, ":"));
	struct rtable_t* rtable = (struct rtable_t*)malloc(sizeof(struct rtable_t));
	rtable->server_address = hostname;
	rtable->server_port = port;
	rtable->sockfd = -1;
	// rtable->root = tree_create();
	free(adrsport);
	return network_connect(rtable) == -1 ? NULL : rtable;
}

/* Termina a associação entre o cliente e o servidor, fechando a
* ligação com o servidor e libertando toda a memória local.
* Retorna 0 se tudo correr bem, ou -1 em caso de erro.
*/
int rtable_disconnect(struct rtable_t *rtable){
    if(network_close(rtable) == -1){
        return -1;
    }

    free(rtable->server_address);
    free(rtable->server_port);
    free(rtable);
    return 0;
}

/* Função para adicionar um elemento na tabela.
* Se a key já existe, vai substituir essa entrada pelos novos dados.
* Retorna 0 (OK, em adição/substituição), ou -1 (erro).
*/
int rtable_put(struct rtable_t *rtable, struct entry_t *entry){
    if(!rtable || !entry){
        return -1;
    }
    MessageT* msg = (MessageT*)malloc(sizeof(MessageT));
    msg->opcode = OC_PUT;
    msg->content.entry = entry;
    int result = network_send_receive(rtable, msg);
    free(msg);
    return result;

}


/* Retorna o elemento da tabela com chave key, ou NULL caso não exista
* ou se ocorrer algum erro.
*/
struct data_t *rtable_get(struct rtable_t *rtable, char *key){
    if(!rtable || !key){
        return NULL;
    }
    MessageT* msg = (MessageT*)malloc(sizeof(MessageT));
    msg->opcode = OC_GET;
    msg->content.key = key;
    int result = network_send_receive(rtable, msg);
    free(msg);
    return result == -1 ? NULL : msg->content.data;

}
/* Função para remover um elemento da tabela. Vai libertar
* toda a memoria alocada na respetiva operação rtable_put().
* Retorna 0 (OK), ou -1 (chave não encontrada ou erro).
*/
int rtable_del(struct rtable_t *rtable, char *key){
    if(!rtable || !key){
        return -1;
    }
    MessageT* msg = (MessageT*)malloc(sizeof(MessageT));
    msg->opcode = OC_DEL;
    msg->content.key = key;
    int result = network_send_receive(rtable, msg);
    free(msg);
    return result;

}
/* Retorna o número de elementos contidos na tabela ou -1 em caso de erro.
*/
int rtable_size(struct rtable_t *rtable){
    if(!rtable){
        return -1;
    }
    MessageT* msg = (MessageT*)malloc(sizeof(MessageT));
    msg->opcode = OC_SIZE;
    int result = network_send_receive(rtable, msg);
    free(msg);
    return result == -1 ? -1 : msg->content.result;
}
/* Retorna um array de char* com a cópia de todas as keys da tabela,
* colocando um último elemento do array a NULL.
* Retorna NULL em caso de erro.
*/
char **rtable_get_keys(struct rtable_t *rtable);
/* Liberta a memória alocada por rtable_get_keys().
*/
void rtable_free_keys(char **keys){
    int i = 0;
	while (keys[i] != NULL) {
		printf("%s\n", keys[i]);
		free(keys[i++]);
	}
	free(keys[i]);
	free(keys);
}
/* Retorna um array de entry_t* com todo o conteúdo da tabela, colocando
* um último elemento do array a NULL. Retorna NULL em caso de erro.
*/
struct entry_t **rtable_get_table(struct rtable_t *rtable){
    if(!rtable){
        return NULL;
    }
    MessageT* msg = (MessageT*)malloc(sizeof(MessageT));
    msg->opcode = OC_GET_TABLE;
    int result = network_send_receive(rtable, msg);
    free(msg);
    return result == -1 ? NULL : msg->content.entries;

}
/* Liberta a memória alocada por rtable_get_table().
*/
void rtable_free_entries(struct entry_t **entries){
    int i = 0;
    while (entries[i] != NULL) {
        free(entries[i++]);
    }
    free(entries[i]);
    free(entries);
}