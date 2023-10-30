#include "data.h"
#include "entry.h"
#include "list.h"
#include "table.h"
#include "table-private.h"
#include "list-private.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#define PUT "put"
#define GET "get"
#define DEL "del"
#define SIZE "size"
#define GETKEYS "getkeys"
#define GETTABLE "gettable"
#define QUIT "quit"

// INPUT
// put <key> <data>
// get <key>
// del <key>
// size
// getkeys
// gettable
// quit

int main(int argc, char const* argv[]){
    if(argc < 2){
        printf("Usage: %s <table_size>\n", argv[0]);
        exit(0);
    }

    struct rtable_t* rtable = rtable_connect(argv[1]);
    if(!rtable){
        printf("Error connecting to table\n");
        exit(0);
    }

    char option[1024];

    do{
        show_options();
        readOption(option);
        if(strncmp(option, PUT, strlen(PUT)) == 0){ //------------------PUT    LEAKS
            char key[1024];
            char data[1024];
            readKey(key);
            readData(data);
            if(rtable_put(rtable, key, data) == -1){
                printf("Error putting data\n");
            }
        }else if(strncmp(option, GET, strlen(GET)) == 0){   //--------------GET   LEAKS
            char key[1024];
            readKey(key);
            struct data_t* data = rtable_get(rtable, key);
            if(!data){
                printf("Error getting data\n");
            }else{
                printf("Data: %s\n", data->data);
            }
        }else if(strncmp(option, DEL, strlen(DEL)) == 0){   //--------------DEL   LEAKS
            char key[1024];
            readKey(key);
            if(rtable_del(rtable, key) == -1){
                printf("Error deleting data\n");
            }
        }else if(strncmp(option, SIZE, strlen(SIZE)) == 0){ //--------------SIZE
            int size = rtable_size(rtable);
            if(size == -1){
                printf("Error getting size\n");
            }else{
                printf("Size: %d\n", size);
            }
        }else if(strncmp(option, GETKEYS, strlen(GETKEYS)) == 0){   //--------------GETKEYS
            char** keys = rtable_get_keys(rtable);
            if(!keys){
                printf("Error getting keys\n");
                continue;
            }else{
                if(keys[0] == NULL){
                    printf("No keys\n");
                    continue;
                }

                for(int i = 0; keys[i] != NULL; i++){
                    printf("Key: %s\n", keys[i]);
                }
                rtable_free_keys(keys);
            }
        }else if(strncmp(option, GETTABLE, strlen(GETTABLE)) == 0){ //--------------GETTABLE
            struct table_t* table = rtable_get_table(rtable);
            if(!table){
                printf("Error getting table\n");
                continue;
            }else{
                for(int i = 0; i < table->size; i++){
                    struct list_t* list = table->lists[i];
                    if(list->size == 0){
                        continue;
                    }
                    printf("List %d:\n", i);
                    for(int j = 0; j < list->size; j++){
                        struct entry_t* entry = list->entries[j];
                        printf("\tKey: %s\n", entry->key);
                        printf("\tData: %s\n", entry->value->data);
                    }
                }
                table_destroy(table);
            }

        }else if(strncmp(option, QUIT, strlen(QUIT)) == 0){
            printf("Quitting...\n");
            rtable_disconnect(rtable);
        }else{
            printf("Invalid option\n");
        }
    }while(strncmp(option, QUIT, strlen(QUIT)) != 0);
    return 0;
}

void show_options(){
    printf("Options:\n");
    printf("put <key> <data>\n");
    printf("get <key>\n");
    printf("del <key>\n");
    printf("size\n");
    printf("getkeys\n");
    printf("gettable\n");
    printf("quit\n");
}

void readOption(char* option){
    printf("Option: ");
    fgets(option, 1024, stdin);
    option[strlen(option) - 1] = '\0';
}

void readKey(char* key){
    printf("Key: ");
    fgets(key, 1024, stdin);
    key[strlen(key) - 1] = '\0';
}

void readData(char* data){
    printf("Data: ");
    fgets(data, 1024, stdin);
    data[strlen(data) - 1] = '\0';
}

