#ifndef UNIPIPES_H
#define UNIPIPES_H
#include "ram.h"
#include <pthread.h>
typedef int (*func) (void* ct, void* m);
typedef struct tp{
    outr* out;
    func f;
    void* ct;
    void* m;
}thrPrms;
typedef struct bigtp{
    thrPrms* tp1;
    thrPrms* tp2;
}bt;
int getpipe();
int srvopening();
int zatichopening();
int usesrvpipe(void* arg,int fd);
int usezatichka(int fd);
void unlinks();
void setsrv(void* btprm,int srv, int fdsm);
// int publicfd[10];
extern pthread_mutex_t mutex; 
#endif