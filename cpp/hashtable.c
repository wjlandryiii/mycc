/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define HASHTABLE_DEFAULT_SIZE (32)
#define HASHTABLE_INC_FACTOR (2)


struct hash_entry {
	int occupied;
	int hash;
	char *key;
	void *value;
};

struct hash_table {
	struct hash_entry *entries;
	int count;
	int size;
};

struct hash_table *newHashTable(){
	struct hash_table *hashTable;
	hashTable = calloc(1, sizeof(*hashTable));
	assert(hashTable != NULL);
	hashTable->size = HASHTABLE_DEFAULT_SIZE;
	hashTable->count = 0;
	hashTable->entries = calloc(hashTable->size, sizeof(struct hash_entry));
	assert(hashTable->entries != NULL);
	return hashTable;
}

void freeHashTable(struct hash_table *hashTable){
	free(hashTable->entries);
	hashTable->entries = NULL;
	free(hashTable);
}

static int hashfn(char *s){
	int h = 0x11111;
	while(*s){
		h += *s++ * 31;
		if(h < 0){
			h *= -1;
		}
	}
	return h;
}


static int linearProbe(struct hash_entry *buf, int size,
		int hash, char *key, struct hash_entry **entry, int *present){
	struct hash_entry *end = buf + size;
	struct hash_entry *e = buf + (hash % size);
	int n = size;

	while(n > 0){
		if(e->occupied == 0){
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


static void hashTableIncreaseSize(struct hash_table *hashTable){
	int newSize;
	struct hash_entry *newEntries;
	struct hash_entry *oldEntry;
	struct hash_entry *newEntry;
	struct hash_entry *endEntry;
	int present;

	newSize = hashTable->size * HASHTABLE_INC_FACTOR;
	assert(hashTable->size < newSize * sizeof(struct hash_entry));
	newEntries = malloc(newSize * sizeof(struct hash_entry));
	memset(newEntries, 0, newSize * sizeof(struct hash_entry));
	assert(newEntries != NULL);

	endEntry = hashTable->entries + hashTable->size;
	oldEntry = hashTable->entries;
	while(oldEntry < endEntry){
		if(oldEntry->occupied){
			linearProbe(newEntries, newSize,
					oldEntry->hash, oldEntry->key, &newEntry, &present);
			assert(present == 0);
			*newEntry = *oldEntry;
		}
		oldEntry++;
	}
	free(hashTable->entries);
	hashTable->entries = newEntries;
	hashTable->size = newSize;
}

int hashTableSetValue(struct hash_table *hashTable, char *key, void *value){
	struct hash_entry *entry;
	int present;
	int hash;

	if(hashTable->size / 2 < hashTable->count){
		hashTableIncreaseSize(hashTable);
	}

	assert(hashTable->count < hashTable->size);

	hash = hashfn(key);
	linearProbe(hashTable->entries, hashTable->size, hash, key, &entry, &present);
	if(present){
		entry->value = value;
	} else {
		hashTable->count += 1;
		entry->occupied = 1;
		entry->hash = hash;
		entry->key = key;
		entry->value = value;
	}
	return 0;
}


int hashTableGetValue(struct hash_table *hashTable, char *key, void **value, int *found){
	struct hash_entry *entry;
	int hash;
	int present;

	hash = hashfn(key);
	linearProbe(hashTable->entries, hashTable->size, hash, key, &entry, &present);
	if(present){
		*found = 1;
		*value = entry->value;
	} else {
		*found = 0;
		*value = NULL;
	}
	return 0;
}

int hashTableRemove(struct hash_table *hashTable, char *key, void **value, int *found){
	struct hash_entry *entry;
	int hash;
	int present;

	hash = hashfn(key);
	linearProbe(hashTable->entries, hashTable->size, hash, key, &entry, &present);
	if(present){
		*found = 1;
		*value = entry->value;
		entry->occupied = 0;
		entry->hash = 0;
		entry->key = 0;
		entry->value = 0;
	} else {
		*found = 0;
		*value = NULL;
	}
	return 0;

}




#ifdef TESTING

char *testStrings[][2] = {
	{"helicopter", "blade"},
	{"hands", "fingers"},
	{"battle", "calvery"},
	{"stop", "moving"},
	{"jesus", "judas"},
	{"future", "past"},
	{"master", "class"},
	{NULL, NULL},
};


void test_hashTable(void){
	struct hash_table *hashTable;
	int i;

	hashTable = newHashTable();


	i = 0;
	while(testStrings[i][0] != NULL){
		char *key, *value;
		key = testStrings[i][0];
		value = testStrings[i][1];

		printf("STORE: %s -> %s\n", key, value);
		hashTableSetValue(hashTable, key, value);
		i += 1;
	}

	assert(hashTable->count == i);

	i = 0;
	while(testStrings[i][0] != NULL){
		char *key, *value, *hashValue;
		int found;
		key = testStrings[i][0];
		value = testStrings[i][1];
		printf("LOOKUP: %s\n", key);
		hashTableGetValue(hashTable, key, (void **)&hashValue, &found);
		assert(found);
		assert(value == hashValue);
		i += 1;
	}

	i = 0;
	while(testStrings[i][0] != NULL){
		char *key, *value, *hashValue;
		int found;
		key = testStrings[i][0];
		value = testStrings[i][1];
		printf("DELETE: %s\n", key);
		hashTableRemove(hashTable, key, (void **)&hashValue, &found);
		assert(found);
		assert(value == hashValue);
		i += 1;
	}

	i = 0;
	while(testStrings[i][0] != NULL){
		char *key, *value, *hashValue;
		int found;
		key = testStrings[i][0];
		value = testStrings[i][1];
		printf("LOOKUP: %s\n", key);
		hashTableGetValue(hashTable, key, (void **)&hashValue, &found);
		assert(found == 0);
		i += 1;
	}

	freeHashTable(hashTable);
}


int main(int argc, char *argv[]){
	test_hashTable();
	printf("OK\n");
	return 0;
}

#endif
