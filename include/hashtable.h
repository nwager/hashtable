#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "linkedlist.h"
#include <stdlib.h>
#include <stdbool.h>

/**
 * @brief Function that takes a pointer to data and returns its hash as a
 * size_t integer.
 */
typedef size_t (*hash_fn)(void *k);
/**
 * @brief Function that takes pointers to two keys and returns true iff
 * they are equal.
 */
typedef bool (*keycmp_fn)(void *ka, void *kb);
/**
 * @brief Function that takes a pointer to a key and value and frees both.
 * 
 */
typedef void (*kvfree_fn)(void *k, void *v);

/**
 * @brief Hashtable object.
 */
struct hashtable;

/**
 * @brief Allocates an empty hashtable.
 * 
 * @param hash Hash function to use on keys.
 * @param keycmp Compare function to check equality of keys.
 * @return struct hashtable* Pointer to hashtable.
 */
extern struct hashtable *hashtable_alloc(hash_fn hash, keycmp_fn keycmp);

/**
 * @brief Frees a hashtable.
 * 
 * @param ht Pointer to hashtable to free.
 * @param fn Function to free keys and values, or NULL for no-op.
 */
extern void hashtable_free(struct hashtable *ht, kvfree_fn fn);

/**
 * @brief Put an entry into the hashtable. If an entry with the same key exists,
 * it is replaced.
 * 
 * @param ht Pointer to hashtable.
 * @param key Key to insert.
 * @param newvalue Value to insert.
 * @param oldvalue Output pointer to replaced value (if applicable).
 * @return true iff an old value was replaced.
 */
extern bool hashtable_put(struct hashtable *ht, void *key, void *newvalue, void **oldvalue);

/**
 * @brief Get the value mapped to a key in the hashtable.
 * 
 * @param ht Pointer to hashtable.
 * @param key Key to get.
 * @return void* Value mapped to key, or NULL if key doesn't exist.
 */
extern void *hashtable_get(struct hashtable *ht, void *key);

/**
 * @brief Check if hashtable contains the key.
 * 
 * @param ht Pointer to hashtable.
 * @param key Key to check.
 * @return true iff the hashtable has an entry for the key.
 */
extern bool hashtable_contains(struct hashtable *ht, void *key);

/**
 * @brief Remove a key-value entry from the hashtable.
 * 
 * @param ht Pointer to hashtable.
 * @param key Key of entry to remove.
 * @param kfree Function to free key stored in hashtable, or NULL for no-op.
 * @return void* Value of removed entry.
 */
extern void *hashtable_remove(struct hashtable *ht, void *key, data_free_fn kfree);

/**
 * @brief Get number of entries in hashtable.
 * 
 * @param ht Pointer to hashtable.
 * @return unsigned int Number of entries in hashtable.
 */
extern unsigned int hashtable_size(struct hashtable *ht);

#ifdef DEBUG
// Get internal data of hashtable.
extern struct linkedlist **hashtable_buckets(struct hashtable *ht, unsigned int *nb, unsigned int *nu, float *lf);
#endif

#endif // HASHTABLE_H
