#include <data.h>
#include <stdlib.h>
#include <string.h>

/* Função que cria um novo elemento de dados data_t e que inicializa
* os dados de acordo com os argumentos recebidos, sem necessidade de
* reservar memória para os dados.
* Retorna a nova estrutura ou NULL em caso de erro.
*/
struct data_t* data_create(int size, void *data){
    if(size <= 0 || !data){
        return NULL;
    }

    struct data_t* data_st = malloc(sizeof(struct data_t));
    if(!data_st){
        return NULL;
    }

    data_st->datasize = size;
    data_st->data = data;

    return data_st;
}

/* Função que elimina um bloco de dados, apontado pelo parâmetro data,
* libertando toda a memória por ele ocupada.
* Retorna 0 (OK) ou -1 em caso de erro.
*/
int data_destroy(struct data_t *data){
	if(data){
		if(data->data) {
			free(data->data);
			data->data = NULL;
		}
		free(data);
		data = NULL;
	}

    //TODO test the return
    return (data || data->data) ? -1 : 0;
}

/* Função que duplica uma estrutura data_t, reservando a memória
* necessária para a nova estrutura.
* Retorna a nova estrutura ou NULL em caso de erro.
*/
struct data_t *data_dup(struct data_t *data){
    if (!data || data->datasize <= 0 || !data->data) {
		return NULL;
	}

	struct data_t* data_st = malloc(sizeof(struct data_t));
	if (!data_st) { // error on init
		return NULL;
	}

	data_st->datasize = data->datasize;
	data_st->data = malloc(data->datasize);

	// error on init
	if (!data_st->data) {
		data_destroy(data_st);
		return NULL;
	}
	memcpy(data_st->data, data->data, data->datasize);
	return data_st;
}

/* Função que substitui o conteúdo de um elemento de dados data_t.
* Deve assegurar que liberta o espaço ocupado pelo conteúdo antigo.
* Retorna 0 (OK) ou -1 em caso de erro.
*/
int data_replace(struct data_t *data, int new_size, void *new_data){
    free(data->data);	//reclaim memory
	data->data = new_data;
	data->datasize = new_size;

    //TODO test the return
    return (data->datasize == new_size && data->data == new_data) ? 0 : -1;
}

