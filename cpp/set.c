/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define STRINGSET_DEFAULT_SIZE (4)
#define STRINGSET_INC_FACTOR (2)

struct string_set_entry {
	int hash;
	char *string;
};

struct string_set {
	struct string_set_entry *entries;
	int count;
	int size;
};

struct string_set *newStringSet(void){
	struct string_set *stringSet;

	stringSet = calloc(1, sizeof(*stringSet));
	assert(stringSet != NULL);

	stringSet->count = 0;
	stringSet->size = STRINGSET_DEFAULT_SIZE;
	stringSet->entries = calloc(stringSet->size, sizeof(struct string_set_entry));
	assert(stringSet->entries != NULL);
	return stringSet;
}

void freeStringSet(struct string_set *stringSet){
	free(stringSet->entries);
	stringSet->entries = 0;
	free(stringSet);
}

static int stringSetHashFn(char *s){
	int h = 0x11111;
	while(*s){
		h += *s++ * 31;
		if(h < 0){
			h *= -1;
		}
	}
	return h;
}


static int linearProbe(struct string_set_entry *entries, int size, int hash, char *key,
		struct string_set_entry **entry, int *present){

	struct string_set_entry *e;
	int n;

	e = entries + (hash % size);
	n = size;
	while(n > 0){
		if(e->string == NULL){
			*entry = e;
			*present = 0;
			return 0;
		} else if(e->hash == hash && strcmp(e->string, key) == 0){
			*entry = e;
			*present = 1;
			return 0;
		} else {
			e++;
			if(e >= entries + size){
				e = entries;
			}
			n -= 1;
		}
	}
	*entry = NULL;
	*present = 0;
	return -1;
}

static int stringSetIncreaseSize(struct string_set *set){
	int newSize;
	struct string_set_entry *newEntries;
	struct string_set_entry *e;

	newSize = set->size * STRINGSET_INC_FACTOR;
	assert(set->size < newSize);

	newEntries = calloc(newSize, sizeof(struct string_set_entry));
	assert(newEntries != NULL);

	e = set->entries;
	while(e < set->entries + set->size){
		if(e->string){
			struct string_set_entry *newEntry;
			int present;

			linearProbe(newEntries, newSize, e->hash, e->string, &newEntry, &present);
			assert(present == 0);
			*newEntry = *e;
		}
		e++;
	}
	set->entries = newEntries;
	set->size = newSize;
	return 0;
}

int stringSetInsertString(struct string_set *set, char *s){
	int hash;
	int n;

	if(set->size / 2 < set->count){
		stringSetIncreaseSize(set);
	}
	assert(set->count < set->size);


	hash = stringSetHashFn(s);

	struct string_set_entry *foundEntry;
	int present;

	linearProbe(set->entries, set->size, hash, s, &foundEntry, &present);
	if(!present){
		foundEntry->hash = hash;
		foundEntry->string = s;
		set->count += 1;
	}
	return 0;
}

int stringSetIsMember(struct string_set *set, char *s){
	int hash;
	int n;

	/*
	if(set->size / 2 < set->count){
		stringSetIncreaseSize(set);
	}
	assert(set->count < set->size);
	*/

	hash = stringSetHashFn(s);

	struct string_set_entry *foundEntry;
	int present;

	linearProbe(set->entries, set->size, hash, s, &foundEntry, &present);
	return present;
}

#ifdef TESTING

char *testStrings[] = {
	"herp",
	"derp",
	"show",
	"joust",
	"town",
	"pixels",
	"pacman",
	"quarter",
	NULL,
};

void test_stringSet(void){
	struct string_set *stringSet;
	int member;
	int i;

	stringSet = newStringSet();

	i = 0;
	while(testStrings[i]){
		printf("GET: %s\n", testStrings[i]);
		member = stringSetIsMember(stringSet, testStrings[i]);
		assert(member == 0);
		printf("SET: %s\n", testStrings[i]);
		stringSetInsertString(stringSet, testStrings[i]);
		member = stringSetIsMember(stringSet, testStrings[i]);
		assert(member != 0);
		i += 1;
	}

	i = 0;
	while(testStrings[i]){
		printf("MEM: %s\n", testStrings[i]);
		member = stringSetIsMember(stringSet, testStrings[i]);
		assert(member != 0);
		i += 1;
	}
	freeStringSet(stringSet);
	printf("OK\n");
}

int main(int argc, char *argv[]){
	test_stringSet();
	return 0;
}

#endif
