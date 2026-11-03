#define MAX_KEY_LENGTH 21
#define MAX_PHONE_NUMBER_LENGTH 11

struct Communicate
{
    char phone_number[MAX_PHONE_NUMBER_LENGTH];
};

struct Node
{   
    uint8_t type;
    uint16_t length;
    char key[MAX_KEY_LENGTH];
    void* value;
    struct Node* next; 
};

static size_t value_element_size(const uint8_t type)
{
    switch(type)
    {
    case 1:
        return sizeof(int);
    case 2:
        return sizeof(char);
    case 3:
        return sizeof(struct Communicate);
    default:
        return 0;
    }
}

static void* copy_value(const uint8_t type, const uint16_t length,
    const void* const value)
{
    const size_t element_size = value_element_size(type);
    if(element_size == 0 || (length > 0 && value == NULL))
    {
        return NULL;
    }

    if(length == 0)
    {
        return NULL;
    }

    void* copy = malloc(element_size * length);
    if(copy != NULL)
    {
        memcpy(copy, value, element_size * length);
    }
    return copy;
}

static struct Node* copy_node(const struct Node* const source)
{
    struct Node* copy = (struct Node*)malloc(sizeof(struct Node));
    if(copy == NULL)
    {
        return NULL;
    }

    *copy = *source;
    copy->next = NULL;
    copy->value = copy_value(source->type, source->length, source->value);
    if(source->length > 0 && copy->value == NULL)
    {
        free(copy);
        return NULL;
    }

    return copy;
}

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
    *first = NULL;
    return first;
}

struct Node* READ(const void** const db, const char* const key)
{
    const struct Node* const res = search_node((struct Node*)*db, key);
    if(!res)
    {
        printf("Error: Key '%s' DNE.\n", key);
        return NULL;
    }

    struct Node* copy = copy_node(res);
    if(copy == NULL)
    {
        printf("Error: Unable to copy key '%s'.\n", key);
    }
    return copy;
}

void FREE_NODE_COPY(struct Node* const node)
{
    if(node != NULL)
    {
        free(node->value);
        free(node);
    }
}

bool UPDATE(const void** const db, const char* const key, const uint8_t type,
    const uint16_t length, const void* const value)
{
    struct Node* target = (struct Node*)search_node(*db, key);
    if(target == NULL)
    {
        printf("Error: Key '%s' DNE.\n", key);
        return false;
    }

    void* value_copy = copy_value(type, length, value);
    if(value_element_size(type) == 0 || (length > 0 && value_copy == NULL))
    {
        printf("Error: Unable to copy value for key '%s'.\n", key);
        return false;
    }

    free(target->value);
    target->type = type;
    target->length = length;
    target->value = value_copy;
    return true;
}

bool CREATE(void** const db, const char* const key, const uint8_t type,
    const uint16_t length, const void* const value)
{   
    if(search_node(*db, key) != NULL)
    {
        printf("Error: Key '%s' has already existed.\n", key);
        return false;
    }

    const size_t key_length = strlen(key);
    if(key_length >= MAX_KEY_LENGTH)
    {
        printf("Error: Key exceeds the maximum length of %d.\n", MAX_KEY_LENGTH - 1);
        return false;
    }

    void* value_copy = copy_value(type, length, value);
    if(value_element_size(type) == 0 || (length > 0 && value_copy == NULL))
    {
        printf("Error: Unable to copy value for key '%s'.\n", key);
        return false;
    }

    struct Node* new = (struct Node*)malloc(sizeof(struct Node));
    if(new == NULL)
    {
        free(value_copy);
        printf("Error: Unable to create key '%s'.\n", key);
        return false;
    }

    new->type = type;
    new->length = length;
    memcpy(new->key, key, key_length + 1);
    new->value = value_copy;
    new->next = NULL;

    if(!*db)
    {
        *db = new;
    }
    else
    {
        struct Node* last = get_last_node((struct Node*)*db);
        last->next = new;
    }

    return true;
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

void REMOVE_DB(void** const db)
{
    struct Node* cursor = (struct Node*)*db;
    while(cursor)
    {
        struct Node* temp = cursor;
        cursor = cursor->next;
        free(temp->value);
        free(temp);
    }

    free(db);
    return;
}