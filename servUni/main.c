// #include "winservice.h"
// #include <stdio.h>
// #include <windows.h>
// #include <WinUser.h>
// int main(){
//     // получить все процессы если есть работающий не запускать - return ; 
//     SC_HANDLE srv = NULL;
//     FILE *fp = fopen("D:\\agent\\file1.txt","w");
//     if(!fp){
//         return 0;
//     }
//     int code = InitSvc(&srv);
//     fprintf(fp,"sv init code: %d \n",code);
//     if(code!=1){
//         ReportSvc(SERVICE_STOP_PENDING,NO_ERROR,5000);
//         ReportSvc(SERVICE_STOPPED,NO_ERROR,5000);
//         DelSvc(srv);
//         return 0;
//     }
//     // printf("print smth to start service \n");
//     // int n = 10; 
//     // while(n==10){
//     //     scanf("%d",&n);
//     // }
//     ReportSvc(SERVICE_START_PENDING,NO_ERROR,5000);
//     ReportSvc(SERVICE_RUNNING,NO_ERROR,5000);
//     fprintf(fp,"service running \n");
//     int code2 = StartServiceA(srv,0,NULL); 
//     if(!code2){
//         fprintf(fp,"starting failed: %lu\n",GetLastError());
//     }
//     // printf("print smth to delete service \n");
//     // int n2 = 10; 
//     // while(n2==10){
//     //     scanf("%d",&n2);
//     // }   
//     // ReportSvc(SERVICE_STOP_PENDING,NO_ERROR,5000);
//     // ReportSvc(SERVICE_STOPPED,NO_ERROR,5000);
//     // DelSvc(srv);
//     fclose(fp);
//     return code;
// }


//------------------UNIX------------------
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "uniservice.h"
#include <syslog.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/wait.h>
int main(){ 
    killalts(1);
    killifcl();//исключение одной траблы
    daemoninit();
    syslog (LOG_NOTICE, "serv daemon started.");
    pid_t pid = fork();
    if(pid == 0){
        syslog (LOG_NOTICE, "daemon forked for agent.");
        execl("/mnt/disk_d/agentUni/agent","agent",NULL); //запуск должен состоятся, любыми методами заспустить
        exit(1);
    }   
    else if(pid > 0){
        wait(NULL);
        double time = 0.0;
        pthread_t tid ;
        pthread_create(&tid,NULL,&runclient,NULL);
    }
    else{
        syslog(LOG_NOTICE,"fork() for agentUni failed");
    }
    syslog(LOG_NOTICE,"serv pid - %d \n",getpid());
    killalts(0);
    while(1){
        syslog(LOG_NOTICE,"serv is not terminated. \n");
    }
    syslog(LOG_NOTICE,"serv daemon terminated.\n");
    closelog();
    return 1;
}