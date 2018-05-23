#include "_test_common.h"

extern void _rotate_left(struct rbtree_t* tree, rbnode_t* node);
extern void _rotate_right(struct rbtree_t* tree, rbnode_t* node);

void setup_rotate_test_tree(void** state) {
  /* create a tree like this(R for red and B for black):
                      8B
                  /       \
                4R        10B
              /    \     /  \
             2B    6B  9R   11R
            / \   / \
           1R 3R 5R 7R
  */
  const LargestIntegralType node_nums[] = {
    8, 4, 10, 2, 6, 9, 11, 1, 3, 5, 7,
  };
  int i;
  struct rbtree_t* tree = create_rbtree(_alloc_tnode, _free_tnode, _tnode_compare, _tvalue_compare);

  assert_true(NULL != tree);
  assert_true(sizeof(uintptr_t) <= sizeof(LargestIntegralType));
  for (i = 0; i < countof(node_nums); i++) {
    assert_true(rbt_insert(tree, (void*)(int)node_nums[i]));
  }

  *state = tree;
}

void teardown_rotate_test_tree(void** state) {
  struct rbtree_t* tree = (struct rbtree_t*)*state;
  destroy_rbtree(tree);
}

typedef struct _node_expectation_t {
  int node_num;
  bool node_is_red;

  int left_num;
  int right_num;
  int parent_num;
}_node_expectation_t;

const _node_expectation_t* _get_node_expection(const _node_expectation_t* expects, const int expects_cnt, const int num) {
  int i;
  for (i = 0; i < expects_cnt; ++i) {
    if (num == expects[i].node_num) {
      return expects + i;
    }
  }

  assert_false("can't find num in rotate_results");
  return NULL;
}

bool _node_is_red(const _node_expectation_t* expects, const int expects_cnt, const int num) {
  const _node_expectation_t* expect = _get_node_expection(expects, expects_cnt, num);

  return expect->node_is_red;
}

typedef struct _rotate_acc_t {
  const _node_expectation_t* expects;
  int expects_cnt;
}_rotate_acc_t;

static void
_node_expect_check(rbnode_t* node, const int expect_num, const _node_expectation_t* expects, const int expects_cnt, const char* const fmt, ...) {
  tnode_t* tnode = container_of(node, tnode_t, node);
  bool expect_is_red;
  //va_list args;

  if (-1 == expect_num) {
    assert_true(NULL == node);
    return;
  }

  /*
  va_start(args, fmt);
  vprint_message(fmt, args);
  va_end(args);
  */

  assert_true(NULL != node);
  assert_int_equal(expect_num, tnode->num);

  expect_is_red = _node_is_red(expects, expects_cnt, expect_num);
  assert_true(_is_red(node) == expect_is_red);
}

static void
_assert_rotated_tree(tnode_t* tnode, int red_cnt, int black_cnt, void* arg) {
  const _rotate_acc_t* ra = (const _rotate_acc_t*)arg;
  rbnode_t* node = &tnode->node;
  rbnode_t *parent = _parent(node);
  rbnode_t *left = _left_child(node);
  rbnode_t *right = _right_child(node);
  const _node_expectation_t* expect = _get_node_expection(ra->expects, ra->expects_cnt, tnode->num);

  _node_expect_check(node, expect->node_num, ra->expects, ra->expects_cnt, "checking node %d", tnode->num);
  _node_expect_check(left, expect->left_num, ra->expects, ra->expects_cnt, "checking node %d left", tnode->num);
  _node_expect_check(right, expect->right_num, ra->expects, ra->expects_cnt, "checking node %d right", tnode->num);
  _node_expect_check(parent, expect->parent_num, ra->expects, ra->expects_cnt, "checking node %d parent", tnode->num);
}

void test_rbtree_rotate_left(void** state) {
  struct tnode_t* root_tnode;
  struct rbtree_t* tree = (struct rbtree_t*)*state;
  struct rbnode_t* rotating_node = _left_child(_root(tree));
  /* after rotate left of rotating_node, the tree like this:
                      8B
                  /       \
                 6R       10B
                /  \      /  \
              4B   7R    9R   11R
             /  \
            2B  5R
           / \
          1R 3R
  */
  const _node_expectation_t rotated_expects[] = {
    {1, true, -1, -1, 2},
    {2, false, 1, 3, 4},
    {3, true, -1, -1, 2},
    {4, false, 2, 5, 6},
    {5, true, -1, -1, 4},
    {6, true, 4, 7, 8},
    {7, true, -1, -1, 6},
    {8, false, 6, 10, -1},
    {9, true, -1, -1, 10},
    {10, false, 9, 11, 8},
    {11, true, -1, -1, 10},
  };
  _rotate_acc_t ra = {rotated_expects, countof(rotated_expects)};

  assert(NULL != rotating_node);
  _rotate_left(tree, rotating_node);

  root_tnode = container_of(_root(tree), struct tnode_t, node);
  assert_int_equal(8, root_tnode->num);

  _pre_order_traversal(tree, _assert_rotated_tree, &ra);
}

void test_rbtree_rotate_right(void** state) {
  struct tnode_t* root_tnode;
  struct rbtree_t* tree = (struct rbtree_t*)*state;
  struct rbnode_t* rotating_node = _left_child(_root(tree));
  /*after rotate right ot rotating_node, the tree looks like this:
                      8B
                  /       \
                2R        10B
              /    \     /  \
             1R    4B  9R   11R
                  / \
                 3R 6B
                   / \
                  5R 7R
  */
  const _node_expectation_t rotated_expects[] = {
    {1, true, -1, -1, 2},
    {2, true, 1, 4, 8},
    {3, true, -1, -1, 4},
    {4, false, 3, 6, 2},
    {5, true, -1, -1, 6},
    {6, false, 5, 7, 4},
    {7, true, -1, -1, 6},
    {8, false, 2, 10, -1},
    {9, true, -1, -1, 10},
    {10, false, 9, 11, 8},
    {11, true, -1, -1, 10},
  };
  _rotate_acc_t ra = {rotated_expects, countof(rotated_expects)};

  assert(NULL != rotating_node);
  _rotate_right(tree, rotating_node);

  root_tnode = container_of(_root(tree), tnode_t, node);
  assert_int_equal(8, root_tnode->num);

  _pre_order_traversal(tree, _assert_rotated_tree, &ra);
}

void test_rbtree_root_rotate_left(void** state) {
  struct tnode_t* root_tnode;
  struct rbtree_t* tree = (struct rbtree_t*)*state;
  struct rbnode_t* rotating_node = _root(tree);
  /*after rotate right ot rotating_node, the tree looks like this:
                      10B
                    /    \
                   8B    11R
                  /  \
                4R   9R
              /    \
             2B    6B
            / \   / \
           1R 3R 5R 7R
  */
  const _node_expectation_t rotated_expects[] = {
    {1, true, -1, -1, 2},
    {2, false, 1, 3, 4},
    {3, true, -1, -1, 2},
    {4, true, 2, 6, 8},
    {5, true, -1, -1, 6},
    {6, false, 5, 7, 4},
    {7, true, -1, -1, 6},
    {8, false, 4, 9, 10},
    {9, true, -1, -1, 8},
    {10, false, 8, 11, -1},
    {11, true, -1, -1, 10},
  };
  _rotate_acc_t ra = {rotated_expects, countof(rotated_expects)};

  assert(NULL != rotating_node);
  _rotate_left(tree, rotating_node);

  root_tnode = container_of(_root(tree), tnode_t, node);
  assert_int_equal(10, root_tnode->num);

  _pre_order_traversal(tree, _assert_rotated_tree, &ra);
}

void test_rbtree_root_rotate_right(void** state) {
  struct tnode_t* root_tnode;
  struct rbtree_t* tree = (struct rbtree_t*)*state;
  struct rbnode_t* rotating_node = _root(tree);
  /*after rotate right ot rotating_node, the tree looks like this:
                     4B
                 /       \
                2B        8R
              /  \       /   \
             1R  3R    6B   10B
                      / \   / \
                     5R 7R 9R 11R
  */
  const _node_expectation_t rotated_expects[] = {
    {1, true, -1, -1, 2},
    {2, false, 1, 3, 4},
    {3, true, -1, -1, 2},
    {4, false, 2, 8, -1},
    {5, true, -1, -1, 6},
    {6, false, 5, 7, 8},
    {7, true, -1, -1, 6},
    {8, true, 6, 10, 4},
    {9, true, -1, -1, 10},
    {10, false, 9, 11, 8},
    {11, true, -1, -1, 10},
  };
  _rotate_acc_t ra = {rotated_expects, countof(rotated_expects)};

  assert(NULL != rotating_node);
  _rotate_right(tree, rotating_node);

  root_tnode = container_of(_root(tree), tnode_t, node);
  assert_int_equal(4, root_tnode->num);

  _pre_order_traversal(tree, _assert_rotated_tree, &ra);
}
