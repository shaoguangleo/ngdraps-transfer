/******************************************************************************/
/*                                                                            */
/*                        shao_ng_dts_ P r o g M o n . C                         */
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
#include "shao_ng_dts_Config.h"
#include "shao_ng_dts_Debug.h"
#include "shao_ng_dts_File.h"
#include "shao_ng_dts_Headers.h"
#include "shao_ng_dts_ProgMon.h"
#include "shao_ng_dts_Timer.h"
#include "shao_ng_dts_ZCX.h"

/******************************************************************************/
/*                      G l o b a l   V a r i a b l e s                       */
/******************************************************************************/

extern shao_ng_dts_Config   shao_ng_dts_Config;

/******************************************************************************/
/*           E x t e r n a l   I n t e r f a c e   R o u t i n e s            */
/******************************************************************************/
  
extern "C"
{
void *shao_ng_dts_MonProg(void *pp)
{
     shao_ng_dts_ProgMon *pmp = (shao_ng_dts_ProgMon *)pp;
     pmp->Monitor();
     return (void *)0;
}
}

/******************************************************************************/
/*                               M o n i t o r                                */
/******************************************************************************/
  
void shao_ng_dts_ProgMon::Monitor()
{
   unsigned int  elptime, lasttime, intvtime;
   long long     cxbytes = 0, curbytes, lastbytes = 0;
   float         cratio;
   double        xfrtime, xfrtnow;
   shao_ng_dts_Timer    etime;
   char          buff[200], pbuff[24], tbuff[24], cxinfo[40], *cxip;
   const char    *xtXB, *xaXB;
   int           bewordy = shao_ng_dts_Config.Options & shao_ng_dts_VERBOSE;

// Determine whether we need to report compression ratio (assume no percentages)
//
   if (CXp) cxip = cxinfo;
      else cxip = (char *)"";
   strcpy(pbuff, "not");

// Run a loop until we are killed, reporting what we see
//
   etime.Start(); etime.Report(lasttime);
   while(!alldone && CondMon.Wait(wtime))
      {etime.Stop(); etime.Report(elptime);

       curbytes = FSp->Stats();
       if (Tbytes) sprintf(pbuff,"%d%%",static_cast<int>(curbytes*100/Tbytes));
       xfrtime =  static_cast<double>(curbytes)
               / (static_cast<double>(elptime) / 1000.0);

       if (CXp)
          {if (!(cxbytes = CXp->Bytes())) cratio = 0.0;
              else cratio = static_cast<float>(curbytes*10/cxbytes) / 10.0;
           sprintf(cxinfo, " compressed %.1f", cratio);
          }

       if (bewordy)
          {intvtime = elptime - lasttime;
           xfrtnow =  static_cast<double>(curbytes-lastbytes)
                   / (static_cast<double>(intvtime) / 1000.0);
           xaXB = shao_ng_dts_Config::Scale(xfrtnow);
          }
       lastbytes = curbytes;
       lasttime  = elptime;
       xtXB = shao_ng_dts_Config::Scale(xfrtime);

       if (shao_ng_dts_Config.Logfn) *tbuff = 0;
          else etime.Format(tbuff);
       if (bewordy)
          sprintf(buff, "shao_ng_dts: %s %s done; %.1f %sB/s, "
                        "avg %.1f %sB/s%s\n",
                        tbuff,  pbuff, xfrtnow, xaXB, xfrtime, xtXB, cxip);
          else
          sprintf(buff, "shao_ng_dts: %s %s done; %.1f %sB/s%s\n",
                         tbuff,  pbuff, xfrtime, xtXB, cxip);
       cerr <<buff <<flush;
      }

// Post the semaphore to indicate we are done. This is a far safer way to
// synchronize exits than using pthread_join().
//
   monDone.Post();
}
  
/******************************************************************************/
/*                                 S t a r t                                  */
/******************************************************************************/
  
void shao_ng_dts_ProgMon::Start(shao_ng_dts_File *fs_obj, shao_ng_dts_ZCX *cx_obj, int pint,
                         long long xfrbytes)
{  int retc;

// If we are already monitoring, issue a stop
//
   if (mytid) Stop();

// Preset all values
//
   alldone = 0;
   FSp     = fs_obj;
   CXp     = cx_obj;
   wtime   = pint;
   Tbytes  = xfrbytes;

// Run a thread to start the monitor
//
   if (retc = shao_ng_dts_Thread_Run(shao_ng_dts_MonProg, (void *)this, &mytid))
      {DEBUG("Error " <<retc <<" starting progress monitor thread.");}
      else {DEBUG("Thread " <<mytid <<" monitoring progress.");}
   return;
}

/******************************************************************************/
/*                                  S t o p                                   */
/******************************************************************************/
  
void shao_ng_dts_ProgMon::Stop()
{

// Simply issue a cancel to the thread running the progress monitor
//
   alldone = 1;
   if (mytid) 
      {DEBUG("Progress monitor " <<mytid <<" stopped.");
       CondMon.Signal(); 
       monDone.Wait();
       mytid = 0;
      }
}
