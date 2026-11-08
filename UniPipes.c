#define _GNU_SOURCE
#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "UniPipes.h"
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "ram.h"
#include <syslog.h>
#include <poll.h>
#include <unistd.h>
#include <sys/types.h>

typedef struct crouts{
    double cpout;
    double ttlkbph;
    double fkbph;
    double avl;
}outs; 

char *pipe1 = "/tmp/my_n_pipe";

int thisfdsm;
void* thisbtprm;
int thissrv;
extern __attribute__((visibility("default"))) getloop(){
    loop(usesrvpipe,thisbtprm,thissrv,1,thisfdsm); 
}
void setsrv(void* btprm,int srv,int fdsm){
    thisbtprm = btprm;
    thissrv = srv;
    thisfdsm = fdsm;
}
extern __attribute__((visibility("default"))) extrawrite(){
    write(thissrv,1,1);
}
extern __attribute__((visibility("hidden")))
int getpipe(){
    if(mkfifo(pipe1,0666)<0){
        if(errno==EEXIST){
            syslog(LOG_NOTICE,"eexists \n");
            errno=0;
            unlink(pipe1);
            if(mkfifo(pipe1,0666)<0){
                syslog(LOG_NOTICE,"second mkfifo < 0 \n");
                return 0;
            }
            syslog(LOG_NOTICE,"errno after unlink - %d\n",errno);
        }
    }
    syslog(LOG_NOTICE,"serv ok \n");
    return 1;   
}
// int count=0;
// void pipefabric(){
//     //одно и то же ли имя? генерить новый пайп?
//     count++;
//     if(count>10){
//         return;
//     }
//     publicfd[count]= open(O_WRONLY);
//     pipefabric();
// }
int stopclock;
pthread_mutex_t mutex;
int zatfd = 0;

extern __attribute__((visibility("default"))) void* tryopening (void* arg){
    syslog(LOG_NOTICE,"tryopening hello");
    int * fdo = (int *) arg;
    int fd = open(pipe1,O_WRONLY);
    pthread_mutex_lock(&mutex);
    syslog(LOG_NOTICE,"stoplock = 1");
    stopclock = 1;
    *fdo = fd;
    syslog(LOG_NOTICE,"fd - %d",*fdo);
    pthread_mutex_unlock(&mutex);

    if(zatfd>0){
        sleep(3);
        usezatichka(zatfd);
    }
    
    if(fd<0){
        //         В СЛУЧАЕ  ПРОВАЛА ФД ПОМОЧЬ ДЕФОЛТ КЛИЕНТУ ОТКРЫТЬСЯ   !!!!!
        syslog(LOG_NOTICE,"tryopening code - %d \n ",fd); 
        perror("open");
    }
    syslog(LOG_NOTICE,"fd - %d \n",fd);
}

extern __attribute__((visibility("default"))) int srvopening(){ 
    stopclock = 0;
    syslog(LOG_NOTICE,"from srvopening");
    pthread_t tid;
    int arg = 0;
    if(pthread_create(&tid,NULL,&tryopening,(void*)(&arg))==0){
        int i = 0;
        while(!stopclock){
            i++;
            syslog(LOG_NOTICE,"i = %d",i);
            if(i>1000){
                pthread_t helptid;
                int helparg = 2;
                execve("../helper/help","help",NULL);//вызов хелпы
                zatfd = zatichopening();//execl(./agentcl);
                if(zatfd>0){
                    syslog(LOG_NOTICE,"zatichka is opened");//в случае с сетью клиентов сканить систему и создавать затычки на простаивающие
                }
            }
        }
    }
    return arg;
}
int zatichopening(){
    int fd = open(pipe1,O_RDONLY);              
    if(fd<0){
        perror("open");
    }
    syslog(LOG_NOTICE,"zatichka opening fd - %d \n",fd);
    return fd;
}
int usezatichka(int fd){
    outs* outcr = malloc(sizeof(*outcr));
    if(outcr){
        syslog(LOG_NOTICE,"zatichka using \n");
        int readcode = read(fd,outcr,sizeof(*outcr));
        if(readcode<0){
            close(fd);
            syslog(LOG_NOTICE,"reading pipe error \n");
        }else{
            syslog(LOG_NOTICE,"reading cpu - %lf, reading ram total - %lf \n",outcr->cpout,outcr->ttlkbph);
        }
        free(outcr);
    }
    return 1;
}
int usesrvpipe(void* arg,int fd){
    bt* b = (bt*) (arg);
    //cpu arg decode
    func cpu = b->tp1->f;
    void* ct = b->tp1->ct;
    void* m = b->tp1->m; 
    //ram arg decode
    func ram = b->tp2->f;
    void* rct = b->tp2->ct;
    outr* rout = b->tp2->out; 
    
    syslog(LOG_NOTICE,"getting cpu \n");
    int ccode = cpu(ct,m);
    if(!ccode){
        syslog(LOG_NOTICE,"getting cpu failed \n");
        close(fd);
        return 0;
    }
    syslog(LOG_NOTICE,"getting ram\n");
    int rcode = ram(rct,(void*)rout);
    if(!rcode){
        syslog(LOG_NOTICE,"getting ram failed \n");
        close(fd);
        return 0;
    }
    //запись в pipe
    outs* outcr = malloc(sizeof(*outcr));
    if(!outcr){
        syslog(LOG_NOTICE,"getting outcr failed \n");
        close(fd);
        return 0;
    }
    outcr->cpout = *((double*)m);
    outcr->ttlkbph= rout->ttlkbph;
    outcr->avl= rout->avl;
    outcr->fkbph= rout->fkbph;
    // for(int i = 0;i<9;i++){
    //     writecode = write(publicfd[i],outcr,sizeof(*outcr));
    // }
    int writecode = write(fd,outcr,sizeof(*outcr));
    if(writecode <0){
        syslog(LOG_NOTICE,"writing in pipe error \n");
        perror("writting err");
        free(outcr);
        close(fd);
        return 0;
    }
    syslog(LOG_NOTICE,"writecode (written bytes) - %d \n",writecode);
    syslog(LOG_NOTICE,"written outcr->cpout - %lf, outcr->ttlkbph - %lf , outcr->fkbph - %lf, outcr->avl - %lf\n",
        outcr->cpout,outcr->ttlkbph,outcr->fkbph,outcr->avl);
    syslog(LOG_NOTICE,"written cpout");
    free(outcr);
    return 1;
}
void unlinks(){
    unlink(pipe1);
}
