class clnt { //bare minimum client
public:
    clnt();
    ~clnt();
    void hello();
private:
    // clloop* cll;
    //получаю путь до clloop, посередине кода во время компиляции темлпами (compile time programing) инклюдаю его и говорю препроц обработать еще один инклюд, тоже темлпами
    //получаю путь до clloop, копирую в пустой файл clloop.h, подключаю инклюд темплами 
    //clloop - T, tempalte? T.func????
    //получить путь до clloopWQ.h и client.h, получаю файл
    //отсрочка описания clloop с помощью dynamic_cast
};