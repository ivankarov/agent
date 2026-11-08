#ifndef LOOP_H
#define LOOP_H
// typedef int (*func1) (int fdc);
typedef int (*func2) (void* arg,int fds);
int loop(func2 f2,void* arg,int fds,int s,int fdsm);
#endif