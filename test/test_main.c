#include "_test_common.h"

void test_rbtree_create_assert(void** state);
void test_rbtree_destroy_assert(void** state);
void test_rbtree_create_destroy(void** state);

void setup_rotate_test_tree(void** state);
void teardown_rotate_test_tree(void** state);
void test_rbtree_rotate_left(void** state);
void test_rbtree_rotate_right(void** state);
void test_rbtree_root_rotate_left(void** state);
void test_rbtree_root_rotate_right(void** state);

void test_rbtree_remove_left_red_leaf(void** state);
void test_rbtree_remove_left_black_leaf__case31(void** state);
void test_rbtree_remove_left_black_leaf__case32(void** state);
void test_rbtree_remove_left_black_leaf__case33(void** state);
void test_rbtree_remove_left_black_leaf__case34(void** state);
void test_rbtree_remove_right_red_leaf(void** state);
void test_rbtree_remove_right_black_leaf__case31(void** state);
void test_rbtree_remove_right_black_leaf__case32(void** state);
void test_rbtree_remove_right_black_leaf__case33(void** state);
void test_rbtree_remove_right_black_leaf__case34(void** state);
void test_rbtree_remove_left_red_node__has_only_left_child__substitued_by_red__case1(void** state);
void test_rbtree_remove_left_red_node__has_only_left_child__substitued_by_black__case2(void** state);
void test_rbtree_remove_left_red_node__has_only_left_child__substitued_by_black__case31(void** state);
void test_rbtree_remove_left_red_node__has_only_left_child__substitued_by_black__case32(void** state);
void test_rbtree_remove_left_red_node__has_only_left_child__substitued_by_black__case33(void** state);
void test_rbtree_remove_left_red_node__has_only_left_child__substitued_by_black__case34(void** state);
void test_rbtree_remove_left_red_node__has_only_left_child__substitued_by_black__case35(void** state);
void test_rbtree_remove_left_red_node__has_only_right_child__substitued_by_red__case1(void** state);
void test_rbtree_remove_left_red_node__has_only_right_child__substitued_by_black__case2(void** state);
void test_rbtree_remove_left_red_node__has_only_right_child__substitued_by_black__case31(void** state);
void test_rbtree_remove_left_red_node__has_only_right_child__substitued_by_black__case32(void** state);
void test_rbtree_remove_left_red_node__has_only_right_child__substitued_by_black__case33(void** state);
void test_rbtree_remove_left_red_node__has_only_right_child__substitued_by_black__case34(void** state);
void test_rbtree_remove_left_red_node__has_only_right_child__substitued_by_black__case35(void** state);
void test_rbtree_remove_right_red_node__has_only_left_child__substitued_by_red__case1(void** state);
void test_rbtree_remove_right_red_node__has_only_left_child__substitued_by_black__case2(void** state);
void test_rbtree_remove_right_red_node__has_only_left_child__substitued_by_black__case31(void** state);
void test_rbtree_remove_right_red_node__has_only_left_child__substitued_by_black__case32(void** state);
void test_rbtree_remove_right_red_node__has_only_left_child__substitued_by_black__case33(void** state);
void test_rbtree_remove_right_red_node__has_only_left_child__substitued_by_black__case34(void** state);
void test_rbtree_remove_right_red_node__has_only_left_child__substitued_by_black__case35(void** state);
void test_rbtree_remove_right_red_node__has_only_right_child__substitued_by_red_case1(void** state);
void test_rbtree_remove_right_red_node__has_only_right_child__substitued_by_black__case2(void** state);
void test_rbtree_remove_right_red_node__has_only_right_child__substitued_by_black__case31(void** state);
void test_rbtree_remove_right_red_node__has_only_right_child__substitued_by_black__case32(void** state);
void test_rbtree_remove_right_red_node__has_only_right_child__substitued_by_black__case33(void** state);
void test_rbtree_remove_right_red_node__has_only_right_child__substitued_by_black__case34(void** state);
void test_rbtree_remove_right_red_node__has_only_right_child__substitued_by_black__case35(void** state);
void test_rbtree_remove_left_black_node__has_only_left_child__substitued_by_red__case1(void** state);
void test_rbtree_remove_left_black_node__has_only_left_child__substitued_by_black__case2(void** state);
void test_rbtree_remove_left_black_node__has_only_left_child__substitued_by_black__case31(void** state);
void test_rbtree_remove_left_black_node__has_only_left_child__substitued_by_black__case32(void** state);
void test_rbtree_remove_left_black_node__has_only_left_child__substitued_by_black__case33(void** state);
void test_rbtree_remove_left_black_node__has_only_left_child__substitued_by_black__case34(void** state);
void test_rbtree_remove_left_black_node__has_only_left_child__substitued_by_black__case35(void** state);
void test_rbtree_remove_left_black_node__has_only_right_child__substitued_by_red__case1(void** state);
void test_rbtree_remove_left_black_node__has_only_right_child__substitued_by_black__case2(void** state);
void test_rbtree_remove_left_black_node__has_only_right_child__substitued_by_black__case31(void** state);
void test_rbtree_remove_left_black_node__has_only_right_child__substitued_by_black__case32(void** state);
void test_rbtree_remove_left_black_node__has_only_right_child__substitued_by_black__case33(void** state);
void test_rbtree_remove_left_black_node__has_only_right_child__substitued_by_black__case34(void** state);
void test_rbtree_remove_left_black_node__has_only_right_child__substitued_by_black__case35(void** state);
void test_rbtree_remove_right_black_node__has_only_left_child__substitued_by_red__case1(void** state);
void test_rbtree_remove_right_black_node__has_only_left_child__substitued_by_black__case2(void** state);
void test_rbtree_remove_right_black_node__has_only_left_child__substitued_by_black__case31(void** state);
void test_rbtree_remove_right_black_node__has_only_left_child__substitued_by_black__case32(void** state);
void test_rbtree_remove_right_black_node__has_only_left_child__substitued_by_black__case33(void** state);
void test_rbtree_remove_right_black_node__has_only_left_child__substitued_by_black__case34(void** state);
void test_rbtree_remove_right_black_node__has_only_left_child__substitued_by_black__case35(void** state);
void test_rbtree_remove_right_black_node__has_only_right_child__substitued_by_red__case1(void** state);
void test_rbtree_remove_right_black_node__has_only_right_child__substitued_by_black__case2(void** state);
void test_rbtree_remove_right_black_node__has_only_right_child__substitued_by_black__case31(void** state);
void test_rbtree_remove_right_black_node__has_only_right_child__substitued_by_black__case32(void** state);
void test_rbtree_remove_right_black_node__has_only_right_child__substitued_by_black__case33(void** state);
void test_rbtree_remove_right_black_node__has_only_right_child__substitued_by_black__case34(void** state);
void test_rbtree_remove_right_black_node__has_only_right_child__substitued_by_black__case35(void** state);
void test_rbtree_remove_left_red_fullnode__substitued_by_red(void** state);
void test_rbtree_remove_left_red_fullnode__substitued_by_black(void** state);
void test_rbtree_remove_left_black_fullnode__substitued_by_red(void** state);
void test_rbtree_remove_left_black_fullnode__substitued_by_black(void** state);
void test_rbtree_remove_right_red_fullnode__substitued_by_red(void** state);
void test_rbtree_remove_right_red_fullnode__substitued_by_black(void** state);
void test_rbtree_remove_right_black_fullnode__substitued_by_red(void** state);
void test_rbtree_remove_right_black_fullnode__substitued_by_black(void** state);
void test_rbtree_remove_root__without_child(void** state);
void test_rbtree_remove_root__has_only_left_child__substitued_by_red(void** state);
void test_rbtree_remove_root__has_only_right_child__substitued_by_red(void** state);
void test_rbtree_remove_root__has_all_childs__substitued_by_red(void** state);
void test_rbtree_remove_root__has_all_childs__substitued_by_black(void** state);


void test_rbtree_ascending_insert(void** state);
void test_rbtree_descending_insert(void** state);

void setup_enumerate_test_rbtree(void** state);
void teardown_enumerate_test_rbtree(void** state);
void test_rbtree_enumeration_safe(void** state);
void test_rbtree_enumeration_safe_del_first(void** state);
void test_rbtree_enumeration_safe_del_last(void** state);
void test_rbtree_enumeration_safe_del_root(void** state);
void test_rbtree_enumeration_safe_del_two_leaf(void** state);
void test_rbtree_enumeration_safe_del_three_fullnode(void** state);

void test_rbtree_find(void** state);

int main(int argc, char* argv[]) {
    const UnitTest tests[] = {
        unit_test(test_rbtree_create_assert),
        unit_test(test_rbtree_destroy_assert),
        unit_test(test_rbtree_create_destroy),
        unit_test_setup_teardown(test_rbtree_rotate_left, setup_rotate_test_tree, teardown_rotate_test_tree),
        unit_test_setup_teardown(test_rbtree_rotate_right, setup_rotate_test_tree, teardown_rotate_test_tree),
        unit_test_setup_teardown(test_rbtree_root_rotate_left, setup_rotate_test_tree, teardown_rotate_test_tree),
        unit_test_setup_teardown(test_rbtree_root_rotate_right, setup_rotate_test_tree, teardown_rotate_test_tree),
        unit_test(test_rbtree_ascending_insert),
        unit_test(test_rbtree_descending_insert),
        unit_test(test_rbtree_find),
        unit_test(test_rbtree_remove_left_red_leaf),
        unit_test(test_rbtree_remove_left_black_leaf__case31),
        unit_test(test_rbtree_remove_left_black_leaf__case32),
        unit_test(test_rbtree_remove_left_black_leaf__case33),
        unit_test(test_rbtree_remove_left_black_leaf__case34),
        unit_test(test_rbtree_remove_right_red_leaf),
        unit_test(test_rbtree_remove_right_black_leaf__case31),
        unit_test(test_rbtree_remove_right_black_leaf__case32),
        unit_test(test_rbtree_remove_right_black_leaf__case33),
        unit_test(test_rbtree_remove_right_black_leaf__case34),
        unit_test(test_rbtree_remove_left_red_node__has_only_left_child__substitued_by_red__case1),
        unit_test(test_rbtree_remove_left_red_node__has_only_left_child__substitued_by_black__case2),
        unit_test(test_rbtree_remove_left_red_node__has_only_left_child__substitued_by_black__case31),
        unit_test(test_rbtree_remove_left_red_node__has_only_left_child__substitued_by_black__case32),
        unit_test(test_rbtree_remove_left_red_node__has_only_left_child__substitued_by_black__case33),
        unit_test(test_rbtree_remove_left_red_node__has_only_left_child__substitued_by_black__case34),
        unit_test(test_rbtree_remove_left_red_node__has_only_left_child__substitued_by_black__case35),
        unit_test(test_rbtree_remove_left_red_node__has_only_right_child__substitued_by_red__case1),
        unit_test(test_rbtree_remove_left_red_node__has_only_right_child__substitued_by_black__case2),
        unit_test(test_rbtree_remove_left_red_node__has_only_right_child__substitued_by_black__case31),
        unit_test(test_rbtree_remove_left_red_node__has_only_right_child__substitued_by_black__case32),
        unit_test(test_rbtree_remove_left_red_node__has_only_right_child__substitued_by_black__case33),
        unit_test(test_rbtree_remove_left_red_node__has_only_right_child__substitued_by_black__case34),
        unit_test(test_rbtree_remove_left_red_node__has_only_right_child__substitued_by_black__case35),
        unit_test(test_rbtree_remove_right_red_node__has_only_left_child__substitued_by_red__case1),
        unit_test(test_rbtree_remove_right_red_node__has_only_left_child__substitued_by_black__case2),
        unit_test(test_rbtree_remove_right_red_node__has_only_left_child__substitued_by_black__case31),
        unit_test(test_rbtree_remove_right_red_node__has_only_left_child__substitued_by_black__case32),
        unit_test(test_rbtree_remove_right_red_node__has_only_left_child__substitued_by_black__case33),
        unit_test(test_rbtree_remove_right_red_node__has_only_left_child__substitued_by_black__case34),
        unit_test(test_rbtree_remove_right_red_node__has_only_left_child__substitued_by_black__case35),
        unit_test(test_rbtree_remove_right_red_node__has_only_right_child__substitued_by_red_case1),
        unit_test(test_rbtree_remove_right_red_node__has_only_right_child__substitued_by_black__case2),
        unit_test(test_rbtree_remove_right_red_node__has_only_right_child__substitued_by_black__case31),
        unit_test(test_rbtree_remove_right_red_node__has_only_right_child__substitued_by_black__case32),
        unit_test(test_rbtree_remove_right_red_node__has_only_right_child__substitued_by_black__case33),
        unit_test(test_rbtree_remove_right_red_node__has_only_right_child__substitued_by_black__case34),
        unit_test(test_rbtree_remove_right_red_node__has_only_right_child__substitued_by_black__case35),
        unit_test(test_rbtree_remove_left_black_node__has_only_left_child__substitued_by_red__case1),
        unit_test(test_rbtree_remove_left_black_node__has_only_left_child__substitued_by_black__case2),
        unit_test(test_rbtree_remove_left_black_node__has_only_left_child__substitued_by_black__case31),
        unit_test(test_rbtree_remove_left_black_node__has_only_left_child__substitued_by_black__case32),
        unit_test(test_rbtree_remove_left_black_node__has_only_left_child__substitued_by_black__case33),
        unit_test(test_rbtree_remove_left_black_node__has_only_left_child__substitued_by_black__case34),
        unit_test(test_rbtree_remove_left_black_node__has_only_left_child__substitued_by_black__case35),
        unit_test(test_rbtree_remove_left_black_node__has_only_right_child__substitued_by_red__case1),
        unit_test(test_rbtree_remove_left_black_node__has_only_right_child__substitued_by_black__case2),
        unit_test(test_rbtree_remove_left_black_node__has_only_right_child__substitued_by_black__case31),
        unit_test(test_rbtree_remove_left_black_node__has_only_right_child__substitued_by_black__case32),
        unit_test(test_rbtree_remove_left_black_node__has_only_right_child__substitued_by_black__case33),
        unit_test(test_rbtree_remove_left_black_node__has_only_right_child__substitued_by_black__case34),
        unit_test(test_rbtree_remove_left_black_node__has_only_right_child__substitued_by_black__case35),
        unit_test(test_rbtree_remove_right_black_node__has_only_left_child__substitued_by_red__case1),
        unit_test(test_rbtree_remove_right_black_node__has_only_left_child__substitued_by_black__case2),
        unit_test(test_rbtree_remove_right_black_node__has_only_left_child__substitued_by_black__case31),
        unit_test(test_rbtree_remove_right_black_node__has_only_left_child__substitued_by_black__case32),
        unit_test(test_rbtree_remove_right_black_node__has_only_left_child__substitued_by_black__case33),
        unit_test(test_rbtree_remove_right_black_node__has_only_left_child__substitued_by_black__case34),
        unit_test(test_rbtree_remove_right_black_node__has_only_left_child__substitued_by_black__case35),
        unit_test(test_rbtree_remove_right_black_node__has_only_right_child__substitued_by_red__case1),
        unit_test(test_rbtree_remove_right_black_node__has_only_right_child__substitued_by_black__case2),
        unit_test(test_rbtree_remove_right_black_node__has_only_right_child__substitued_by_black__case31),
        unit_test(test_rbtree_remove_right_black_node__has_only_right_child__substitued_by_black__case32),
        unit_test(test_rbtree_remove_right_black_node__has_only_right_child__substitued_by_black__case33),
        unit_test(test_rbtree_remove_right_black_node__has_only_right_child__substitued_by_black__case34),
        unit_test(test_rbtree_remove_right_black_node__has_only_right_child__substitued_by_black__case35),
        unit_test(test_rbtree_remove_left_red_fullnode__substitued_by_red),
        unit_test(test_rbtree_remove_left_red_fullnode__substitued_by_black),
        unit_test(test_rbtree_remove_left_black_fullnode__substitued_by_red),
        unit_test(test_rbtree_remove_left_black_fullnode__substitued_by_black),
        unit_test(test_rbtree_remove_right_red_fullnode__substitued_by_red),
        unit_test(test_rbtree_remove_right_red_fullnode__substitued_by_black),
        unit_test(test_rbtree_remove_right_black_fullnode__substitued_by_red),
        unit_test(test_rbtree_remove_right_black_fullnode__substitued_by_black),
        unit_test(test_rbtree_remove_root__without_child),
        unit_test(test_rbtree_remove_root__has_only_left_child__substitued_by_red),
        unit_test(test_rbtree_remove_root__has_only_right_child__substitued_by_red),
        unit_test(test_rbtree_remove_root__has_all_childs__substitued_by_red),
        unit_test(test_rbtree_remove_root__has_all_childs__substitued_by_black),
        unit_test_setup_teardown(test_rbtree_enumeration_safe_del_first, setup_enumerate_test_rbtree, teardown_enumerate_test_rbtree),
        unit_test_setup_teardown(test_rbtree_enumeration_safe_del_last, setup_enumerate_test_rbtree, teardown_enumerate_test_rbtree),
        unit_test_setup_teardown(test_rbtree_enumeration_safe_del_root, setup_enumerate_test_rbtree, teardown_enumerate_test_rbtree),
        unit_test_setup_teardown(test_rbtree_enumeration_safe_del_two_leaf, setup_enumerate_test_rbtree, teardown_enumerate_test_rbtree),
        unit_test_setup_teardown(test_rbtree_enumeration_safe_del_three_fullnode, setup_enumerate_test_rbtree, teardown_enumerate_test_rbtree),
    };

    return run_tests(tests);
}
