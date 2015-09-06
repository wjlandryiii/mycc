/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef HASHTABLE_H
#define HASHTABLE_H

struct hash_table;

struct hash_table *newHashTable();

void freeHashTable(
		struct hash_table *hashTable);
int hashTableSetValue(
		struct hash_table *hashTable,
		char *key,
		void *value);
int hashTableGetValue(
		struct hash_table *hashTable,
		char *key,
		void **value,
		int *found);
int hashTableRemove(
		struct hash_table *hashTable,
		char *key,
		void **value,
		int *found);

#endif
