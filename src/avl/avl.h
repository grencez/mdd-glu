/*
 * Revision Control Information
 *
 * /projects/hsis/CVS/utilities/avl/avl.h,v
 * rajeev
 * 1.3
 * 1995/08/08 22:36:24
 *
 */
#ifndef AVL_INCLUDED
#define AVL_INCLUDED


typedef struct avl_node_struct avl_node;
struct avl_node_struct {
    avl_node *left, *right;
    char *key;
    char *value;
    int height;
};


typedef struct avl_tree_struct avl_tree;
struct avl_tree_struct {
    avl_node *root;
    int (*compar)(const void *, const void *);
    int num_entries;
    int modified;
};


typedef struct avl_generator_struct avl_generator;
struct avl_generator_struct {
    avl_tree *tree;
    avl_node **nodelist;
    int count;
};


#define AVL_FORWARD 	0
#define AVL_BACKWARD 	1


EXTERN avl_tree *avl_init_table(int (*)(const void *, const void *));
EXTERN int avl_delete(avl_tree *, void *, void *);
EXTERN int avl_insert(avl_tree *, void *, void *);
EXTERN int avl_lookup(avl_tree *, const void *, void *);
EXTERN int avl_first(avl_tree *, char **, char **);
EXTERN int avl_last(avl_tree *, char **, char **);
EXTERN int avl_find_or_add(avl_tree *, char *, char ***);
EXTERN int avl_count(avl_tree *);
EXTERN int avl_numcmp(const void *, const void *);
EXTERN int avl_check_tree(avl_tree *tree);
EXTERN int avl_gen(avl_generator *, char **, char **);
EXTERN void avl_foreach(avl_tree *, void (*)(const void *, const void *), int);
EXTERN void avl_free_table(avl_tree *, void (*)(char *), void (*)(char *));
EXTERN void avl_free_gen(avl_generator *);
EXTERN avl_generator *avl_init_gen(avl_tree *, int);

#define avl_is_member(tree, key)	avl_lookup(tree, key, (char **) 0)

#define avl_foreach_item(table, gen, dir, key_p, value_p) 	\
    for(gen = avl_init_gen(table, dir); 			\
	    avl_gen(gen, key_p, value_p) || (avl_free_gen(gen),0);)

#endif
