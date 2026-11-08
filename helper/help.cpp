#include <stdio.h>
#include <dlfcn.h>
#include "cl/clnt.hpp" //!!!!!!!!!!!!!!!!!!!!!!!!!!
#include "srv/srv.hpp"

int main(int argc, char *argv[]/*дескрипторы мб*//*коды ошибок*/){ 
    server* srv = new server();
    clnt* cl = new clnt();
    // cl->getfuncs((void*)dlsym(dlopen("/mnt/disk_d/agentUni/libagent.so",RTLD_LAZY),"funcsStruct")); //передача функционала, нужно сделать при первом сбое
    if((*argv[0]) == '0'){ //0 - значит сервер // НЕНУЖНО, всмысле а че чинить то?
        while(open("/tmp/my_n_pipe")){
            srv->loop();
            cl->loop();
        }
        srv->srvopening();// как то еще умудрится передать аргументы
        //если ниче не помогло ребут сервера
    }
    if((*argv[0]) == '1'){//НЕНУЖНО
        //тот же алг
        while(){}
        cl->clopening();
    }
    if((*argv[0]) == '2'){
        int spid = srv->cnct((void*)cl,argv);
        if(spid){
            return spid;//кидаю пид в клиентский код
        }
    }   
    return 1;
}