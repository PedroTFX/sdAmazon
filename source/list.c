#include <stdlib.h>
#include "data.h"
#include "entry.h"
#include "list.h"
#include "list-private.h"
#include <string.h>



/* Função que cria e inicializa uma nova lista (estrutura list_t a
* ser definida pelo grupo no ficheiro list-private.h).
* Retorna a lista ou NULL em caso de erro.
*/
struct list_t *list_create(){   //TODO check if works
    struct list_t* list = malloc(sizeof(struct list_t));
    if(!list){
        return NULL;
    }
    return list; // if failed to create should return NULL
}

/* Função que elimina uma lista, libertando *toda* a memória utilizada
* pela lista.
* Retorna 0 (OK) ou -1 em caso de erro.
*/
int list_destroy(struct list_t *list){ //TODO test
    if(list){
        if(list->next){
            list_destroy(list->next);
        }
        if(list->entry){
            entry_destroy(list->entry);
        }
        free(list);
        list = NULL;
    }
    return (list || list->entry || list->next) ? -1 : 0;
}

/* Função que adiciona à lista a entry passada como argumento.
* A entry é inserida de forma ordenada, tendo por base a comparação
* de entries feita pela função entry_compare do módulo entry e
* considerando que a entry menor deve ficar na cabeça da lista.
* Se já existir uma entry igual (com a mesma chave), a entry
* já existente na lista será substituída pela nova entry,
* sendo libertada a memória ocupada pela entry antiga.
* Retorna 0 se a entry ainda não existia, 1 se já existia e foi
* substituída, ou -1 em caso de erro.
*/
int list_add(struct list_t *list, struct entry_t *entry){ //TODO test
    if(!list || !entry){
        return -1;
    }

    if(!list->entry){ //entry null means new list return should be 0
        list->entry = entry_create(entry->key, entry->value); // this should make leaks
        return (list->entry) ? 0 : -1;
    }

    int comp = entry_compare(list->entry, entry);
    if(comp == 0){ // replace list entry return should be 1
        return entry_replace(list->entry, entry->key, entry->value) == 0 ? 1 : -1;
    }
    if(!list->next){
        list->next = list_create(); // correct ??
    }
    if(comp == -1){ //entry < list->entry
        return list_add(list->next, list->entry);
    }
    if(comp == 1){ //entry > list->entry
        return list_add(list->next, entry);
    }
    return -1;
}

/* Função que elimina da lista a entry com a chave key, libertando a
* memória ocupada pela entry.
* Retorna 0 se encontrou e removeu a entry, 1 se não encontrou a entry,
* ou -1 em caso de erro.
*/
int list_remove(struct list_t *list, char *key){
    if(!list || !key){
        return -1;
    }

    //find the prev
    struct list_t* prev = list;
    while(prev->next && strcmp(prev->next->entry->key, key) != 0){
        prev = prev->next;
    }

    //get the next next
    struct list_t* next_next = prev->next->next;

    //remove the next (key)
    entry_destroy(prev->next->entry);
    free(prev->next);

    //set the prev next to the next next
    prev->next = next_next;

    return 0;
}

/* Função que obtém da lista a entry com a chave key.
* Retorna a referência da entry na lista ou NULL se não encontrar a
* entry ou em caso de erro.
*/
// struct entry_t *list_get(struct list_t *list, char *key){
//     int comp = strcmp(list->entry->key, key);
//     return comp == 0 ? list->entry : ((comp > 0 && list->next) ? list_get(list->next, key) : NULL); //TODO test
// }

/* Função que conta o número de entries na lista passada como argumento.
* Retorna o tamanho da lista ou -1 em caso de erro.
*/
int list_size(struct list_t *list){ //this should work
    if(!list){
        return -1;
    }

    struct list_t* current_list = list;
    int count = 0;
    while(current_list->next){
        count++;
        current_list = current_list->next;
    }
    return count;
}

/* Função que constrói um array de char* com a cópia de todas as keys na
* lista, colocando o último elemento do array com o valor NULL e
* reservando toda a memória necessária.
* Retorna o array de strings ou NULL em caso de erro.
*/
char **list_get_keys(struct list_t *list){
    int size = list_size(list);
    char** keys = malloc(sizeof(char*) * (size + 1));
    if(!keys){
        return NULL;
    }

    struct list_t* current_list = list;
    int i = 0;
    while(current_list->next){
        keys[i] = strdup(current_list->entry->key);
        if(!keys[i]){
            return NULL;
        }
        i++;
        current_list = current_list->next;
    }
    keys[i] = NULL;
    return keys;
}

/* Função que liberta a memória ocupada pelo array de keys obtido pela
* função list_get_keys.
* Retorna 0 (OK) ou -1 em caso de erro.
*/
int list_free_keys(char **keys){
    if(!keys){
        return -1;
    }

    int i = 0;
    while(keys[i]){
        free(keys[i]);
        i++;
    }
    free(keys);
    return 0;
}