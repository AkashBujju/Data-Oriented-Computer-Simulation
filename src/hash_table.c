#include "hash_table.h"
#include "sim.h"

unsigned int compute_hash(const unsigned int id, const unsigned int limit) {
	return (id & 0x7fffffff) % limit;
}

void put_road(unsigned int *all_ids, Road *roads, unsigned int id, Road *road, unsigned int limit) {
	unsigned int i;
	for(i = compute_hash(id, limit); all_ids[i] != 0; i = (i + 1) & (limit - 1)) {
		if(all_ids[i] == id)
			break;
	}

	all_ids[i] = id;
	roads[i] = *road;
}

Road* get_road(unsigned int *all_ids, Road *roads, unsigned int id, unsigned int limit) {
	unsigned int i = compute_hash(id, limit);
	unsigned int k = all_ids[i];

	while(k != 0) {
		if(k == id)
			return &roads[i];

		i = (i + 1) & (limit - 1);
		k = all_ids[i];
	}

	return 0;
}

void put_junction(unsigned int *all_ids, Junction *junctions, unsigned int id, Junction *junction, unsigned int limit) {
	unsigned int i;
	for(i = compute_hash(id, limit); all_ids[i] != 0; i = (i + 1) & (limit - 1)) {
		if(all_ids[i] == id)
			break;
	}

	all_ids[i] = id;
	junctions[i] = *junction;
}

Junction* get_junction(unsigned int *all_ids, Junction *junctions, unsigned int id, unsigned int limit) {
	unsigned int i = compute_hash(id, limit);
	unsigned int k = all_ids[i];

	while(k != 0) {
		if(k == id)
			return &junctions[i];

		i = (i + 1) & (limit - 1);
		k = all_ids[i];
	}

	return 0;
}

void put_signal(unsigned int *all_ids, Signal *signals, unsigned int id, Signal *signal, unsigned int limit) {
	unsigned int i;
	for(i = compute_hash(id, limit); all_ids[i] != 0; i = (i + 1) & (limit - 1)) {
		if(all_ids[i] == id)
			break;
	}

	all_ids[i] = id;
	signals[i] = *signal;
}

Signal* get_signal(unsigned int *all_ids, Signal *signals, unsigned int id, unsigned int limit) {
	unsigned int i = compute_hash(id, limit);
	unsigned int k = all_ids[i];

	while(k != 0) {
		if(k == id)
			return &signals[i];

		i = (i + 1) & (limit - 1);
		k = all_ids[i];
	}

	return 0;
}
