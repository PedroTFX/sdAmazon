#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "table.h"
#include "list.h"
#include "data.h"
#include "entry.h"
#include "list-private.h"
#include "table-private.h"


/* Função para criar e inicializar uma nova tabela hash, com n
 * linhas (n = módulo da função hash).
 * Retorna a tabela ou NULL em caso de erro.
 */
struct table_t *table_create(int n) {
    struct table_t* table = malloc(sizeof(struct table_t));
    if(!table){
        return NULL;
    }
    table->size = n;
    table->lists = malloc(sizeof(struct list_t*)*n);
    if(!table->lists){
        free(table);
        return NULL;
    }
    for(int i = 0; i < n; i++){
        table->lists[i] = list_create();
        if(!table->lists[i]){
            for(int j = 0; j < i; j++){
                list_destroy(table->lists[j]);
            }
            free(table->lists);
            free(table);
            table = NULL;
            return NULL;
        }
    }
    return table;
}

/* Função que elimina uma tabela, libertando *toda* a memória utilizada
 * pela tabela.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int table_destroy(struct table_t *table) {
    if(!table){
        return -1;
    }
    for(int i = 0; i < table->size; i++){
        list_destroy(table->lists[i]);
    }
    free(table->lists);
    free(table);
    table = NULL;
    return 0;
}

/*
A função de hash a ser usada para mapear as chaves (strings) em índices de listas (inteiros
entre 0 e n-1) pode ser qualquer uma, a decidir pelo grupo. Por exemplo, uma possível função
de hash consiste em somar o valor ASCII de todos os caracteres da chave e depois calcular o
resto da divisão dessa soma por n (i.e, soma(key) % n).
*/
int hash_function(char *key, int size) {
    if(!key || size <= 0){
        return -1;
    }
    int hash = 0;
    for(int i = 0; key[i] != '\0'; i++){
        hash += key[i];
    }
    return hash % size;
}

/* Função para adicionar um par chave-valor à tabela. Os dados de entrada
 * desta função deverão ser copiados, ou seja, a função vai criar uma nova
 * entry com *CÓPIAS* da key (string) e dos dados. Se a key já existir na
 * tabela, a função tem de substituir a entry existente na tabela pela
 * nova, fazendo a necessária gestão da memória.
 * Retorna 0 (ok) ou -1 em caso de erro.
 */
int table_put(struct table_t *table, char *key, struct data_t *value) {
    if(!table || !key || !value){
        return -1;
    }

    //init
    int hash = hash_function(key, table->size);
    struct entry_t* entry = entry_create(key, value);

    return list_add(table->lists[hash], entry);
}

/* Função que procura na tabela uma entry com a chave key. 
 * Retorna uma *CÓPIA* dos dados (estrutura data_t) nessa entry ou 
 * NULL se não encontrar a entry ou em caso de erro.
 */
struct data_t *table_get(struct table_t *table, char *key) {
    if(!table || !key){
        return NULL;
    }

    //get hash
    int hash = hash_function(key, table->size);

    //get data
    struct entry_t* entry = list_get(table->lists[hash], key);
    if(!entry){
        return NULL;
    }
    return data_dup(entry->value);
}

/* Função que remove da lista a entry com a chave key, libertando a
 * memória ocupada pela entry.
 * Retorna 0 se encontrou e removeu a entry, 1 se não encontrou a entry,
 * ou -1 em caso de erro.
 */
int table_remove(struct table_t *table, char *key) {
    if(!table || !key){
        return -1;
    }
    int hash = hash_function(key, table->size);
    return list_remove(table->lists[hash], key);
}

/* Função que conta o número de entries na tabela passada como argumento.
 * Retorna o tamanho da tabela ou -1 em caso de erro.
 */
int table_size(struct table_t *table) {
    if(!table){
        return -1;
    }
    int size = 0;
    for(int i = 0; i < table->size; i++){
        size += list_size(table->lists[i]);
    }
    return size;
}

/* Função que constrói um array de char* com a cópia de todas as keys na 
 * tabela, colocando o último elemento do array com o valor NULL e
 * reservando toda a memória necessária.
 * Retorna o array de strings ou NULL em caso de erro.
 */
char **table_get_keys(struct table_t *table) {
    
    return NULL;
}

/* Função que liberta a memória ocupada pelo array de keys obtido pela 
 * função table_get_keys.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int table_free_keys(char **keys) {
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

