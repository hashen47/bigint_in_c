#include <stdio.h>

#define BIGINT_ASSERT_ENABLED
#include "../bigint.h"

void bgi_init_and_bgi_free_test() {
    char msg[200] = {0};

    typedef struct {
        bool sign;
        const char* text;
        BigIntStatusCode code;
    } Testcase;

    printf("(TESTING) bgi_init_and_bgi_free_test (STARTED)\n");

    Testcase testcases[] = {
        (Testcase){.sign=true , .text="12345"     , .code=BGI_OK},
        (Testcase){.sign=false, .text="-1234534"  , .code=BGI_OK},
        (Testcase){.sign=true , .text="12345.123" , .code=BGI_OK},
        (Testcase){.sign=false, .text="-12345.123", .code=BGI_OK},
        (Testcase){.sign=false, .text="-12345.123", .code=BGI_OK},
        (Testcase){.sign=false, .text="-0.123"    , .code=BGI_OK},
        (Testcase){.sign=false, .text="-0.23"     , .code=BGI_OK},
        (Testcase){.sign=false, .text="-0.0000001", .code=BGI_OK},
        (Testcase){.sign=true , .text="-0.0"      , .code=BGI_OK},
        (Testcase){.sign=true , .text="00000.000" , .code=BGI_OK},
        (Testcase){.sign=true , .text="+0000.000" , .code=BGI_OK},
        (Testcase){.sign=true , .text="-000.0000" , .code=BGI_OK},
        (Testcase){.sign=true , .text="-0."       , .code=BGI_INVALID_TEXT_VALUE},
        (Testcase){.sign=true , .text="-0. 23"    , .code=BGI_INVALID_TEXT_VALUE},
        (Testcase){.sign=true , .text="-.123"     , .code=BGI_INVALID_TEXT_VALUE},
        (Testcase){.sign=true , .text="+."        , .code=BGI_INVALID_TEXT_VALUE},
    };

    for (size_t i = 0; i < sizeof(testcases)/sizeof(Testcase); i++) {
        Testcase tc = testcases[i];
        BigInt *bi = bgi_init(tc.text);

        if (tc.code == BGI_OK) {
            sprintf(msg, "(TESTCASE FAIL): (bgi_init): index %zu: text %s: allocation fail", i, tc.text);
            bgi_assert(bi != NULL, msg);
            sprintf(msg, "(TESTCASE FAIL): (bgi_init) index %zu: text %s: %s", i, tc.text, bgi_get_status_msg(bi));
            bgi_assert(bi->status_code == BGI_OK, msg);
            sprintf(msg, "(TESTCASE FAIL): (bgi_init) index %zu: text %s: signs are not equal, expect: %s, real: %s", i, tc.text, tc.sign ? "true" : "false", bi->sign ? "true" : "false");
            bgi_assert(tc.sign == bi->sign, msg);
        } else {
            sprintf(msg, "(TESTCASE FAIL): (bgi_init): index %zu: text %s: status_codes are not equal, expect: %d, real: %d", i, tc.text, tc.code, bi->status_code);
            bgi_assert(bi->status_code == tc.code, msg);
        }

        bgi_free(bi);

        printf("TESTCASE (%zu) PASSED...\n", i);
    }

    printf("(TESTING) bgi_init_and_bgi_free_test (COMPLETED)\n\n");
}

void bgi_cmp_test() {
    printf("(TESTING) bgi_cmp_test (STARTED)\n");

    typedef struct {
        const char *text1;
        const char *text2;
        int expect;
    } Testcase;

    char msg[200] = {0};

    Testcase testcases[] = {
        {
            .text1="+1.000000",
            .text2="+1.00",
            .expect=0,
        },
        {
            .text1="+1.000001",
            .text2="+1.00",
            .expect=1,
        },
        {
            .text1="+1.000001",
            .text2="-1.00",
            .expect=1,
        },
        {
            .text1="-1.000001",
            .text2="-1.00",
            .expect=-1,
        },
        {
            .text1="200",
            .text2="2000",
            .expect=-1,
        },
        {
            .text1="2000",
            .text2="-2000",
            .expect=1,
        },
        {
            .text1="-2000",
            .text2="2000",
            .expect=-1,
        },
        {
            .text1="-200",
            .text2="-2000",
            .expect=1,
        },
        {
            .text1="0.0",
            .text2="0",
            .expect=0,
        },
        {
            .text1="+0.0",
            .text2="-0.000",
            .expect=0,
        },
    };

    for (size_t i = 0; i < sizeof(testcases)/sizeof(Testcase); i++) {
        Testcase tc = testcases[i];
        BigInt *bi1 = bgi_init(tc.text1);

        sprintf(msg, "TESTCASE FAIL: index: %zu: bi1 cannot be NULL", i);
        bgi_assert(bi1 != NULL, msg);
        sprintf(msg, "TESTCASE FAIL: index: %zu: %s", i, bgi_get_status_msg(bi1));
        bgi_assert(bi1->status_code == BGI_OK, msg);

        BigInt *bi2 = bgi_init(tc.text2);
        sprintf(msg, "TESTCASE FAIL: index: %zu: bi2 cannot be NULL", i);
        bgi_assert(bi2 != NULL, msg);
        sprintf(msg, "TESTCASE FAIL: index: %zu: %s", i, bgi_get_status_msg(bi2));
        bgi_assert(bi2->status_code == BGI_OK, msg);

        int val = bgi_cmp(bi1, bi2);
        sprintf(msg, "TESTCASE FAIL: index: %zu: %s", i, bgi_get_status_msg(bi1));
        bgi_assert(bi1->status_code == BGI_OK, msg);
        sprintf(msg, "TESTCASE FAIL: index: %zu: %s", i, bgi_get_status_msg(bi2));
        bgi_assert(bi2->status_code == BGI_OK, msg);

        sprintf(msg, "TESTCASE FAIL: index: %zu: values should be equal: expect: %d, real: %d", i, tc.expect, val);
        bgi_assert(val == tc.expect, msg);

        bgi_free(bi1);
        bgi_free(bi2);

        printf("TESTCASE (%zu) PASSED...\n", i);
    }

    printf("(TESTING) bgi_cmp_test (COMPLETED)\n\n");
}

void bgi_abs_cmp_test() {
    printf("(TESTING) bgi_abs_cmp_test (STARTED)\n");

    typedef struct {
        const char *text1;
        const char *text2;
        int expect;
    } Testcase;

    char msg[200] = {0};

    Testcase testcases[] = {
        {
            .text1="+1.000000",
            .text2="+1.00",
            .expect=0,
        },
        {
            .text1="+1.000001",
            .text2="+1.00",
            .expect=1,
        },
        {
            .text1="+1.000001",
            .text2="-1.00",
            .expect=1,
        },
        {
            .text1="1.00000",
            .text2="-1.00",
            .expect=0,
        },
        {
            .text1="1.00001",
            .text2="-1.00001",
            .expect=0,
        },
        {
            .text1="-1.000001",
            .text2="-1.00",
            .expect=1,
        },
        {
            .text1="200",
            .text2="2000",
            .expect=-1,
        },
        {
            .text1="2000",
            .text2="-2000",
            .expect=0,
        },
        {
            .text1="-2000",
            .text2="2000",
            .expect=0,
        },
        {
            .text1="-200",
            .text2="-2000",
            .expect=-1,
        },
        {
            .text1="+00000.0000000",
            .text2="-0",
            .expect=0,
        },
        {
            .text1="+00000.0000000",
            .text2="-0.0000",
            .expect=0,
        },
    };

    for (size_t i = 0; i < sizeof(testcases)/sizeof(Testcase); i++) {
        Testcase tc = testcases[i];
        BigInt *bi1 = bgi_init(tc.text1);

        sprintf(msg, "TESTCASE FAIL: index: %zu: bi1 cannot be NULL", i);
        bgi_assert(bi1 != NULL, msg);
        sprintf(msg, "TESTCASE FAIL: index: %zu: %s", i, bgi_get_status_msg(bi1));
        bgi_assert(bi1->status_code == BGI_OK, msg);

        BigInt *bi2 = bgi_init(tc.text2);
        sprintf(msg, "TESTCASE FAIL: index: %zu: bi2 cannot be NULL", i);
        bgi_assert(bi2 != NULL, msg);
        sprintf(msg, "TESTCASE FAIL: index: %zu: %s", i, bgi_get_status_msg(bi2));
        bgi_assert(bi2->status_code == BGI_OK, msg);

        int val = bgi_abs_cmp(bi1, bi2);
        sprintf(msg, "TESTCASE FAIL: index: %zu: %s", i, bgi_get_status_msg(bi1));
        bgi_assert(bi1->status_code == BGI_OK, msg);
        sprintf(msg, "TESTCASE FAIL: index: %zu: %s", i, bgi_get_status_msg(bi2));
        bgi_assert(bi2->status_code == BGI_OK, msg);

        sprintf(msg, "TESTCASE FAIL: index: %zu: values should be equal: expect: %d, real: %d", i, tc.expect, val);
        bgi_assert(val == tc.expect, msg);

        bgi_free(bi1);
        bgi_free(bi2);

        printf("TESTCASE (%zu) PASSED...\n", i);
    }

    printf("(TESTING) bgi_abs_cmp_test (COMPLETED)\n\n");
}

void bgi_add_test() {
    printf("(TESTING) bgi_add_test (STARTED)\n");

    char msg[1000] = {0};

    typedef struct {
        const char *n1;
        const char *n2;
        const char *expect;
    } Testcase;

    Testcase testcases[] = {
        (Testcase){.n1="0"           , .n2="+2000"           , .expect="+2000"},
        (Testcase){.n1="+1000"       , .n2="+2000"           , .expect="+3000"},
        (Testcase){.n1="+10"         , .n2="+2000"           , .expect="+2010"},
        (Testcase){.n1="+200"        , .n2="+2000.234"       , .expect="+2200.234"},
        (Testcase){.n1="-200"        , .n2="+2000.234"       , .expect="+1800.234"},
        (Testcase){.n1="-200.34"     , .n2="-2000.234"       , .expect="-2200.574"},
        (Testcase){.n1="-23423.25252", .n2="6893245459.99999", .expect="6893222036.74747"},
        (Testcase){.n1="20927268"    , .n2="209272680"       , .expect="230199948"},
        (Testcase){
            .n1    ="6893245459.999998080234234234234234",
            .n2    ="23423.252520000000000000000000",
            .expect="6893268883.252518080234234234234234",
        },
    };

    for (size_t i = 0; i < sizeof(testcases)/sizeof(Testcase); i++) {
        Testcase tc = testcases[i];

        BigInt *bi1 = bgi_init(tc.n1);
        sprintf(msg, "TESTCASE FAIL: index %zu: bi1 cannot be NULL", i);
        bgi_assert(bi1 != NULL, msg);
        sprintf(msg, "TESTCASE FAIL: index %zu: %s", i, bgi_get_status_msg(bi1));
        bgi_assert(bi1->status_code == BGI_OK, msg);

        BigInt *bi2 = bgi_init(tc.n2);
        sprintf(msg, "TESTCASE FAIL: index %zu: bi2 cannot be NULL", i);
        bgi_assert(bi2 != NULL, msg);
        sprintf(msg, "TESTCASE FAIL: index %zu: %s", i, bgi_get_status_msg(bi2));
        bgi_assert(bi2->status_code == BGI_OK, msg);

        BigInt *expect = bgi_init(tc.expect);
        sprintf(msg, "TESTCASE FAIL: index %zu: 'expect' cannot be NULL", i);
        bgi_assert(expect != NULL, msg);
        sprintf(msg, "TESTCASE FAIL: index %zu: %s", i, bgi_get_status_msg(expect));
        bgi_assert(expect->status_code == BGI_OK, msg);

        BigInt *result = bgi_add(bi1, bi2);
        sprintf(msg, "TESTCASE FAIL: index %zu: result cannot be NULL", i);
        bgi_assert(result != NULL, msg);
        sprintf(msg, "TESTCASE FAIL: index %zu: %s", i, bgi_get_status_msg(result));
        bgi_assert(result->status_code == BGI_OK, msg);

        int val = bgi_cmp(expect, result);
        sprintf(msg, "TESTCASE FAIL: index %zu: %s", i, bgi_get_status_msg(expect));
        bgi_assert(expect->status_code == BGI_OK, msg);
        sprintf(msg, "TESTCASE FAIL: index %zu: %s", i, bgi_get_status_msg(result));
        bgi_assert(expect->status_code == BGI_OK, msg);

        sprintf(msg, "TESTCASE FAIL: n1 %s: n2 %s: expect %s: real %s: cmp %d", tc.n1, tc.n2, tc.expect, bgi_get_text(result), val);
        bgi_assert(val == 0, msg);

        bgi_free(bi1);
        bgi_free(bi2);
        bgi_free(expect);
        bgi_free(result);

        printf("TESTCASES (%zu) PASSED...\n", i);
    }

    printf("(TESTING) bgi_add_test (COMPLETED)\n\n");
}

void bgi_sub_test() {
    printf("(TESTING) bgi_sub_test (STARTED)\n");

    char msg[1000] = {0};

    typedef struct {
        const char *n1;
        const char *n2;
        const char *expect;
    } Testcase;

    Testcase testcases[] = {
        (Testcase){.n1="+1000"       , .n2="+2000"           , .expect="-1000"},
        (Testcase){.n1="+10"         , .n2="+2000"           , .expect="-1990"},
        (Testcase){.n1="+200"        , .n2="+2000.234"       , .expect="-1800.234"},
        (Testcase){.n1="-200"        , .n2="+2000.234"       , .expect="-2200.234"},
        (Testcase){.n1="-200.34"     , .n2="-2000.234"       , .expect="1799.894"},
        (Testcase){.n1="-23423.25252", .n2="6893245459.99999", .expect="-6893268883.25251"},
        (Testcase){
            .n1    ="6893245459.999998080234234234234234",
            .n2    ="23423.252520000000000000000000",
            .expect="6893222036.747478080234234234234234",
        },
        (Testcase){
            .n1    ="6893245459.999998080234234234234234",
            .n2    ="-23423.252520000000000000000000",
            .expect="6893268883.252518080234234234234234",
        },
    };

    for (size_t i = 0; i < sizeof(testcases)/sizeof(Testcase); i++) {
        Testcase tc = testcases[i];

        BigInt *bi1 = bgi_init(tc.n1);
        sprintf(msg, "TESTCASE FAIL: index %zu: bi1 cannot be NULL", i);
        bgi_assert(bi1 != NULL, msg);
        sprintf(msg, "TESTCASE FAIL: index %zu: %s", i, bgi_get_status_msg(bi1));
        bgi_assert(bi1->status_code == BGI_OK, msg);

        BigInt *bi2 = bgi_init(tc.n2);
        sprintf(msg, "TESTCASE FAIL: index %zu: bi2 cannot be NULL", i);
        bgi_assert(bi2 != NULL, msg);
        sprintf(msg, "TESTCASE FAIL: index %zu: %s", i, bgi_get_status_msg(bi2));
        bgi_assert(bi2->status_code == BGI_OK, msg);

        BigInt *expect = bgi_init(tc.expect);
        sprintf(msg, "TESTCASE FAIL: index %zu: 'expect' cannot be NULL", i);
        bgi_assert(expect != NULL, msg);
        sprintf(msg, "TESTCASE FAIL: index %zu: %s", i, bgi_get_status_msg(expect));
        bgi_assert(expect->status_code == BGI_OK, msg);

        BigInt *result = bgi_sub(bi1, bi2);
        sprintf(msg, "TESTCASE FAIL: index %zu: result cannot be NULL", i);
        bgi_assert(result != NULL, msg);
        sprintf(msg, "TESTCASE FAIL: index %zu: %s", i, bgi_get_status_msg(result));
        bgi_assert(result->status_code == BGI_OK, msg);

        int val = bgi_cmp(expect, result);
        sprintf(msg, "TESTCASE FAIL: index %zu: %s", i, bgi_get_status_msg(expect));
        bgi_assert(expect->status_code == BGI_OK, msg);
        sprintf(msg, "TESTCASE FAIL: index %zu: %s", i, bgi_get_status_msg(result));
        bgi_assert(expect->status_code == BGI_OK, msg);

        sprintf(msg, "TESTCASE FAIL: n1 %s: n2 %s: expect %s: real %s: cmp %d", tc.n1, tc.n2, tc.expect, bgi_get_text(result), val);
        bgi_assert(val == 0, msg);

        bgi_free(bi1);
        bgi_free(bi2);
        bgi_free(expect);
        bgi_free(result);

        printf("TESTCASES (%zu) PASSED...\n", i);
    }

    printf("(TESTING) bgi_sub_test (COMPLETED)\n\n");
}

void bgi_mult_test() {
    printf("(TESTING) bgi_mult_test (STARTED)\n");

    char msg[200] = {0};

    typedef struct {
        const char *n1;
        const char *n2;
        const char *expect;
    } Testcase;

    Testcase testcases[] = {
        (Testcase){.n1="0.25"      , .n2="0.25"       , .expect="0.0625"},
        (Testcase){.n1="1"         , .n2="2"          , .expect="2"},
        (Testcase){.n1="-1"        , .n2="2"          , .expect="-2"},
        (Testcase){.n1="1"         , .n2="-2"         , .expect="-2"},
        (Testcase){.n1="-1"        , .n2="-2"         , .expect="+2"},
        (Testcase){.n1="-1"        , .n2="-2"         , .expect="+2"},
        (Testcase){.n1="1234.234"  , .n2="2342349.24" , .expect="2891007071.88216"},
        (Testcase){.n1="-1234.234" , .n2="2342349.24" , .expect="-2891007071.88216"},
        (Testcase){.n1="1234.234"  , .n2="-2342349.24", .expect="-2891007071.88216"},
        (Testcase){.n1="-1234.234" , .n2="-2342349.24", .expect="2891007071.88216"},
        (Testcase){.n1="0.25"      , .n2="1"          , .expect="0.25"},
        (Testcase){.n1="-0.25"     , .n2="4"          , .expect="-1"},
        (Testcase){.n1="0.25"      , .n2="-4"         , .expect="-1"},
        (Testcase){.n1="-0.25"     , .n2="-4"         , .expect="1"},
        (Testcase){.n1="0.25"      , .n2="4"          , .expect="1"},
        (Testcase){.n1="-0.25"     , .n2="0.25"       , .expect="-0.0625"},
        (Testcase){.n1="0.25"      , .n2="-0.25"      , .expect="-0.0625"},
        (Testcase){.n1="-0.25"     , .n2="-0.25"      , .expect="0.0625"},
        (Testcase){.n1="0.000125"  , .n2="0.0625"     , .expect="0.0000078125"},
        (Testcase){.n1="-0.000125" , .n2="0.0625"     , .expect="-0.0000078125"},
        (Testcase){.n1="0.000125"  , .n2="-0.0625"    , .expect="-0.0000078125"},
        (Testcase){.n1="-0.000125" , .n2="-0.0625"    , .expect="0.0000078125"},
    };

    for (size_t i = 0; i < sizeof(testcases)/sizeof(Testcase); i++) {
        Testcase tc = testcases[i];

        BigInt *bi1 = bgi_init(tc.n1);
        sprintf(msg, "TESTCASE FAIL: index %zu: bi1 cannot be NULL", i);
        bgi_assert(bi1 != NULL, msg);
        sprintf(msg, "TESTCASE FAIL: index %zu: %s", i, bgi_get_status_msg(bi1));
        bgi_assert(bi1->status_code == BGI_OK, msg);

        BigInt *bi2 = bgi_init(tc.n2);
        sprintf(msg, "TESTCASE FAIL: index %zu: bi2 cannot be NULL", i);
        bgi_assert(bi2 != NULL, msg);
        sprintf(msg, "TESTCASE FAIL: index %zu: %s", i, bgi_get_status_msg(bi2));
        bgi_assert(bi2->status_code == BGI_OK, msg);

        BigInt *expect = bgi_init(tc.expect);
        sprintf(msg, "TESTCASE FAIL: index %zu: 'expect' cannot be NULL", i);
        bgi_assert(expect != NULL, msg);
        sprintf(msg, "TESTCASE FAIL: index %zu: %s", i, bgi_get_status_msg(expect));
        bgi_assert(expect->status_code == BGI_OK, msg);

        BigInt *result = bgi_mult(bi1, bi2);
        sprintf(msg, "TESTCASE FAIL: index %zu: result cannot be NULL", i);
        bgi_assert(result != NULL, msg);
        sprintf(msg, "TESTCASE FAIL: index %zu: %s", i, bgi_get_status_msg(result));
        bgi_assert(result->status_code == BGI_OK, msg);

        int val = bgi_cmp(expect, result);
        sprintf(msg, "TESTCASE FAIL: index %zu: %s", i, bgi_get_status_msg(expect));
        bgi_assert(expect->status_code == BGI_OK, msg);
        sprintf(msg, "TESTCASE FAIL: index %zu: %s", i, bgi_get_status_msg(result));
        bgi_assert(expect->status_code == BGI_OK, msg);

        sprintf(msg, "TESTCASE FAIL: n1 %s: n2 %s: expect %s: real %s: cmp %d", tc.n1, tc.n2, tc.expect, bgi_get_text(result), val);
        bgi_assert(val == 0, msg);

        bgi_free(bi1);
        bgi_free(bi2);
        bgi_free(expect);
        bgi_free(result);

        printf("TESTCASES (%zu) PASSED...\n", i);
    }

    printf("(TESTING) bgi_mult_test (COMPLETED)\n");
}

int main(void) {
    bgi_init_and_bgi_free_test();
    bgi_cmp_test();
    bgi_abs_cmp_test();
    bgi_add_test();
    bgi_sub_test();
    bgi_mult_test();
    return 0;
}
