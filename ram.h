#ifndef RAM_H
#define RAM_H
typedef struct ram_context rctx;
typedef unsigned long int uli;
typedef struct ram_contextout{
    double ttlkbph;
    double fkbph;
    double avl;
}outr;
typedef unsigned long long ull;
rctx* ram_create(void);
int ram_init(rctx* ct, int n);
void ram_destroy(rctx* ct);
int ram_update(void* ct1, void* out1);
#endif