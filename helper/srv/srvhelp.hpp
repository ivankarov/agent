// #include "../cl/clnt.hpp"
class server{
public:
    server();
    int cnct(void* cl,char* arg[]);
    void* func1;
    void *func2;
    int getsrvpid();
private:
    int srvpid;
};