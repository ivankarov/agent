#include "srvhelp.hpp"
#include <dlfcn.h>
#include <iostream>
#include <fcntl.h>
#include "ownthread.hpp"
typedef void* tryopening(void* arg);
typedef int srvopening();
int server::getsrvpid(){
    return srvpid;
}
server::server(){
    void* agent = dlopen("/mnt/disk_d/agentUni/libagent.so",RTLD_LAZY);// !!!!!!!!!!!111
    if(agent){
        tryopening* tryopen = (tryopening*) dlsym(agent,"tryopening");
        srvopening* hidden = (srvopening*) dlsym(agent,"srvopening");
        //запустить bare minimum server
        if(tryopen && hidden){
            func1 = (void*)tryopen;
            func2 = (void*)hidden;//... загрузить все доступные функции
        }
        dlclose(agent);
    }
    else{
        std::cout<<"dlerror - %s",dlerror();
    }
}
int server::cnct(void* cl,char* arg[] /*какая проблема*/){
    if((*arg[1]) == '0'){//ВЫНЕСТИ В ОБЩИЙ MAIN                       !!!!!!!!!!!!!!!!!!!!!!!!!
        /*шаблонный*/ownthread while(this->extrawrite()){//пока не будет несанкционированного действия, можно ожидать по другом (творчество)
        /*в шаблонном ownthread потоке*/this->getloop(); //колбэк из unipipes
        /*в шаблонном ownthread потоке*/cl->loop();
        }
        //передаем пид серва клиенту
        return this->getsrvpid();
    }else{
        //тоже запустить резерв клиент сервер
        //инит серва наступает с новым потоком pthread_create(&tid,NULL,&tryopening,(void*)(&arg)) - анализ /proc/<PID>/task/ ; не факт что успею поймать
        //100% проверка - дернуть shm, если agent позже cl - shm не существует (client->rwtryshm()); 
        //тогда запуск резерва (до проверки) должен быть изолированным, чтобы не помешать проверке, чтобы тру клиент не схватился за сервер резерва
        if(client::running){//если клиент запущен раньше серва - убийство старого, запуск нового клиента после инита серва
                
        }
        //или не запущен вовсе - просто новый клиент
    }
    return 0;
}
// extern void* tryopening (void* arg); - нединамическая загрузка библы
// server::server(){
    // int a = 0;
    // void * arg = &a;
    // tryopening(arg);
// }