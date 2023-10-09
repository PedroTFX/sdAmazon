
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
    if(n <= 0){
        return NULL;
    }

    struct table_t *table = malloc(sizeof(struct table_t));
    if (!table) {
        return NULL; 
    }
    
    table->size = n;
    table->lists = malloc(sizeof(struct list_t *) * n);
    if (!table->lists) {
        free(table);
        return NULL; 
    }
    
    for (int i = 0; i < n; i++) {
        table->lists[i] = NULL;
    }
    
    return table;
}

/* Função que elimina uma tabela, libertando *toda* a memória utilizada
 * pela tabela.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int table_destroy(struct table_t *table) {
    if (!table) {
        return -1; 
    }
    
    for (int i = 0; i < table->size; i++) {
        if (table->lists[i]) {
            list_destroy(table->lists[i]);
        }
    }
    
    free(table->lists);
    free(table);
    return 0;
}

//deve ser uma funcao auxiliar
int hash_function(char *key, int size) {
    int sum = 0;
    for (int i = 0; key[i] != '\0'; i++) {
        sum += key[i];
    }
    return sum % size;
}

/* Função para adicionar um par chave-valor à tabela. Os dados de entrada
 * desta função deverão ser copiados, ou seja, a função vai criar uma nova
 * entry com *CÓPIAS* da key (string) e dos dados. Se a key já existir na
 * tabela, a função tem de substituir a entry existente na tabela pela
 * nova, fazendo a necessária gestão da memória.
 * Retorna 0 (ok) ou -1 em caso de erro.
 */
int table_put(struct table_t *table, char *key, struct data_t *value) {
    if (!table || !key || !value) {
        return -1; 
    }

    // get list by hash_index
    int hash_index = hash_function(key, table->size);
    if (!table->lists[hash_index]) {
        table->lists[hash_index] = list_create();
        if (!table->lists[hash_index]) {
            return -1; 
        }
    }

    // dup values
    char *key_dup = strdup(key);
    if (!key_dup) {
        return -1; 
    }

    struct data_t *value_dup = data_dup(value);
    if (!value_dup) {
        free(key_dup);
        return -1; 
    }

    // add entry to list and return result value
    if(list_add(table->lists[hash_index], entry_create(key_dup, value_dup)) == -1){
        return -1;
    }
    return 0;
}

/* Função que procura na tabela uma entry com a chave key. 
 * Retorna uma *CÓPIA* dos dados (estrutura data_t) nessa entry ou 
 * NULL se não encontrar a entry ou em caso de erro.
 */
struct data_t *table_get(struct table_t *table, char *key) {
    if (!table || !key) {
        return NULL; 
    }
    
    int index = hash_function(key, table->size);
    
    if (!table->lists[index]) {
        return NULL; 
    }
    
    struct entry_t *entry = list_get(table->lists[index], key);
    
    if (!entry) {
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
    if (!table || !key) {
        return -1; 
    }
    
    int index = hash_function(key, table->size);
    
    if (!table->lists[index]) {
        return 1; 
    }
    
    int result = list_remove(table->lists[index], key);
    
    return result;
}

/* Função que conta o número de entries na tabela passada como argumento.
 * Retorna o tamanho da tabela ou -1 em caso de erro.
 */
int table_size(struct table_t *table) {
    if (!table) {
        return -1; 
    }
    
    int size = 0;
    
    for (int i = 0; i < table->size; i++) {
        if (table->lists[i]) {
            size += list_size(table->lists[i]); //should we check for null cases of non null list?
        }
    }
    
    return size;
}

/* Função que constrói um array de char* com a cópia de todas as keys na 
 * tabela, colocando o último elemento do array com o valor NULL e
 * reservando toda a memória necessária.
 * Retorna o array de strings ou NULL em caso de erro.
 */
char **table_get_keys(struct table_t *table) {
    if (!table) {
        return NULL; 
    }
    
    char **keys = malloc(sizeof(char *) * (table_size(table) + 1));
    if (!keys) {
        return NULL; 
    }
    
    int index = 0;
    
    for (int i = 0; i < table->size; i++) {
        if (table->lists[i]) {
            char **list_keys = list_get_keys(table->lists[i]);
            if (list_keys) {
                for (int j = 0; list_keys[j] != NULL; j++) {
                    keys[index++] = strdup(list_keys[j]);
                }
                list_free_keys(list_keys);
            }
        }
    }
    
    keys[index] = NULL; 
    
    return keys;
}

/* Função que liberta a memória ocupada pelo array de keys obtido pela 
 * função table_get_keys.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int table_free_keys(char **keys) {
    if (!keys) {
        return -1; 
    }
    
    for (int i = 0; keys[i] != NULL; i++) {
        free(keys[i]);
    }
    
    free(keys);
    return 0;
}

