#include "puzzle.h"

/* Funções de Hash */

unsigned int hash_function(const char *key)
{
    unsigned long int value = 0;
    unsigned int i = 0;
    unsigned int key_len = strlen(key);

    for (; i < key_len; ++i)
    {
        value = value * 37 + key[i];
    }

    value = value % HASH_TABLE_SIZE;

    return value;
}

HashTable *create_hashtable()
{
    HashTable *table = (HashTable *)malloc(sizeof(HashTable));
    table->nodes = (HashNode **)malloc(sizeof(HashNode *) * HASH_TABLE_SIZE);
    return table;
}

int hashtable_contains(HashTable *table, const char *key)
{
    unsigned int bucket = hash_function(key);
    HashNode *node = table->nodes[bucket];
    while (node != NULL)
    {
        if (strcmp(node->key, key) == 0)
            return 1;
        node = node->next;
    }
    return 0;
}

void hashtable_insert(HashTable *table, const char *key)
{
    if (hashtable_contains(table, key))
        return;
    unsigned int bucket = hash_function(key);

    HashNode *new_node = (HashNode *)malloc(sizeof(HashNode));
    new_node->key = strdup(key);
    new_node->next = table->nodes[bucket];

    table->nodes[bucket] = new_node;
}

void free_hashtable(HashTable *table)
{
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        HashNode *node = table->nodes[i];
        while (node != NULL)
        {
            HashNode *temp = node;
            node = node->next;
            free(temp->key);
            free(temp);
        }
    }
    free(table->nodes);
    free(table);
}
