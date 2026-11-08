#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <dirent.h>
#include <sched.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <execinfo.h>
#include "uniservice.h"

void daemoninit(){
    pid_t pid;
    pid = fork();
    if (pid < 0) {
        exit(0);
    }
    if (pid > 0){
        exit(0);
    }
    if (setsid() < 0){
        exit(0);
    }
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    pid = fork();
    if (pid < 0){
        exit(0);
    }
    if (pid > 0){
       exit(0);
    }
    umask(0);
    chdir("/");
    int x;
    for (x = sysconf(_SC_OPEN_MAX); x>=0; x--)
    {
        close(x);
    }
    openlog("serv", LOG_PID, LOG_DAEMON);
}

int killalts(int a){
    // void* buffer[100];//творчество динамик функ
    // int nptrs = backtrace(buffer,100);
    // char** strings = backtrace_symbols(buffer,100);
    // for (size_t j = 0; j < nptrs; j++){
    //     printf("%s\n", strings[j]);
    // }
    // free(strings);

    
    char *proc = "/proc/";
    struct dirent* di;
    char path[256]; 
    FILE* cl;
    DIR *od = opendir(proc);
    if(od == NULL){
        perror("opendir");
        return 0;
    }
    while(1){
        di = readdir(od);
        if(di==NULL){
            closedir(od);
            perror("1st opening");
            return 0;
        }
        if(isdigit(di->d_name[0])){
            snprintf(path,sizeof(path),"/proc/%s/cmdline",di->d_name);
            cl = fopen(path,"r");
            if(!cl){
                closedir(od);
                return 0;
            }
            int pid = atoi(di->d_name);
            char res[256];
            while(fgets(res,50,cl)){
                printf("path - %s res %s \n", path,res);
                if((strcmp("./servUni",res)==0)||(strcmp("./agentcl",res)==0)){
                    if(a){
                        if(getpid()!=pid){
                            printf("popalsya pid- %d \n",pid);
                            if(kill(pid,SIGKILL)==0){
                                syslog(LOG_NOTICE,"is not suicide");
                            }
                        }
                    }else{
                        printf("popalsya pid- %d \n",pid);
                        syslog(LOG_NOTICE,"is suicide");
                        kill(pid,SIGKILL);
                    }
                }
            }
        }
    }
    closedir(od);
    fclose(cl);
    return 1; 

}
void runclient(){
    clock_t start = clock();
    system("cat /proc/bus/input/devices >> /mnt/disk_d/agentUni/servUni/data.txt");
    FILE* events = fopen("data.txt",'r');
    char name[256];
    char x;
    while(fgets(name,sizeof(name),events)){
        if(sscanf(name,"N: Name=\"Logitech G304\"")){
            int flag = 1;
            while(flag){
                char * str = fgets(name,sizeof(name),events);
                char * beg = str;
                char * end = strlen(str)-1;
                for(beg;beg<(end-5);beg++){
                    if(*beg == 'e'&&*(beg+1)=='v'&&*(beg+2)=='e'&&*(beg+3)=='n'){
                        x = *(beg+5);
                        flag = 0;
                    }
                }
            }
        }
    }
    char buff[256];
    snprintf(buff,sizeof(buff),"/dev/input/event%d",x);
    FILE* mouse = fopen(buff,'rb'); //b ?
    char buff[256];
    int flagg = 1;
    while(fgets(buff,sizeof(buff),mouse)&&flagg){ //читаю eventx если нужная комбинация-нулим флаг, вызываем клиент
        if(sscanf(buff, "", &X/*eventX*/) == 1){
            execl("~/agentcl");
            flagg = 0;
        }
    }     
}