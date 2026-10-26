#define MAX_COMMAND_LENGTH 256

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ev.h>

#include "db.h"
#include "db_data_type.h"

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

void get_next_arg(const char* const cmd, char* const arg, uint8_t* const cursor)
{
    bool is_get_first_char = false;

    for(uint8_t i = 0; *cursor < MAX_COMMAND_LENGTH - 1; (*cursor)++)
    {
        if(!is_get_first_char && cmd[*cursor] == ' ')
        {
            continue;
        }

        if(cmd[*cursor] == ' ' || cmd[*cursor] == '\n' || cmd[*cursor] == '\0')
        {
            *cursor += 1;
            arg[i] = '\0';
            //printf("arg: %s\n", arg);
            return;
        }

        arg[i] = cmd[*cursor];
        is_get_first_char = true;
        i++;
    }

    return;
}

void process_command(void** const db, const char* const cmd)
{   
    uint8_t cursor = 0;
    char operation[MAX_COMMAND_LENGTH] = "";
    get_next_arg(cmd, operation, &cursor);

    //printf("Operation: %s\n", operation);

    if(strcmp(operation, "CREATE") == 0){
        char key[MAX_COMMAND_LENGTH] = "";
        char val[MAX_COMMAND_LENGTH] = "";
        get_next_arg(cmd, key, &cursor);
        get_next_arg(cmd, val, &cursor);
        //printf("Key: %s\n",key);
        //printf("Val: %s\n",val);

        if(key[0] == '\0' || val[0]  == '\0')
        {
            printf("Error: Command '%s' requires 2 arguments(%s key value).\n", operation, operation);
            return;
        }

        void* mem = save_data_in_mem(3, 1, (void*)val);

        if(!mem)
        {
            return;
        }

        void* new = CREATE(db, key, 3, 1, mem);

        if(!new)
        {
            return;
        }

        printf("Created\n");
        PRINT(new);

        return;
    }
    else if(strcmp(operation, "READ") == 0)
    {
        char key[MAX_COMMAND_LENGTH] = "";
        get_next_arg(cmd, key, &cursor);

        if(key[0] == '\0')
        {
            printf("Error: Command '%s' requires 1 arguments(%s key).\n", operation, operation);
            return;
        }

        void* res = READ((const void ** const)db, key);

        if(res)
        {
            PRINT(res);
        }

        return;
    }
    else if(strcmp(operation, "UPDATE") == 0)
    {
        char key[MAX_COMMAND_LENGTH] = "";
        char val[MAX_COMMAND_LENGTH] = "";
        get_next_arg(cmd, key, &cursor);
        get_next_arg(cmd, val, &cursor);

        if(key[0] == '\0' || val[0]  == '\0')
        {
            printf("Error: Command '%s' requires 2 arguments(%s key value).\n", operation, operation);
            return;
        }


        void* mem = save_data_in_mem(3, 1, (void*)val);

        if(mem)
        {
            void* new = UPDATE((const void ** const)db, key, 3, 1, mem);
            if(new)
            {
                printf("Updated\n");
                PRINT(new);
            }
        }

        return;
    }
    else if(strcmp(operation, "DELETE") == 0)
    {
        char key[MAX_COMMAND_LENGTH] = "";
        get_next_arg(cmd, key, &cursor);

        if(key[0] == '\0')
        {
            printf("Error: Command '%s' requires 1 arguments(%s key).\n", operation, operation);
            return;
        }

        bool res = DELETE(db, key);
        
        if(res)
        {
            printf("Deleted.\n");
            printf("Key:%s\n", key);
        }

        return;
    }
    else
    {
        printf("Error: Unknown Command'%s'.", operation);
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

        process_command(w->data, buffer);

        printf("\nCommand\t>");
        fflush(stdout);
    }
}

int main(void)
{
    struct ev_loop* loop = EV_DEFAULT;
    void** db = init_db();

    ev_io stdin_watcher;
    stdin_watcher.data = db;
    ev_io_init(&stdin_watcher, stdin_cb, STDIN_FILENO, EV_READ);
    ev_io_start(loop, &stdin_watcher);

    printf("\nCommand\t>");
    fflush(stdout);
    ev_run(loop, 0);

    ev_io_stop(loop, &stdin_watcher);

    // char str1[10] = "hello\0";
    // char str2[10] = "hello";
    // char* ptr = str1;

    // printf("%d\n", strcmp(ptr, "hello") == 0);

    return 0;
}