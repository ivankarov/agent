#ifndef CPU_H
#define CPU_H
typedef struct cpu_context ctx; 
ctx* cpu_create(void);
int cpu_init(ctx* ct,int n);
void cpu_destroy(ctx*ct);
int cpu_update(void*ct, void* out);
#endif