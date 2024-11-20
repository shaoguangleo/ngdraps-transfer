/******************************************************************************/
/*                                                                            */
/*                         shao_ng_dts_ R T C o p y . C                          */
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
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "shao_ng_dts_Config.h"
#include "shao_ng_dts_Emsg.h"
#include "shao_ng_dts_FileSystem.h"
#include "shao_ng_dts_Pthread.h"
#include "shao_ng_dts_RTCopy.h"

#ifndef EADV
#ifdef MACOS
#define EADV ENOATTR
#else
#define EADV EPERM
#endif
#endif

/******************************************************************************/
/*                         G l o b a l   V a l u e s                          */
/******************************************************************************/
  
extern shao_ng_dts_Config   shao_ng_dts_Config;

       shao_ng_dts_RTCopy   shao_ng_dts_RTCopy;

/******************************************************************************/
/*            E x t e r n a l   T h r e a d   I n t e r f a c e s             */
/******************************************************************************/
  
extern "C"
{
void *shao_ng_dts_RTCopyLK(void *pp)
{
   return shao_ng_dts_RTCopy.Lock((shao_ng_dts_Semaphore *)pp);
}
}

/******************************************************************************/
/*                                  L o c k                                   */
/******************************************************************************/
  
void *shao_ng_dts_RTCopy::Lock(shao_ng_dts_Semaphore *semP)
{
   struct flock fl;
   long long lfSize = 1;
   int rc;

// Initialize the lock structure
//
   fl.l_type = F_RDLCK;
   fl.l_whence = SEEK_SET;
   fl.l_start = 0;
   fl.l_len = 0;

// Indicate that we are all initialized
//
   if (semP) semP->Post();

// Obtain a shared lock on the file
//
   do {rc = fcntl(lkFD, F_SETLKW, &fl);} while(rc < 0 && errno == EINTR);

// Check how we ended here. Note that since we are permanent object, thread
// locking considerations do not apply here.
//
   if (rc && Grow)
      {rc = errno;
       shao_ng_dts_Emsg("RTCopy", rc, "locking", lkFN);
       Grow = -rc;
       return 0;
      }

// Check if we should verify this copy
//
   if (Grow && shao_ng_dts_Config.Options & shao_ng_dts_RTCVERC)
      {lfSize = FSp->getSize(lkFD);
       if (lfSize < 0)
          {rc = static_cast<int>(-lfSize);
           shao_ng_dts_Emsg("RTCopy", rc, "stating", lkFN);
           Grow = -rc;
           return 0;
          }
        if (!lfSize) {Grow = -EADV; return 0;}
      }

// Obtain the final size of the source file
//
   if ((xLim = FSp->getSize(ioFD)) < 0) Grow = static_cast<int>(xLim);
      else Grow = 0;

// All done
//
   return 0;
}

/******************************************************************************/
/* Public:                          P r e p                                   */
/******************************************************************************/
  
long long shao_ng_dts_RTCopy::Prep(long long rdsk, int rdsz, ssize_t &rlen)
{
   long long Hwm, cSize, pSize;
   int tLim, tVal;

// If we need not wait then return bytes left
//
   if (!Grow) return (xLim < rdsk ? 0 : xLim - rdsk);

// Check if we can satisfy this read with what we already have
//
   if (Grow > 0 && Left >= rdsz)
      {Left -= rdsz;
       return (size_t)0x7fffffff;
      }

// Check if we should adjust read size to blocking to number of streams
//
   if (Blok) rdsz *= Blok;

// Setup high watermark and the time limit, if any
//
   Hwm   = rdsk + rdsz;
   tVal  = shao_ng_dts_Config.rtLimit;
   pSize = -1;

// We continue waiting for enough data to fully satisfy this read or until
// writes to the file are finished (then we need not wait).
//
   while(Grow > 0)
        {if ((cSize = FSp->getSize(ioFD)) < 0) {rlen = -errno; return -1;}
         if (cSize >= Hwm)
            {Left = cSize - Hwm;
             return (size_t)0x7fffffff;
            }
         sleep(shao_ng_dts_Config.rtCheck);
         if (tVal)
            {if (cSize != pSize) {tLim = tVal; pSize = cSize;}
                 else if ((tLim -= shao_ng_dts_Config.rtCheck) <= 0)
                         {rlen = -ETIMEDOUT; return -1;}
            }
        }

// If we ended with an error, return no bytes left
//
   if (Grow < 0) {rlen = Grow; return -1;}

// If we no longer need to wait, return actual bytes left
//
   return (xLim < rdsk ? 0 : xLim - rdsk);
}

/******************************************************************************/
/* Public:                         S t a r t                                  */
/******************************************************************************/
  
int shao_ng_dts_RTCopy::Start(shao_ng_dts_FileSystem *fsp, const char *iofn, int iofd)
{
   shao_ng_dts_Semaphore xSem(0);
   int rc;

// Initialize the common variables
//
   ioFD = iofd;
   Grow = 1;
   Left = 0;
   Blok = (shao_ng_dts_Config.Options & shao_ng_dts_RTCBLOK ? shao_ng_dts_Config.Streams : 0);
   FSp  = fsp;

// Initialize variable dependent on how we will do locking
//
   if (shao_ng_dts_Config.rtLockf)
      {lkFN = shao_ng_dts_Config.rtLockf;
       lkFD = shao_ng_dts_Config.rtLockd;
      } else {
       lkFN = iofn;
       lkFD = dup(iofd);
      }

// Now start a thread that will try to obtain a shared lock
//
   if ((rc = shao_ng_dts_Thread_Run(shao_ng_dts_RTCopyLK, (void *)&xSem, &Tid)) < 0)
      {shao_ng_dts_Emsg("RTCopy", rc, "starting file r/t lock thread.");
       Grow = -rc;
       return 0;
      }

// Wait for the thread to set up
//
   xSem.Wait();
   return (Grow >= 0);
}

/******************************************************************************/
/*                                  S t o p                                   */
/******************************************************************************/
  
void shao_ng_dts_RTCopy::Stop()
{
// Now if we have a thread, kill it
//
   if (Grow > 0) {Grow = 0; pthread_cancel(Tid);}

// Now close our lock file
//
   if (lkFD >= 0) {close(lkFD); lkFD = -1;}
}
