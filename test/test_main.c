#include <setjmp.h>
#include <stdarg.h>
#include <google/cmockery.h>

void null_test_success(void **state) {
}

int main(int argc, char* argv[]) {
    const UnitTest tests[] = {
        unit_test(null_test_success),
    };
    return run_tests(tests);
}
