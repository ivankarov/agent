#include "clnt.hpp"
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdio.h>
#define CL_FUNC 0// есть ли на машине функционал клиента
clnt::clnt(){//эмуляция сервера
    //if(!CL_FUNC){//выход/отсутствие использования функ., если запущено на машине без функционала клиента (ЕСЛИ КОМПИЛЯТОР обманывается
    //  return ;
    //}
    //temples; std::string waytoh = ""+ "";
    1 #include waytoh //2 temples/constexpr; построчное копирование файла // 3 выводить текст файла и перенаправить вывод
    //temples; execv("cpp ./clnt.cpp");

    // open("tmp/named_pipe",O_WRONLY);
    // void* dataptr;
    // int shmid = shmget(ftok("/",'e'),1024,0666);
    // if(shmid!=-1){
    //     dataptr = (int*)shmat(shmid,NULL,0); 
    //     //temples; std::string waytoh = "";
    //     //#include waytoh
    //     //temples; execv("cpp -");
    //     cll = dynamic_cast<clloop*>(dataptr);
    //     shmdt(dataptr);
    // }
    // shmctl(shmid,IPC_RMID,NULL);
}   
// clnt::hello(){
//     cll->hello();
// }


#include "clnthelp.hpp"
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdio.h>
f(){
    asm{//не асм а машинный код
        jmp beginoffile
        как идти вниз и копировать?
    }
}
clnt::clnt(){
    void* dataptr;
    int shmid = shmget(ftok("/",'e'),1024,0666); //это первым делом при запуске проги
    if(shmid!=-1){
        dataptr = (int*)shmat(shmid,NULL,0); 
        int *p;
        f();
        *dataptr = p+1;
        cll = dynamic_cast<clloop*>(dataptr);
        shmdt(dataptr);
    }
    shmctl(shmid,IPC_RMID,NULL);
}   
