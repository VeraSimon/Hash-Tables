#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
  Hash table key/value pair with linked list pointer.

  Note that an instance of `LinkedPair` is also a node in a linked list.
  More specifically, the `next` field is a pointer pointing to the the
  next `LinkedPair` in the list of `LinkedPair` nodes.
 */
typedef struct LinkedPair
{
    char *key;
    char *value;
    struct LinkedPair *next;
} LinkedPair;

/*
  Hash table with linked pairs.
 */
// NOTE: If we add an additional pointer array to this, we can use it to track
// the end of the linked list for each index, shaving off some time during
// resize and insert (maybe) if an index's linked list grows particularly large.
typedef struct HashTable
{
    int capacity;
    LinkedPair **storage;
} HashTable;

/*
  Create a key/value linked pair to be stored in the hash table.
 */
LinkedPair *create_pair(char *key, char *value)
{
    LinkedPair *pair = malloc(sizeof(LinkedPair));
    pair->key = strdup(key);
    pair->value = strdup(value);
    pair->next = NULL;

    return pair;
}

/*
  Use this function to safely destroy a hashtable pair.
 */
void destroy_pair(LinkedPair *pair)
{
    if (pair != NULL)
    {
        free(pair->key);
        free(pair->value);
        free(pair);
    }
}

/*
  djb2 hash function

  Do not modify this!
 */
unsigned int hash(char *str, int max)
{
    unsigned long hash = 5381;
    int c;
    unsigned char *u_str = (unsigned char *)str;

    while ((c = *u_str++))
    {
        hash = ((hash << 5) + hash) + c;
    }

    return hash % max;
}

/*
  Fill this in.

  All values in storage should be initialized to NULL
 */
HashTable *create_hash_table(int capacity)
{
    // struct
    HashTable *ht = malloc(sizeof(HashTable *));
    // capacity
    ht->capacity = capacity;
    // storage
    ht->storage = calloc(capacity, sizeof(LinkedPair *));

    return ht;
}

/*
  Fill this in.

  Inserting values to the same index with different keys should be
  added to the corresponding LinkedPair list.

  Inserting values to the same index with existing keys can overwrite
  the value in the existing LinkedPair list.
 */
void hash_table_insert(HashTable *ht, char *key, char *value)
{
    int hashKey = hash(key, ht->capacity);
    LinkedPair *newNode = create_pair(strdup(key), strdup(value));

    if (ht->storage[hashKey] == NULL)
    {
        ht->storage[hashKey] = newNode;
    }
    else
    {
        LinkedPair *curNode = ht->storage[hashKey];
        while (curNode->next != NULL)
        {
            if (strcmp(curNode->key, key) == 0)
            {
                // work on the current iteration of the node
                break;
            }
            else
            {
                // keep going to the next node
                curNode = curNode->next;
            }
        }
        // check if the curNode key and the passed key match
        // if so, replace the curNode value
        if (strcmp(curNode->key, key) == 0)
        {
            curNode->value = strdup(value);
        }
        // otherwise, we've hit the end of the linked list, and we can insert a new node
        else
        {
            curNode->next = newNode;
        }
    }
}

/*
  Fill this in.

  Should search the entire list of LinkedPairs for existing
  keys and remove matching LinkedPairs safely.

  Don't forget to free any malloc'ed memory!
 */
void hash_table_remove(HashTable *ht, char *key)
{
}

/*
  Fill this in.

  Should search the entire list of LinkedPairs for existing
  keys.

  Return NULL if the key is not found.
 */
char *hash_table_retrieve(HashTable *ht, char *key)
{
    return NULL;
}

/*
  Fill this in.

  Don't forget to free any malloc'ed memory!
 */
void destroy_hash_table(HashTable *ht)
{
    for (int i = 0; i < ht->capacity; i++)
    {
        // Destroy all of the nodes at an index
        if (ht->storage[i] != NULL)
        {
            LinkedPair *nextNode = ht->storage[i];
            do
            {
                LinkedPair *curNode = nextNode;
                nextNode = nextNode->next;
                destroy_pair(curNode);
            } while (nextNode != NULL);
        }
    }
    // Destroy the array itself
    free(ht->storage);
    // Destroy the hash table struct
    free(ht);
}

/*
  Fill this in.

  Should create a new hash table with double the capacity
  of the original and copy all elements into the new hash table.

  Don't forget to free any malloc'ed memory!
 */
HashTable *hash_table_resize(HashTable *ht)
{
    // struct
    HashTable *new_ht = malloc(sizeof(HashTable *));
    // capacity
    new_ht->capacity = ht->capacity * 2;
    // storage
    LinkedPair **newStorage = calloc(new_ht->capacity, sizeof(LinkedPair *));
    // rehash all the things, then put them into the new ht storage
    for (int i = 0; i < ht->capacity; i++)
    {
        if (ht->storage[i] != NULL)
        {
            // rehash all the things!
            LinkedPair *nextNode = ht->storage[i];
            do
            {
                LinkedPair *curNode = nextNode;
                nextNode = nextNode->next;
                int newHashKey = hash(curNode->key, new_ht->capacity);
                LinkedPair *newPair = create_pair(curNode->key, curNode->value);

                // index is fresh and clean. just insert the new node.
                if (new_ht->storage[newHashKey] == NULL)
                {
                    new_ht->storage[newHashKey] = newPair;
                }
                // there's an existing node at this index. find the end of the
                // chain, then insert the new node there.
                else
                {
                    LinkedPair *curNewNode = new_ht->storage[newHashKey];
                    while (curNewNode->next != NULL)
                    {
                        curNewNode = curNewNode->next;
                    }
                    curNewNode->next = newPair;
                }
            } while (nextNode != NULL);
        }
    }
    // drop in that hot new storage
    new_ht->storage = newStorage;

    // free up the old hash table
    destroy_hash_table(ht);

    // return the new hash table
    return new_ht;
}

#ifndef TESTING
int main(void)
{
    struct HashTable *ht = create_hash_table(2);

    hash_table_insert(ht, "line_1", "Tiny hash table\n");
    hash_table_insert(ht, "line_2", "Filled beyond capacity\n");
    hash_table_insert(ht, "line_3", "Linked list saves the day!\n");

    printf("%s", hash_table_retrieve(ht, "line_1"));
    printf("%s", hash_table_retrieve(ht, "line_2"));
    printf("%s", hash_table_retrieve(ht, "line_3"));

    int old_capacity = ht->capacity;
    ht = hash_table_resize(ht);
    int new_capacity = ht->capacity;

    printf("\nResizing hash table from %d to %d.\n", old_capacity, new_capacity);

    destroy_hash_table(ht);

    return 0;
}
#endif
