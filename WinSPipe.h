#ifndef SPIPE_H
#define SPIPE_H
#include <windows.h>
#include "ram.h"
typedef int (*func) (void* ct, void* m);
typedef struct tp{
    outr* out;
    HANDLE hPipe;
    func f;
    void* ct;
    void* m;
    int ms;
}thrPrms;
typedef struct bigtp{
    thrPrms* tp1;
    thrPrms* tp2;
}bt;
typedef struct prms{
    TCHAR* pbuff;
    BOOL flag;
    DWORD cbr;
    HANDLE hpipe;
}Rprms;
int GetServPipeAndCnct(HANDLE *hpipeOut, BOOL* fcnctdOut);
int WINAPI RWServPipe(LPVOID lpvParam);
// int GetClientPipe(LPVOID lp);
#endif 