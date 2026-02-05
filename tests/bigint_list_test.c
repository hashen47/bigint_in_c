#define BIGINT_ASSERT_ENABLED
#include <time.h>
#include "../bigint.h"

size_t sizes[] = {
    1,
    10,
    100,
    1000,
    5000,
    10000,
    100000,
    1000000,
    10000000,
    100000000,
};

void list_init_and_list_free_test() {
    printf("(TESTING) list_init_and_list_free_test (STARTED)\n");
    char msg[200];
    for (size_t i = 0; i < sizeof(sizes)/sizeof(size_t); i++) {
        List *l = list_init();
        sprintf(msg, "TESTCASE FAIL (list_init_and_list_free_test): index (%zu): list cannot be NULL", i);
        bgi_assert(l != NULL, msg);
        sprintf(msg, "TESTCASE FAIL (list_init_and_list_free_test): index (%zu): %s", i, list_get_status_msg(l));
        bgi_assert(l->status_code == LIST_OK, msg);
        list_free(l);
        printf("TESTCASE (%zu) passed...\n", i);
    }
    printf("(TESTING) list_init_and_list_free_test (COMPLETED)\n\n");
}

void list_len_test() {
    printf("(TESTING) list_len_test (STARTED)\n");
    char msg[200];
    for (size_t i = 0; i < sizeof(sizes)/sizeof(size_t); i++) {
        List *l = list_init();
        sprintf(msg, "TESTCASE FAIL (list_len_test): index (%zu): list cannot be NULL", i);
        bgi_assert(l != NULL, msg);
        sprintf(msg, "TESTCASE FAIL (list_len_test): index (%zu): %s", i, list_get_status_msg(l));
        bgi_assert(l->status_code == LIST_OK, msg);

        size_t len = 0;
        for (size_t i = 0; i < sizes[i]; i++) {
            list_append(l, rand() % CHAR_MAX);
            if (l->status_code != LIST_OK) {
                sprintf(msg, "TESTCASE FAIL (list_len_test): index (%zu): %s", i, list_get_status_msg(l));
                bgi_assert(l->status_code == LIST_OK, msg);
            }
            len++;
        }

        sprintf(msg, "TESTCASE FAIL (list_len_test): index (%zu): list_len(l) should equal to len", i);
        bgi_assert(list_len(l) == len, msg);

        list_free(l);
        printf("TESTCASE (%zu) passed...\n", i);
    }
    printf("(TESTING) list_len_test (COMPLETED)\n\n");
}

void list_append_test() {
    printf("(TESTING) list_append_test (STARTED)\n");
    char msg[200];
    for (size_t i = 0; i < sizeof(sizes)/sizeof(size_t); i++) {
        List *l = list_init();
        sprintf(msg, "TESTCASE FAIL (list_append_test): index (%zu): list cannot be NULL", i);
        bgi_assert(l != NULL, msg);
        sprintf(msg, "TESTCASE FAIL (list_append_test): index (%zu): %s", i, list_get_status_msg(l));
        bgi_assert(l->status_code == LIST_OK, msg);

        for (size_t j = 0; j < sizes[i]; j++) {
            list_append(l, (int8)(j % CHAR_MAX));
            if (l->status_code != LIST_OK) {
                sprintf(msg, "TESTCASE FAIL (list_append_test): index (%zu): %s", i, list_get_status_msg(l));
                bgi_assert(l->status_code == LIST_OK, msg);
            }
        }

        // compare
        for (size_t j = 0; j < sizes[i]; j++) {
            int8 real = *((int8*)l->buf + j);
            sprintf(msg, "TESTCASE FAIL (list_append_test): index (%zu): values are not equal: expect: %d, real: %d", i, (int8)(j % CHAR_MAX), real);
            bgi_assert(real == (int8)(j % CHAR_MAX), msg);
        }

        list_free(l);
        printf("TESTCASE (%zu) passed...\n", i);
    }
    printf("(TESTING) list_append_test (COMPLETED)\n\n");
}

void list_get_test() {
    printf("(TESTING) list_get_test (STARTED)\n");
    char msg[200];
    for (size_t i = 0; i < sizeof(sizes)/sizeof(size_t); i++) {
        List *l = list_init();
        sprintf(msg, "TESTCASE FAIL (list_get_test): index (%zu): list cannot be NULL", i);
        bgi_assert(l != NULL, msg);
        sprintf(msg, "TESTCASE FAIL (list_get_test): index (%zu): %s", i, list_get_status_msg(l));
        bgi_assert(l->status_code == LIST_OK, msg);

        for (size_t j = 0; j < sizes[i]; j++) {
            list_append(l, (int8)(rand() % CHAR_MAX));
            if (l->status_code != LIST_OK) {
                sprintf(msg, "TESTCASE FAIL (list_get_test): index (%zu): %s", i, list_get_status_msg(l));
                bgi_assert(l->status_code == LIST_OK, msg);
            }
        }

        // compare
        for (size_t j = 0; j < sizes[i]; j++) {
            int8 real   = list_get(l, j);
            int8 expect = *((int8*)l->buf + j);
            sprintf(msg, "TESTCASE FAIL (list_get_test): index (%zu): values are not equal: expect: %d, real: %d", i, expect, real);
            bgi_assert(expect == real, msg);
        }

        // try with invalid indexes
        for (size_t j = 0; j < sizes[i]; j++) {
            int8 value = list_get(l, j + list_len(l));
            sprintf(msg, "TESTCASE FAIL (list_get_test): index (%zu): should be invalid (index outbound)", i);
            bgi_assert(l->status_code != LIST_OK, msg);
            sprintf(msg, "TESTCASE FAIL (list_get_test): index (%zu): value should be 0", i);
            bgi_assert(value == 0, msg);
        }

        list_free(l);
        printf("TESTCASE (%zu) passed...\n", i);
    }
    printf("(TESTING) list_get_test (COMPLETED)\n\n");
}

void list_set_test() {
    printf("(TESTING) list_set_test (STARTED)\n");
    char msg[200];
    for (size_t i = 0; i < sizeof(sizes)/sizeof(size_t); i++) {
        List *l = list_init();
        sprintf(msg, "TESTCASE FAIL (list_set_test): index (%zu): list cannot be NULL", i);
        bgi_assert(l != NULL, msg);
        sprintf(msg, "TESTCASE FAIL (list_set_test): index (%zu): %s", i, list_get_status_msg(l));
        bgi_assert(l->status_code == LIST_OK, msg);

        for (size_t j = 0; j < sizes[i]; j++) {
            list_append(l, 0);
            if (l->status_code != LIST_OK) {
                sprintf(msg, "TESTCASE FAIL (list_set_test): index (%zu): %s", i, list_get_status_msg(l));
                bgi_assert(l->status_code == LIST_OK, msg);
            }
        }

        for (size_t j = 0; j < sizes[i]; j++) {
            list_set(l, j, (int8)(j % CHAR_MAX));
            if (l->status_code != LIST_OK) {
                sprintf(msg, "TESTCASE FAIL (list_set_test): index (%zu): %s", i, list_get_status_msg(l));
                bgi_assert(l->status_code == LIST_OK, msg);
            }
        }

        // compare
        for (size_t j = 0; j < sizes[i]; j++) {
            int8 real   = list_get(l, j);
            int8 expect = *((int8*)l->buf + j);
            sprintf(msg, "TESTCASE FAIL (list_set_test): index (%zu): values are not equal: expect: %d, real: %d", i, expect, real);
            bgi_assert(expect == real, msg);
        }

        // try with invalid indexes
        for (size_t j = 0; j < sizes[i]; j++) {
            list_set(l, j + list_len(l), 0);
            sprintf(msg, "TESTCASE FAIL (list_set_test): index (%zu): should be invalid (index outbound)", i);
            bgi_assert(l->status_code != LIST_OK, msg);
        }

        list_free(l);
        printf("TESTCASE (%zu) passed...\n", i);
    }
    printf("(TESTING) list_set_test (COMPLETED)\n\n");
}

void list_reverse_test() {
    printf("(TESTING) list_reverse_test (STARTED)\n");
    char msg[200];
    for (size_t i = 0; i < sizeof(sizes)/sizeof(size_t); i++) {
        List *l = list_init();
        sprintf(msg, "TESTCASE FAIL (list_reverse_test): index (%zu): list cannot be NULL", i);
        bgi_assert(l != NULL, msg);
        sprintf(msg, "TESTCASE FAIL (list_reverse_test): index (%zu): %s", i, list_get_status_msg(l));
        bgi_assert(l->status_code == LIST_OK, msg);

        for (size_t j = 0; j < sizes[i]; j++) {
            list_append(l, (int8)(rand() % CHAR_MAX));
            if (l->status_code != LIST_OK) {
                sprintf(msg, "TESTCASE FAIL (list_reverse_test): index (%zu): %s", i, list_get_status_msg(l));
                bgi_assert(l->status_code == LIST_OK, msg);
            }
        }

        list_reverse(l);
        if (l->status_code != LIST_OK) {
            sprintf(msg, "TESTCASE FAIL (list_reverse_test): index (%zu): %s", i, list_get_status_msg(l));
            bgi_assert(l->status_code == LIST_OK, msg);
        }

        // compare
        for (size_t j = 0; j < sizes[i]; j++) {
            int8 real   = list_get(l, j);
            int8 expect = *((int8*)l->buf + j);
            sprintf(msg, "TESTCASE FAIL (list_reverse_test): index (%zu): values are not equal: expect: %d, real: %d", i, expect, real);
            bgi_assert(expect == real, msg);
        }

        list_free(l);
        printf("TESTCASE (%zu) passed...\n", i);
    }
    printf("(TESTING) list_reverse_test (COMPLETED)\n\n");
}

void list_clone_test() {
    printf("(TESTING) list_clone_test (STARTED)\n");
    char msg[200];
    for (size_t i = 0; i < sizeof(sizes)/sizeof(size_t); i++) {
        List *l = list_init();
        sprintf(msg, "TESTCASE FAIL (list_clone_test): index (%zu): list cannot be NULL", i);
        bgi_assert(l != NULL, msg);
        sprintf(msg, "TESTCASE FAIL (list_clone_test): index (%zu): %s", i, list_get_status_msg(l));
        bgi_assert(l->status_code == LIST_OK, msg);

        for (size_t j = 0; j < sizes[i]; j++) {
            list_append(l, (int8)(rand() % CHAR_MAX));
            if (l->status_code != LIST_OK) {
                sprintf(msg, "TESTCASE FAIL (list_clone_test): index (%zu): %s", i, list_get_status_msg(l));
                bgi_assert(l->status_code == LIST_OK, msg);
            }
        }

        List* l2 = list_clone(l);
        sprintf(msg, "TESTCASE FAIL (list_clone_test): index (%zu): l2 cannot be NULL", i);
        bgi_assert(l2 != NULL, msg);
        sprintf(msg, "TESTCASE FAIL (list_clone_test): index (%zu): %s", i, list_get_status_msg(l2));
        bgi_assert(l2->status_code == LIST_OK, msg);

        // compare two lists
        for (size_t j = 0; j < sizes[i]; j++) {
            int8 real   = list_get(l2, j);
            int8 expect = list_get(l, j); 
            sprintf(msg, "TESTCASE FAIL (list_clone_test): index (%zu): values are not equal: expect: %d, real: %d", i, expect, real);
            bgi_assert(expect == real, msg);
        }

        list_free(l);
        printf("TESTCASE (%zu) passed...\n", i);
    }
    printf("(TESTING) list_clone_test (COMPLETED)\n\n");
}

int main(void) {
    srand(time(NULL));
    list_init_and_list_free_test();
    list_len_test();
    list_append_test();
    list_get_test();
    list_set_test();
    list_reverse_test();
    list_clone_test();
    return 0;
}
