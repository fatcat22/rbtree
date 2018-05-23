#ifndef __ZY_RB_TREE_H__
#define __ZY_RB_TREE_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

#include <stddef.h>
#include <stdbool.h>

struct rbtree_t;

/*
 * 将rbnode结构暴露出来是为了调用者可以将其嵌入到自己定义的结构体中，这样可以减少内存申请次数。
 * 但不建议直接使用rb_node内的任何一个字段。
*/
typedef struct rbnode_t {
  struct rbnode_t* _parent;
  struct rbnode_t* _left;
  struct rbnode_t* _right;
  int _flags;
}rbnode_t;

typedef rbnode_t* (*rbnode_alloc_t)(void* val);
typedef void (*rbnode_free_t)(rbnode_t* node);
typedef int (*rbnode_compare_t)(const rbnode_t* node1, const rbnode_t* node2);
typedef int (*rbvalue_compare_t)(void* val, const rbnode_t* node);

#ifndef container_of
#define container_of(ptr, type, member) ({ \
     const typeof( ((type *)0)->member ) *__mptr = (ptr); \
     (type *)( (char *)__mptr - offsetof(type,member) );})
#endif /*container_of*/


struct rbtree_t* create_rbtree(rbnode_alloc_t rb_alloc, rbnode_free_t rb_free, rbnode_compare_t node_cmp, rbvalue_compare_t value_cmp);
void destroy_rbtree(struct rbtree_t* tree);

bool rbt_insert(struct rbtree_t* tree, void* val);
//bool rbt_insert(struct rbtree_t* root, rbnode_t* node);

/*remove and free a node*/
bool rbt_remove(struct rbtree_t* tree, void* val);
bool rbt_remove2(struct rbtree_t* root, rbnode_t* node);
/*remove safely when enumearte the tree.*/

void* rbt_begin_enumeration(struct rbtree_t* tree);
rbnode_t* rbt_next_node(void*);
void rbt_end_enumeration(void*);

/*a inner loop enumeration which wrap up rbt_begin_enumeration/rbt_end_enumeration
  accessor return false if you want stop enumeration.
*/
bool rbt_foreach_safe(struct rbtree_t* root, bool (*accessor)(const rbnode_t* node, void* arg), void* arg);

rbnode_t* rbt_find(struct rbtree_t* tree, void* val);



#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*__ZY_RB_TREE_H__*/
