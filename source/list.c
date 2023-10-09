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

int node_destroy(struct node_t *node){
    if(!node){
        return -1;
    }

    if(node->next){
        node_destroy(node->next);
    }
    if(node->entry){
        entry_destroy(node->entry);
    }
    free(node);
    return 0;
}

/* Função que elimina uma lista, libertando *toda* a memória utilizada
* pela lista.
* Retorna 0 (OK) ou -1 em caso de erro.
*/
int list_destroy(struct list_t *list){ //TODO test
    if(!list || !list->head){
        return -1;
    }

    int return_value_node = node_destroy(list->head);
    free(list);

    return return_value_node;

}

int add_entry_on_node_list(struct node_t *node, struct entry_t *entry){
    if(!node || !entry){
        return -1;
    }

    if(!node->entry){
        node->entry = entry;
        node->next = NULL;
        return 0;
    }

    int comp = entry_compare(node->entry, entry);
    if(comp == 0){ // replace entry
        entry_replace(node->entry, entry->key, entry->value);
        return 1;
    }
    if(comp < 0){
        if(!node->next){
            node->next = malloc(sizeof(struct node_t));
            if(!node->next){
                return -1;
            }
        }
        return add_entry_on_node_list(node->next, entry);
    }if(comp > 0){
        struct node_t* aux = malloc(sizeof(struct node_t));
        if(!aux){
            return -1;
        }
        aux->entry = entry;
        aux->next = node;
        node = aux;
        return 0;
    }
    return -1;
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
    
    //
    if(!list->head){
        list->head = malloc(sizeof(struct node_t));
        list->head->entry = entry; 
        list->head->next = NULL;
        list->size = 1;
        return 0;
    }

    //add entry
    int ret = add_entry_on_node_list(list->head, entry);
    if(ret != -1){
        list->size++;
    }
    return ret;
}

/* Função que elimina da lista a entry com a chave key, libertando a
* memória ocupada pela entry.
* Retorna 0 se encontrou e removeu a entry, 1 se não encontrou a entry,
* ou -1 em caso de erro.
*/
int list_remove(struct list_t *list, char *key){
    if(!list || !list->head || !key){
        return -1;
    }

    struct node_t* node = list->head;
    if(strcmp(node->entry->key, key) == 0){
        list->head = node->next;
        entry_destroy(node->entry);
        free(node);
        return 0;
    }

    while(node->next){
        int comp = strcmp(node->next->entry->key, key);
        if(comp == 0){
            struct node_t* aux = node->next->next;
            node->next = aux;
            entry_destroy(node->next->entry);
            free(node->next);
            return 0;
        }
    }
    return 1;
}

/* Função que obtém da lista a entry com a chave key.
* Retorna a referência da entry na lista ou NULL se não encontrar a
* entry ou em caso de erro.
*/
struct entry_t *list_get(struct list_t *list, char *key){
    if(!list || !list->head || !key){
        return NULL;
    }

    struct node_t* node = list->head;
    int comp = strcmp(node->entry->key, key);
    if(comp == 0){
        return node->entry;
    }else if(comp > 0){
        return NULL;
    }else{
        while(node->next){
            comp = strcmp(node->next->entry->key, key);
            if(comp == 0){
                return node->next->entry;
            }else if(comp > 0){
                return NULL;
            }
            node = node->next;
        }
        return NULL;
    }
}

/* Função que conta o número de entries na lista passada como argumento.
* Retorna o tamanho da lista ou -1 em caso de erro.
*/
int list_size(struct list_t *list){ //this should work
    if(!list){
        return -1;
    }

    int size = 0;
    struct node_t* node = list->head;
    while(node){
        size++;
        node = node->next;
    }
    return size;
}

/* Função que constrói um array de char* com a cópia de todas as keys na
* lista, colocando o último elemento do array com o valor NULL e
* reservando toda a memória necessária.
* Retorna o array de strings ou NULL em caso de erro.
*/
char **list_get_keys(struct list_t *list){
    if(!list){
        return NULL;
    }

    int size = list_size(list);
    char** keys = malloc(sizeof(char*) * (size + 1));
    if(!keys){
        return NULL;
    }

    struct node_t* node = list->head;
    int i = 0;
    while(node){
        keys[i] = strdup(node->entry->key);
        if(!keys[i]){
            list_free_keys(keys);
            return NULL;
        }
        i++;
        node = node->next;
    }
    keys[i] = NULL;
    if(i == 0){
        return NULL;
    }

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