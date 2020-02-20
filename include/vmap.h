#ifndef MAGNA_VMAP_HEADER_GUARD

#include <stdlib.h>
#include <stdint.h>
#include <time.h>

typedef uint8_t *vmap_key_t;
typedef uint8_t *vmap_val_t;
typedef size_t vmap_hval_t;

/* TODO macro for 32bit constants */
const size_t fnv1a_prime = 0x100000001b3;
const size_t fnv1a_hash = 0xcbf29ce484222325;

union vmap_slot_node {
	struct vmap_slot **virt;
	struct vmap_slot *real;
	struct vmap_slot *free;
};

struct vmap_slot {
	/* 'prev' must be first */
	union vmap_slot_node prev;
	union vmap_slot_node next;
	vmap_key_t *key;
	vmap_val_t *val;
};

struct vmap {
	struct vmap_slot **vslots;
	struct vmap_slot *slots;
	size_t size;
	size_t seed;

	/* Points to the next free slot in a list of free slots */
	union vmap_slot_node next;
};

#ifdef MAGNA_MODULE
extern struct vmap init_vmap(const size_t);
extern void clean_vmap(struct vmap *);
extern struct vmap_slot **get_vslot_for_key(struct vmap *, vmap_key_t,
					    size_t len);
extern struct vmap_slot *alloc_slot_in_vslot(struct vmap *, 
					     struct vmap_slot **);
extern void free_slot_in_vmap(struct vmap *, struct vmap_slot *);
#else
#include "vmap.c"
#endif

#define MAGNA_VMAP_HEADER_GUARD
#endif
