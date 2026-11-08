#include "ram.h"
#include <stdio.h>
#include <stdlib.h>
typedef struct ram_context{
    double ttlkbph;
    double fkbph;
    double avail;
    int init;
    int n;
    int i; 
}rctx;
// int getpcntg(DWORDLONG *pcn,DWORDLONG* ttlkbp,DWORDLONG* fkbp){
//     MEMORYSTATUSEX statex;
//     statex.dwLength = sizeof(statex);
//     if(!GlobalMemoryStatusEx(&statex)){
//         return -1;
//     }
//     *pcn=statex.dwMemoryLoad;
//     *ttlkbp=statex.ullTotalPhys;
//     *fkbp=statex.ullAvailPhys;
//     return 1;
// }
int getpcnt(double *totalMemory,double *freekb,double *availb){
    FILE *stat = fopen("/proc/meminfo","r");
    if(!stat){
        return 0;
    }
    char buff [256];
    ull ttl,fr,avlb;
    while(fgets(buff, sizeof(buff), stat))
    {
        if(sscanf(buff, "MemTotal: %llu kB", &ttl) == 1)
        {
            *totalMemory = ttl/1024.0;
        }
        if(sscanf(buff,"MemFree: %llu kB",&fr)==1){
           *freekb = fr/1024.0; 
        }
        if(sscanf(buff,"MemAvailable: %llu kB",&avlb)==1){
            *availb = avlb/1024.0; 
        }
    }
    return 1;
}
rctx* ram_create(void){
    rctx* ct = malloc(sizeof(*ct));
    if(!ct){
        return NULL;
    }
    ct->init = 0; 
    return ct;          
}

int ram_init(rctx* ct, int n){
    if(!ct || n<0){
        return 0;
    }
    if(!getpcnt(&ct->ttlkbph,&ct->fkbph,&ct->avail)){
        return 0; 
    }
    ct->init = 1;
    ct->n = n;
    ct->i = 0; 
    return 1;
}
void ram_destroy(rctx* ct){
    if(ct){
        free(ct);
    }
}
int ram_update(void* ct1, void* out1){ 
    if(!(rctx*)ct1 || !((rctx*)ct1)->init){
        printf("rct problem \n");
        return 0;
    }
    rctx* ct = (rctx*) ct1;
    if(!(outr*)out1){
        printf("outr problem \n");
        return 0;
    }
    outr* out = (outr*) out1;
    double ttlkb;
    double fr;
    double av;
    if(!getpcnt(&ttlkb,&fr,&av)){
        printf("getpcnt ram problem \n");
        return 0;
    }
    if(!ttlkb){
        printf("ttlkb problem \n");
        return 0; 
    }
    out->ttlkbph = ttlkb;
    out->fkbph=fr;
    out->avl=av;
    
    ct->ttlkbph = ttlkb;
    ct->fkbph= fr;
    ct->avail =av;
    // (ct->i)++;
    // if(ct->i==ct->n){
    //     return 2;
    // }
    return 1;
} 
