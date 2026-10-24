struct Communicate
{
    char* phone_number;
};

struct Node
{   
    char* key;
    void* value;
    struct Node* next; 
};

bool is_match(const char* const a, const char* const b)
{
    for(uint8_t i = 0; i < MAX_COMMAND_LENGTH - 1; i++)
    {
        if(a[i] == '\0' && b[i] == '\0')
        {
            break;
        }
        if(a[i] != b[i])
        {
            return false;
        }
    }

    return true;
}

struct Node* init_list()
{
    struct Node* first_node = NULL;
    return first_node;
}

struct Node* get_last_node(const struct Node* const first)
{   
    if(first == NULL)
    {
        return NULL;
    }

    struct Node* current = (struct Node*)first;
    while(current->next != NULL){
        current = current->next;
    }
    return current;
}

struct Node* search_node(const struct Node* const first, const char* const key)
{
    struct Node* cursor = (struct Node*)first;

    while(cursor != NULL)
    {
        //printf("now: %s\ntarget:%s\n", cursor->key, key);
        if(is_match(cursor->key, key))
        {
            return cursor;
        }

        cursor = cursor->next;
    }

    return NULL;
}

struct Node* create_node(struct Node** const first, const char* const key, const void* const value)
{   
    if(search_node(*first, key) != NULL)
    {
        printf("Error: key '%s' has already existed.\n", key);
        return NULL;
    }
    struct Node* new = (struct Node*)malloc(sizeof(struct Node));
    new->key = (char*)key;
    new->value = (void*)value;
    new->next = NULL;

    struct Node* last = get_last_node(*first);
    //printf("first:%p\n", *first);
    if(!*first)
    {
        //printf("hit\n");
        *first = new;
    }
    else{
        last->next = new;
    }

    //printf("first:%p\n", *first);
    // printf("last:%a\n", last);
    // printf("last->next:%a\n", last->next);
    // printf("new:%a\n", new);
    //printf("new->key:%s\n", new->key);
    //printf("new->val:%s\n", new->value);
    return new;
}

struct Node* update_node(const struct Node* const first, const char* const key, const void* const value)
{
    struct Node * target = search_node(first, key);
    if(target != NULL)
    {
        target->value = (void*)value;
        return target;
    }
    else
    {
        return NULL;
    }
}

bool remove_node(struct Node* first, const char* const key)
{
    struct Node * target = search_node(first, key);
    if(target == first)
    {
        first = first->next;
        free(target);
        return true;
    }
    else if(target != NULL)
    {
        free(target);
        return true;
    }
    else
    {
        return false;
    }
}