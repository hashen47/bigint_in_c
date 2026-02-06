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

        if (l->status_code != LIST_OK) {
            return;
        }
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
const char *bgi_get_text(BigInt *bi);
BigInt *bgi_clone(BigInt *bi);
int bgi_cmp(BigInt *bi1, BigInt *bi2);
int bgi_abs_cmp(BigInt *bi1, BigInt *bi2);
BigInt *bgi_add(BigInt *bi1, BigInt *bi2);
BigInt *bgi_sub(BigInt *bi1, BigInt *bi2);
BigInt *bgi_mult(BigInt *bi1, BigInt *bi2);
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

    bi->sign = true;

    size_t index = 0;
    if (text[index] == '+') {
        bi->sign = true;
        index++;
    } else if (text[index] == '-') {
        bi->sign = false;
        index++;
    } else if (text[index] < 48 || text[index] > 57) {
        bi->status_code = BGI_INVALID_TEXT_VALUE;
        return bi;
    }

    bool is_all_numeric_digits_are_zeros = true;
    bool is_all_decimal_digits_are_zeros = true;
    size_t decimal_part_start_index = 0;

    for (size_t i = index; i < strlen(text); i++) {
        if (text[i] == '.') {
            if (i == index) {
                bi->status_code = BGI_INVALID_TEXT_VALUE;
                return bi;
            }
            decimal_part_start_index = i+1;
            break;
        }
        if (text[i] >= 48 && text[i] <= 57) {
            if (text[i] != '0') {
                is_all_numeric_digits_are_zeros = false;
            }
        } else {
            bi->status_code = BGI_INVALID_TEXT_VALUE;
            return bi;
        }
    }

    if (!is_all_numeric_digits_are_zeros) {
        for (size_t i = index; i < strlen(text); i++) {
            if (text[i] == '.') {
                break;
            }
            list_append(bi->numeric, text[i]-'0');
            if (bi->numeric->status_code != LIST_OK) {
                bi->status_code = BGI_NUMERIC_FAIL;
                return bi;
            }
        }
    }

    if (decimal_part_start_index > 0) {
        if (decimal_part_start_index >= strlen(text)) {
            bi->status_code = BGI_INVALID_TEXT_VALUE;
            return bi;
        }
        for (size_t i = decimal_part_start_index; i < strlen(text); i++) {
            if (text[i] >= 48 && text[i] <= 57) {
                if (text[i] != '0') {
                    is_all_decimal_digits_are_zeros = false;
                }
            } else {
                bi->status_code = BGI_INVALID_TEXT_VALUE;
                return bi;
            }
        }
    }

    if (!is_all_decimal_digits_are_zeros) {
        for (size_t i = decimal_part_start_index; i < strlen(text); i++) {
            list_append(bi->decimal, text[i]-'0');
            if (bi->decimal->status_code != LIST_OK) {
                bi->status_code = BGI_DECIMAL_FAIL;
                return bi;
            }
        }
    }

    if (list_len(bi->numeric) == 0 && list_len(bi->decimal) == 0) {
        bi->sign = true;
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

    printf("<BigInt %c", bi->sign ? '+' : '-');

    if (list_len(bi->numeric) == 0) {
        printf("0");
    }

    for (size_t i = 0; i < list_len(bi->numeric); i++) {
        int8 value = list_get(bi->numeric, i);
        if (bi->numeric->status_code != LIST_OK) {
            return;
        }
        printf("%d", value);
    }

    if (list_len(bi->decimal) == 0) {
        printf(">\n");
    } else {
        printf(".");
        for (size_t i = 0; i < list_len(bi->decimal); i++) {
            int8 value = list_get(bi->decimal, i);
            if (bi->decimal->status_code != LIST_OK) {
                return;
            }
            printf("%d", value);
        }
        printf(">\n");
    }
}

const char *bgi_get_text(BigInt *bi) {
    bgi_assert(bi != NULL, "bi cannot be NULL");
    bgi_assert(bi->numeric != NULL, "bi->numeric cannot be NULL");
    bgi_assert(bi->decimal != NULL, "bi->decimal cannot be NULL");

    if (bi == NULL || bi->numeric == NULL || bi->decimal == NULL) {
        return NULL;
    }

    if (bi->status_code != BGI_OK || bi->numeric->status_code != LIST_OK || bi->decimal->status_code != LIST_OK) {
        return NULL;
    }

    size_t length = 0;

    if (list_len(bi->numeric) == 0 && list_len(bi->decimal) == 0) {
        length++; // '0'
    }

    if (list_len(bi->numeric) > 0 && list_len(bi->decimal) == 0) {
        length += 1 + list_len(bi->numeric); // ex: +23, -23
    }

    if (list_len(bi->numeric) == 0 && list_len(bi->decimal) > 0) {
        length += 3 + list_len(bi->numeric); // ex: +0.23, -0.23
    }

    if (list_len(bi->numeric) > 0 && list_len(bi->decimal) > 0) {
        length += 2 + list_len(bi->numeric) + list_len(bi->decimal);  // ex: +23.23, -23.23
    }

    length++; // for last null terminator for the string

    char *text = (char*)calloc(length, sizeof(char));
    if (text == NULL) {
        return NULL;
    }

    if (list_len(bi->numeric) == 0 && list_len(bi->decimal) == 0) {
        text[0] = '0';
    }

    text[0] = bi->sign ? '+' : '-';

    if (list_len(bi->numeric) > 0 && list_len(bi->decimal) == 0) {
        for (size_t i = 0; i < list_len(bi->numeric); i++) {
            int8 value = list_get(bi->numeric, i);
            if (bi->numeric->status_code != LIST_OK) {
                bi->status_code = BGI_NUMERIC_FAIL;
                free(text);
                return NULL;
            }
            text[i+1] = value + '0'; 
        }
    }

    if (list_len(bi->numeric) == 0 && list_len(bi->decimal) > 0) {
        text[1] = '0';
        text[2] = '.';
        for (size_t i = 0; i < list_len(bi->decimal); i++) {
            int8 value = list_get(bi->decimal, i);
            if (bi->decimal->status_code != LIST_OK) {
                bi->status_code = BGI_DECIMAL_FAIL;
                free(text);
                return NULL;
            }
            text[i+3] = value + '0'; 
        }
    }

    if (list_len(bi->numeric) > 0 && list_len(bi->decimal) > 0) {
        for (size_t i = 0; i < list_len(bi->numeric); i++) {
            int8 value = list_get(bi->numeric, i);
            if (bi->numeric->status_code != LIST_OK) {
                bi->status_code = BGI_NUMERIC_FAIL;
                free(text);
                return NULL;
            }
            text[i+1] = value + '0'; 
        }
        text[1 + list_len(bi->numeric)] = '.';
        for (size_t i = 0; i < list_len(bi->decimal); i++) {
            int8 value = list_get(bi->decimal, i);
            if (bi->decimal->status_code != LIST_OK) {
                bi->status_code = BGI_DECIMAL_FAIL;
                free(text);
                return NULL;
            }
            text[i+2+list_len(bi->numeric)] = value + '0'; 
        }
    }

    return text;
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

    bi_copy->sign = bi->sign;
    bi_copy->status_code = BGI_OK;

    // numeric part
    bi_copy->numeric = list_clone(bi->numeric);
    if (bi_copy->numeric == NULL || bi_copy->numeric->status_code != LIST_OK) {
        bi_copy->status_code = BGI_NUMERIC_FAIL;
        return bi_copy;
    }

    // decimal part
    bi_copy->decimal = list_clone(bi->decimal);
    if (bi_copy->decimal == NULL || bi_copy->decimal->status_code != LIST_OK) {
        bi_copy->status_code = BGI_DECIMAL_FAIL;
        return bi_copy;
    }

    return bi_copy;
}

int bgi_cmp(BigInt *bi1, BigInt *bi2) {
    if (bi1->sign && !bi2->sign) {
        return 1;
    }

    if (!bi1->sign && bi2->sign) {
        return -1;
    }

    if (bi1->sign) {
        if (list_len(bi1->numeric) > list_len(bi2->numeric)) {
            return 1;
        }

        if (list_len(bi1->numeric) < list_len(bi2->numeric)) {
            return -1;
        }

        for (size_t i = 0; i < list_len(bi1->numeric); i++) {
            int8 n1 = list_get(bi1->numeric, i);
            int8 n2 = list_get(bi2->numeric, i);
            if (n1 > n2) {
                return 1;
            }
            if (n1 < n2) {
                return -1;
            }
        }

        size_t len = list_len(bi1->decimal);
        if (list_len(bi1->decimal) > list_len(bi2->decimal)) {
            len = list_len(bi2->decimal);
        }

        for (size_t i = 0; i < len; i++) {
            int8 n1 = list_get(bi1->decimal, i);
            int8 n2 = list_get(bi2->decimal, i);
            if (n1 > n2) {
                return 1;
            }
            if (n1 < n2) {
                return -1;
            }
        }

        if (list_len(bi1->decimal) > list_len(bi2->decimal)) {
            return 1;
        }

        if (list_len(bi1->decimal) < list_len(bi2->decimal)) {
            return -1;
        }

        return 0;
    }

    if (list_len(bi1->numeric) > list_len(bi2->numeric)) {
        return -1;
    }

    if (list_len(bi1->numeric) < list_len(bi2->numeric)) {
        return 1;
    }

    for (size_t i = 0; i < list_len(bi1->numeric); i++) {
        int8 n1 = list_get(bi1->numeric, i);
        int8 n2 = list_get(bi2->numeric, i);
        if (n1 > n2) {
            return -1;
        }
        if (n1 < n2) {
            return 1;
        }
    }

    size_t len = list_len(bi1->decimal);
    if (list_len(bi1->decimal) > list_len(bi2->decimal)) {
        len = list_len(bi2->decimal);
    }

    for (size_t i = 0; i < len; i++) {
        int8 n1 = list_get(bi1->decimal, i);
        int8 n2 = list_get(bi2->decimal, i);
        if (n1 > n2) {
            return -1;
        }
        if (n1 < n2) {
            return 1;
        }
    }

    if (list_len(bi1->decimal) > list_len(bi2->decimal)) {
        return -1;
    }

    if (list_len(bi1->decimal) < list_len(bi2->decimal)) {
        return 1;
    }

    return 0;
}

int bgi_abs_cmp(BigInt *bi1, BigInt *bi2) {
    if (list_len(bi1->numeric) > list_len(bi2->numeric)) {
        return 1;
    }

    if (list_len(bi1->numeric) < list_len(bi2->numeric)) {
        return -1;
    }

    for (size_t i = 0; i < list_len(bi1->numeric); i++) {
        int8 n1 = list_get(bi1->numeric, i);
        int8 n2 = list_get(bi2->numeric, i);
        if (n1 > n2) {
            return 1;
        }
        if (n1 < n2) {
            return -1;
        }
    }

    size_t len = list_len(bi1->decimal);
    if (list_len(bi1->decimal) > list_len(bi2->decimal)) {
        len = list_len(bi2->decimal);
    }

    for (size_t i = 0; i < len; i++) {
        int8 n1 = list_get(bi1->decimal, i);
        int8 n2 = list_get(bi2->decimal, i);
        if (n1 > n2) {
            return 1;
        }
        if (n1 < n2) {
            return -1;
        }
    }

    if (list_len(bi1->decimal) > list_len(bi2->decimal)) {
        return 1;
    }

    if (list_len(bi1->decimal) < list_len(bi2->decimal)) {
        return -1;
    }

    return 0;
}

BigInt *bgi_add(BigInt *bi1, BigInt *bi2) {
    bgi_assert(bi1 != NULL, "bi1 cannot be NULL");
    bgi_assert(bi1->numeric != NULL, "bi1->numeric cannot be NULL");
    bgi_assert(bi1->decimal != NULL, "bi1->decimal cannot be NULL");

    if (bi1 == NULL || bi1->numeric == NULL || bi1->decimal == NULL) {
        return NULL;
    }

    if (bi1->status_code != BGI_OK || bi1->status_code != LIST_OK || bi1->status_code != LIST_OK) {
        return NULL;
    }

    bgi_assert(bi2 != NULL, "bi2 cannot be NULL");
    bgi_assert(bi2->numeric != NULL, "bi2->numeric cannot be NULL");
    bgi_assert(bi2->decimal != NULL, "bi2->decimal cannot be NULL");

    if (bi2 == NULL || bi2->numeric == NULL || bi2->decimal == NULL) {
        return NULL;
    }

    if (bi2->status_code != BGI_OK || bi2->status_code != LIST_OK || bi2->status_code != LIST_OK) {
        return NULL;
    }

    if (bi1->sign != bi2->sign) {
        if (bi1->sign) {
            bi2->sign = true;
            BigInt *result = bgi_sub(bi1, bi2);
            bi2->sign = false;
            return result;
        }
        bi1->sign = true;
        BigInt *result = bgi_sub(bi2, bi1);
        bi1->sign = false;
        return result;
    }

    int8 carrier = 0;
    BigInt *bi1_copy = bgi_clone(bi1);

    if (bi1_copy == NULL) {
        return NULL;
    }

    if (bi1_copy->status_code != BGI_OK) {
        return bi1_copy;
    }

    BigInt *bi2_copy = bgi_clone(bi2);

    if (bi2_copy == NULL) {
        bgi_free(bi1_copy);
        return NULL;
    }

    if (bi2_copy->status_code != BGI_OK) {
        bgi_free(bi1_copy);
        return bi2_copy;
    }

    // do addition in decimal part
    if (list_len(bi2_copy->decimal) > list_len(bi1_copy->decimal)) {
        size_t diff = list_len(bi2_copy->decimal) - list_len(bi1_copy->decimal);
        for (size_t i = 0; i < diff; i++) {
            list_append(bi1_copy->decimal, 0);
            if (bi1_copy->decimal->status_code != LIST_OK) {
                bi1_copy->status_code = BGI_DECIMAL_FAIL;
                bgi_free(bi2_copy);
                return bi1_copy;
            }
        }
    } else {
        size_t diff = list_len(bi1_copy->decimal) - list_len(bi2_copy->decimal);
        for (size_t i = 0; i < diff; i++) {
            list_append(bi2_copy->decimal, 0);
            if (bi2_copy->decimal->status_code != LIST_OK) {
                bi2_copy->status_code = BGI_NUMERIC_FAIL;
                bgi_free(bi1_copy);
                return bi2_copy;
            }
        }
    }

    for (int i = list_len(bi1_copy->decimal)-1; i >= 0; i--) {
        int8 n1 = list_get(bi1_copy->decimal, i);
        int8 n2 = list_get(bi2_copy->decimal, i);

        if (bi1_copy->decimal->status_code != LIST_OK) {
            bi1_copy->status_code = BGI_DECIMAL_FAIL;
            bgi_free(bi2_copy);
            return bi1_copy;
        }

        if (bi2_copy->decimal->status_code != LIST_OK) {
            bi2_copy->status_code = BGI_DECIMAL_FAIL;
            bgi_free(bi1_copy);
            return bi2_copy;
        }

        list_set(bi1_copy->decimal, i, (n1 + n2 + carrier) % 10);
        if (bi1_copy->decimal->status_code != LIST_OK) {
            bi1_copy->status_code = BGI_DECIMAL_FAIL;
            bgi_free(bi2_copy);
            return bi1_copy;
        }

        carrier = (n1 + n2) / 10;
    }

    list_reverse(bi1_copy->numeric);
    if (bi1_copy->numeric->status_code != LIST_OK) {
        bi1_copy->status_code = BGI_NUMERIC_FAIL;
        bgi_free(bi2_copy);
        return bi1_copy;
    }

    list_reverse(bi2_copy->numeric);
    if (bi2_copy->numeric->status_code != LIST_OK) {
        bi2_copy->status_code = BGI_NUMERIC_FAIL;
        bgi_free(bi1_copy);
        return bi2_copy;
    }

    // do addition in numeric part
    if (list_len(bi1_copy->numeric) < list_len(bi2_copy->numeric)) {
        size_t diff = list_len(bi2_copy->numeric) - list_len(bi1_copy->numeric);
        for (size_t i = 0; i < diff; i++) {
            list_append(bi1_copy->numeric, 0);
            if (bi1_copy->numeric->status_code != LIST_OK) {
                bi1_copy->status_code = BGI_NUMERIC_FAIL;
                bgi_free(bi2_copy);
                return bi1_copy;
            }
        }
    } else {
        size_t diff = list_len(bi1_copy->numeric) - list_len(bi2_copy->numeric);
        for (size_t i = 0; i < diff; i++) {
            list_append(bi2_copy->numeric, 0);
            if (bi2_copy->numeric->status_code != LIST_OK) {
                bi2_copy->status_code = BGI_NUMERIC_FAIL;
                bgi_free(bi1_copy);
                return bi2_copy;
            }
        }
    }

    for (int i = 0; i < list_len(bi1_copy->numeric); i++) {
        int8 n1 = list_get(bi1_copy->numeric, i);
        int8 n2 = list_get(bi2_copy->numeric, i);
        if (bi1_copy->numeric->status_code != LIST_OK) {
            bi1_copy->status_code = BGI_NUMERIC_FAIL;
            bgi_free(bi2_copy);
            return bi1_copy;
        }
        if (bi2_copy->numeric->status_code != LIST_OK) {
            bi2_copy->status_code = BGI_NUMERIC_FAIL;
            bgi_free(bi1_copy);
            return bi2_copy;
        }

        list_set(bi1_copy->numeric, i, (n1 + n2 + carrier) % 10);
        if (bi1_copy->numeric->status_code != LIST_OK) {
            bi1_copy->status_code = BGI_NUMERIC_FAIL;
            bgi_free(bi2_copy);
            return bi1_copy;
        }
        carrier = (n1 + n2 + carrier) / 10;
    }

    if (carrier > 0) {
        list_append(bi1_copy->numeric, carrier);
        if (bi1_copy->numeric->status_code != LIST_OK) {
            bi1_copy->status_code = BGI_NUMERIC_FAIL;
            bgi_free(bi2_copy);
            return bi1_copy;
        }
    }

    list_reverse(bi1_copy->numeric);
    if (bi1_copy->numeric->status_code != LIST_OK) {
        bi1_copy->status_code = BGI_NUMERIC_FAIL;
        bgi_free(bi2_copy);
        return bi1_copy;
    }

    bgi_free(bi2_copy);
    return bi1_copy;
}

BigInt *bgi_sub(BigInt *bi1, BigInt *bi2) {
    bgi_assert(bi1 != NULL, "bi1 cannot be NULL");
    bgi_assert(bi1->numeric != NULL, "bi1->numeric cannot be NULL");
    bgi_assert(bi1->decimal != NULL, "bi1->decimal cannot be NULL");

    if (bi1 == NULL || bi1->numeric == NULL || bi1->decimal == NULL) {
        return NULL;
    }

    if (bi1->status_code != BGI_OK || bi1->status_code != LIST_OK || bi1->status_code != LIST_OK) {
        return NULL;
    }

    bgi_assert(bi2 != NULL, "bi2 cannot be NULL");
    bgi_assert(bi2->numeric != NULL, "bi2->numeric cannot be NULL");
    bgi_assert(bi2->decimal != NULL, "bi2->decimal cannot be NULL");

    if (bi2 == NULL || bi2->numeric == NULL || bi2->decimal == NULL) {
        return NULL;
    }

    if (bi2->status_code != BGI_OK || bi2->status_code != LIST_OK || bi2->status_code != LIST_OK) {
        return NULL;
    }

    BigInt *bi2_copy = bgi_clone(bi2);

    if (bi2_copy == NULL) {
        return NULL;
    }

    if (bi2_copy->status_code != BGI_OK) {
        return bi2_copy;
    }

    if (bi1->sign && !bi2->sign) {
        bi2_copy->sign = true;
        BigInt *result = bgi_add(bi1, bi2_copy);
        bgi_free(bi2_copy);
        return result;
    }

    BigInt *bi1_copy = bgi_clone(bi1);

    if (bi1_copy == NULL) {
        bgi_free(bi2_copy);
        return NULL;
    }

    if (bi1_copy->status_code != BGI_OK) {
        bgi_free(bi2_copy);
        return bi1_copy;
    }

    if (!bi1_copy->sign && bi2_copy->sign) {
        bi2_copy->sign = false;
        BigInt *result = bgi_add(bi1, bi2_copy);
        bgi_free(bi2_copy);
        return result;
    }

    int val = bgi_abs_cmp(bi1_copy, bi2_copy);
    if (bi1_copy->status_code != BGI_OK) {
        bgi_free(bi2_copy);
        return bi1_copy;
    }

    if (bi2_copy->status_code != BGI_OK) {
        bgi_free(bi1_copy);
        return bi2_copy;
    }

    if (val == 0) {
        bi1_copy->sign    = true;
        list_free(bi1_copy->numeric);
        list_free(bi1_copy->decimal);

        bi1_copy->numeric = list_init();
        if (bi1_copy->numeric == NULL || bi1_copy->numeric->status_code != LIST_OK) {
            bi1_copy->status_code = BGI_NUMERIC_FAIL;
            bgi_free(bi2_copy);
            return bi1_copy;
        }

        bi1_copy->decimal = list_init();
        if (bi1_copy->decimal == NULL || bi1_copy->decimal->status_code != LIST_OK) {
            bi1_copy->status_code = BGI_DECIMAL_FAIL;
            bgi_free(bi2_copy);
            return bi1_copy;
        }

        return bi1_copy;
    }

    bool sign = bi1_copy->sign;
    if (val == -1) {
        sign = !bi2_copy->sign;
        BigInt *temp = bi1_copy;
        bi1_copy = bi2_copy;
        bi2_copy = temp;
    }

    // decimal part subtraction
    if (list_len(bi1_copy->decimal) > 0) {
        size_t min_decimal_length = list_len(bi1_copy->decimal);
        if (list_len(bi1_copy->decimal) < list_len(bi2_copy->decimal)) {
            size_t diff = list_len(bi2_copy->decimal) - list_len(bi1_copy->decimal);
            for (size_t i = 0; i < diff; i++) {
                list_append(bi1_copy->decimal, 0);
                if (bi1_copy->decimal->status_code != LIST_OK) {
                    bi1_copy->status_code = BGI_DECIMAL_FAIL;
                    bgi_free(bi2_copy);
                    return bi1_copy;
                }
            }
        } else {
            min_decimal_length = list_len(bi2_copy->decimal);
            size_t diff = list_len(bi1_copy->decimal) - list_len(bi2_copy->decimal);
            for (size_t i = 0; i < diff; i++) {
                list_append(bi2_copy->decimal, 0);
                if (bi2_copy->decimal->status_code != LIST_OK) {
                    bi2_copy->status_code = BGI_DECIMAL_FAIL;
                    bgi_free(bi1_copy);
                    return bi2_copy;
                }
            }
        }

        for (int i = min_decimal_length-1; i >= 0; i--) {
            int8 n1 = list_get(bi1_copy->decimal, i);
            int8 n2 = list_get(bi2_copy->decimal, i);

            if (bi1_copy->decimal->status_code != LIST_OK) {
                bi1_copy->status_code = BGI_DECIMAL_FAIL;
                bgi_free(bi2_copy);
                return bi1_copy;
            }

            if (bi2_copy->decimal->status_code != LIST_OK) {
                bi2_copy->status_code = BGI_DECIMAL_FAIL;
                bgi_free(bi1_copy);
                return bi2_copy;
            }

            if (n1 >= n2) {
                list_set(bi1_copy->decimal, i, n1-n2);
                if (bi1_copy->decimal->status_code != LIST_OK) {
                    bi1_copy->status_code = BGI_DECIMAL_FAIL;
                    bgi_free(bi1_copy);
                    return bi1_copy;
                }
            } else {
                int carrier_index = -1;

                for (int j = i-1; j >= 0; j--) {
                    int8 value = list_get(bi1_copy->decimal, j);
                    if (bi1_copy->decimal->status_code != LIST_OK) {
                        bi1_copy->status_code = BGI_DECIMAL_FAIL;
                        bgi_free(bi2_copy);
                        return bi1_copy;
                    }
                    if (value > 0) {
                        carrier_index = j;
                        break;
                    }
                }

                if (carrier_index > -1) {
                    int8 value = list_get(bi1_copy->decimal, carrier_index);
                    if (bi1_copy->decimal->status_code != LIST_OK) {
                        bi1_copy->status_code = BGI_DECIMAL_FAIL;
                        bgi_free(bi2_copy);
                        return bi1_copy;
                    }

                    list_set(bi1_copy->decimal, carrier_index, value-1);
                    if (bi1_copy->decimal->status_code != LIST_OK) {
                        bi1_copy->status_code = BGI_DECIMAL_FAIL;
                        bgi_free(bi2_copy);
                        return bi1_copy;
                    }

                    for (int j = carrier_index+1; j < i; j++) {
                        list_set(bi1_copy->decimal, j, 9);
                        if (bi1_copy->decimal->status_code != LIST_OK) {
                            bi1_copy->status_code = BGI_DECIMAL_FAIL;
                            bgi_free(bi2_copy);
                            return bi1_copy;
                        }
                    }

                    list_set(bi1_copy->decimal, i, n1+10-n2);
                    if (bi1_copy->decimal->status_code != LIST_OK) {
                        bi1_copy->status_code = BGI_DECIMAL_FAIL;
                        bgi_free(bi2_copy);
                        return bi1_copy;
                    }
                } else { // have to carrier from numeric part
                    for (int j = list_len(bi1_copy->numeric)-1; j >= 0; j--) {
                        int8 value = list_get(bi1_copy->numeric, j);
                        if (bi1_copy->numeric->status_code != LIST_OK) {
                            bi1_copy->status_code = BGI_NUMERIC_FAIL;
                            bgi_free(bi2_copy);
                            return bi1_copy;
                        }
                        if (value > 0) {
                            carrier_index = j;
                            break;
                        }
                    }

                    bgi_assert(carrier_index != -1, "carrier_index cannot be -1 (something went wrong)");
                    if (carrier_index == -1) {
                        bgi_free(bi1_copy);
                        bgi_free(bi2_copy);
                        return NULL;
                    }

                    int8 value = list_get(bi1_copy->numeric, carrier_index);
                    if (bi1_copy->numeric->status_code != LIST_OK) {
                        bi1_copy->status_code = BGI_NUMERIC_FAIL;
                        bgi_free(bi2_copy);
                        return bi1_copy;
                    }
                    list_set(bi1_copy->numeric, carrier_index, value-1);
                    if (bi1_copy->numeric->status_code != LIST_OK) {
                        bi1_copy->status_code = BGI_NUMERIC_FAIL;
                        bgi_free(bi2_copy);
                        return bi1_copy;
                    }

                    for (int j = carrier_index+1; j < list_len(bi1_copy->numeric); j++) {
                        list_set(bi1_copy->numeric, j, 9);
                        if (bi1_copy->numeric->status_code != LIST_OK) {
                            bi1_copy->status_code = BGI_NUMERIC_FAIL;
                            bgi_free(bi2_copy);
                            return bi1_copy;
                        }
                    }

                    for (int j = 0; j < i; j++) {
                        list_set(bi1_copy->decimal, j, 9);
                        if (bi1_copy->decimal->status_code != LIST_OK) {
                            bi1_copy->status_code = BGI_DECIMAL_FAIL;
                            bgi_free(bi2_copy);
                            return bi1_copy;
                        }
                    }

                    list_set(bi1_copy->decimal, i, n1+10-n2);
                    if (bi1_copy->decimal->status_code != LIST_OK) {
                        bi1_copy->status_code = BGI_DECIMAL_FAIL;
                        bgi_free(bi2_copy);
                        return bi1_copy;
                    }
                }
            }
        }
    }

    if (list_len(bi1_copy->numeric) > 0 && list_len(bi2_copy->numeric) > 0) {
        bgi_assert(list_len(bi1_copy->numeric) >= list_len(bi2_copy->numeric), "bi1_copy->numeric length should be greater or equal to bi2_copy->numeric length");
        if (list_len(bi1_copy->numeric) < list_len(bi2_copy->numeric)) {
            bgi_free(bi1_copy);
            bgi_free(bi2_copy);
            return NULL;
        }

        for (size_t i = 0; i < list_len(bi2_copy->numeric); i++) {
            int b1i = list_len(bi1_copy->numeric)-i-1;
            int b2i = list_len(bi2_copy->numeric)-i-1;

            int8 n1 = list_get(bi1_copy->numeric, b1i);
            int8 n2 = list_get(bi2_copy->numeric, b2i);

            if (bi1_copy->numeric->status_code != LIST_OK) {
                bi1_copy->status_code = BGI_NUMERIC_FAIL;
                bgi_free(bi2_copy);
                return bi1_copy;
            }

            if (bi2_copy->numeric->status_code != LIST_OK) {
                bi2_copy->status_code = BGI_NUMERIC_FAIL;
                bgi_free(bi1_copy);
                return bi2_copy;
            }

            if (n1 >= n2) {
                list_set(bi1_copy->numeric, b1i, n1-n2);
                if (bi1_copy->numeric->status_code != LIST_OK) {
                    bi1_copy->status_code = BGI_NUMERIC_FAIL;
                    bgi_free(bi2_copy);
                    return bi1_copy;
                }
            } else {
                int carrier_index = -1;
                for (int j = b1i-1; j >= 0; j--) {
                    int8 value = list_get(bi1_copy->numeric, j);
                    if (bi1_copy->numeric->status_code != LIST_OK) {
                        bi1_copy->status_code = BGI_NUMERIC_FAIL;
                        bgi_free(bi2_copy);
                        return bi1_copy;
                    }
                    if (value > 0) {
                        carrier_index = j;
                        list_set(bi1_copy->numeric, j, value-1);
                        if (bi1_copy->numeric->status_code != LIST_OK) {
                            bi1_copy->status_code = BGI_NUMERIC_FAIL;
                            bgi_free(bi2_copy);
                            return bi1_copy;
                        }
                        break;
                    }
                }

                bgi_assert(carrier_index != -1, "carrier_index cannot be -1");
                if (carrier_index == -1) {
                    bgi_free(bi1_copy);
                    bgi_free(bi2_copy);
                    return NULL;
                }

                for (int j = carrier_index+1; j < b1i; j++) {
                    list_set(bi1_copy->numeric, j, 9);
                    if (bi1_copy->numeric->status_code != LIST_OK) {
                        bi1_copy->status_code = BGI_NUMERIC_FAIL;
                        bgi_free(bi2_copy);
                        return bi1_copy;
                    }
                }

                list_set(bi1_copy->numeric, b1i, n1+10-n2);
                if (bi1_copy->numeric->status_code != LIST_OK) {
                    bi1_copy->status_code = BGI_NUMERIC_FAIL;
                    bgi_free(bi2_copy);
                    return bi1_copy;
                }
            }
        }
    }

    BigInt *bi3 = bgi_init("0");
    if (bi3 == NULL) {
        bgi_free(bi1_copy);
        bgi_free(bi2_copy);
        return NULL;
    }

    if (bi3->status_code != BGI_OK) {
        bgi_free(bi1_copy);
        bgi_free(bi2_copy);
        return bi3;
    }

    bool is_all_numeric_digits_are_zeros = true;
    for (size_t i = 0; i < list_len(bi1_copy->numeric); i++) {
        int8 value = list_get(bi1_copy->numeric, i);
        if (bi1_copy->numeric->status_code != LIST_OK) {
            bi1_copy->status_code = BGI_NUMERIC_FAIL;
            bgi_free(bi2_copy);
            return bi1_copy;
        }
        if (is_all_numeric_digits_are_zeros && value == 0) {
            continue;
        }
        is_all_numeric_digits_are_zeros = false; 
        list_append(bi3->numeric, value);
        if (bi3->numeric->status_code != LIST_OK) {
            bi3->status_code = BGI_NUMERIC_FAIL;
            bgi_free(bi1_copy);
            bgi_free(bi2_copy);
            return bi3;
        }
    }

    int last_valid_decimal_index = -1;
    for (int i = list_len(bi1_copy->decimal)-1; i >= 0; i--) {
        int8 value = list_get(bi1_copy->decimal, i);
        if (bi1_copy->decimal->status_code != LIST_OK) {
            bi1_copy->status_code = BGI_DECIMAL_FAIL;
            bgi_free(bi2_copy);
            bgi_free(bi3);
            return bi1_copy;
        }
        if (value != 0) {
            last_valid_decimal_index = i;
            break;
        }
    }

    for (int i = 0; i <= last_valid_decimal_index; i++) {
        int8 value = list_get(bi1_copy->decimal, i);
        if (bi1_copy->decimal->status_code != LIST_OK) {
            bi1_copy->status_code = BGI_DECIMAL_FAIL;
            bgi_free(bi2_copy);
            bgi_free(bi3);
            return bi1_copy;
        }
        list_append(bi3->decimal, value);
        if (bi3->decimal->status_code != LIST_OK) {
            bi3->status_code = BGI_DECIMAL_FAIL;
            bgi_free(bi1_copy);
            bgi_free(bi2_copy);
            return bi3;
        }
    }

    bi3->sign = sign;
    bgi_free(bi1_copy);
    bgi_free(bi2_copy);

    return bi3;
}

BigInt *bgi_mult(BigInt *bi1, BigInt *bi2) {
    bgi_assert(bi1 != NULL, "bi1 cannot be NULL");
    bgi_assert(bi1->numeric != NULL, "bi1->numeric cannot be NULL");
    bgi_assert(bi1->decimal != NULL, "bi1->decimal cannot be NULL");

    if (bi1 == NULL || bi1->numeric == NULL || bi1->decimal == NULL) {
        return NULL;
    }

    if (bi1->status_code != BGI_OK || bi1->status_code != LIST_OK || bi1->status_code != LIST_OK) {
        return NULL;
    }

    bgi_assert(bi2 != NULL, "bi2 cannot be NULL");
    bgi_assert(bi2->numeric != NULL, "bi2->numeric cannot be NULL");
    bgi_assert(bi2->decimal != NULL, "bi2->decimal cannot be NULL");

    if (bi2 == NULL || bi2->numeric == NULL || bi2->decimal == NULL) {
        return NULL;
    }

    if (bi2->status_code != BGI_OK || bi2->status_code != LIST_OK || bi2->status_code != LIST_OK) {
        return NULL;
    }

    // handle the multipication by zero
    BigInt *zero = bgi_init("0"); 
    if (zero == NULL) {
        return NULL;
    }

    if (zero->status_code != BGI_OK) {
        return zero;
    }

    int cmp1 = bgi_cmp(zero, bi1);
    if (bi1->status_code != BGI_OK) {
        return bi1;
    }
    if (zero->status_code != BGI_OK) {
        return zero;
    }

    int cmp2 = bgi_cmp(zero, bi2);
    if (bi2->status_code != BGI_OK) {
        return bi2;
    }
    if (zero->status_code != BGI_OK) {
        return zero;
    }

    if (cmp1 == 0 || cmp2 == 0) {
        return zero;
    }

    bgi_free(zero);

    // handle non zero multipication
    size_t decimal_points = list_len(bi1->decimal) + list_len(bi2->decimal);

    BigInt *bi1_copy = bgi_init("0");
    if (bi1_copy == NULL) {
        return NULL;
    }

    if (bi1_copy->status_code != BGI_OK) {
        return bi1_copy;
    }

    bi1_copy->sign    = bi1->sign;
    bi1_copy->numeric = list_clone(bi1->numeric);

    if (bi1_copy->numeric == NULL || bi1_copy->numeric->status_code != LIST_OK) {
        bi1_copy->status_code = BGI_NUMERIC_FAIL;
        return bi1_copy;
    }

    for (size_t i = 0; i < list_len(bi1->decimal); i++) {
        int8 value = list_get(bi1->decimal, i);
        if (bi1->decimal->status_code != LIST_OK) {
            return NULL;
        }
        list_append(bi1_copy->numeric, value);
        if (bi1_copy->numeric->status_code != LIST_OK) {
            bi1_copy->status_code = BGI_NUMERIC_FAIL;
            return bi1_copy;
        }
    }

    BigInt *result = bgi_init("0");
    if (result == NULL) {
        return NULL;
    }
    if (result->status_code != BGI_OK) {
        bgi_free(bi1_copy);
        return result;
    }

    List *lists[2] = {bi2->decimal, bi2->numeric};
    int8 carrier = 0;
    for (size_t li = 0; li < 2; li++) {
        for (size_t i = 0; i < list_len(lists[li]); i++) {
            BigInt *temp = bgi_clone(bi1_copy);
            if (temp == NULL) {
                bgi_free(bi1_copy);
                return NULL;
            }
            if (temp->status_code != BGI_OK) {
                bgi_free(bi1_copy);
                bgi_free(result);
                return temp;
            }
            list_reverse(temp->numeric);
            if  (temp->numeric->status_code != LIST_OK) {
                temp->status_code = BGI_NUMERIC_FAIL;
                bgi_free(bi1_copy);
                bgi_free(result);
                return temp;
            }
            int8 multiplier = list_get(lists[li], list_len(lists[li])-i-1);
            if (bi2->decimal->status_code != LIST_OK) {
                bgi_free(bi1_copy);
                bgi_free(temp);
                bgi_free(result);
                return NULL;
            }

            for (size_t j = 0; j < list_len(temp->numeric); j++) {
                int8 n1 = list_get(temp->numeric, j);
                if (temp->numeric->status_code != LIST_OK) {
                    temp->status_code = BGI_NUMERIC_FAIL;
                    bgi_free(bi1_copy);
                    bgi_free(result);
                    return temp;
                }

                int8 value = carrier + (n1 * multiplier);
                carrier = value/10;

                list_set(temp->numeric, j, value%10);
                if (temp->numeric->status_code != LIST_OK) {
                    temp->status_code = BGI_NUMERIC_FAIL;
                    bgi_free(bi1_copy);
                    bgi_free(result);
                    return temp;
                }
            }
            if (carrier != 0) {
                list_append(temp->numeric, carrier);
                if (temp->numeric->status_code != LIST_OK) {
                    temp->status_code = BGI_NUMERIC_FAIL;
                    bgi_free(result);
                    bgi_free(bi1_copy);
                    return temp;
                }
                carrier = 0;
            }

            list_reverse(temp->numeric);
            if (temp->numeric->status_code != LIST_OK) {
                bgi_free(bi1_copy);
                bgi_free(result);
                return temp;
            }

            size_t limit = i;
            if (li == 1 && list_len(bi2->decimal) > 0) {
                limit = i + list_len(bi2->decimal);
            }
            for (size_t j = 0; j < limit; j++) {
                list_append(temp->numeric, 0);
                if (temp->numeric->status_code != LIST_OK) {
                    bgi_free(bi1_copy);
                    bgi_free(result);
                    return temp;
                }
            }

            BigInt* new_result = bgi_add(result, temp);
            if (new_result == NULL) {
                bgi_free(bi1_copy);
                bgi_free(result);
                bgi_free(temp);
                return NULL;
            }

            bgi_free(result);
            bgi_free(temp);
            result = new_result;
        }
    }

    // set correct numeric and decimal values 
    BigInt *bi3 = bgi_init("0");
    if (bi3 == NULL) {
        bgi_free(bi1_copy);
        bgi_free(result);
        return NULL;
    }

    if (bi3->status_code != BGI_OK) {
        bgi_free(bi1_copy);
        bgi_free(result);
        return bi3;
    }

    // set numeric digits
    if (list_len(result->numeric) >= decimal_points) {
        for (size_t i = 0; i < list_len(result->numeric) - decimal_points; i++) {
            int8 value = list_get(result->numeric, i);
            if (result->numeric->status_code != LIST_OK) {
                result->status_code = BGI_NUMERIC_FAIL;
                bgi_free(bi3);
                bgi_free(bi1_copy);
                return result;
            }
            list_append(bi3->numeric, value);
            if (bi3->numeric->status_code != LIST_OK) {
                bi3->status_code = BGI_NUMERIC_FAIL;
                bgi_free(result);
                bgi_free(bi1_copy);
                return bi3;
            }
        }

        // set decimal digits
        for (size_t i = list_len(result->numeric) - decimal_points; i < list_len(result->numeric); i++) {
            int8 value = list_get(result->numeric, i);
            if (result->numeric->status_code != LIST_OK) {
                result->status_code = BGI_NUMERIC_FAIL;
                bgi_free(bi3);
                bgi_free(bi1_copy);
                return result;
            }
            list_append(bi3->decimal, value);
            if (bi3->numeric->status_code != LIST_OK) {
                bi3->status_code = BGI_DECIMAL_FAIL;
                bgi_free(result);
                bgi_free(bi1_copy);
                return bi3;
            }
        }

        // remove unnecessary zeros from end of the decimal digits
        int first_non_zero_index = -1;
        for (int i = list_len(bi3->decimal)-1; i >= 0; i--) {
            int8 value = list_get(bi3->decimal, i);
            if (bi3->decimal->status_code != LIST_OK) {
                bi3->status_code = BGI_DECIMAL_FAIL;
                bgi_free(result);
                bgi_free(bi1_copy);
                return bi3;
            }
            if (value != 0) {
                first_non_zero_index = i;
                break;
            }
        }

        if (first_non_zero_index != -1) {
            bi3->decimal->index = first_non_zero_index+1;
        } else {
            free(bi3->decimal);
            bi3->decimal = list_init();
            if (bi3->decimal == NULL) {
                bgi_free(result);
                bgi_free(bi1_copy);
                bgi_free(bi3);
                return NULL;
            }

            if (bi3->decimal->status_code != LIST_OK) {
                bi3->status_code = BGI_DECIMAL_FAIL;
                bgi_free(bi1_copy);
                bgi_free(result);
                return bi3;
            }
        }
    } else {
        for (size_t i = 0; i < decimal_points-list_len(result->numeric); i++) {
            list_append(bi3->decimal, 0);
            if (bi3->decimal->status_code != LIST_OK) {
                bi3->status_code = BGI_DECIMAL_FAIL;
                bgi_free(result);
                bgi_free(bi1_copy);
                return bi3;
            }
        }

        for (size_t i = 0; i < list_len(result->numeric); i++) {
            int8 value = list_get(result->numeric, i);
            if (result->numeric->status_code != LIST_OK) {
                result->status_code = BGI_NUMERIC_FAIL;
                bgi_free(bi1_copy);
                bgi_free(bi3);
                return result;
            }
            list_append(bi3->decimal, value);
            if (bi3->decimal->status_code != LIST_OK) {
                bi3->status_code = BGI_DECIMAL_FAIL;
                bgi_free(result);
                bgi_free(bi1_copy);
                return bi3;
            }
        }
    }

    // set the sign
    bi3->sign = true;
    if (bi1->sign != bi2->sign) {
        bi3->sign = false;
    }

    bgi_free(result);
    bgi_free(bi1_copy);
    return bi3;
}

void bgi_free(BigInt *bi) {
    if (bi == NULL) return;
    if (bi->numeric) {
        list_free(bi->numeric);
    }
    if (bi->decimal) {
        list_free(bi->decimal);
    }
    free(bi);
}

#endif
