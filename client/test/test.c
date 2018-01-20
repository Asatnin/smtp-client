//
// Created by andrey on 19.01.18.
//

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include "../str_helper.h"

int is_run = 1;

int init_suite() {
    return 0;
}

int clean_suite() {
    return 0;
}

void test_split_str(void) {
    char *str = "vasya@gmail.com";
    char *res = str_split(str, '@');

    if (res == NULL) {
        return;
    }

    int sts = strcmp(res, "gmail.com");
    free(res);

    CU_ASSERT_EQUAL(sts, 0);
}

int main() {
    CU_pSuite pSuite = NULL;

    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    /* add a suite to the registry */
    pSuite = CU_add_suite("Suite_1", init_suite, clean_suite);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if ((NULL == CU_add_test(pSuite, "test_split_str", test_split_str))) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}

