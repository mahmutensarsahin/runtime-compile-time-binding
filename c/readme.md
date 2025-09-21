Şimdi de `Derived` (mesela `Dog` ve `Cat`) yapıp C’de **manuel vtable** ile gösteriyoruz. Hem stack hem heap örnekleri ile, atamalar değiştikçe `b`'nin hangi `foo`'u çağırdığı adım adım anlattım.*vtable sabit, vptr nesnede yazılır; hangi nesneye işaret ettiği runtime’da belli olur.*

# Kod — iki derived (Dog, Cat)

```c
#include <stdio.h>
#include <stdlib.h>

/* VTable tipi (her virtual fonksiyon için pointer) */
typedef struct BaseVTable {
    void (*foo)(void *self);
} BaseVTable;

/* Base "sınıfı" */
typedef struct Base {
    BaseVTable *vptr;   /* manuel vptr (polymorphic için) */
} Base;

/* Dog (Derived1) */
typedef struct Dog {
    Base base;  /* vptr burada (ilk üye) */
    int dog_data;
} Dog;

/* Cat (Derived2) */
typedef struct Cat {
    Base base;
    int cat_data;
} Cat;

/* Fonksiyon implementasyonları */
void Base_foo(void *self) { puts("Base::foo"); }
void Dog_foo(void *self)  { puts("Dog::foo - hav!"); }
void Cat_foo(void *self)  { puts("Cat::foo - miyav!"); }

/* Vtable'lar (static, programın veri segmentinde) */
BaseVTable base_vtable  = { Base_foo };
BaseVTable dog_vtable   = { Dog_foo };
BaseVTable cat_vtable   = { Cat_foo };

/* "Constructor" benzeri init fonksiyonları (vptr ayarı burada yapılır) */
void Dog_init(Dog *d) {
    d->base.vptr = &dog_vtable;   /* runtime'da nesneye vptr yazılır */
    d->dog_data = 1;
}
void Cat_init(Cat *c) {
    c->base.vptr = &cat_vtable;
    c->cat_data = 2;
}

/* Çağrı helper: Base* üzerinden foo çağrısı (tam C++ davranışı) */
void call_foo(Base *b) {
    b->vptr->foo(b);   /* indirect call: runtime dispatch */
}

int main(void) {
    /* --- Stack örneği --- */
    Dog dstack;
    Dog_init(&dstack);         /* dstack.vptr -> dog_vtable  (runtime) */
    Base *b = (Base*)&dstack;  /* atama (runtime): b artık dstack'i gösteriyor */
    call_foo(b);               /* Dog::foo çağrılır */

    /* --- Heap örneği --- */
    Cat *che = malloc(sizeof *che);
    Cat_init(che);             /* che->vptr -> cat_vtable */
    b = (Base*)che;            /* b artık heap'teki cat'i gösteriyor (runtime) */
    call_foo(b);               /* Cat::foo çağrılır */

    /* --- Dinamik atama: aynı b farklı nesneleri gösterebilir --- */
    b = (Base*)&dstack;        /* tekrar dstack'e döndü */
    call_foo(b);               /* Dog::foo */

    /* heap'ı başka bir Dog ile değiştir */
    Dog *dheap = malloc(sizeof *dheap);
    Dog_init(dheap);           /* dheap->vptr -> dog_vtable */
    b = (Base*)dheap;          /* b şimdi heap'teki Dog'a işaret ediyor */
    call_foo(b);               /* Dog::foo */

    free(che);
    free(dheap);
    return 0;
}
```

# Kısa, net adım adım

1. `dog_vtable` ve `cat_vtable` **derleme/link zamanında** hazırlanır (programın veri bölümünde sabit tablolar).
2. `Dog_init(&dstack)` çalışınca `dstack` içindeki `vptr` alanına `&dog_vtable` **yazılır** — bu yazma **runtime**'da olur (constructor-benzeri init çağrısı sırasında).
3. `Base *b = (Base*)&dstack;` — bu atama da program çalışırken olur; derleyici *b'nin tipinin* `Base*` olduğunu bilir ama `b`'ye hangi adresin geleceğini derleme zamanında **bilemez**.
4. `call_foo(b)` içinde: kod `b->vptr` okur, o vptr'ın gösterdiği vtable'daki `foo` entry'sini alır ve `call *func_ptr` ile o fonksiyonu çağırır. Çünkü vptr `dog_vtable`'ı gösteriyorsa `Dog_foo`, `cat_vtable`'ı gösteriyorsa `Cat_foo` çalışır.
5. Önemli: aynı `b` değişkeni farklı zamanlarda farklı nesneleri gösterebilir (stack'teki Dog, heap'teki Cat, heap'teki Dog vs.). Hangi `foo` çağrılacağı sadece **çağrı anında** belli olur — işte runtime binding.

# Hafıza şeması (basit)

```
DATA SEGMENT (static vtables)
0x5000: dog_vtable -> [ &Dog_foo ]
0x6000: cat_vtable -> [ &Cat_foo ]

STACK
0x1000: dstack (Dog)
       [ vptr -> 0x5000 ]   <- Dog_init yazdı
       [ dog_data = 1 ]

HEAP
0x2000: che (Cat)
       [ vptr -> 0x6000 ]   <- Cat_init yazdı
       [ cat_data = 2 ]
0x3000: dheap (Dog)
       [ vptr -> 0x5000 ]
       [ dog_data = 1 ]
```

Çağrı akışı (ör. `call_foo(b)`):

* `b` -> 0x1000 (dstack)
* oku: vptr = \[0x1000] = 0x5000
* oku: func\_ptr = \[0x5000 + offset\_foo] = \&Dog\_foo
* `call func_ptr` → Dog\_foo çalışır.

# Özet

* `vtable` sabit (programın veri bölgesinde) — derleyici/linker oluşturur.
* `vptr` nesnenin içinde **yazılır** (init/constructor sırasında) — bu yazma runtime’dır.
* `Base *b`’nin tipi derleyici için önemlidir; ama `b`'nin hangi nesneyi gösterdiği **runtime**'da belli olur. Bu yüzden çağrı `b->foo()` **çağrı anında** (vptr→vtable üzerinden) hangi fonksiyonu çalıştıracağına karar verir: *runtime binding*.
* Aynı `b`'yi farklı nesnelere işaret edecek şekilde değiştirebilirsin; çağrılar her seferinde o an `b`'nin gösterdiği nesneye göre davranır.

# Son olarak
* fptr.c ve test.c dosyalarımızda fonksiyon pointer kullanımı ile ilgili örneklere göz atabilirsiniz
