#include <stdlib.h>
#include "data.h"
#include "entry.h"
#include "list.h"
#include "list-private.h"
#include <string.h>
#include <stdio.h>



/* Função que cria e inicializa uma nova lista (estrutura list_t a
* ser definida pelo grupo no ficheiro list-private.h).
* Retorna a lista ou NULL em caso de erro.
*/
struct list_t *list_create(){   //TODO check if works
    struct list_t* list = malloc(sizeof(struct list_t));
    if(!list){
        return NULL;
    }
    list->size = 0;
    list->head = NULL;
    return list; // if failed to create should return NULL
}

//recursive function to destroy a linked list
int node_destroy(struct node_t* node){
    if(!node){
        return -1;
    }
    if(node->next){
        node_destroy(node->next);
    }
    entry_destroy(node->entry);
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

    int ret = node_destroy(list->head);
    free(list);
    return ret;
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

    //head is empty
    if(!list->head){
        list->head = malloc(sizeof(struct node_t));
        if(!list->head){
            return -1;
        }
        list->head->entry = entry;
        list->head->next = NULL;
        list->size++;
        return 0;
    }

    //head is not empty
    struct node_t* current = list->head;
    int comp = entry_compare(current->entry, entry);
    if(comp == 0){
        entry_destroy(current->entry);
        current->entry = entry;
        return 1;
    }else if(comp > 0){
        struct node_t* new = malloc(sizeof(struct node_t));
        if(!new){
            return -1;
        }
        new->entry = entry;
        new->next = list->head;
        list->head = new;
        list->size++;
        return 0;
    }

    //its not in the head
    while(comp < 0 && current->next){
        current = current->next;
        comp = entry_compare(current->entry, entry);
    }


    

    //its in the middle
    if(comp == 0){
        entry_destroy(current->entry);
        current->entry = entry;
        return 1;
    }else if(comp > 0){
        struct node_t* new = malloc(sizeof(struct node_t));
        if(!new){
            return -1;
        }
        new->entry = entry;
        new->next = current->next;
        current->next = new;
        list->size++;
        return 0;
    }
    //its the last one
    if(!current->next){ 
        struct node_t* new = malloc(sizeof(struct node_t));
        if(!new){
            return -1;
        }
        new->entry = entry;
        new->next = NULL;
        current->next = new;
        list->size++;
        return 0;
    }
    return -1;
}



int node_remove_last(struct node_t* node){
    if (!node) {
        return -1;  // Handle the case of an empty list or an invalid node.
    }

    // If the current node is the last node, remove it.
    if (!node->next) {
        entry_destroy(node->entry);
        // free(node);
        return 0;
    }

    // Recursively traverse the list.
    if (node_remove_last(node->next) == 0) {
        // If the last node has been successfully removed,
        // update the next pointer of the current node.
        node->next = NULL;
    }

    return -1;  // Handle any other error case if necessary.
}

//move node to tail
int remove_node(struct node_t* node){
    if(!node){
        return -1;
    }
    if(!node->next){
        // printf("last node\n");
        entry_destroy(node->entry);
        
        node->entry = NULL;
        node->next = NULL;
        node = NULL;
        return 0;
    }
    //trade nodes entries
    // printf("%s", node->entry->key);
    // printf("<-before-> %s\n", node->next->entry->key);

    struct entry_t* temp = node->entry;
    node->entry = node->next->entry;
    node->next->entry = temp;

    // printf("%s", node->entry->key);
    // printf("<-after-> %s\n", node->next->entry->key);
    // printf("----------------------------------------------------------------------------------- %s\n", node->next->entry->key);


    return remove_node(node->next);
}

/* Função que elimina da lista a entry com a chave key, libertando a
* memória ocupada pela entry.
* Retorna 0 se encontrou e removeu a entry, 1 se não encontrou a entry,
* ou -1 em caso de erro.
*/
int list_remove(struct list_t *list, char *key){
    // printf("to_remove: %s\n", key);
    if(!list || !key){
        return -1;
    }
    struct node_t* current = list->head;
    int comp = strcmp(list->head->entry->key, key);
    if(comp == 0){
        int ret = remove_node(list->head);
        if(ret == 0){
            list->size -= 1;
            return 0;
        }
        return -1;
    }


    while(comp != 0 && current->next){
        current = current->next;
        comp = strcmp(current->entry->key, key);
    }
    if(comp != 0){
        return 1;
    }
    int ret = remove_node(current);
    if(ret == 0){
        list->size -= 1;
        return 0;
    }
    return -1;
}

/* Função que obtém da lista a entry com a chave key.
* Retorna a referência da entry na lista ou NULL se não encontrar a
* entry ou em caso de erro.
*/
struct entry_t *list_get(struct list_t *list, char *key){
    if(!list || !key){
        return NULL;
    }

    //init
    struct node_t* current = list->head;
    int comp = strcmp(current->entry->key, key);
    

    //search
    while(comp != 0 && current->next){
        current = current->next;
        comp = strcmp(current->entry->key, key);
    }
    return (comp == 0) ? current->entry : NULL;
}

/* Função que conta o número de entries na lista passada como argumento.
* Retorna o tamanho da lista ou -1 em caso de erro.
*/
int list_size(struct list_t *list){ //this should work
    if(!list){
        return -1;
    }
    return list->size;
}

/* Função que constrói um array de char* com a cópia de todas as keys na
* lista, colocando o último elemento do array com o valor NULL e
* reservando toda a memória necessária.
* Retorna o array de strings ou NULL em caso de erro.
*/
char **list_get_keys(struct list_t *list){
    if(!list || !list->head){
        return NULL;
    }
    char** keys = malloc(sizeof(char*) * (list->size + 1));
    if(!keys){
        return NULL;
    }
    struct node_t* current = list->head;
    int i = 0;
    while(current){
        keys[i] = strdup(current->entry->key);
        current = current->next;
        i++;
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