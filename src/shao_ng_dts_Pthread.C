/******************************************************************************/
/*                                                                            */
/*                        shao_ng_dts_ P t h r e a d . C                         */
/*                                                                            */
/*(c) 2014-2024 by Shanghai Astronomical Observatory , Chinese Academy Of Sciences*/
/*      All Rights Reserved. See shao_ng_dts_Version.C for complete License Terms    */
/*                            All Rights Reserved                             */
/*   Produced by Shaoguang Guo Shanghai Astronomical Observatory , Chinese Academy Of Sciences*/
/*                                                                            */
/* shao_ng_dts is free software: you can redistribute it and/or modify it under      */
/* the terms of the GNU Lesser General Public License as published by the     */
/* Free Software Foundation, either version 3 of the License, or (at your     */
/* option) any later version.                                                 */
/*                                                                            */
/* shao_ng_dts is distributed in the hope that it will be useful, but WITHOUT        */
/* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or      */
/* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public       */
/* License for more details.                                                  */
/*                                                                            */
/* You should have received a copy of the GNU Lesser General Public License   */
/* along with shao_ng_dts in a file called COPYING.LESSER (LGPL license) and file    */
/* COPYING (GPL license).  If not, see <http://www.gnu.org/licenses/>.        */
/*                                                                            */
/* The copyright holder's institutional names and contributor's names may not */
/* be used to endorse or promote products derived from this software without  */
/* specific prior written permission of the institution or contributor.       */
/******************************************************************************/
  
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include "shao_ng_dts_Debug.h"
#include "shao_ng_dts_Pthread.h"

/******************************************************************************/
/*                          shao_ng_dts_ C o n d V a r                           */
/******************************************************************************/
/******************************************************************************/
/*                                  W a i t                                   */
/******************************************************************************/
  
int shao_ng_dts_CondVar::Wait()
{
 int retc;

// Wait for the condition
//
   if (relMutex) Lock();
   retc = pthread_cond_wait(&cvar, &cmut);
   if (relMutex) UnLock();
   return retc;
}

/******************************************************************************/
  
int shao_ng_dts_CondVar::Wait(int sec)
{
 struct timespec tval;
 int retc;

// Get the mutex before calculating the time
//
   if (relMutex) Lock();

// Simply adjust the time in seconds
//
   tval.tv_sec  = time(0) + sec;
   tval.tv_nsec = 0;

// Wait for the condition or timeout
//
   do {retc = pthread_cond_timedwait(&cvar, &cmut, &tval);}
   while (retc && (retc != ETIMEDOUT));

   if (relMutex) UnLock();
   return retc == ETIMEDOUT;
}

/******************************************************************************/
/*                                W a i t M S                                 */
/******************************************************************************/
  
int shao_ng_dts_CondVar::WaitMS(int msec)
{
 int sec, retc, usec;
 struct timeval tnow;
 struct timespec tval;

// Adjust millseconds
//
   if (msec < 1000) sec = 0;
      else {sec = msec / 1000; msec = msec % 1000;}
   usec = msec * 1000;

// Get the mutex before getting the time
//
   if (relMutex) Lock();

// Get current time of day
//
   gettimeofday(&tnow, 0);

// Add the second and microseconds
//
   tval.tv_sec  = tnow.tv_sec  +  sec;
   tval.tv_nsec = tnow.tv_usec + usec;
   if (tval.tv_nsec > 1000000)
      {tval.tv_sec += tval.tv_nsec / 1000000;
       tval.tv_nsec = tval.tv_nsec % 1000000;
      }
   tval.tv_nsec *= 1000;


// Now wait for the condition or timeout
//
   do {retc = pthread_cond_timedwait(&cvar, &cmut, &tval);}
   while (retc && (retc != ETIMEDOUT));

   if (relMutex) UnLock();
   return retc == ETIMEDOUT;
}
 
/******************************************************************************/
/*                        shao_ng_dts_ S e m a p h o r e                         */
/******************************************************************************/
/******************************************************************************/
/*                              C o n d W a i t                               */
/******************************************************************************/
  
#if defined(MACOS) || defined(AIX)

int shao_ng_dts_Semaphore::CondWait()
{
   int rc;

// Get the semaphore only we can get it without waiting
//
   semVar.Lock();
   if ((rc = (semVal > 0) && !semWait)) semVal--;
   semVar.UnLock();
   return rc;
}

/******************************************************************************/
/*                                  P o s t                                   */
/******************************************************************************/
  
void shao_ng_dts_Semaphore::Post()
{
// Add one to the semaphore counter. If we the value is > 0 and there is a
// thread waiting for the sempagore, signal it to get the semaphore.
//
   semVar.Lock();
   semVal++;
   if (semVal && semWait) semVar.Signal();
   semVar.UnLock();
}

/******************************************************************************/
/*                                  W a i t                                   */
/******************************************************************************/
  
void shao_ng_dts_Semaphore::Wait()
{

// Wait until the sempahore value is positive. This will not be starvation
// free is the OS implements an unfair mutex;
//
   semVar.Lock();
   if (semVal < 1 || semWait)
      while(semVal < 1)
           {semWait++;
            semVar.Wait();
            semWait--;
           }

// Decrement the semaphore value and return
//
   semVal--;
   semVar.UnLock();
}
#endif
 
extern "C"
{
int shao_ng_dts_Thread_Cancel(pthread_t tid)
    {return pthread_cancel(tid);}

int shao_ng_dts_Thread_CanType(int Async)
    {int oldType;
     return pthread_setcanceltype((Async ? PTHREAD_CANCEL_ASYNCHRONOUS
                                         : PTHREAD_CANCEL_DEFERRED), &oldType);
    }

int shao_ng_dts_Thread_Detach(pthread_t tid)
    {return pthread_detach(tid);}

int  shao_ng_dts_Thread_Run(void *(*proc)(void *), void *arg, pthread_t *tid)
     {int retc = shao_ng_dts_Thread_Start(proc, arg, tid);
      if (!retc) pthread_detach(*tid);
      return retc;
     }

int shao_ng_dts_Thread_Signal(pthread_t tid, int snum)
    {
     return pthread_kill(tid, snum);
    }

int  shao_ng_dts_Thread_Start(void *(*proc)(void *), void *arg, pthread_t *tid)
     {int rc = pthread_create(tid, NULL, proc, arg);
      return (rc ? -rc : 0);
     }

void *shao_ng_dts_Thread_Wait(pthread_t tid)
    {long retc;
     void *tstat;
     if (retc = pthread_join(tid, (void **)&tstat)) tstat = (void *)retc;
     return tstat;
    }

void shao_ng_dts_Thread_MT(int mtval)
   {
#ifdef SUN
#ifndef SUN6 // Ranch version added this #ifndef SUN6
    int rc, oldmt = pthread_getconcurrency(), newmt = mtval;

    while(mtval)
         if ((rc = pthread_setconcurrency(mtval)) && rc == EAGAIN) mtval--;
            else break;

    DEBUG("MT set rc=" <<rc <<" omt=" <<oldmt <<" rmt=" <<newmt <<" nmt=" <<mtval);
#endif
#endif
   }
}
