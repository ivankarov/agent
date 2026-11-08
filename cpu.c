#include "cpu.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
typedef unsigned long long ull;
typedef struct cpu_context{
    ull idl;
    ull krn;
    ull usr;
    int init;
    int n;
    int i; 
}ctx;

ctx* cpu_create(void){
    ctx* ct = malloc(sizeof(*ct));
    if(!ct){
        return NULL;
    }
    ct->init = 0; 
    return ct;          
}
void cpu_destroy(ctx* ct){
    if(ct){
        free(ct);
    }
}
int getProcStats( ull *system,ull *idle,ull *user_total){
    FILE *stat = fopen("/proc/stat","r");
    if(!stat){
        perror("fopen");
        return 0;
    }
    char line [256];
    ull user,nice;
    while (fgets(line, sizeof(line), stat)) {
        if (strncmp(line, "cpu ", 4) == 0) {
            sscanf(line, "cpu %llu %llu %llu %llu",
                   &user, &nice, system, idle);
            break;
        }
    }
    fclose(stat);

    *user_total = user + nice;
    return 1;
}
int cpu_init(ctx* ct, int n){
    if(!ct || n<0){
        return 0;
    }
    getProcStats(&ct->krn,&ct->idl,&ct->usr);
    if(ct->krn && ct->idl && ct->usr){
        ct->init = 1;
        ct->n = n;
        ct->i = 0; 
        return 1;
    }
    return 0; 
}
int cpu_update(void* ct1, void* out1){
    if(!(ctx*)ct1 || !((ctx*)ct1)->init){
        printf("ct problem \n");
        return 0;
    }
    ctx* ct = (ctx*) ct1;
    double* out = (double*) out1;    
    ull kr;
    ull id;
    ull us;
    sleep(1);// временно
    int code = getProcStats(&kr,&id,&us);
    if(!code){
        printf("getprocstats problem \n");
        return 0;
    }

    ull deltakrn = kr - ct->krn;
    ull deltaidl = id - ct->idl;
    ull deltausr = us - ct->usr;

    ull total = deltaidl+deltakrn+deltausr;
    ull busy = deltakrn+deltausr;
    if(total==0){
        printf("cpu total = 0 , kr - %llu,id - %llu , us - %llu, ct->krn - %llu , ct->idl - %llu, ct->usr - %llu \n",
            kr,id,us,ct->krn,ct->idl,ct->usr);
        return 0; 
    }
    if(out){
        *out = (100*(double)busy/(double)total);
    }
    ct->idl = id;
    ct->krn= kr;
    ct->usr = us;
    (ct->i)++;
    return 1;
}   