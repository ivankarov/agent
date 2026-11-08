// #include "cpu.h"
// #include "loop.h"
// #include <windows.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include "SPipe.h"
// #include "ram.h"
// #define BUFSIZE 512
// extern i;

// int main (void){
//     printf("hello from agent \n");
//     //цп инициализация
//     ctx* ct = cpu_create(); 
//     if(!ct){
//         printf("cpu_create bug \n");
//         return -1;
//     }
//     if(!cpu_init(ct,10)){
//         printf("cpu_init bug \n");
//         cpu_destroy(ct);
//         return -1;
//     }
//     double x = 0.0;
//     double* m = &x;
//     int ms = 1000;


//     //озу инициализация
//     rctx* rct = ram_create(); 
//     if(!rct){
//         printf("ram_create bug \n");
//         return -1;
//     }
//     if(!ram_init(rct,10)){
//         printf("ram_init bug \n");
//         ram_destroy(rct);
//         cpu_destroy(ct);
//         return -1;
//     }

//     // выходной параметр ram_update();
//     outr* rout = malloc(sizeof(*rout));
//     if(!rout){
//         printf("thrPrms alloc bug\n");
//         cpu_destroy(ct);
//         ram_destroy(rct);
//         return -1;
//     }


//         // DWORD dwThreadIdc = 0; //--------------------- иммитация клиента----------------------------
//         // HANDLE hThreadc = NULL;
//         // thrPrms* cpuRWPrmsc = malloc(sizeof(thrPrms));
//         // if(!cpuRWPrmsc){
//         //     printf("thrPrmsc alloc bug\n");
//         //     cpu_destroy(ct);
//         //     return -1;
//         // }
//         // TCHAR*  chBufout[BUFSIZE];
//         // cpuRWPrmsc->chb = chBufout;
//         // hThreadc = CreateThread(NULL,0,GetClientPipe,(LPVOID) cpuRWPrmsc,0,&dwThreadIdc);
//         // if(!hThreadc){
//         //     printf("htrheadc не открылся \n ");
//         //     free(cpuRWPrmsc);
//         //     cpu_destroy(ct);
//         //     return -1;
//         // }                       ---------------------------------------------------------------------
     
    

//     //структура с параметрами функции записи метрик цп в пайп, передаваемая в поток
//     thrPrms* cpuRWPrms = malloc(sizeof(*cpuRWPrms));
//     if(!cpuRWPrms){
//         printf("thrPrms alloc bug\n");
//         cpu_destroy(ct);
//         ram_destroy(rct);
//         free(rout);
//         // free(cpuRWPrmsc); клиент
//         return -1;
//     }
//     cpuRWPrms->ct = ct;
//     cpuRWPrms->f = cpu_update;
//     cpuRWPrms->m = (void*) m;
//     cpuRWPrms->ms = ms;

    
//     //структура с параметрами функции записи метрик озу в пайп, передаваемая в поток
//     thrPrms* ramRWPrms = malloc(sizeof(*ramRWPrms));
//     if(!ramRWPrms){
//         printf("thrPrms alloc bug\n");
//         cpu_destroy(ct);
//         ram_destroy(rct);
//         free(cpuRWPrms);
//         free(rout);
//         return -1;
//     }
    
//     ramRWPrms->ct = rct;
//     ramRWPrms->f = ram_update;
//     ramRWPrms->out = rout;

//     //структура из двух структур-параметров (цп,озу),передается в поток
//     bt* btprm = malloc(sizeof(*btprm));
//     if(!btprm){
//         printf("bt alloc bug \n");
//         cpu_destroy(ct);
//         ram_destroy(rct);
//         free(rout);
//         free(cpuRWPrms);
//         free(ramRWPrms);
//         return -1;
//     }
//     btprm->tp1 = cpuRWPrms;
//     btprm->tp2= ramRWPrms;


//     DWORD  dwThreadId = 0;
//     HANDLE hThread = NULL;
//     HANDLE hPipe = INVALID_HANDLE_VALUE;
//     BOOL isCnctd = FALSE; 
//     for(;;){
//     GetServPipeAndCnct(&hPipe,&isCnctd);
//     if(!hPipe){
//         printf("hPipe is empty \n");
//         cpu_destroy(ct);
//         ram_destroy(rct);
//         free(rout);
//         free(cpuRWPrms);
//         free(ramRWPrms);
//         free(btprm);
//         return -1;
//     }
//     btprm->tp1->hPipe = hPipe; 
//     if (isCnctd) { 
//          printf("Client connected, creating a processing thread.\n"); 
//          hThread = CreateThread( 
//             NULL,               
//             0,                 
//             RWServPipe,    
//             (LPVOID) btprm,   
//             0,                 
//             &dwThreadId);   
//          if(i){
//             printf("i = 1, returning \n");
//             return 1;
//          }
//          if (hThread == NULL) 
//          {
//             printf(TEXT("CreateThread failed, GLE=%d.\n"), GetLastError()); 
//             cpu_destroy(ct);
//             ram_destroy(rct);
//             free(rout);
//             free(cpuRWPrms);
//             free(ramRWPrms);
//             free(btprm);
//             return -1;
//          }
//          else{
//             CloseHandle(hThread); 
//          } 
//        } 
//         else{
//             CloseHandle(hPipe); 
//         }
//     }
//     cpu_destroy(ct);
//     ram_destroy(rct);
//     free(rout);
//     free(cpuRWPrms);
//     free(ramRWPrms);
//     free(btprm);
//     return 0;
// }





//UNIX
#include <syslog.h>
#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>
#include "UniPipes.h"
#include "ram.h"
#include <pthread.h>
#include "loop.h"
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

const char interp_section[] __attribute__((section(".interp")))  = "/usr/lib/ld-linux-x86-64.so.2";
#ifdef __GLIBC__
extern const int _IO_stdin_used;
const int _IO_stdin_used __attribute__((weak)) = 131073;
#endif

__attribute__((force_align_arg_pointer))
int main (void){
    syslog(LOG_NOTICE,"hello from agent \n");
    //цп инициализация
    ctx* ct = cpu_create(); 
    if(!ct){
        syslog(LOG_NOTICE,"cpu_create bug \n");
        return -1;
    }
    if(!cpu_init(ct,10)){
        syslog(LOG_NOTICE,"cpu_init bug \n");
        cpu_destroy(ct);
        return -1;
    }
    double x = 0.0;
    double* m = &x;
    int ms = 1000;
    //озу инициализация
    rctx* rct = ram_create(); 
    if(!rct){
        syslog(LOG_NOTICE,"ram_create bug \n");
        cpu_destroy(ct);
        return -1;
    }
    if(!ram_init(rct,10)){
        syslog(LOG_NOTICE,"ram_init bug \n");
        ram_destroy(rct);
        cpu_destroy(ct);
        return -1;
    }
    // выходной параметр ram_update();
    outr* rout = malloc(sizeof(*rout));
    if(!rout){
        syslog(LOG_NOTICE,"thrPrms alloc bug\n");
        cpu_destroy(ct);
        ram_destroy(rct);
        return -1;
    }
    //структура с параметрами функции записи метрик цп в пайп, передаваемая в поток
    thrPrms* cpuRWPrms = malloc(sizeof(*cpuRWPrms));
    if(!cpuRWPrms){
        syslog(LOG_NOTICE,"thrPrms alloc bug\n");
        cpu_destroy(ct);
        ram_destroy(rct);
        free(rout);
        // free(cpuRWPrmsc); клиент
        return -1;
    }
    cpuRWPrms->ct = ct;
    cpuRWPrms->f = cpu_update;
    cpuRWPrms->m = (void*) m;
    //структура с параметрами функции записи метрик озу в пайп, передаваемая в поток
    thrPrms* ramRWPrms = malloc(sizeof(*ramRWPrms));
    if(!ramRWPrms){
        syslog(LOG_NOTICE,"thrPrms alloc bug\n");
        cpu_destroy(ct);
        ram_destroy(rct);
        free(cpuRWPrms);
        free(rout);
        return -1;
    }
    ramRWPrms->ct = rct;
    ramRWPrms->f = ram_update;
    ramRWPrms->out = rout;
    //структура из двух структур-параметров (цп,озу),передается в поток
    bt* btprm = malloc(sizeof(*btprm));
    if(!btprm){
        syslog(LOG_NOTICE,"bt alloc bug \n");
        cpu_destroy(ct);
        ram_destroy(rct);
        free(rout);
        free(cpuRWPrms);
        free(ramRWPrms);
        return -1;
    }
    btprm->tp1 = cpuRWPrms;
    btprm->tp2= ramRWPrms;

    openlog("main", LOG_PID, LOG_DAEMON);
    syslog(LOG_NOTICE,"before getpipe()");
    //подготовка pipe line
    int getsrv = getpipe();
    if(!getsrv){
        syslog(LOG_NOTICE,"getting srv problem\n");
        cpu_destroy(ct);
        ram_destroy(rct);
        free(rout);
        free(cpuRWPrms);
        free(ramRWPrms);
        free(btprm);
        unlinks();
        return -1;
    }
    extern pthread_mutex_t mutex;
    pthread_mutex_init(&mutex,NULL);
    syslog(LOG_NOTICE,"before srvopening()");
    int srv = srvopening();
    syslog(LOG_NOTICE,"srv - %d",srv);
    if(srv<0){
        syslog(LOG_NOTICE,"srv failed \n");
        unlinks();
        // close(cln);
        cpu_destroy(ct);
        ram_destroy(rct);
        free(rout);
        free(cpuRWPrms);
        free(ramRWPrms);
        free(btprm);
        unlinks();
        pthread_mutex_destroy(&mutex);
        return -1;
    }
    syslog(LOG_NOTICE,"before using");
    int fdsm = open("/mnt/disk_d/agentUni/data.bin",O_RDWR,0);
    syslog(LOG_NOTICE,"main fdsm - %d",fdsm);
    if(fdsm>0){
        setsrv(btprm,srv,fdsm);
        /*больше клиентов*/int lop = loop(usesrvpipe,btprm,srv,1,fdsm);// параллельно проверять zatichka ? useclient() : continue; 
        close(fdsm);
    }
    else{
        syslog(LOG_NOTICE,"fdsm fuck");
        syslog(LOG_NOTICE,"errno - %d",errno);
    }
    unlinks();
    cpu_destroy(ct);
    ram_destroy(rct);
    free(rout);
    free(cpuRWPrms);
    free(ramRWPrms);
    free(btprm);
    pthread_mutex_destroy(&mutex);
    closelog();
    return 1;
}

