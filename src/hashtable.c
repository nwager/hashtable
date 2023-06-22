#include "hashtable.h"
#include <assert.h>
#ifdef DEBUG
#include <stdio.h>
#endif

const unsigned int INITIAL_BUCKETS = 64;
const unsigned int MAX_BUCKETS = 1 << 30;

struct hashtable {
  struct linkedlist **buckets; // array of ptrs to lists (NULL if empty list)
  hash_fn hash;
  keycmp_fn keycmp; // compare keys for equality
  float load_factor;
  unsigned int num_buckets;
  unsigned int num_used;
  unsigned int size;
};

// the data in each linkedlist entry
struct kv {
  void *k;
  void *v;
};

/**
 * @brief Allocates memory for an empty bucket list.
 * 
 * @param num_buckets Number of buckets the list holds.
 * @return struct linkedlist** Pointer to bucket list (array of linkedlists).
 */
static struct linkedlist **alloc_buckets(unsigned int num_buckets);

/**
 * @brief Frees the given buckets, including all data in the buckets' chains.
 * 
 * @param buckets Bucket list.
 * @param num_buckets Number of buckets in list.
 * @param kvfree (optional) Function to free chain entries.
 */
static void free_buckets(struct linkedlist **buckets, int num_buckets, kvfree_fn kvfree);

/**
 * @brief Get pointer to the kv entry with the given key, or NULL if it doesn't exist.
 * 
 * @param ht Hashtable to search.
 * @param key Key to search for.
 * @param remove Whether the entry should be removed from the table.
 * @return struct kv* Entry with the key, or NULL.
 */
static struct kv *get_entry(struct hashtable *ht, void *key, bool remove);

/**
 * @brief Creates a new bucket list of the specified length and moves data
 * to it, and updates the hashtable.
 * 
 * @param ht Hashtable to resize.
 * @param num_buckets Number of buckets to resize to.
 */
static void resize(struct hashtable *ht, unsigned int num_buckets);

struct hashtable *hashtable_alloc(hash_fn hash, keycmp_fn keycmp) {
  struct hashtable *ht = malloc(sizeof(struct hashtable));
  ht->hash = hash;
  ht->keycmp = keycmp;

  ht->load_factor = 0.75;
  ht->num_buckets = INITIAL_BUCKETS;
  ht->num_used = 0;
  ht->buckets = alloc_buckets(ht->num_buckets);
  ht->size = 0;

  return ht;
}

void hashtable_free(struct hashtable *ht, kvfree_fn fn) {
  free_buckets(ht->buckets, ht->num_buckets, fn);
  free(ht);
}

bool hashtable_put(struct hashtable *ht, void *key, void *newvalue, void **oldvalue) {
  struct kv *entry = get_entry(ht, key, false);
  if (entry != NULL) {
    // entry with that key already exists
    printf("key already exists!\n");
    if (oldvalue != NULL)
      *oldvalue = entry->v;
    entry->v = newvalue;
    return true;
  }
  // insert new key
  size_t ihash = ht->hash(key) % ht->num_buckets;
  // allocate new linkedlist if empty
  if (ht->buckets[ihash] == NULL)
    ht->buckets[ihash] = linkedlist_alloc();
  struct linkedlist *bucket = ht->buckets[ihash];
  struct kv *e = malloc(sizeof(struct kv));
  e->k = key;
  e->v = newvalue;
  linkedlist_push(bucket, e);
  ht->size++;
  // if first entry in bucket, a new bucket was used
  if (linkedlist_size(bucket) == 1) {
    ht->num_used++;
    if (ht->num_used > ht->load_factor * ht->num_buckets) {
      unsigned int newnb = ht->num_buckets * 2;
      resize(ht, newnb < MAX_BUCKETS ? newnb : MAX_BUCKETS);
    }
  }
  return false;
}

void *hashtable_get(struct hashtable *ht, void *key) {
  struct kv *entry = get_entry(ht, key, false);
  return entry == NULL ? NULL : entry->v;
}

bool hashtable_contains(struct hashtable *ht, void *key) {
  return get_entry(ht, key, false) != NULL;
}

void *hashtable_remove(struct hashtable *ht, void *key, data_free_fn kfree) {
  assert(hashtable_contains(ht, key));
  struct kv *entry = get_entry(ht, key, true);
  void *data = entry->v;
  if (kfree != NULL)
    kfree(entry->k);
  free(entry);
  return data;
}

unsigned int hashtable_size(struct hashtable *ht) {
  return ht->size;
}

#ifdef DEBUG
struct linkedlist **hashtable_buckets(struct hashtable *ht, unsigned int *nb, unsigned int *nu, float *lf) {
  if (nb != NULL) *nb = ht->num_buckets;
  if (nu != NULL) *nu = ht->num_used;
  if (lf != NULL) *lf = ht->load_factor;
  return ht->buckets;
}
#endif

// helpers

static struct linkedlist **alloc_buckets(unsigned int num_buckets) {
  // initialize all to NULL
  struct linkedlist **buckets = calloc(num_buckets, sizeof(struct linkedlist *));
  return buckets;
}

static void free_buckets(struct linkedlist **buckets, int num_buckets, kvfree_fn kvfree) {
  // free list in each bucket
  for (int i = 0; i < num_buckets; i++) {
    struct linkedlist *ll = buckets[i];
    if (ll == NULL) continue; // skip empty buckets
    // free data
    for (lliter_init(ll); lliter_hasnext(ll);) {
      struct kv *e = (struct kv *)lliter_next(ll);
      if (kvfree != NULL)
        kvfree(e->k, e->v);
    }
    // free entries
    linkedlist_free(ll, free);
  }
  // free bucket array
  free(buckets);
}

static struct kv *get_entry(struct hashtable *ht, void *key, bool remove) {
  size_t ihash = ht->hash(key) % ht->num_buckets;
  struct linkedlist *bucket = ht->buckets[ihash];
  // check if bucket is empty
  if (bucket == NULL) return NULL;
  // if not, check bucket for key
  for (lliter_init(bucket); lliter_hasnext(bucket);) {
    struct kv *e = (struct kv *)lliter_next(bucket);
    if (!ht->keycmp(key, e->k)) continue;
    if (remove) {
      lliter_remove(bucket);
      ht->size--;
      // if list becomes empty, it is now unused
      if (linkedlist_size(bucket) == 0) {
        ht->num_used--;
        // empty list should be nulled
        linkedlist_free(bucket, NULL);
        ht->buckets[ihash] = NULL;
      }
    }
    return e;
  }
  return NULL;
}

static void resize(struct hashtable *ht, unsigned int num_buckets) {
  // effectively clear the hashtable and allocate the new buckets
  // then transfer entries from the old buckets
  if (num_buckets == 0) num_buckets = 1; // min 1 bucket
  unsigned int oldnb = ht->num_buckets;
  struct linkedlist **oldbuckets = ht->buckets;
  struct linkedlist **newbuckets = alloc_buckets(num_buckets);
  ht->buckets = newbuckets;
  ht->num_buckets = num_buckets;
  ht->num_used = 0;
  ht->size = 0;
  for (unsigned int i = 0; i < oldnb; i++) {
    // for each old chain, re-map all the entries
    struct linkedlist *ll = oldbuckets[i];
    if (ll == NULL) continue; // skip empty chains
    for (lliter_init(ll); lliter_hasnext(ll);) {
      struct kv *e = (struct kv *)lliter_next(ll);
      hashtable_put(ht, e->k, e->v, NULL);
    }
  }
  // free old buckets
  free_buckets(oldbuckets, oldnb, NULL);
}
