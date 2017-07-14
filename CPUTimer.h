#ifndef CPUTIME_H
#define CPUTIME_H

#include <stdlib.h>

#if defined WIN32
  #include <windows.h>
#endif

class CPUTimer {
  public:
    CPUTimer();
    double     getCPUCurrSecs();
    double     getCPUTotalSecs();
    double     getCronoCurrSecs();
    double     getCronoTotalSecs();
    bool       start();
    bool       stop();
    void       reset();
    void       operator +=( CPUTimer t );
    inline void increaseCPUTotalSecs( double s){CPUTotalSecs += s; };
    bool         started;

  private:
    #if defined WIN32
      HANDLE       hFile;
      FILETIME     FT_CreationTime;
      FILETIME     FT_ExitTime;
      FILETIME     FT_KernelTime;
      FILETIME     FT_UserTime;
      FILETIME     FT_CronoStartTime;
      FILETIME     FT_CronoStopTime;
      FILETIME     FT_Aux1Time;
      FILETIME     FT_Aux2Time;
    #else
      double CPUTStart;            
      double CPUTStop;             
      double CronoTStart;
      double CronoTStop;
      double zeit();
      double real_zeit();
    #endif

#ifdef WIN32
    SYSTEMTIME   ST_CPUCurrTime;
    SYSTEMTIME   ST_CPUTotalTime;

    SYSTEMTIME   ST_CronoStartTime;
    SYSTEMTIME   ST_CronoStopTime; 
    SYSTEMTIME   ST_CronoTotalTime;
    
    ULONG64      cpu_ti;           
    ULONG64      cpu_tf;           
    ULONG64      crono_ti;         
    ULONG64      crono_tf;         
     LONG64      aux;              
     LONG64      secs;             
     LONG64      msecs;            
#endif
    double       CPUCurrSecs;      
    double       CPUTotalSecs;     
    double       CronoCurrSecs;    
    double       CronoTotalSecs;   
    bool         gottime;          
};
#endif
