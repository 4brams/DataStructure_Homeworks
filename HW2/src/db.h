#define MAX_KEY_LENGTH 21

struct Node
{   
    uint8_t type;
    uint16_t length;
    char key[MAX_KEY_LENGTH];
    void* value;
    struct Node* next; 
};

static struct Node* get_last_node(const struct Node* const first)
{   
    struct Node* current = (struct Node*)first;

    if(first == NULL)
    {
        return NULL;
    }

    while(current->next != NULL){
        current = current->next;
    }
    return current;
}

static struct Node* search_node(const struct Node* const first, const char* const key)
{
    struct Node* cursor = (struct Node*)first;

    while(cursor != NULL)
    {
        //printf("now: %s\ntarget:%s\n", cursor->key, key);
        if(strcmp(cursor->key, key) == 0)
        {
            break;
        }

        cursor = cursor->next;
    }

    return cursor;
}

void** init_db()
{
    void** first = (void**)malloc(sizeof(struct Node*));
    return first;
}

void* READ(const void** const db, const char* const key)
{
    const struct Node* const res = search_node((struct Node*)*db, key);
    if(!res)
    {
        printf("Error: Key '%s' DNE.\n", key);
        return NULL;
    }

    struct Node* new = (struct Node*)malloc(sizeof(struct Node));
    strcpy(new->key, res->key);
    new->length = res->length;
    new->type = res->type;
    new->value = res->value;
    new->next = NULL;

    return (void*)new;
}

void* UPDATE(const void** const db, const char* const key, const uint8_t type, const uint16_t length, const void* const value)
{
    struct Node* target = (struct Node*)search_node(*db, key);
    if(target != NULL)
    {
        target->type = type;
        target->length = length;
        target->value = (void*)value;
        return READ(db, key);
    }
    else
    {
        printf("Error: Key '%s' DNE.\n", key);
        return NULL;
    }
}

void* CREATE(void** const db, const char* const key, const uint8_t type, const uint16_t length, const void* const value)
{   
    if(search_node(*db, key) != NULL)
    {
        printf("Error: Key '%s' has already existed.\n", key);
        return NULL;
    }
    struct Node* new = (struct Node*)malloc(sizeof(struct Node));
    new->type = type;
    new->length = length;
    strcpy(new->key, (char*)key);
    new->value = (void*)value;
    new->next = NULL;

    // printf("type: %d\n",new->type);
    // printf("length: %d\n",new->length);
    // printf("key: %s\n",new->key);
    // printf("val: %p\n",new->value);
    // printf("next: %p\n",new->next);

    if(!*db)
    {
        *db = new;
    }
    else
    {
        struct Node* last = get_last_node((struct Node*)*db);
        last->next = new;
    }

    return READ((const void ** const)db, key);
}

bool DELETE(void** const db, const char* const key)
{
    struct Node* cursor = (struct Node*)*db;
    struct Node* last_cursor = NULL;

    while(cursor != NULL)
    {
        if(strcmp(cursor->key, key) == 0)
        {
            break;
        }

        last_cursor = cursor;
        cursor = cursor->next;
    }

    if(cursor == *db && *db)
    {
        *db = (void*)(((struct Node*)*db)->next);
        free(cursor->value);
        free(cursor);
        return true;
    }
    else if(cursor != NULL)
    {
        last_cursor->next = cursor->next;
        free(cursor->value);
        free(cursor);
        return true;
    }
    else
    {
        printf("Error: Key '%s' DNE.\n", key);
        return false;
    }
}