#include "_test_common.h"

void test_rbtree_find(void** state) {
    int i;
    rbnode_t* node;
    const int node_nums[] = {
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
        17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
    };

    struct rbtree_t* tree = create_rbtree(_alloc_tnode, _free_tnode, _tnode_compare, _tvalue_compare);
    for (i = 0; i < countof(node_nums); i++) {
        rbt_insert(tree, (void*)node_nums[i]);
    }

    for (i = countof(node_nums) - 1; i > 0 ; i--) {
        node = rbt_find(tree, (void*)node_nums[i]);
        assert_int_equal(node_nums[i], container_of(node, tnode_t, node)->num);
    }

    destroy_rbtree(tree);
}
