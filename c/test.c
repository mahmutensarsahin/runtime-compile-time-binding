#include <stdio.h>

/* Tip tanımları */
typedef int (*high_order_fn)(int (*cb)(int, int (*)(int)), int, int (*)(int));
typedef int (*func_with_callback)(int, int (*)(int));
typedef int (*unary_fn)(int);

/* Fonksiyon bildirimleri */
int f1(int (*t)(int, int (*)(int)), int a, int (*x)(int));
int f2(int (*t)(int, int (*)(int)), int a, int (*x)(int));
int karele(int a);
int func(int a, int (*x)(int));
int k(int x(int, int), int a, int b);
int topla(int a, int b);

/* Fonksiyon tanımları */
int f1(int (*t)(int, int (*)(int)), int a, int (*x)(int)) {
    return t(a, x);
}

int f2(int (*t)(int, int (*)(int)), int a, int (*x)(int)) {
    return t(a, x);
}

int karele(int a) {
    return a * a;
}

int func(int a, int (*x)(int)) {
    return x(a);
}

int k(int x(int, int), int a, int b) {
    return x(a, b);
}

int topla(int a, int b) {
    return a + b;
}

int main(void) {
    high_order_fn function = f1;        /* f1 veya f2 atanabilir */
    func_with_callback function1 = func;
    unary_fn function2 = karele;

    printf("%d", function(function1, 13, function2)); /* func(13, karele) -> 169 */
    printf(" %d", k(topla, 6, 4));                    /* topla(6,4) -> 10 */

    return 0;
}
