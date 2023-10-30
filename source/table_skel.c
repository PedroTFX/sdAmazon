#include "table.h"
#include "sdmessage.pb-c.h"
#include "table_skel.h"
#include "message-private.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>



/* Inicia o skeleton da tabela.
* O main() do servidor deve chamar esta função antes de poder usar a
* função invoke(). O parâmetro n_lists define o número de listas a
* serem usadas pela tabela mantida no servidor.
* Retorna a tabela criada ou NULL em caso de erro.
*/
struct table_t *table_skel_init(int n_lists){
    return table_create(n_lists);
}
/* Liberta toda a memória ocupada pela tabela e todos os recursos
* e outros recursos usados pelo skeleton.
* Retorna 0 (OK) ou -1 em caso de erro.
*/
int table_skel_destroy(struct table_t *table){
    return table_destroy(table);
}
/* Executa na tabela table a operação indicada pelo opcode contido em msg
* e utiliza a mesma estrutura MessageT para devolver o resultado.
* Retorna 0 (OK) ou -1 em caso de erro.
*/
int invoke(MessageT *msg, struct table_t *table){
    if(!msg || !table){
        return -1;
    }
    switch(msg->opcode){
        case OC_PUT:
            return table_put(table, msg->content.entry->key, msg->content.entry->value);
        case OC_GET:
            msg->content.data = table_get(table, msg->content.key);
            return 0;
        case OC_DEL:
            return table_del(table, msg->content.key);
        case OC_SIZE:
            msg->content.result = table_size(table);
            return 0;
        case OC_GETKEYS:
            msg->content.keys = table_get_keys(table);
            return 0;
        case OC_GETTABLE:
            msg->content.table = table_get_table(table);
            return 0;
        default:
            return -1;
    }
}
