#ifndef _AVL_H
#define _AVL_H

#include <stdint.h>
#include <stddef.h>

/*
 * The definition has been stolen from the Linux kernel.
 */
#ifdef __GNUC__
#  define avltree_container_of(node, type, member) ({			\
	const struct avltree_node *__mptr = (node);			\
	(type *)( (char *)__mptr - offsetof(type,member) );})
#else
#  define avltree_container_of(node, type, member)			\
	((type *)((char *)(node) - offsetof(type, member)))
#endif	/* __GNUC__ */

struct avltree_node {
	struct avltree_node *left, *right;
	struct avltree_node *parent;
	signed balance:3;		/* balance factor [-2:+2] */
};

typedef int (*avltree_cmp_fn_t)(const struct avltree_node *, const struct avltree_node *);

struct avltree {
	struct avltree_node *root;
	avltree_cmp_fn_t cmp_fn;
	int height;
	struct avltree_node *first, *last;
	uint64_t reserved[4];
};

struct avltree_node *avltree_first(const struct avltree *tree);
struct avltree_node *avltree_last(const struct avltree *tree);
struct avltree_node *avltree_next(const struct avltree_node *node);
struct avltree_node *avltree_prev(const struct avltree_node *node);

struct avltree_node *avltree_lookup(const struct avltree_node *key, const struct avltree *tree);
struct avltree_node *avltree_insert(struct avltree_node *node, struct avltree *tree);
void avltree_remove(struct avltree_node *node, struct avltree *tree);
void avltree_replace(struct avltree_node *old, struct avltree_node *node, struct avltree *tree);
int avltree_init(struct avltree *tree, avltree_cmp_fn_t cmp, unsigned long flags);

#endif 
