#define _GNU_SOURCE 
#include "loop.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include <syslog.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <ucontext.h>

static int sig = 0;
static pid_t clpid = 0;
static void handler(int signo, siginfo_t *info, void *context){
        clpid = info->si_pid;
        syslog(LOG_NOTICE,"SIGNAAAAAAAAAAAL!! client pid - %d, server pid - %d, sig++ \n",info->si_pid,getpid());
        //return context->var++ //залезание в стек и регистры времени когда был получен сигнал
}
// extern shmfuncs(int key?);// только clnt загружает и пользуется по ключу  !!
// /*??*/ extern /*чтобы было при dlsym*/ void *findcmpfd;
extern __attribute__((visibility("default")))
int loop(func2 f2, void* arg,int fds,int s,int fdsm){
    openlog("loop", LOG_PID, LOG_DAEMON);
    struct sigaction act ={};
    act.sa_sigaction = &handler;
    act.sa_flags = SA_SIGINFO;
    if (sigaction(SIGCONT, &act, NULL) == -1) {
        perror("sigaction");
        return 0;
    }
    int ccode = 1;
    int sigcpy;
    int ft = ftruncate(fdsm, sizeof(int));
    syslog(LOG_NOTICE,"after ftruncate fdsm - %d",fdsm);
    if(ft!=-1){
        key_t key = ftok("/", 'e');
        if(key!=-1){
            int shmid = shmget(key,1024,IPC_CREAT|0666);
            if(shmid!=-1){
                int * dataptr = (int*)shmat(shmid,NULL,0);
                //if(fdsm){
                    // struct stat st;
                    // if(fstat(fdsm,&st)==0){
                    //     size_t fsize = (size_t)st.st_size;
                    //     unsigned char* dataptr =(unsigned char*) mmap(NULL,fsize,PROT_READ|PROT_WRITE,MAP_SHARED,fdsm,0);
                        if(dataptr){
                            // findcmpfd = *dataptr; //                    ПОЛУЧАЕМ ФУНКЦИОНАЛ КЛИЕНТСКОЙ ЛУПЫ!!!!!!!!!!!!
                            while(ccode){
                            sigcpy = sig;
                            sleep(s);
                            syslog(LOG_NOTICE,"srv pid - %d\n",getpid());
                            syslog(LOG_NOTICE,"fd /tmp/my_n_pipe - %d\n",fds);
                            ccode = f2(arg,fds);
                            while(sigcpy==sig){
                                //на клиенте на этом месте открывается хелпа
                                // че то делать если нет сигнала, помогать клиенту открыться например !
                                // syslog(LOG_NOTICE,"wasnt signal \n");
                            }
                            syslog(LOG_NOTICE , "was signal");
                            //kill(clpid,SIGKILL);  //ответный сигнал   

                                    // *((int*)dataptr) = 1;
                                    // syslog(LOG_NOTICE,"carapka");
            
                            
                            // SIG_DFL; !
                            // shared memory - waituntilreaded();  !
                            // shmget() shmat() !
                            // по ip портам
                            
                            
                            *(dataptr)=1;
                            syslog(LOG_NOTICE,"carapka");
                            // чекать и реагировать на файловую систему - имена файлов/дир

                            // чекать данные о процессе, по сути углуб. реагирование на файловую систему
                            // ==      
                            // можно изменять имя или другой параметр процесса в качестве сигнала серверу
                            }
                            shmdt(dataptr);
                        }
                        shmctl(shmid, IPC_RMID, NULL);
                        //munmap(dataptr,fsize);
                    //}
                //}
            }
        }
    }syslog(LOG_NOTICE,"errno - %d", errno);
    syslog(LOG_NOTICE,"ussrv - 0 from loop ");
    closelog();
    return 1;
}








