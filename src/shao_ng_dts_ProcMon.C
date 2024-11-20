/******************************************************************************/
/*                                                                            */
/*                        shao_ng_dts_ P r o c M o n . C                         */
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
  
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include "shao_ng_dts_Config.h"
#include "shao_ng_dts_Debug.h"
#include "shao_ng_dts_Emsg.h"
#include "shao_ng_dts_Headers.h"
#include "shao_ng_dts_Node.h"
#include "shao_ng_dts_ProcMon.h"
  
/******************************************************************************/
/*           E x t e r n a l   I n t e r f a c e   R o u t i n e s            */
/******************************************************************************/
  
extern "C"
{
void *shao_ng_dts_MonProc(void *pp)
{
     shao_ng_dts_ProcMon *pmp = (shao_ng_dts_ProcMon *)pp;
     if (pmp->MonPool) pmp->Limit();
        else pmp->Monitor();
     return (void *)0;
}
}
  
/******************************************************************************/
/*                                 L i m i t                                  */
/******************************************************************************/
  
void shao_ng_dts_ProcMon::Limit()
{

// Wait until the time limit or the process is done
//
   CondMon.Wait(TimeLimit);

   if (!alldone)
      {MonPool->Abort();
       shao_ng_dts_Fmsg("Limit", "Time limit exceeded");
      }

// Post the semaphore to indicate we are done. This is a far safer way to
// synchronize exits than using pthread_join().
//
   monDone.Post();
}

/******************************************************************************/
/*                               M o n i t o r                                */
/******************************************************************************/
  
void shao_ng_dts_ProcMon::Monitor()
{
   static const int wtime = 5;   // 5 second periods
   static const int pitvl =36;   // 3 minute interval for pings
                int pleft =36;   // Expressed as number of 5 second intervals
   shao_ng_dts_Node    *pingDest;

// Allow immediate cancellations to avoid hang-ups
//
   shao_ng_dts_Thread_CanType(1);

// Run a loop until we are killed checking that process we want is still alive
//
   while(!alldone && CondMon.Wait(wtime))
        {if (kill(monPID, 0) && ESRCH == errno)
            {DEBUG("Process " <<monPID <<" has died");
             _exit(8);
            }
         if (pingNode && !pleft--)
            {pingMutex.Lock(); pingDest = pingNode; pingMutex.UnLock();
             if (pingDest) pingDest->Put("\n", 1);
             pleft = pitvl;
            }
        }

// Post the semaphore to indicate we are done. This is a far safer way to
// synchronize exits than using pthread_join().
//
   monDone.Post();
}

/******************************************************************************/
/*                                 S t a r t                                  */
/******************************************************************************/
  
void shao_ng_dts_ProcMon::Start(pid_t monit, shao_ng_dts_Node *Remote)
{
   int retc;

// If we are already monitoring, issue a stop
//
   if (mytid) Stop();

// Preset all values
//
   alldone = 0;
   monPID = (monit ? monit : getppid());
   pingMutex.Lock();
   pingNode = Remote;
   pingMutex.UnLock();

// Run a thread to start the monitor
//
   if (retc = shao_ng_dts_Thread_Run(shao_ng_dts_MonProc, (void *)this, &mytid))
      {DEBUG("Error " <<retc <<" starting MonProc thread.");}
      else {DEBUG("Thread " <<mytid <<" monitoring process " <<monPID);}
   return;
}
  
void shao_ng_dts_ProcMon::Start(int seclim, shao_ng_dts_BuffPool *buffpool)
{
   int retc;

// If we are already monitoring, issue a stop
//
   if (mytid) Stop();

// Preset all values
//
   alldone   = 0;
   TimeLimit = seclim;
   MonPool   = buffpool;

// Run a thread to start the monitor
//
   if (retc = shao_ng_dts_Thread_Run(shao_ng_dts_MonProc, (void *)this, &mytid))
      {DEBUG("Error " <<retc <<" starting buffpool monitor thread.");}
      else {DEBUG("Thread " <<mytid <<" monitoring buffpool.");}
   return;
}
/******************************************************************************/
/*                                  S t o p                                   */
/******************************************************************************/
  
void shao_ng_dts_ProcMon::Stop()
{

// Stop any pings
//
   pingMutex.Lock();
   pingNode = 0;
   pingMutex.UnLock();

// Simply issue a kill to the thread running the progress monitor
//
   alldone = 1;
   if (mytid) 
      {DEBUG("Process monitor " <<mytid <<" stopped.");
       CondMon.Signal();
       monDone.Wait();
       mytid = 0;
      }
   return;
}
 
