#ifdef MAGNA_MODULE
#include "vmap.h"
#endif

/* 
 *	Use get_vslot_for_key(your_vmap, your_key)
 *		To get virtual slot for any of your needs.
 *
 *	alloc_slot_in_vslot(your_vmap, virtual_slot)
 *		will return you a free to use slot in O(1);
 *
 *	free_slot_in_vmap(your_vmap, real_slot)
 *		will delete specified slot in O(1).
 *
 */

struct vmap init_vmap(const size_t sz)
{
	struct vmap h;
	union vmap_slot_node n;
	struct vmap_slot *last;

	srand(time(NULL));

	/* Both vslots and slots lies in the same memchunk */
	h.vslots = (struct vmap_slot **)
	    malloc((sizeof(struct vmap_slot *) +
		    sizeof(struct vmap_slot)) * sz);
	h.slots = (struct vmap_slot *)(h.vslots + sz);
	h.next.free = h.slots;
	h.size = sz;
	h.seed = (fnv1a_hash ^ rand()) * fnv1a_prime;
	last = h.slots + sz - 1;

	/* All virtual slots must point to the NULL at start */
	for (n.virt = h.vslots; n.real < h.slots; n.virt++)
		*n.virt = NULL;

	/* All real slots(except the last one) must point to the next slot */
	for (n.real = h.slots; n.real < last; n.real++)
		n.real->next.free = n.real + 1;
	n.real->next.free = NULL;

	return h;
}

void clean_vmap(struct vmap *self)
{
	free(self->vslots);
}

struct vmap_slot **get_vslot_for_key(struct vmap *self, vmap_key_t k,
				     size_t len)
{
	size_t hash = self->seed;
	
	for (; len; len--, k++)
		hash = (hash ^ *k) * fnv1a_prime;
	return &self->vslots[hash % self->size];
}

struct vmap_slot *alloc_slot_in_vslot(struct vmap *self, struct vmap_slot **v)
{
	struct vmap_slot *r;

	/* Occupy free slot */
	r = self->next.free;
	self->next.free = r->next.free;

	/* Insert occupied slot into vslot */
	if (!*v) {
		r->prev.virt = v;
		r->next.virt = NULL;
		*v = r;
	} else {
		r->prev.virt = v;
		(*v)->prev.real = r;
		r->next.real = *v;
	}

	return r;
}

void free_slot_in_vmap(struct vmap *self, struct vmap_slot *r)
{
	/* If last element set virtual slot to NULL */
	if (!r->next.real && *r->prev.virt == r)
		*r->prev.virt = NULL;

	/* Insert freed slot back into self->next.free */
	r->next.free = self->next.free;
	self->next.free = r;
}
