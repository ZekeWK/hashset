/*
This is a hashset implementation by William Kraft (ZekeWK).

It is NOT YET TESTED properly.

As I am a novice in C, I leave no guarantees as to the quality, speed, or correctness of the code.
 
While it is written primarily as a hashset, it can be used as a hashmap by only using the key in the hash and eq functions supplied to the hashset_create function.

It uses an open addressing double hashing algorithm. 
*/



#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>



///The hashset struct.
typedef struct HashSet {
    size_t len;
    void **storage;
    size_t (*hash)(void *key); 
    bool (*eq)(void *key1, void *key2);
} HashSet;


///Is used in the storage to indicate that no item is here, and that no item has ever been here.
#define HASHSET_STOP NULL

///Is used in the storage to indicate that no item is here, but that some item has been here before. As it is the same as the storage pointer, it means that no such pointer should ever be inserted into the hashset.
#define HASHSET_GAP(hashset) ((void *) (hashset).storage)

///A hashset consisting of NULL pointers, used to indicate an error in the HASHSET relocation function
#define HASHSET_NULL hashset_create(NULL, 0, (size_t (*)(void *)) NULL, (bool (*)(void *, void *)) NULL)

///Part of a do while loop that, given a variable, a start location, a step size, the lenght of the storage, will iterate over the values in the correct order. It will stop if it is back at the beginning. If used together with the hashset_stop_step function, it will always start at the hash %len, opening for perfect hash functions.
#define HASHSET_WHILE(var, start, step, len) while(var = (var + step) % len, var != start)



///Gives a hashset struct with the given parameters. The storage should point to a space of NULL or one could also call hashset clear directly after creating for same effect. Len needs to be a prime for full capacity. 
HashSet hashset_create(void **storage, size_t len, size_t (*hash)(void *key), bool (*eq)(void *key1, void *key2));

//Removes all elements in the hashset. Can be used right after creation of a set to ensure the storage is NULL
void hashset_clear(HashSet hashset);

///Gives the start and stop value when iterating through the hashset.
void hashset_start_step(HashSet hashset, void *element, size_t *start, size_t *step);

///Inserts an item into the hashset. If two elements are equal, it will replace the old pointer with the new and return the old pointer. If it was not equal the function will return NULL. If there was not enough space the function will return a pointer to the storage. This should however not be the case as one should attempt not to excede about 75% load capacity for performance reasons.
void *hashset_insert(HashSet hashset, void *element);

///Attempts to find an equal element in the hashset. If it is found it will return a pointer to the equal element. If it is found it will return NULL
void *hashset_find(HashSet hashset, void *element);

///Attempts to find an equal element and remove it from the hashset. If finds an equal element it will return its pointer. If it does not, it will return NULL.
void *hashset_pop(HashSet hashset, void *element);

///Returns the number of elements in the hashset.
size_t hashset_used(HashSet hashset);


///Will drain the hashset of one element. If there is an element left, it will return the pointer to that element, and if there are no elements left it will return NULL. By doing this operation the len is changed which changes the way items would be inserted and found. One should therefore not use any other functions on a partially drained hashset. The elements are not removed when draining, and by changing the len to its original, the hashset would be restored.  After having drained a set completly, every pointer inserted into the hashset should have been return exactly once from either hashset_insert, hashset_pop or hashset drain, it is therefore safe to use for memory manangement.
void *hashset_drain(HashSet *hashset);

///Will attempt to relocate the sets by draining the first into one created by the new storage and len. If it is succeded it will return the new hashset, if it failes due to insufficient space, it will return a HASHSET_NULL. The old hashset will at that point have been partially drained and the storage partially filled. If one wishes to retry one can restore the hashset from being drained (mentioned in hashset_drain) and attempt again. New storage does need to be filled with NULL.
HashSet hashset_relocate(HashSet *hashset_old, void **storage_new, size_t len_new);