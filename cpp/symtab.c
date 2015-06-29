/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef TESTING
#define HASHTABLE_DEFAULT_SIZE (4)
#define HASHTABLE_INC_FACTOR (2)
#define DEFAULT_STRINGSPACE_SIZE (4)
#define STRINGTABLE_INC_SIZE (4)
#else
#define HASHTABLE_DEFAULT_SIZE (64)
#define HASHTABLE_INC_FACTOR (2)
#define DEFAULT_STRINGSPACE_SIZE (4*1024)
#define STRINGTABLE_INC_SIZE (4*1024)
#endif

struct attributes;


struct string_space {
	char *buf;
	int size;
	int count;
};

struct string_space *newStringSpace(){
	struct string_space *stringSpace;

	stringSpace = calloc(1, sizeof(*stringSpace));
	assert(stringSpace != NULL);
	stringSpace->size = DEFAULT_STRINGSPACE_SIZE;
	stringSpace->count = 0;
	stringSpace->buf = calloc(stringSpace->size, sizeof(char));
	assert(stringSpace->buf != NULL);
	return stringSpace;
}

void freeStringSpace(struct string_space *stringSpace){
	free(stringSpace->buf);
	stringSpace->buf = 0;
	free(stringSpace);
}

int stringSpaceIncreaseSize(struct string_space *stringSpace, int needed){
	int newSize;
	char *newBuf;

	newSize = stringSpace->size + STRINGTABLE_INC_SIZE;
	newSize += (needed / STRINGTABLE_INC_SIZE) * STRINGTABLE_INC_SIZE;
	assert(stringSpace->size < newSize);
	newBuf = realloc(stringSpace->buf, newSize);
	assert(newBuf != NULL);
	stringSpace->buf = newBuf;
	stringSpace->size = newSize;
	return 0;
}

int stringSpaceAddString(struct string_space *stringSpace, char *s, int len){
	int index;
	if(stringSpace->size <= stringSpace->count + len){
		stringSpaceIncreaseSize(stringSpace, len);
	}
	assert(stringSpace->count + len < stringSpace->size);
	index = stringSpace->count;
	stringSpace->count += len;
	memcpy(stringSpace->buf + index, s, len);
	return index;
}

char *stringSpaceGetString(struct string_space *stringSpace, int index, int len){
	char *s;

	if(stringSpace->count < index + len){
		return NULL;
	} else {
		s = malloc(len+1);
		s[len] = 0;
		memcpy(s, stringSpace->buf + index, len);
		return s;
	}
}





#ifdef TESTING

void test_stringSpace(){
	int i0, i1, i2, i3;
	int l0, l1, l2, l3;
	char *s0, *s1, *s2, *s3;
	char *s;
	struct string_space *stringSpace;

	stringSpace = newStringSpace();

	s0 = "Hello World!";
	l0 = strlen(s0);
	i0 = stringSpaceAddString(stringSpace, s0, l0);
	s1 = "Kill Kill Kill!";
	l1 = strlen(s1);
	i1 = stringSpaceAddString(stringSpace, s1, l1);
	s2 = "murda murda murda!";
	l2 = strlen(s2);
	i2 = stringSpaceAddString(stringSpace, s2, l2);
	s3 = "i'm never caught slippin', keep my heat on the dash";
	l3 = strlen(s3);
	i3 = stringSpaceAddString(stringSpace, s3, l3);

	s = stringSpaceGetString(stringSpace, i0, l0);
	assert(strcmp(s, s0) == 0);
	s = stringSpaceGetString(stringSpace, i1, l1);
	assert(strcmp(s, s1) == 0);
	s = stringSpaceGetString(stringSpace, i2, l2);
	assert(strcmp(s, s2) == 0);
	s = stringSpaceGetString(stringSpace, i3, l3);
	assert(strcmp(s, s3) == 0);

	freeStringSpace(stringSpace);
}

struct entry {
	int used;
	int hash;
	char *key;
	void *value;
};

struct hash_table {
	struct entry *buf;
	int size;
	int count;
};

struct hash_table *newHashTable(){
	struct hash_table *ht;

	ht = calloc(1, sizeof(*ht));
	assert(ht != 0);
	ht->size = 4;
	ht->count = 0;
	ht->buf = calloc(ht->size,  sizeof(struct entry));
	assert(ht->buf != 0);
	return ht;
}

static int hashfn(char *s){
	int h = 0;
	while(*s){
		h += *s++;
		if(h < 0){
			h *= -1;
		}
	}
	return h;
}


static int linearProbe(struct entry *buf, int size,
		int hash, char *key, struct entry **entry, int *present){
	struct entry *end = buf + size;
	struct entry *e = buf + (hash % size);
	int n = size;

	while(n > 0){
		if(e->used == 0){
			*entry = e;
			*present = 0;
			return 0;
		} else if(e->hash == hash && strcmp(e->key, key) == 0){
			*entry = e;
			*present = 1;
			return 0;
		} else {
			e++;
			if(e == end){
				e = buf;
			}
			n--;
		}
	}
	return -1;
}

static int hashTableIncreaseSize(struct hash_table *ht){
	int newSize;
	struct entry *newBuf;
	int present;

	newSize = ht->size * 2;
	printf("BUMP! %d\n", newSize);
	newBuf = calloc(newSize, sizeof(*newBuf));
	assert(newBuf != NULL);

	struct entry *e = ht->buf;
	struct entry *newEntry;
	struct entry *end = ht->buf + ht->size;
	while(e < end){
		if(e->used){
			linearProbe(newBuf, newSize, e->hash, e->key, &newEntry, &present);
			assert(present == 0);
			*newEntry = *e;
		}
		e++;
	}
	ht->buf = newBuf;
	ht->size = newSize;
	return 0;
}

int hashTableEnter(struct hash_table *ht, char *key, struct entry **entryOut, int *presentOut){
	int hash;
	struct entry *entry;
	int present;

	if(ht->count >= ht->size / 2){
		hashTableIncreaseSize(ht);
	}

	hash = hashfn(key);
	if(!linearProbe(ht->buf, ht->size, hash, key, &entry, &present)){
		if(present){
			*entryOut = entry;
			*presentOut = 1;
			return 0;
		} else {
			ht->count += 1;
			entry->used = 1;
			entry->hash = hash;
			entry->key = strdup(key);
			*entryOut = entry;
			*presentOut = 0;
			return 0;
		}
	} else {
		return -1;
	}
}

int hashTableEntrySetValue(struct entry *e, void *value){
	e->value = value;
	return 0;
}

int hashTableEntryGetValue(struct entry *e, void **value){
	*value = e->value;
	return 0;
}

int hashTableSetValue(struct hash_table *ht, char *key, void *value){
	struct entry *entry = 0;
	int present = 0;
	if(hashTableEnter(ht, key, &entry, &present) == 0){
		hashTableEntrySetValue(entry, value);
		return 0;
	} else {
		return -1;
	}
}

void *hashTableGetValue(struct hash_table *ht, char *key){
	int h = hashfn(key);
	int index = h % ht->size;
	struct entry *entry;
	int present;

	linearProbe(ht->buf, ht->size, h, key, &entry, &present);

	if(present){
		return entry->value;
	} else {
		fprintf(stderr, "NOT FOUND!\n");
		exit(1);
	}
}

void test_hashTable(){
	struct hash_table *ht = newHashTable();
	char *s;

	assert(hashTableSetValue(ht, "poop", "hat") == 0);
	assert(hashTableSetValue(ht, "fire", "golf") == 0);
	assert(hashTableSetValue(ht, "hot", "knife") == 0);
	assert(hashTableSetValue(ht, "find", "reason") == 0);

	s = hashTableGetValue(ht, "poop");
	printf("poop: %s\n", s);
	assert(strcmp(s, "hat") == 0);
	s = hashTableGetValue(ht, "fire");
	printf("fire: %s\n", s);
	assert(strcmp(s, "golf") == 0);
	s = hashTableGetValue(ht, "hot");
	printf("hot: %s\n", s);
	assert(strcmp(s, "knife") == 0);
	s = hashTableGetValue(ht, "find");
	printf("find: %s\n", s);
	assert(strcmp(s, "reason") == 0);
}


int main(int argc, char *argv[]){
	test_stringSpace();
	test_hashTable();
	return 0;
}

#endif
