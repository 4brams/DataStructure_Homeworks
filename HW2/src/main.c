#define MAX_COMMAND_LENGTH 256
#define MAX_KEY_LENGTH 256
#define MAX_PHONE_NUMBER_LENGTH 11

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ev.h>

#include "structures.h"

bool is_start_with(const char* const a, const char* const b, const uint8_t length)
{
    bool match = true;
    for(uint8_t i = 0; i < length; i++)
    {
        if(a[i] != b[i])
        {
            match = false;
            return match;
        }
    }

    return match;
}

char* get_one_arg(const char* const cmd, uint8_t* const cursor)
{
    char* arg = (char*)malloc(MAX_COMMAND_LENGTH * sizeof(char));
    bool is_get_first_char = false;

    for(uint8_t i = 0; *cursor < MAX_COMMAND_LENGTH - 1; (*cursor)++)
    {
        if(cmd[*cursor] == ' ' || cmd[*cursor] == '\n' || cmd[*cursor] == '\0')
        {
            if(!is_get_first_char)
            {
                printf("Error: Unexpected space at index:%d\n", *cursor);
                return NULL;
            }
            *cursor += 1;
            arg[i] = '\0';
            //printf("arg: %s\n", arg);
            return arg;
        }

        arg[i] = cmd[*cursor];
        is_get_first_char = true;
        i++;
    }

    return NULL;
}

void process_command(const char* const cmd, struct Node** first)
{   
    //printf("\n%s\n", cmd);
    //printf("\n%d\n", is_start_with(cmd, "CREATE ", 7));
    if(is_start_with(cmd, "CREATE ", 7)){
        uint8_t cursor = 7;
        char* key = get_one_arg(cmd, &cursor);
        //printf("key: %s\n", key);
        struct Communicate* value = (struct Communicate*)malloc(sizeof(struct Communicate));
        value->phone_number = (char*)get_one_arg(cmd, &cursor);
        //printf("val: %s\n", (char*)value);

        if(!(key && value))
        {
            return;
        }

        struct Node* new = create_node(first, key, (void*)value);
        
        printf("Created\n");
        printf("Key: %s\n", new->key);
        printf("Value: %s\n", ((struct Communicate*)(new->value))->phone_number);
        return;
    }
    else if(is_start_with(cmd, "READ ", 5))
    {
        uint8_t cursor = 5;
        char* key = get_one_arg(cmd, &cursor);

        if(!key)
        {
            return;
        }

        struct Node* res = search_node(*first, key);
        
        if(!res)
        {
            printf("Error: Key '%s' DNE.\n", key);
            return;
        }
        printf("Key: %s\n", res->key);
        printf("Val: %s\n", ((struct Communicate*)(res->value))->phone_number);
        return;
    }
    else
    {
        printf("Error: Unknown Command.\n");
        return;
    }
}

static void stdin_cb(EV_P_ ev_io *w, int revents)
{
    (void)revents;
    char buffer[MAX_COMMAND_LENGTH];

    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        //printf("Buffer >%s", buffer);
        //fflush(stdout);

        if(is_start_with(buffer, "EXIT", 4) || is_start_with(buffer, "exit", 4) )
        {
            exit(0);
        }

        struct Node** first = (struct Node**)w->data;
        process_command(buffer, first);

        printf("\nCommand\t>");
        fflush(stdout);
    }
}

int main(void)
{
    struct Node* first = init_list();
    struct ev_loop* loop = EV_DEFAULT;

    ev_io stdin_watcher;
    stdin_watcher.data = &first;
    ev_io_init(&stdin_watcher, stdin_cb, STDIN_FILENO, EV_READ);
    ev_io_start(loop, &stdin_watcher);

    printf("\nCommand\t>");
    fflush(stdout);
    ev_run(loop, 0);

    ev_io_stop(loop, &stdin_watcher);

    return 0;
}