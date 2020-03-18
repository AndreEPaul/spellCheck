/**
 * CS 261 Data Structures
 * Assignment 5
 * Name:    Andre Paul
 * Date:    3/7/2020
 */

#include "hashMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

int hashFunction1(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += key[i];
    }
    return r;
}

int hashFunction2(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += (i + 1) * key[i];
    }
    return r;
}

/**
 * Creates a new hash table link with a copy of the key string.
 * @param key Key string to copy in the link.
 * @param value Value to set in the link.
 * @param next Pointer to set as the link's next.
 * @return Hash table link allocated on the heap.
 */
HashLink* hashLinkNew(const char* key, int value, HashLink* next)
{
    HashLink* link = malloc(sizeof(HashLink));
    link->key = malloc(sizeof(char) * (strlen(key) + 1));
    strcpy(link->key, key);
    link->value = value;
    link->next = next;
    return link;
}

/**
 * Free the allocated memory for a hash table link created with hashLinkNew.
 * @param link
 */
static void hashLinkDelete(HashLink* link)
{
    free(link->key);
    free(link);
}

/**
 * Initializes a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param map
 * @param capacity The number of table buckets.
 */
void hashMapInit(HashMap* map, int capacity)
{
    map->capacity = capacity;
    map->size = 0;
    map->table = malloc(sizeof(HashLink*) * capacity);
    for (int i = 0; i < capacity; i++)
    {
        map->table[i] = NULL;
    }
}

/**
 * Removes all links in the map and frees all allocated memory. You can use
 * hashLinkDelete to free the links.
 * @param map
 */
void hashMapCleanUp(HashMap* map)
{
    assert(map != NULL);

    HashLink* traverse;
    HashLink* nextLink;
    // Iterate through the buckets.
    for(int i = 0; i < map->capacity; i++){

        traverse = map->table[i];

        // Free each link. Need to use temps to traverse LL.
        while(traverse != NULL){
            nextLink = traverse->next;
            hashLinkDelete(traverse);
            traverse = nextLink;
        }
    }

    // Finally, free the table, set size to 0.
    free(map->table);
    map->size = 0;
}

/**
 * Creates a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param capacity The number of buckets.
 * @return The allocated map.
 */
HashMap* hashMapNew(int capacity)
{
    HashMap* map = malloc(sizeof(HashMap));
    hashMapInit(map, capacity);
    return map;
}

/**
 * Removes all links in the map and frees all allocated memory, including the
 * map itself.
 * @param map
 */
void hashMapDelete(HashMap* map)
{
    hashMapCleanUp(map);
    free(map);
}

/**
 * Returns a pointer to the value of the link with the given key and skip traversing as well. Returns NULL
 * if no link with that key is in the table.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 * 
 * @param map
 * @param key
 * @return Link value or NULL if no matching link.
 */
int* hashMapGet(HashMap* map, const char* key)
{
    assert(map != NULL && key != NULL);

    int bucket = HASH_FUNCTION(key) % map->capacity;
    if(bucket < 0){
        bucket += map->capacity;
    }

    // Traverse the list at correct bucket.
    HashLink* traverse = map->table[bucket];

    // Run through list, breaking when key is found.
    while(traverse != NULL){
        if(strcmp(key, traverse->key) == 0){
            return &(traverse->value);
        }
        traverse = traverse->next;
    }

    // Didn't break, so key isn't in list.
    return NULL;
}

/**
 * Resizes the hash table to have a number of buckets equal to the given 
 * capacity (double of the old capacity). After allocating the new table, 
 * all of the links need to rehashed into it because the capacity has changed.
 * 
 * Remember to free the old table and any old links if you use hashMapPut to
 * rehash them.
 * 
 * @param map
 * @param capacity The new number of buckets.
 */
void resizeTable(HashMap* map, int capacity)
{
    assert(map != NULL);

    HashMap* newMap = hashMapNew(capacity);
    HashLink* traverse;

    for(int i = 0; i < map->capacity; i++){
        traverse = map->table[i];
        // Go through LL, "copy" the elements of LL to new map.
        while(traverse != NULL){
            hashMapPut(newMap, traverse->key, traverse->value);
            traverse = traverse->next;
        }
    }

    // Delete the old map, reassign.
    hashMapCleanUp(map);
    map->table = newMap->table;
    map->capacity = newMap->capacity;
    map->size = newMap->size;

    // Memory / dangling ptr management
    newMap->table = NULL;
    free(newMap);
}

/**
 * Updates the given key-value pair in the hash table. If a link with the given
 * key already exists, this will just update the value and skip traversing. Otherwise, it will
 * create a new link with the given key and value and add it to the table
 * bucket's linked list. You can use hashLinkNew to create the link.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket.
 * 
 * @param map
 * @param key
 * @param value
 */
void hashMapPut(HashMap* map, const char* key, int value)
{
    assert(map != NULL && key != NULL);

    // In this case, the given key is present
    // in the map already. Recall that hashMapGet
    // returns a pointer to value field.
    // So deref-ing it like this updates the value field.
    if(hashMapContainsKey(map, key)){
        int* toUpdate = hashMapGet(map, key);
        (*toUpdate) = value;
    // Other case, given key is not present so we
    // need to add it in.
    }else{
        int bucket = HASH_FUNCTION(key) % map->capacity;
        if(bucket < 0){
            bucket += map->capacity;
        }
        // Add in new link. Check cases for empty bucket or not.
        HashLink* newLink = hashLinkNew(key, value, NULL);
        assert(newLink != NULL);

        // Empty bucket.
        if(map->table[bucket] == NULL){
            map->table[bucket] = newLink;
        // Non-empty bucket.
        }else{
            HashLink* traverse = map->table[bucket];
            while(traverse->next != NULL){
                traverse = traverse->next;
            }
            traverse->next = newLink;
        }
        map->size++;
        // Resize if necessary.
        if(hashMapTableLoad(map) >= MAX_TABLE_LOAD){
            resizeTable(map, (2 * map->capacity));
        }
    }
}

/**
 * Removes and frees the link with the given key from the table. If no such link
 * exists, this does nothing. Remember to search the entire linked list at the
 * bucket. You can use hashLinkDelete to free the link.
 * @param map
 * @param key
 */
void hashMapRemove(HashMap* map, const char* key)
{
    int bucket = HASH_FUNCTION(key) % map->capacity;
    if(bucket < 0){
        bucket += map->capacity;
    }

    HashLink* traverse = map->table[bucket];
    HashLink* prev = NULL;

    while(traverse != NULL){
        if(strcmp(traverse->key, key) == 0) {
        // Need case to account for key being
        // found at first link.
            if(prev == NULL){
                map->table[bucket] = traverse->next;
            }else{
                prev->next = traverse->next;
            }
            hashLinkDelete(traverse);
            map->size--;
            return;
        }
        // Iterate through links.
        prev = traverse;
        traverse = traverse->next;
    }
}

/**
 * Returns 1 if a link with the given key is in the table and 0 otherwise.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 * 
 * @param map
 * @param key
 * @return 1 if the key is found, 0 otherwise.
 */
int hashMapContainsKey(HashMap* map, const char* key)
{
    assert(map != NULL && key != NULL);

    int bucket = HASH_FUNCTION(key) % map->capacity;
    if(bucket < 0){
        bucket += map->capacity;
    }
    // Traverse list at correct bucket.
    HashLink* traverse = map->table[bucket];
    // Traverse LL until reaching end of list.
    while(traverse != NULL){
        if(strcmp(traverse->key, key) == 0){
            return 1;
        }
        traverse = traverse->next;
    }
    // reaching this part of the function means the
    // key was not found.
    return 0;
}

/**
 * Returns the number of links in the table.
 * @param map
 * @return Number of links in the table.
 */
int hashMapSize(HashMap* map)
{
    assert(map != NULL);
    return map->size;
}

/**
 * Returns the number of buckets in the table.
 * @param map
 * @return Number of buckets in the table.
 */
int hashMapCapacity(HashMap* map)
{
    assert(map != NULL);
    return map->capacity;
}

/**
 * Returns the number of table buckets without any links.
 * @param map
 * @return Number of empty buckets.
 */
int hashMapEmptyBuckets(HashMap* map)
{
    assert(map != NULL);
    int emptyBucketCount = 0;
    for(int i = 0; i < map->capacity; i++){
        if(map->table[i] == NULL){
            emptyBucketCount++;
        }
    }

    return emptyBucketCount;
}

/**
 * Returns the ratio of (number of links) / (number of buckets) in the table.
 * Remember that the buckets are linked lists, so this ratio tells you nothing
 * about the number of empty buckets. Remember also that the load is a floating
 * point number, so don't do integer division.
 * @param map
 * @return Table load.
 */
float hashMapTableLoad(HashMap* map)
{
    assert(map != NULL);
    return (float) map->size / (float) map->capacity;
}

/**
 * Prints all the links in each of the buckets in the table.
 * @param map
 */
void hashMapPrint(HashMap* map)
{
    HashLink* traverse;
    printf("\n");
    for(int i = 0; i < map->capacity; i++){
        printf("Bucket %d -> ", i);
        traverse = map->table[i];
        while(traverse != NULL){
            printf("(%s, %d) -> ", traverse->key, traverse->value);
            traverse = traverse->next;
        }
        printf("\n");
    }
}
