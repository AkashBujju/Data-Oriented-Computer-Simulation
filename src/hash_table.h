#ifndef HASH_TABLE_H
#define HASH_TABLE_H

typedef struct Road Road;
typedef struct Junction Junction;
typedef struct Signal Signal;

unsigned int compute_hash(const unsigned int id, const unsigned int limit);
void put_road(unsigned int *all_ids, Road *roads, unsigned int id, Road *road, unsigned int limit);
Road* get_road(unsigned int *all_ids, Road *roads, unsigned int id, unsigned int limit);
Junction* get_junction(unsigned int *all_ids, Junction *junctions, unsigned int id, unsigned int limit);
void put_junction(unsigned int *all_ids, Junction *junctions, unsigned int id, Junction *junction, unsigned int limit);
Signal* get_signal(unsigned int *all_ids, Signal *signals, unsigned int id, unsigned int limit);
void put_signal(unsigned int *all_ids, Signal *signals, unsigned int id, Signal *signal, unsigned int limit);

#endif
