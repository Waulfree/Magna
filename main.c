#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <math.h>

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

typedef uint8_t *vmap_key_t;
typedef uint8_t *vmap_val_t;
typedef size_t vmap_hval_t;

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

#define STACK_LEN 32

enum tok_type {
	TOK_NONE,
	TOK_OP,
	TOK_ARG,
	TOK_END,

	TOK_SIZE
};

const char *tok_names[] = {
	"TOK_NONE",
	"TOK_OP",
	"TOK_ARG",
	"TOK_END"
};

enum op_type {
	OP_NOP,
	OP_END,
	OP_EQU,
	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV,
	OP_POW,
	OP_OPN,
	OP_CLS,

	OP_ARG,

	OP_SIZE
};

const char *op_names[] = {
	"OP_NOP",
	"OP_END",
	"OP_EQU",
	"OP_ADD",
	"OP_SUB",
	"OP_MUL",
	"OP_DIV",
	"OP_POW",
	"OP_OPN",
	"OP_CLS"
};

int prec[] = {-1, 1, 2, 3, 3, 4, 4, 5, 0, 0};

/*
enum fsm_state {
	PDA_INIT,
	PDA_OPERATOR,
	PDA_OPERAND,
	PDA_END,

	PDA_SIZE
};*/

void tokenize_next(enum op_type *op, const char **lex)
{
	if (*op != OP_NOP)
		(*lex)++;
	while (**lex == ' ') (*lex)++;

	switch (**lex) {
	case '=':
		*op = OP_EQU;
		break;
	case '+':
		*op = OP_ADD;
		break;
	case '-':
		*op = OP_SUB;
		break;
	case '*':
		*op = OP_MUL;
		break;
	case '/':
		*op = OP_DIV;
		break;
	case '^':
		*op = OP_POW;
		break;
	case '(':
		*op = OP_OPN;
		break;
	case ')':
		*op = OP_CLS;
		break;

	case ';':
	case '\0':
		*op = OP_NOP;
		break;
	
	default:
		*op = OP_ARG;
		break;
	}
}

#define OP_STACK_IS_NOT_EMPTY op_top < &op_stack[STACK_LEN]
#define OP_STACK_IS_EMPTY op_top == &op_stack[STACK_LEN]
#define TOP_P prec[*op_top]/*.prior*/
#define CUR_P prec[op]/*.prior*/
#define LOPR *(lex_top + 1)
#define ROPR *lex_top

void parse(const char *str)
{
	enum op_type op = OP_NOP;
	const char *lex = str;
	
	enum op_type op_stack[STACK_LEN];
	enum op_type *op_top = &op_stack[STACK_LEN - 1];

	int lex_stack[STACK_LEN];
	int *lex_top = &lex_stack[STACK_LEN];
	*op_top = OP_NOP;

next_token:
	if (*lex)
		tokenize_next(&op, &lex);
	else
		goto end;

	/* Inductor handlers */
	switch (op) {
	/* Argument handler */
	case OP_ARG:
		*--lex_top = atoi(lex);
		goto next_token;

	/* Operator handlers */
	case OP_EQU:
	case OP_POW:
	case OP_OPN:
		goto induce;
	case OP_CLS:
		goto reduce;

	default:
		if (TOP_P >= CUR_P)
			goto reduce;
		goto induce;
	}

reduce:
	if (TOP_P < CUR_P)
		goto induce;

	/* Reductor operator handlers */
	switch (*op_top) {
	case OP_NOP:
		printf("end\n");
		goto next_token;
	case OP_OPN:
		op_top++;
		goto next_token;

	case OP_EQU:
	case OP_ADD:
		printf("%i + %i = ", LOPR, ROPR);
		LOPR += ROPR;
		goto bop;
	case OP_SUB:
		printf("%i - %i = ", LOPR, ROPR);
		LOPR -= ROPR;
		goto bop;
	case OP_MUL:
		printf("%i * %i = ", LOPR, ROPR);
		LOPR *= ROPR;
		goto bop;
	case OP_DIV:
		printf("%i / %i = ", LOPR, ROPR);
		LOPR /= ROPR;
		goto bop;
	case OP_POW:
		printf("%i ^^ %i = ", LOPR, ROPR);
		LOPR = pow(LOPR, ROPR);
bop:
		printf("%i\n", LOPR);
		op_top++;
		lex_top++;
		goto reduce;

	default:	
		printf("%s\n", op_names[*op_top]);
		op_top++;
		goto reduce;
	}

induce:
	*--op_top = op;
	goto next_token;

end:
	return;
}

int main()
{
	parse("1 + 2 * 3 + (4 + 2 - (3 - 1)) * 3 + 4^3^2 * 7");

	return 0;
}
