#include <stdio.h>

/* Tip tanımları */
typedef int (*binop)(int,int);   /* iki-operandlı fonksiyon pointer'ı */
typedef int (*unop)(int);        /* tek-operandlı fonksiyon pointer'ı */

/* Basit fonksiyonlar */
int add(int a,int b)   { return a + b; }
int mul(int a,int b)   { return a * b; }
int sub(int a,int b)   { return a - b; }
int square(int x)      { return x * x; }
int negate(int x)      { return -x; }

/* Fonksiyon pointer'ı parametre olarak alma */
int apply_binop(binop op, int a, int b) {
    return op(a,b); /* indirect call: hangi fonksiyon olduğu runtime'da belli */
}

/* Fonksiyon pointer döndüren seçici (runtime karar) */
binop choose_binop(char token) {
    if (token == '+') return add;
    if (token == '*') return mul;
    return sub;
}

/* Fonksiyon pointer dizisi (manuel "table") */
unop ops[] = { square, negate };

int main(void) {
    /* Doğrudan atama ve çağrı */
    binop op = add;                 /* f = add */
    printf("add(3,4) = %d\n", op(3,4));

    /* Runtime'da seçme + parametre ile çağırma */
    op = choose_binop('*');         /* runtime: hangi op seçileceği girdiye bağlı */
    printf("choose '*' -> %d\n", apply_binop(op, 3, 4));

    /* Fonksiyon pointer dizisinden alma ve çağırma */
    unop u = ops[0];                /* ops[0] -> square */
    printf("square(5) = %d\n", u(5));

    /* Dizinin elemanını runtime'da değiştirmek mümkün: */
    ops[1] = square;                /* daha önce negate idi, şimdi square */
    printf("ops = %d\n", ops); /* şimdi 25 */

    return 0;
}
