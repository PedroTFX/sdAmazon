#include <entry.h>
#include <data.h>

/* Função que cria uma entry, reservando a memória necessária e
* inicializando-a com a string e o bloco de dados de entrada.
* Retorna a nova entry ou NULL em caso de erro.
*/
struct entry_t *entry_create(char *key, struct data_t *data){
    struct entry_t* entry = malloc(sizeof(struct entry_t));
	if (!entry) {
		return NULL;
	}
	entry->key = key;
	entry->value = data;
	
	return entry;
}

/* Função que elimina uma entry, libertando a memória por ela ocupada.
* Retorna 0 (OK) ou -1 em caso de erro.
*/
int entry_destroy(struct entry_t *entry){
    if (entry) {
		if (entry->value) {
			data_destroy(entry->value);
		}
		if (entry->key) {
			free(entry->key);
			entry->key = NULL;
		}
		free(entry);
		entry = NULL;
	}

    //TODO test return
    return (entry || entry->value || entry->key) -1 : 0;
}

/* Função que duplica uma entry, reservando a memória necessária para a
* nova estrutura.
* Retorna a nova entry ou NULL em caso de erro.
*/
struct entry_t *entry_dup(struct entry_t *entry){
    if (!entry) {
		return NULL;
	}

	struct entry_t* entry2 = malloc(sizeof(struct entry_t));
	if (!entry2) {	// error init
		return NULL;
	}

	entry2->key = strdup(entry->key);
	entry2->value = data_dup(entry->value);
	return entry2;
}

/* Função que substitui o conteúdo de uma entry, usando a nova chave e
* o novo valor passados como argumentos, e eliminando a memória ocupada
* pelos conteúdos antigos da mesma.
* Retorna 0 (OK) ou -1 em caso de erro.
*/
int entry_replace(struct entry_t *entry, char *new_key, struct data_t *new_value){
    free(entry->key);
	entry->key = new_key;

	data_destroy(entry->value);
	entry->value = new_value;

    //TODO test return
    return (entry-> value == new_value && strcmp(entry->key, new_key) == 0) ? 0 : -1; 
}

/* Função que compara duas entries e retorna a ordem das mesmas, sendo esta
* ordem definida pela ordem das suas chaves.
* Retorna 0 se as chaves forem iguais, -1 se entry1 < entry2,
* 1 se entry1 > entry2 ou -2 em caso de erro.
*/
int entry_compare(struct entry_t *entry1, struct entry_t *entry2){
    int result = strcmp(entry1->key, entry2->key);
	return (result == 0) ? 0 : (result > 0) ? 1 : -1;
}