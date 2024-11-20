/******************************************************************************/
/*                                                                            */
/*                        shao_ng_dts_ L o g F i l e . C                         */
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
#include <poll.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "shao_ng_dts_Debug.h"
#include "shao_ng_dts_Emsg.h"
#include "shao_ng_dts_Headers.h"
#include "shao_ng_dts_LogFile.h"
#include "shao_ng_dts_Platform.h"

/******************************************************************************/
/*                         L o c a l   C l a s s e s                          */
/******************************************************************************/
  
class shao_ng_dts_LogFiler
{
public:

shao_ng_dts_LogFiler *Next;
shao_ng_dts_LogFile  *LogF;
char          *Name;
pthread_t      LogT;
int            ErFD;  //  Input file descriptor

      shao_ng_dts_LogFiler(shao_ng_dts_LogFile *lfP, char *name, int efd)
                        : Next(0), LogF(lfP), Name(strdup(name)), LogT(0),
                          ErFD(efd) {}
     ~shao_ng_dts_LogFiler() {if (Name)      free(Name);
                       if (ErFD >= 0) close(ErFD);
                       if (LogT)      shao_ng_dts_Thread_Cancel(LogT);
                       if (Next)      delete Next;
                      }
};

/******************************************************************************/
/*           E x t e r n a l   I n t e r f a c e   R o u t i n e s            */
/******************************************************************************/
  
extern "C"
{
void *shao_ng_dts_FileLog(void *op)
{
     shao_ng_dts_LogFiler *lrP = (shao_ng_dts_LogFiler *)op;
     shao_ng_dts_LogFile::Record(lrP);
     return (void *)0;
}
}

/******************************************************************************/
/*                            D e s t r u c t o r                             */
/******************************************************************************/
  
shao_ng_dts_LogFile::~shao_ng_dts_LogFile()
{
   static const char *Eol = "=-=-=-=-=-=-=-=\n";

// Close the log file and delete the log list which will clean everything
//
   if (Logfd >= 0)
      {write(Logfd, Eol, strlen(Eol));
       close(Logfd);
      }
   if (Logfn) free(Logfn);
   delete Loggers;
}

/******************************************************************************/
/*                                  O p e n                                   */
/******************************************************************************/
  
int shao_ng_dts_LogFile::Open(const char *fname)
{

// Check if we have a logfile already
//
   if (Logfd >= 0) return -ETXTBSY;

// Open the log file
//
   if ((Logfd = open(fname, O_WRONLY | O_CREAT | O_APPEND | O_DSYNC, 0644)) < 0)
      return shao_ng_dts_Emsg("LogFile", -errno, "opening", fname);

// Set up for logging
//
   Logfn = strdup(fname);

// All done
//
   return 0;
}
  
/******************************************************************************/
/*                               M o n i t o r                                */
/******************************************************************************/
  
void shao_ng_dts_LogFile::Monitor(int fdnum, char *fdname)
{
   shao_ng_dts_LogFiler *lrP = new shao_ng_dts_LogFiler(this, fdname, fdnum);
   int retc;

// Start a log file thread (we loose storage upon failure)
//
   if ((retc = shao_ng_dts_Thread_Run(shao_ng_dts_FileLog, (void *)lrP, &(lrP->LogT))))
      {shao_ng_dts_Emsg("LogFile", errno, "start logging thread to", Logfn);
       return;
      }

// Chain this logger into out list of loggers
//
   Flog.Lock();
   lrP->Next = Loggers; Loggers = lrP;
   Flog.UnLock();
   DEBUG("Thread " <<lrP->LogT <<" assigned to logging " <<fdname);
}
 
/******************************************************************************/
/*                                R e c o r d                                 */
/******************************************************************************/

void shao_ng_dts_LogFile::Record(shao_ng_dts_LogFiler *lrP)
{
   static shao_ng_dts_Mutex logMutex;
   shao_ng_dts_Timer  Mytime;
   shao_ng_dts_Stream inData;
   char *inLine, tbuff[24];
   struct iovec iolist[3] = {{(caddr_t)tbuff, 0}, {0,0}, {(char *)"\n", 1}};
   int LogFD = lrP->LogF->Logfd;

// Attach the file descriptor to the stream
//
   inData.Attach(lrP->ErFD);

// Get a full line from the stream to avoid line splittage in the log and
// write it out to the lof file adding appropriate headers.
//
   while((inLine = inData.GetLine()))
        {if (!(*inLine)) continue;
         logMutex.Lock();
         if (LogFD)
            {tbuff[0] = '\0';
             iolist[0].iov_len   = Mytime.Format(tbuff);
             iolist[1].iov_base  = inLine;
             iolist[1].iov_len   = strlen(inLine);

             if (writev(LogFD, (const struct iovec *)&iolist, 3) < 0)
                {shao_ng_dts_Emsg("LogFile",errno,"writing log to",lrP->LogF->Logfn);
                 LogFD = 0;
                }
            } else cerr <<inLine <<endl;
         logMutex.UnLock();
        }
}
