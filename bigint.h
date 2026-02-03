#ifndef _BIGINT_ 
#define _BIGINT_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>


// assertion
#ifdef BIGINT_ASSERT_ENABLED
#define bgi_assert(c, m)                                                             \
do {                                                                                 \
    if (!(c)) {                                                                       \
        fprintf(stderr, __FILE__ ":%d: assertion %s failed: %s\n", __LINE__, #c, m); \
        exit(1);                                                                     \
    }                                                                                \
} while(0)
#endif

#ifndef BIGINT_ASSERT_ENABLED
#define bgi_assert(c, m)
#endif


typedef char int8; 

#define LIST_STATUS(X) \
    X(LIST_OK, "list is ok") \
    X(LIST_ALLOC_FAIL, "list memory allocation fail") \
    X(LIST_BUF_ALLOC_FAIL, "list->buf memory allocation fail") \
    X(LIST_BUF_REALLOC_FAIL, "list->buf memory re-allocation fail") \
    X(LIST_INDEX_OUTBOUND, "list index is outbound") \

typedef enum {
#define X(name, msg) name,
    LIST_STATUS(X)
#undef X
} ListStatusCode;

typedef struct {
    size_t index;
    size_t bufsize;
    void*  buf;
    ListStatusCode status_code;
} List;

const char *list_get_status_msg(List *l);
List *list_init(void);
size_t list_len(List *l);
size_t list_size(List *l);
void list_append(List *l, int8 value);
void list_print(List *l);
int8 list_get(List *l, size_t index);
void list_set(List *l, size_t index, int8 value);
void list_reverse(List *l);
List *list_clone(List *l);
void list_free(List *l);

const char *list_get_status_msg(List *l) {
    bgi_assert(l != NULL, "list cannot be NULL");
    bgi_assert(l->buf != NULL, "l->buf cannot be NULL");

#define X(name, msg) case name: return msg;
    switch (l->status_code) {
        LIST_STATUS(X)
        default:
            return "Unknown status_code";
    }
#undef X
}

List *list_init(void) {
    List* l = (List*)malloc(sizeof(List));

    bgi_assert(l != NULL, "list allocation failed: malloc failed");
    if (l == NULL) {
        return NULL;
    }

    l->index   = 0;
    l->bufsize = 4;
    l->buf     = malloc(sizeof(int8) * l->bufsize);
    l->status_code = LIST_OK;

    bgi_assert(l != NULL, "l->buf allocation failed: malloc failed");
    if (l->buf == NULL) {
        l->status_code = LIST_BUF_ALLOC_FAIL;
    }

    return l;
}

size_t list_len(List *l) {
    bgi_assert(l != NULL, "list cannot be NULL");
    bgi_assert(l->buf != NULL, "l->buf cannot be NULL");
    return l->index;
}

size_t list_size(List *l) {
    bgi_assert(l != NULL, "list cannot be NULL");
    bgi_assert(l->buf != NULL, "l->buf cannot be NULL");
    return l->bufsize;
}

void list_append(List *l, int8 value) {
    bgi_assert(l != NULL, "list cannot be NULL");
    bgi_assert(l->buf != NULL, "l->buf cannot be NULL");

    if (l->index + 1 >= l->bufsize) {
        l->bufsize *= 2;
        void *newbuf = realloc(l->buf, sizeof(int8) * l->bufsize);

        bgi_assert(l != NULL, "l->buf reallocation failed: realloc failed");
        if (newbuf == NULL) {
            l->status_code = LIST_BUF_REALLOC_FAIL;
            return;
        }

        l->buf = newbuf;
    }

    *((int8*)l->buf + l->index++) = value;
    return;
}

void list_print(List *l) {
    bgi_assert(l != NULL, "list cannot be NULL");
    bgi_assert(l->buf != NULL, "l->buf cannot be NULL");

    printf("[");
    for (size_t i = 0; i < l->index; i++) {
        if (i == l->index-1) {
            printf("%d", *((int8*)l->buf + i));
        } else {
            printf("%d, ", *((int8*)l->buf + i));
        }
    }
    printf("]\n");
}

int8 list_get(List *l, size_t index) {
    bgi_assert(l != NULL, "list cannot be NULL");
    bgi_assert(l->buf != NULL, "l->buf cannot be NULL");

    if (index >= l->index) {
        l->status_code = LIST_INDEX_OUTBOUND;
        return 0;
    }

    return *((int8*)l->buf + index);
}

void list_set(List *l, size_t index, int8 value) {
    bgi_assert(l != NULL, "list cannot be NULL");
    bgi_assert(l->buf != NULL, "l->buf cannot be NULL");

    if (index >= l->index) {
        l->status_code = LIST_INDEX_OUTBOUND;
        return;
    }

    *((int8*)l->buf + index) = value;
}

void list_reverse(List *l) {
    bgi_assert(l != NULL, "list cannot be NULL");
    bgi_assert(l->buf != NULL, "l->buf cannot be NULL");

    for (size_t i = 0; i < list_len(l)/2; i++) {
        int8 front_value = list_get(l, i); 
        int8 back_value  = list_get(l, list_len(l)-i-1); 

        if (l->status_code != LIST_OK) {
            return;
        }

        list_set(l, i, back_value);
        list_set(l, list_len(l)-i-1, front_value);
    }
}

List *list_clone(List *l) {
    bgi_assert(l != NULL, "list cannot be NULL");
    bgi_assert(l->buf != NULL, "l->buf cannot be NULL");

    List *l_copy = list_init(); 
    if (l_copy == NULL) {
        return NULL;
    }
    if (l_copy->status_code != LIST_OK) {
        return l_copy;
    }

    l_copy->status_code = LIST_OK;

    for (size_t i = 0; i < list_len(l); i++) {
        int8 value = list_get(l, i);
        if (l->status_code != LIST_OK) {
            list_free(l_copy);
            return NULL;
        }
        list_append(l_copy, value);
        if (l_copy->status_code != LIST_OK) {
            return l_copy;
        }
    }

    return l_copy;
}

void list_free(List *l) {
    if (l == NULL) return;
    if (l->buf != NULL) {
        free(l->buf);
    }
    free(l);
    return;
}


// library functions
/**
 * add 
 * sub
 * mult
 * div
 * exp
 */

#undef X
#define BGI_STATUS_LIST(X) \
    X(BGI_OK, "bigint is ok") \
    X(BGI_ALLOC_FAIL, "memory allocation fail") \
    X(BGI_REALLOC_FAIL, "memory re-allocation fail") \
    X(BGI_NUMERIC_FAIL, "numeric list fail") \
    X(BGI_DECIMAL_FAIL, "decimal list fail") \
    X(BGI_INVALID_TEXT_VALUE, "given text for bgi_init is invalid")

#define X(name, msg) name,
typedef enum {
    BGI_STATUS_LIST(X)
} BigIntStatusCode;
#undef X

typedef struct {
    bool sign;     // '+' - true, '-' - false
    List *numeric;
    List *decimal;
    BigIntStatusCode status_code;
} BigInt;

const char* bgi_get_status_msg(BigInt *bi);
BigInt *bgi_init(const char* text);
void bgi_print(BigInt *bi);
BigInt *bgi_clone(BigInt *bi);
void bgi_add(BigInt *bi1, BigInt *bi2);
void bgi_free(BigInt *bi);

const char* bgi_get_status_msg(BigInt *bi) {
#define X(name, msg) case name: return msg;
    switch (bi->status_code) {
        BGI_STATUS_LIST(X)
        default:
            return "Unknown status_code";
    }
#undef X
}

BigInt *bgi_init(const char* text) {
    BigInt *bi = (BigInt*)malloc(sizeof(BigInt));
    bgi_assert(bi != NULL, "bi cannot be NULL");

    if (bi == NULL) {
        return NULL;
    }

    bool is_numeric = true;
    bi->sign = true;
    bi->numeric = list_init(); 
    bi->decimal = list_init(); 

    bgi_assert(bi->numeric != NULL, "bi->numeric cannot be NULL");
    if (bi->numeric == NULL) {
        bi->status_code = BGI_NUMERIC_FAIL;
        return bi;
    }

    bgi_assert(bi->decimal != NULL, "bi->decimal cannot be NULL");
    if (bi->decimal == NULL) {
        bi->status_code = BGI_DECIMAL_FAIL;
        return bi;
    }

    size_t len = strlen(text);
    bgi_assert(len > 0, "text cannot be empty");
    if (len == 0) {
        bi->status_code = BGI_INVALID_TEXT_VALUE;
        return bi;
    }

    if (text[0] == '-') {
        bi->sign = false;
    }

    for (size_t i = 0; i < len; i++) {
        if (i == 0) {
            if (text[0] == '-') {
                bi->sign = false;
                continue;
            }

            if (text[0] == '+') {
                bi->sign = true;
                continue;
            }

            if (text[0] >= 48 && text[0] <= 57) {
                bi->sign = true;
                list_append(bi->numeric, text[0]-'0');

                if (bi->numeric->status_code != LIST_OK) {
                    bi->status_code = BGI_NUMERIC_FAIL;
                    return bi;
                }
                continue;
            }

            bi->status_code = BGI_INVALID_TEXT_VALUE;
            return bi;
        }

        if (text[i] == '.') {
            if (! is_numeric) {
                bi->status_code = BGI_INVALID_TEXT_VALUE;
                return bi;
            }

            if (list_len(bi->numeric) == 0) {
                bi->status_code = BGI_INVALID_TEXT_VALUE;
                return bi;
            }

            int8 value = list_get(bi->numeric, list_len(bi->numeric)-1);
            if (bi->numeric->status_code != LIST_OK) {
                bi->status_code = BGI_NUMERIC_FAIL;
                return bi;
            }

            if (list_len(bi->numeric) > 1 && value == 0) {
                bi->status_code = BGI_INVALID_TEXT_VALUE;
                return bi;
            }

            is_numeric = false;
            continue;
        }

        if (text[i] >= 48 && text[i] <= 57) {
            if (is_numeric) {
                list_append(bi->numeric, text[i]-'0');
                if (bi->numeric->status_code != LIST_OK) {
                    bi->status_code = BGI_NUMERIC_FAIL;
                    return bi;
                }
            } else {
                list_append(bi->decimal, text[i]-'0');
                if (bi->numeric->status_code != LIST_OK) {
                    bi->status_code = BGI_DECIMAL_FAIL;
                    return bi;
                }
            }
        } else {
            bi->status_code = BGI_INVALID_TEXT_VALUE;
            return bi;
        }
    }

    list_reverse(bi->numeric);
    if (bi->numeric->status_code != LIST_OK) {
        bi->status_code = BGI_NUMERIC_FAIL;
        return bi;
    }

    list_reverse(bi->decimal);
    if (bi->decimal->status_code != LIST_OK) {
        bi->status_code = BGI_DECIMAL_FAIL;
        return bi;
    }

    bi->status_code = BGI_OK;
    return bi;
}

void bgi_print(BigInt *bi) {
    bgi_assert(bi != NULL, "bi cannot be NULL");
    bgi_assert(bi->numeric != NULL, "bi->numeric cannot be NULL");
    bgi_assert(bi->decimal != NULL, "bi->decimal cannot be NULL");

    if (bi == NULL || bi->numeric == NULL || bi->decimal == NULL) {
        return;
    }

    if (bi->status_code != BGI_OK || bi->numeric->status_code != LIST_OK || bi->decimal->status_code != LIST_OK) {
        return;
    }

    printf("----------------\n");
    printf("sign: %c\n", bi->sign ? '+' : '-');


    printf("value: ");
    for (int i = list_len(bi->numeric)-1; i >= 0; i--) {
        int8 value = list_get(bi->numeric, i);
        if (bi->numeric->status_code != LIST_OK) {
            return;
        }
        printf("%d", value);
    }

    if (list_len(bi->decimal) == 0) {
        printf("\n");
    } else {
        printf(".");
        for (int i = list_len(bi->decimal)-1; i >= 0; i--) {
            int8 value = list_get(bi->decimal, i);
            if (bi->decimal->status_code != LIST_OK) {
                return;
            }
            printf("%d", value);
        }
        printf("\n");
    }
    printf("----------------\n");
}

BigInt *bgi_clone(BigInt *bi) {
    bgi_assert(bi != NULL, "bi cannot be NULL");
    bgi_assert(bi->numeric != NULL, "bi->numeric cannot be NULL");
    bgi_assert(bi->decimal != NULL, "bi->decimal cannot be NULL");

    if (bi == NULL || bi->numeric == NULL || bi->decimal == NULL) {
        return NULL;
    }

    if (bi->status_code != BGI_OK || bi->numeric->status_code != LIST_OK || bi->decimal->status_code != LIST_OK) {
        return NULL;
    }

    BigInt *bi_copy = (BigInt*)malloc(sizeof(BigInt));
    if (bi_copy == NULL) {
        return NULL;
    }

    bi->sign = bi->sign;
    bi->status_code = BGI_OK;

    // numeric part
    bi_copy->numeric = list_init();
    if (bi_copy->numeric == NULL || bi_copy->numeric->status_code != LIST_OK) {
        bi_copy->status_code = BGI_NUMERIC_FAIL;
        return bi_copy;
    }

    for (size_t i = 0; i < list_len(bi->numeric); i++) {
        int8 value = list_get(bi->numeric, i);
        if (bi->numeric->status_code != LIST_OK) {
            bi_copy->status_code = BGI_NUMERIC_FAIL;
            return bi_copy;
        }
        list_append(bi_copy->numeric, value);
        if (bi_copy->numeric->status_code != LIST_OK) {
            bi_copy->status_code = BGI_NUMERIC_FAIL;
            return bi_copy;
        }
    }

    // decimal part
    bi_copy->decimal = list_init();
    if (bi_copy->decimal == NULL || bi_copy->decimal->status_code != LIST_OK) {
        bi_copy->status_code = BGI_DECIMAL_FAIL;
        return bi_copy;
    }

    for (size_t i = 0; i < list_len(bi->decimal); i++) {
        int8 value = list_get(bi->decimal, i);
        if (bi->decimal->status_code != LIST_OK) {
            bi_copy->status_code = BGI_DECIMAL_FAIL;
            return bi_copy;
        }
        list_append(bi_copy->decimal, value);
        if (bi_copy->decimal->status_code != LIST_OK) {
            bi_copy->status_code = BGI_DECIMAL_FAIL;
            return bi_copy;
        }
    }

    return bi_copy;
}

void bgi_add(BigInt *bi1, BigInt *bi2) {
    /*
    bgi_assert(bi1 != NULL, "bi1 cannot be NULL");
    bgi_assert(bi1->numeric != NULL, "bi1->numeric cannot be NULL");
    bgi_assert(bi1->decimal != NULL, "bi1->decimal cannot be NULL");

    if (bi1 == NULL || bi1->numeric == NULL || bi1->decimal == NULL) {
        return;
    }

    if (bi1->status_code != BGI_OK || bi1->status_code != LIST_OK || bi1->status_code != LIST_OK) {
        return;
    }

    bgi_assert(bi2 != NULL, "bi2 cannot be NULL");
    bgi_assert(bi2->numeric != NULL, "bi2->numeric cannot be NULL");
    bgi_assert(bi2->decimal != NULL, "bi2->decimal cannot be NULL");

    if (bi2 == NULL || bi2->numeric == NULL || bi2->decimal == NULL) {
        return;
    }

    if (bi2->status_code != BGI_OK || bi2->status_code != LIST_OK || bi2->status_code != LIST_OK) {
        return;
    }

    if (list_len(bi1->numeric) < list_len(bi2->numeric)) {
        for (size_t i = 0; i < list_len(bi2->numeric) - list_len(bi1->numeric); i++) {
            list_append(bi1->numeric, 0);
            if (bi1->numeric->status_code != LIST_OK) {
                bi1->status_code = BGI_NUMERIC_FAIL;
                return;
            }
        }
    }

    List *bi3 = list_init(bi2->text);

    if (list_len(bi1->numeric) > list_len(bi2->numeric)) {
        for (size_t i = 0; i < list_len(bi2->numeric) - list_len(bi1->numeric); i++) {
            list_append(bi1->numeric, 0);
            if (bi1->numeric->status_code != LIST_OK) {
                bi1->status_code = BGI_NUMERIC_FAIL;
                return;
            }
        }
    }
    */
}

void bgi_free(BigInt *bi) {
    if (bi == NULL) return;
    if (bi->numeric) {
        free(bi->numeric);
    }
    if (bi->decimal) {
        free(bi->decimal);
    }
    free(bi);
}

#endif
