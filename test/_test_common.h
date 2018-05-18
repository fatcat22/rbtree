#ifndef __TEST_COMMON_H__
#define __TEST_COMMON_H__

#include <setjmp.h>
#include <stdarg.h>
#include <stdlib.h>
#include <google/cmockery.h>

#ifdef UNIT_TESTING
#undef assert
#define assert(expression) \
    mock_assert((int)(expression), #expression, __FILE__, __LINE__);
#endif // UNIT_TESTING

#include "../rbtree.h"
rbnode_t* _root(struct rbtree_t* tree);
rbnode_t* _parent(rbnode_t* node);
rbnode_t* _left_child(rbnode_t* node);
rbnode_t* _right_child(rbnode_t* node);
bool _empty_tree(struct rbtree_t* tree);
bool _is_root(struct rbtree_t* tree, rbnode_t* node);
bool _is_leaf(rbnode_t* node);
bool _is_red(const rbnode_t* node);
bool _is_black(const rbnode_t* node);
void _set_left_child(rbnode_t* node, rbnode_t* child);
void _set_right_child(rbnode_t* node, rbnode_t* child);
void _set_parent(rbnode_t* node, rbnode_t* parent);
void _set_red(rbnode_t* node);
void _set_black(rbnode_t* node);
void _set_root(struct rbtree_t* tree, rbnode_t* node);


#ifndef countof
#define countof(arr) (sizeof((arr))/sizeof((arr)[0]))
#endif/*countof*/

typedef struct tnode_t
{
  int num;
  rbnode_t node;
}tnode_t;

struct tnode_t* _create_tnode(int num);
void _free_tnode(rbnode_t* node);
int _tnode_compare(const rbnode_t* n1, const rbnode_t* n2);
bool _is_in_set(const int val, const int* sets, const size_t sets_cnt);

/*
  argument of accessor:
  @tnode: the node being accessed.
  @red_cnt: red node count in the path of reaching tnode.
  @black-cnt: black node count in the path of reaching tnode.
  @arg: argument from caller.
*/
void _pre_order_traversal(struct rbtree_t* tree, void(*accessor)(tnode_t* tnode, int red_cnt, int black_cnt, void* arg), void* arg);
void _in_order_traversal(struct rbtree_t* tree, void(*accessor)(tnode_t* tnode, int red_cnt, int black_cnt, void* arg), void* arg);

/*if black_cnt is not -1, the total black node count in every branch must be black_cnt.*/
void _assert_rbtree_rule(struct rbtree_t* tree, int black_cnt);

#endif /*__TEST_COMMON_H__*/
