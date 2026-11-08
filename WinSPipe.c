#include "SPipe.h"
#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <process.h>
#define BUFSIZE 512
LPCTSTR lpszPipename = TEXT("\\\\.\\pipe\\mynamedpipe"); 
int i = 0;

typedef struct prmsforpchr{
    outr* rout;
    void* cpout;
}pfp;

VOID GetAnswerToRequest(LPTSTR a, LPTSTR b , LPDWORD c,pfp* v); 
 

int GetServPipeAndCnct(HANDLE *hpipeOut, BOOL* fcnctdOut)
{ 
    printf("hello from getservPipe\n");
    HANDLE hPipe = INVALID_HANDLE_VALUE;
    BOOL   fConnected = FALSE;  
    hPipe = CreateNamedPipe( 
          lpszPipename,             
          PIPE_ACCESS_DUPLEX,       
          PIPE_TYPE_MESSAGE |       
          PIPE_READMODE_MESSAGE |   
          PIPE_WAIT,                
          PIPE_UNLIMITED_INSTANCES, 
          BUFSIZE,                  
          BUFSIZE,                  
          0,                        
          NULL);                    
    if(!hPipe){
        printf("createnamedpipe() bug\n");
        return 0;
    }
    *hpipeOut = hPipe;
    if (hPipe == INVALID_HANDLE_VALUE) 
    {
        _tprintf(TEXT("CreateNamedPipe failed, GLE=%d.\n"), GetLastError()); 
        return -1;
    }
    printf("пытаемся connectnamedpipe\n");
    fConnected = (ConnectNamedPipe(hPipe, NULL) ? 
         TRUE : (GetLastError() == ERROR_PIPE_CONNECTED)); 
    *fcnctdOut = fConnected;
    printf("connect удался \n");
    return 1;
}

int WINAPI RWServPipe(LPVOID lpvPara){   
    bt* lpvParam = (bt*) lpvPara;
   HANDLE hHeap      = GetProcessHeap();
   TCHAR* pchRequest = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE*sizeof(TCHAR));
   TCHAR* pchReply   = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE*sizeof(TCHAR));

   DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0; 
   BOOL fSuccess = FALSE;
   HANDLE hPipe  = NULL;


   if (!lpvParam)
   {
       printf( "\nERROR - Pipe Server Failure:\n");
       printf( "   InstanceThread got an unexpected NULL value in lpvParam.\n");
       printf( "   InstanceThread exitting.\n");
       if (pchReply != NULL) HeapFree(hHeap, 0, pchReply);
       if (pchRequest != NULL) HeapFree(hHeap, 0, pchRequest);
       return (DWORD)-1;
   }

   if (pchRequest == NULL)
   {
       printf( "\nERROR - Pipe Server Failure:\n");
       printf( "   InstanceThread got an unexpected NULL heap allocation.\n");
       printf( "   InstanceThread exitting.\n");
       if (pchReply != NULL) HeapFree(hHeap, 0, pchReply);
       return (DWORD)-1;
   }

   if (pchReply == NULL)
   {
       printf( "\nERROR - Pipe Server Failure:\n");
       printf( "   InstanceThread got an unexpected NULL heap allocation.\n");
       printf( "   InstanceThread exitting.\n");
       if (pchRequest != NULL) HeapFree(hHeap, 0, pchRequest);
       return (DWORD)-1;
   }
   printf("InstanceThread created, receiving and processing messages.\n");

   hPipe = (HANDLE) (lpvParam->tp1->hPipe); 
   
   // распаковка аргов cpu_update()
   int ms = lpvParam->tp1->ms;
   void* ct = lpvParam->tp1->ct;
   void* m = lpvParam->tp1->m;
   func f = lpvParam->tp1->f;

    // распаковка аргов ram_update()
   void* rct = lpvParam->tp2->ct;
   void* rm = lpvParam->tp2->out;
   func rf = lpvParam->tp2->f;

   while (1) 
   { 
      Sleep(ms);
      int code = f(ct,m);
      int rcode = rf(rct,rm);
      if(!code || !rcode){
          _tprintf("какая то херня с cpu_update\n");
          break;  
      }
      printf("cpu_update работает - %d \n",code);
      printf("ram_update работает - %d \n",rcode);
      pfp p = {.rout=rm,.cpout=m};
      GetAnswerToRequest(pchRequest, pchReply, &cbReplyBytes,&p);
      printf("i -%d \n",i);
      fSuccess = WriteFile( 
           hPipe,        
           pchReply,     
           cbReplyBytes, 
           &cbWritten,   
           NULL);
    //   if(code == 2 ){ // это код запланированной остановки, когда пользователь задает число чтений метрик
    //     _tprintf("cpu_update прочитал все \n");
    //     break;
    //   }
      if (!fSuccess || cbReplyBytes != cbWritten)
      {   
          _tprintf(TEXT("InstanceThread WriteFile failed, GLE=%d.\n"), GetLastError()); 
          break;
      }
      _tprintf("Еще одна итерация записи прошла успешно! \n"); 
       fSuccess = ReadFile( 
         hPipe,        
         pchRequest,   
         BUFSIZE*sizeof(TCHAR), 
         &cbBytesRead, 
         NULL);        

      if (!fSuccess || cbBytesRead == 0)
      {   
          if (GetLastError() == ERROR_BROKEN_PIPE)
          {
              _tprintf(TEXT("InstanceThread: client disconnected.\n")); 
              break;
          }
          else
          {
              _tprintf(TEXT("InstanceThread ReadFile failed, GLE=%d.\n"), GetLastError()); 
          }
      }
      pchRequest[cbBytesRead/sizeof(TCHAR)]=TEXT('\0');
      printf("pchRequest - %s \n",pchRequest); 
      char s[] = "hello";
      int result = _stscanf_s(pchRequest, _T("%s"),&s,BUFSIZE);
      printf("from readpipe pchrequest = %s \n",s);
      if(*s=='1'){
        printf("получено сообщение от клиент - Stop\n");
        i = 1; 
        // int pid = getpid();
        // printf("this pid - %d",pid);        
        // HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
        // if(!hProcess){
        //     printf("cant open proc\n");
        //     return 0;
        // }
        // printf("killing...");
        // if(!TerminateProcess(GetCurrentProcess(), 0)){
        //     printf("cant kill proc\n");
        //     return 0;
        // }
        // printf("killed!");
        // CloseHandle(hProcess);
        break;
      }
      _tprintf("Еще одна итерация чтения прошла успешно! \n");
  }
   printf("i - %d\n",i);
   FlushFileBuffers(hPipe); 
   DisconnectNamedPipe(hPipe); 
   CloseHandle(hPipe); 
   HeapFree(hHeap, 0, pchRequest);
   HeapFree(hHeap, 0, pchReply);
   printf("InstanceThread exiting.\n");
   ExitProcess(0);
   return (DWORD)1;
}

VOID GetAnswerToRequest( LPTSTR pchRequest, 
                         LPTSTR pchReply, 
                         LPDWORD pchBytes,pfp* prm)
{
    _tprintf( TEXT("Client Request String:\"%s\"\n"), pchRequest );
   
      _stprintf_s(pchReply,  BUFSIZE*sizeof(TCHAR),  
      _T("cpu = %f \n pcnt=%I64u \n ttlkbph=%I64u kb \n fkbph=%I64u kb \n"), *(double*)prm->cpout, prm->rout->pcnt, prm->rout->ttlkbph, prm->rout->fkbph);
    printf("данные завернуты \n");
    if(!pchReply){
         *pchBytes = 0;
        pchReply[0] = 0;
        printf("StringCchCopy failed, no outgoing message.\n");
        return;
    }
    
    *pchBytes = (lstrlen(pchReply)+1)*sizeof(TCHAR);
}

// int GetClientPipe(LPVOID lpch){
//     HANDLE chPipe;
//     BOOL   fSuccess = FALSE;
//     DWORD  cbRead, cbToWrite, cbWritten, dwMode;
//     TCHAR  chBuf[BUFSIZE]; 
//     printf("hello from getclientPipe \n");
//     while(1){
//         chPipe = CreateFile( 
//             lpszPipename,   
//             GENERIC_READ |  
//             GENERIC_WRITE, 
//             0,              
//             NULL,           
//             OPEN_EXISTING,  
//             0,              
//             NULL);
//         if(!chPipe){
//             printf("хуйня с createfile() \n");
//             return -1;
//         }
//         printf("creatfile() успешен\n");
//         if ( ! WaitNamedPipe(lpszPipename, 20000)) 
//         { 
//            printf("Could not open pipe: 20 second wait timed out. \n"); 
//            return -1;
//         } 
//         printf("пытаемся прочесть файл\n");
//         fSuccess = ReadFile( 
//             chPipe,    
//             chBuf,     
//             BUFSIZE*sizeof(TCHAR),  
//             &cbRead,  
//             NULL);
            
//         if (!fSuccess) 
//         {
//            _tprintf( TEXT("WriteFile to pipe failed. GLE=%d\n"), GetLastError() ); 
//            return -1;
//         }
//         printf("файл прочтен\n");
//         thrPrms*tp = (thrPrms*)lpch;
//         tp->chb = chBuf;
//         _tprintf(_T("заебись! %s\n"), tp->chb);
//         return 1;
//     }
//     return 0;
// }

// void ReadPipe(LPVOID prm){
//     Rprms * prms = (Rprms*) prm;
//     BOOL fSuccess = prms->flag;
//     TCHAR* pchRequest = prms->pbuff;
//     DWORD cbBytesRead = prms->cbr;
//     HANDLE hPipe = prms->hpipe;
//     while(1){
//         fSuccess = ReadFile( 
//          hPipe,        
//          pchRequest,   
//          BUFSIZE*sizeof(TCHAR), 
//          &cbBytesRead, 
//          NULL);        

//       if (!fSuccess || cbBytesRead == 0)
//       {   
//           if (GetLastError() == ERROR_BROKEN_PIPE)
//           {
//               _tprintf(TEXT("InstanceThread: client disconnected.\n")); 
//               break;
//           }
//           else
//           {
//               _tprintf(TEXT("InstanceThread ReadFile failed, GLE=%d.\n"), GetLastError()); 
//           }
//       }
//       TCHAR s[BUFSIZE];
//       int result = _stscanf_s(pchRequest, _T("%s"),s,BUFSIZE);
//       printf("from readpipe pchrequest = %s \n",s);
//       if(result ==1 && s=="Stop"){
//         printf("получено сообщение от клиент - Stop");
//         break;
//       }
//       _tprintf("Еще одна итерация чтения прошла успешно! \n");
//     }
// }
