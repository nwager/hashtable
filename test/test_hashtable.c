// must compile tests with DEBUG
#ifdef DEBUG

#include "hashtable.h"
#include "linkedlist.h"
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

typedef uint64_t u64;

void test_int(void);
void test_string(void);
void test_large(void);
void test_iter(void);
void test_resize(void);

size_t identity_hash(void *v) {
  return (size_t)v;
}

u64 fnvhash64(char *buffer, int len) {
  // This code is adapted from code by Landon Curt Noll
  // and Bonelli Nicola:
  //     http://code.google.com/p/nicola-bonelli-repo/
  static const u64 FNV1_64_INIT = 0xcbf29ce484222325ULL;
  static const u64 FNV_64_PRIME = 0x100000001b3ULL;
  unsigned char *bp = (unsigned char *) buffer;
  unsigned char *be = bp + len;
  u64 hval = FNV1_64_INIT;

  // FNV-1a hash each octet of the buffer.
  while (bp < be) {
    // XOR the bottom with the current octet.
    hval ^= (u64) * bp++;
    // Multiply by the 64 bit FNV magic prime mod 2^64.
    hval *= FNV_64_PRIME;
  }
  return hval;
}

size_t fnvhash64_int(void *v) {
  char *buffer = (char *)&v;
  int len = sizeof(int);
  return fnvhash64(buffer, len);
}

bool keycmp_int(void *ka, void *kb) {
  return ka == kb;
}

u64 fnvhash64_string(void *v) {
  char *buffer = (char *)v;
  int len = strlen(buffer);
  return fnvhash64(buffer, len);
}

bool keycmp_string(void *ka, void *kb) {
  return strcmp((char *)ka, (char *)kb) == 0;
}

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;

  test_iter();
  test_int();
  test_string();
  test_large();
  test_resize();
}

void test_int(void) {
  // TODO
}

void test_string(void) {
  // TODO
}

void test_large(void) {
  // TODO
}

void test_iter(void) {
  // TODO
}

void test_resize(void) {
  struct hashtable *ht = hashtable_alloc(identity_hash, keycmp_int);

  unsigned int nb, nu;
  float lf;
  hashtable_buckets(ht, &nb, NULL, &lf);

  // fill up table and check that the load is always under the threshold
  u64 runs = 2 * nb;
  for (u64 i = 0; i < runs; i++) {
    hashtable_put(ht, (void *)i, (void *)i, NULL);
    hashtable_buckets(ht, &nb, &nu, &lf);
    assert(hashtable_size(ht) == i+1);
    assert(nu == i+1);
    assert(nu <= lf * nb);
  }

  // make sure all data is still there
  for (u64 i = 0; i < runs; i++) {
    assert((u64)hashtable_get(ht, (void *)i) == i);
  }

  hashtable_free(ht, NULL);
}

#endif // ifdef DEBUG
