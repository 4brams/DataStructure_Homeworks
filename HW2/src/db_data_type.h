#define MAX_PHONE_NUMBER_LENGTH 11

struct Communicate
{
    char phone_number[MAX_PHONE_NUMBER_LENGTH];
};

static void* get_data_mem(const uint8_t type, const uint16_t length)
{   
    if(length == 0)
    {
        return NULL;
    }

    switch (type)
    {
    case 1:
        return (void *)malloc(length * sizeof(int));
    case 2:
        return (void *)malloc(length * sizeof(char));
    case 3:
        return (void *)malloc(length * sizeof(struct Communicate));
    case 0:
    default:
        return NULL;
    }
}

// uint8_t data_type_id(char* str){
//     if(strcmp(str, "INT") == 0)
//     {
//         return 1;
//     }
//     else if(strcmp(str, "CHAR") == 0)
//     {
//         return 2;
//     }
//     else if(strcmp(str, "COMMUTE") == 0)
//     {
//         return 3;
//     }

//     return 0;
// }

void* save_data_in_mem(const uint8_t type, const uint16_t length, const void* const data)
{
    if(length == 0)
    {
        return NULL;
    }

    switch (type)
    {
        case 1:
        {
            int* mem = (int*)get_data_mem(type, length);
            if(mem == NULL)
            {
                return NULL;
            }
            for(uint16_t i = 0; i < length; i++)
            {
                mem[i] = ((int*)data)[0];
            }
            return (void*)mem;
        }
        case 2:
        {
            char* mem = (char*)get_data_mem(type, length);
            if(mem == NULL)
            {
                return NULL;
            }
            for(uint16_t i = 0; i < length; i++)
            {
                mem[i] = ((char*)data)[0];
            }
            return (void*)mem;
        }
        case 3:
        {
            struct Communicate* mem = (struct Communicate*)get_data_mem(type, length);
            if(mem == NULL)
            {
                return NULL;
            }
            for(uint16_t i = 0; i < length; i++)
            {
                strncpy(mem[i].phone_number, (const char*)data + i * MAX_COMMAND_LENGTH,
                    MAX_PHONE_NUMBER_LENGTH - 1);
                mem[i].phone_number[MAX_PHONE_NUMBER_LENGTH - 1] = '\0';
            }
            return (void*)mem;
        }
        case 0:
        default:
            return NULL;
    }
}

void PRINT(const void* const node)
{
    const struct Node* target = (const struct Node*)node;
    printf("Key\t:%s\n", target->key);

    switch (target->type)
    {
    case 1:
    {
        const int* const com = (const int* const)target->value;
        for(uint16_t i = 0; i < target->length; i++)
        {
            printf("Value%d\t:%d\n", i + 1, com[i]);
        }
        return;
    }
    case 2:
    {
        const char* const com = (const char* const)target->value;
        for(uint16_t i = 0; i < target->length; i++)
        {
            printf("Value%d\t:%c\n", i + 1, com[i]);
        }
        return;
    }
    case 3:
    {
        const struct Communicate* const com = (const struct Communicate* const)target->value;
        for(uint16_t i = 0; i < target->length; i++)
        {
            printf("Phone Number%d\t:%s\n", i + 1, com[i].phone_number);
        }
        return;
    }
    case 0:
    default:
        return;
    }

    return;
}