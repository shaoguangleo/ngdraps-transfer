/******************************************************************************/
/*                                                                            */
/*                           shao_ng_dts_ N o d e . C                            */
/*                                                                            */
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
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include "shao_ng_dts_Config.h"
#include "shao_ng_dts_Emsg.h"
#include "shao_ng_dts_Node.h"
#include "shao_ng_dts_BuffPool.h"
#include "shao_ng_dts_Headers.h"
#include "shao_ng_dts_Network.h"
#include "shao_ng_dts_ProgMon.h"
#include "shao_ng_dts_Protocol.h"
#include "shao_ng_dts_Pthread.h"
#include "shao_ng_dts_System.h"
#include "shao_ng_dts_ZCX.h"
  
/******************************************************************************/
/*                      E x t e r n a l   O b j e c t s                       */
/******************************************************************************/
  
extern shao_ng_dts_Config   shao_ng_dts_Config;

extern shao_ng_dts_BuffPool shao_ng_dts_APool;
extern shao_ng_dts_BuffPool shao_ng_dts_BPool;
extern shao_ng_dts_BuffPool shao_ng_dts_CPool;

extern shao_ng_dts_Network  shao_ng_dts_Net;

extern shao_ng_dts_System   shao_ng_dts_OS;
 
/******************************************************************************/
/*            E x t e r n a l   T h r e a d   I n t e r f a c e s             */
/******************************************************************************/

extern "C"
{
void *shao_ng_dts_doCX(void *pp)
{
     shao_ng_dts_ZCX *cxp = (shao_ng_dts_ZCX *)pp;
     long retc = cxp->Process();
     return (void *)retc;
}
void *shao_ng_dts_doWrite(void *pp)
{
     shao_ng_dts_File *fp = (shao_ng_dts_File *)pp;
     long retc = fp->Write_All(shao_ng_dts_BPool, shao_ng_dts_Config.Streams);
     return (void *)retc;
}
void *shao_ng_dts_Buff2Net(void *link)
{
     shao_ng_dts_Link *netlink = (shao_ng_dts_Link *)link;
     long retc = netlink->Buff2Net();
     return (void *)retc;
}
void *shao_ng_dts_Net2Buff(void *link)
{
     shao_ng_dts_Link *netlink = (shao_ng_dts_Link *)link;
     long retc = netlink->Net2Buff();
     return (void *)retc;
}
void *shao_ng_dts_Connect(void *protp)
{
     shao_ng_dts_Protocol *protocol = (shao_ng_dts_Protocol *)protp;
     shao_ng_dts_Link     *link;
     int            retc;

     if (link = shao_ng_dts_Net.Connect(shao_ng_dts_Config.CBhost, shao_ng_dts_Config.CBport))
        {if ((retc = protocol->Login(link, 0)) < 0)
            {delete link; link = 0;}
        }
     return (void *)link;
}
}
  
/******************************************************************************/
/*                           C o n s t r u c t o r                            */
/******************************************************************************/
  
shao_ng_dts_Node::shao_ng_dts_Node(shao_ng_dts_Link *netLink)
{

// If this is a local node, do a simple setup
//
   if (!netLink)
      {nodename = 0; data_link[0] = 0; iocount = dlcount = 0; return;}

// For remote nodes, attach this node to the network link
//
   nodename = strdup(netLink->LinkName());
   NStream.Attach(netLink->FD());
   data_link[0] = netLink;
   dlcount = 1;
   comp1File = comp2File = 0;
   TotFiles = 0;
   TotBytes = 0;
}

/******************************************************************************/
/*                            g e t B u f f e r s                             */
/******************************************************************************/
  
int shao_ng_dts_Node::getBuffers(int isTrg, int isLZO)
{

// Allocate the buffers
//
   return shao_ng_dts_BPool.Allocate(shao_ng_dts_Config.BNum, shao_ng_dts_Config.RWBsz, isTrg);
}

/******************************************************************************/
/*                               G e t L i n e                                */
/******************************************************************************/

char *shao_ng_dts_Node::GetLine()
{
   char *lp = NStream.GetLine();

   DEBUG("Received from " <<nodename <<": " <<lp <<endl);

   return lp;
}
  
/******************************************************************************/
/*                                   P u t                                    */
/******************************************************************************/

int shao_ng_dts_Node::Put(char *data[], int dlen[])
{
   static shao_ng_dts_Mutex putMutex;

   if (DEBUGON)
      {int i= 0;
       cerr <<"shao_ng_dts_" <<shao_ng_dts_Debug.Who <<": Sending to " <<nodename <<": ";
       while(data[i])if (*data[i]) cerr <<data[i++];
                        else i++;
       cerr <<endl;
      }

   if (shao_ng_dts_Config.Options & shao_ng_dts_SRC) return NStream.Put(data, dlen);
      else {int rc;
            putMutex.Lock();
            rc = NStream.Put(data, dlen);
            putMutex.UnLock();
            return rc;
           }
}

/******************************************************************************/
/*                                   R u n                                    */
/******************************************************************************/
  
int shao_ng_dts_Node::Run(char *user, char *host, char *prog, char *parg)
{
   static char ipv4[] = {'-','4','\0'};
   int fderr, numa = 0;
   char *username, *sshDest, bufDest[264], *Argv[1024], *ap, *pp = prog;
   const int ArgvSize = sizeof(Argv)/sizeof(char *)-2;

// Free up any node name here
//
   if (nodename) free(nodename);
   nodename = strdup(host ? host : shao_ng_dts_Config.MyHost);
   username = (user ? user : shao_ng_dts_Config.MyUser);

// Check for an IPV6 address as ssh does not follow the rfc standard
//
   if (*nodename != '[') sshDest = nodename;
      else {int i = strlen(nodename);
            if (i > (int)sizeof(bufDest)) return -EHOSTUNREACH;
            strcpy(bufDest, nodename+1);
            bufDest[i-2] = 0; sshDest= bufDest;
           }

// Break up the command line and perform substitutions
//
   if (!(user || host)) {Argv[0] = shao_ng_dts_Config.MyProg; numa = 1;}
      else for (numa = 0; *pp && numa < ArgvSize; numa++)
               {while(*pp && *pp == ' ') pp++;
                ap = pp;
                while(*pp && *pp != ' ') pp++;
                if (*pp) {*pp = '\0'; pp++;}
                if (*ap == '%' && !ap[2])
                   {     if (ap[1] == 'I')
                            {if (shao_ng_dts_Config.IDfn)
                                {Argv[numa++] = (char *)"-i";
                                 Argv[numa] = shao_ng_dts_Config.IDfn;}
                                else numa--;
                            }
                    else if (ap[1] == 'U') Argv[numa] = username;
                    else if (ap[1] == 'H') Argv[numa] = sshDest;
                    else if (ap[1] == '4')
                            {if (shao_ng_dts_Config.Options & shao_ng_dts_IPV4)
                                           Argv[numa] = ipv4;
                                else numa--;
                            }
                    else                   Argv[numa] = ap;
                   }
                    else Argv[numa] = ap;
               }

// Complete argument list to start the actual copy program
//
   if (numa >= ArgvSize) return shao_ng_dts_Emsg("Run", -E2BIG, "starting", prog);
   Argv[numa++] = parg;
   Argv[numa]   = 0;

// Invoke the program
//
   if ((fderr=NStream.Exec(Argv, 1, shao_ng_dts_Config.MLog != 0)) < 0) return -fderr;

// Perform logging function here
//
   if (shao_ng_dts_Config.MLog) shao_ng_dts_Config.MLog->Monitor(fderr, parg);

// Perform debugging here
//
   if (DEBUGON)
      {int i;
       cerr <<"shao_ng_dts_" <<shao_ng_dts_Debug.Who <<": Running as pid " <<NStream.getPID() <<": ";
       for (i = 0; i < numa; i++) if (Argv[i]) cerr <<Argv[i] <<' ';
       cerr <<endl;
      }

// All done
//
   return 0;
}
  
/******************************************************************************/
/*                                  S t o p                                   */
/******************************************************************************/

void shao_ng_dts_Node::Stop(int Report)
{
   int i;

// If window reporting wanted do so only if very verbose and autotuning
//
   if (shao_ng_dts_Config.Options & shao_ng_dts_BLAB && shao_ng_dts_Net.AutoTune() && data_link[0])
      chkWsz(data_link[0]->FD(), 1);

// Kill any attached process
//
   NStream.Kill();

// Dismantle all of the data connections
//
   for (i = 0; i < dlcount; i++)
       if (data_link[i]) {delete data_link[i]; data_link[i] = 0;}
}
 
/******************************************************************************/
/*                                  W a i t                                   */
/******************************************************************************/

int shao_ng_dts_Node::Wait(shao_ng_dts_Node *other)
{
    pid_t pvec[3];
    shao_ng_dts_Stream *pstrm[2];
    int retc, i=0;

// Collect all of the nodes we will be waiting for
//
   if ((pvec[i] = NStream.getPID()) >= 0) 
      {pstrm[i] = &NStream; i++;}
   if (other && (pvec[i] = other->NStream.getPID()) >= 0)
      {pstrm[i] = &(other->NStream); i++;}
   if (!i) return 0;
   pvec[i] = 0;

// Issue wait and return the status
//
   do {retc = shao_ng_dts_OS.Waitpid(pvec, &i);
       pstrm[i]->clrPID();
       if (!retc)
          {if (!i) {pstrm[0] = pstrm[1]; pvec[0] = pvec[1];}
           pvec[1] = 0;
          }
      } while (!retc && pvec[0]);
   return retc;
}
  
/******************************************************************************/
/*                 D a t a   T r a n s f e r   M e t h o d s                  */
/******************************************************************************/
/******************************************************************************/
/*                              R e c v F i l e                               */
/******************************************************************************/

int shao_ng_dts_Node::RecvFile(shao_ng_dts_FileSpec *fp, shao_ng_dts_Node *Remote)
{
   static const int wOnly = S_IWUSR;

   const char *Args = 0, *Act = "opening", *Path = fp->targpath;
   long tretc = 0;
   int i, oflag, retc, Mode = wOnly, progtid = 0;
   long long startoff = 0;
   pid_t Child[2] = {0,0};
   shao_ng_dts_File *outFile, *seqFile = 0;
   shao_ng_dts_ZCX *cxp = 0;
   pthread_t tid, link_tid[shao_ng_dts_MAXSTREAMS+4];
   shao_ng_dts_Timer Elapsed_Timer;
   shao_ng_dts_ProgMon *pmp = 0;
   float CRatio;

// Perform Force or Append processing
//
        if (shao_ng_dts_Config.Options & shao_ng_dts_XPIPE)
           {oflag = O_WRONLY;
            Path = shao_ng_dts_Config.snkSpec->pathname;
            Args = shao_ng_dts_Config.snkSpec->fileargs;
            if (shao_ng_dts_Config.snkSpec->Info.Otype != 'p') Act = "running";
               else {Mode |= S_IFIFO;
                     if (Args)
                        {shao_ng_dts_Fmsg("RecvFile",
                                   "Spaces disallowed in named output pipe",Path);
                         return -EINVAL;
                        }
                    }
           }
   else if (shao_ng_dts_Config.Options & shao_ng_dts_FORCE)
           {if (!(shao_ng_dts_Config.Options & shao_ng_dts_NOUNLINK))
               fp->FSys()->RM(Path);
            oflag = O_WRONLY | O_CREAT | O_TRUNC;
           }
   else if (shao_ng_dts_Config.Options & shao_ng_dts_APPEND)
           {if (retc = fp->WriteSigFile()) return retc;
            if (startoff = fp->targetsz) oflag = O_WRONLY;
               else oflag = O_CREAT | O_WRONLY;
           }
   else    oflag = O_WRONLY | O_CREAT | O_EXCL;

// Establish mode, we normally make the file write-only
//
   if ( shao_ng_dts_Config.Options &  shao_ng_dts_RTCSNK
   && !(shao_ng_dts_Config.Options & (shao_ng_dts_RTCHIDE|shao_ng_dts_XPIPE)))
      Mode = shao_ng_dts_Config.Mode|S_IWUSR|S_ISUID;

// Tell the user what we are bout to do
//
   if (shao_ng_dts_Config.Options & shao_ng_dts_BLAB | shao_ng_dts_Config.Progint)
      if (shao_ng_dts_Config.Options & shao_ng_dts_APPEND) 
         {char buff[32];
          sprintf(buff, "%lld", startoff);
          shao_ng_dts_Fmsg("RecvFile","Appending to",Path,"at offset",buff);
         }
         else shao_ng_dts_Fmsg("RecvFile", "Creating", Path);
      else DEBUG("Receiving " <<fp->pathname <<" as " <<Path <<" offset=" <<startoff);

// Receive the file in a sub-process so that we don't muck with this one
//
   if ((Child[0] = shao_ng_dts_OS.Fork()) < 0)
      return shao_ng_dts_Emsg("RecvFile", errno, "forking to create", Path);
   if (Child[0]) 
      {char buff[128];
       Parent_Monitor.Start(0,Remote);
       DEBUG("Waiting for child " <<Child[0] <<" to finish");
       retc = shao_ng_dts_OS.Waitpid(Child);
       Parent_Monitor.Stop();
       if (shao_ng_dts_Config.Options & shao_ng_dts_BLAB)
          write(STDERR_FILENO, buff, Usage("Target", buff, sizeof(buff)));
       return retc;
      }

/*******************************************************************************
   (net)->data_link[i]->BPool->CStage[1]->CStage[0]->CPool->outFile->(file)
*******************************************************************************/

// Set Concurrency
//
   shao_ng_dts_Thread_MT(shao_ng_dts_Config.MTLevel);

// Request direct I/O if so wanted
//
   if (shao_ng_dts_Config.Options & shao_ng_dts_ODIO) {fp->FSys()->DirectIO(1);
       DEBUG("Direct output requested.");}

// Open the file and set the starting offset
//
   Elapsed_Timer.Start();
   if (!(outFile = fp->FSys()->Open(Path, oflag, Mode, Args)))
      return shao_ng_dts_Emsg("RecvFile", errno, Act, Path);
   if (startoff && ((retc = outFile->Seek(startoff)) < 0))
      return shao_ng_dts_Emsg("RecvFile",retc,"setting write offset for",Path);
   outFile->setSize(fp->Info.size);

// If compression is wanted, set up the compression objects
//
   if (shao_ng_dts_Config.Options & shao_ng_dts_COMPRESS 
   && !(cxp = setup_CX(0, outFile->ioFD()))) return -ECANCELED;

// Start a thread for each data link we have
//
   for (i = 0; i < dlcount; i++)
       {if ((retc = shao_ng_dts_Thread_Start(shao_ng_dts_Net2Buff, 
                                (void *)data_link[i], &tid))<0)
           {shao_ng_dts_Emsg("RecvFile",retc,"starting net thread for",Path);
            _exit(100);
           }
        link_tid[i] = tid;
        DEBUG("Thread " <<tid <<" assigned to stream " <<i);
       }

// If we are compressing start the sequence thread now
//
   if (shao_ng_dts_Config.Options & shao_ng_dts_COMPRESS)
      {seqFile = new shao_ng_dts_File(Path, 0, 0);
       if ((retc = shao_ng_dts_Thread_Start(shao_ng_dts_doWrite, (void *)seqFile, &tid))<0)
          {shao_ng_dts_Emsg("RecvFile",retc,"starting disk thread for",Path);
           _exit(100);
          }
       link_tid[dlcount++] = tid;
       DEBUG("Thread " <<tid <<" assigned to sequencer as stream " <<i);
      }

// Start the parent process monitor. It is stopped at exit.
//
   Parent_Monitor.Start();

// If a periodic progress message is wanted, start a progress thread
//
   if (shao_ng_dts_Config.Progint) 
      {pmp = new shao_ng_dts_ProgMon();
       pmp->Start(outFile, cxp, shao_ng_dts_Config.Progint, fp->Info.size-startoff);
      }

// Write the whole file
//
   if (shao_ng_dts_Config.Options & shao_ng_dts_COMPRESS)
           retc = outFile->Write_All(shao_ng_dts_APool, 1);
      else retc = outFile->Write_All(shao_ng_dts_BPool, shao_ng_dts_Config.Streams);
   DEBUG("File write ended; rc=" <<retc);

// Wait for the expansion thread to end
//
   if (shao_ng_dts_Config.Options & shao_ng_dts_COMPRESS)
      {if (tretc = (long)shao_ng_dts_Thread_Wait(cxp->TID)) retc = 128;
       DEBUG("File expansion ended; rc=" <<tretc);
      }

// Kill the progress monitor
//
   if (pmp) 
      {DEBUG("Deleting progress monitor");
       delete pmp;
      }

// Make sure each thread has terminated normally
//
   for (i = 0; i < dlcount; i++)
       {if (tretc = (long)shao_ng_dts_Thread_Wait(link_tid[i])) retc = 128;
        DEBUG("Thread " <<link_tid[i] <<" stream " <<i <<" ended; rc=" <<tretc);
       }

// Make sure that all of the bytes were transfered
//
   if (!retc && strncmp(Path, "/dev/null/", 10))
      {shao_ng_dts_FileInfo Info;
       if ((retc = fp->FSys()->Stat(Path, &Info)) < 0)
          {retc = -retc;
           shao_ng_dts_Emsg("RecvFile", retc, "finding", Path);
          }
          else if (Info.size != fp->Info.size && Info.mode
               &&  !(shao_ng_dts_Config.Options & shao_ng_dts_NOFSZCHK))
                  {const char *What = (Info.size < fp->Info.size
                                    ?  "Not all" : "Too much");
                   retc = 29;
                   shao_ng_dts_Fmsg("RecvFile",What,"data was transfered for",Path);
                   DEBUG("src size=" <<fp->Info.size <<" snk size=" <<Info.size);
                  }
      } DEBUG("Outfile " <<Path <<" closed");

// Report detailed I/O stats, if so wanted
//
   Elapsed_Timer.Stop();
   if (!retc && shao_ng_dts_Config.Options & shao_ng_dts_VERBOSE)
      {double ttime;
       Elapsed_Timer.Report(ttime);
       Report(ttime, fp, outFile, cxp);
      }

// All done
//
   Parent_Monitor.Stop();
                delete outFile;
   if (cxp)     delete(cxp);
   if (seqFile) delete(seqFile);
   retc = fp->Finalize(retc);
   close(1); close(2);
   DEBUG("Process " <<getpid() <<" exiting with rc=" <<retc);
   exit(retc);
   return(retc);  // some compilers insist on a return in int functions
}

/******************************************************************************/
/*                              S e n d F i l e                               */
/******************************************************************************/

int shao_ng_dts_Node::SendFile(shao_ng_dts_FileSpec *fp)
{
   const char *Act = "opening";
   int i, retc, tretc = 0, Mode = 0;
   pid_t Child[2] = {0,0};
   shao_ng_dts_File *inFile;
   shao_ng_dts_ProcMon *TLimit = 0;
   shao_ng_dts_ZCX *cxp;
   pthread_t tid, link_tid[shao_ng_dts_MAXSTREAMS+1];

// Set open options (check for pipes)
//
   if (shao_ng_dts_Config.Options & shao_ng_dts_XPIPE)
      {if (fp->Info.Otype == 'p') Mode = S_IFIFO;
          else Act = "running";
      }

// Send the file in a sub-process so that we don't muck with this one
//
   DEBUG("Sending file " <<fp->targpath <<"; offset=" <<fp->targetsz);
   if ((Child[0] = shao_ng_dts_OS.Fork()) < 0)
      return shao_ng_dts_Emsg("SendFile", errno, "forking to send", fp->pathname);
   if (Child[0])
      {char buff[128];
       Parent_Monitor.Start();
       retc = shao_ng_dts_OS.Waitpid(Child);
       Parent_Monitor.Stop();
       if (shao_ng_dts_Config.Options & shao_ng_dts_BLAB)
          write(STDERR_FILENO, buff, Usage("Source", buff, sizeof(buff)));
       return retc;
      }

/*******************************************************************************
    (file)->inFile->CPool->CStage[0]->CStage[1]->BPool->data_link[i]->(net)
*******************************************************************************/

// Set Concurrency
//
   shao_ng_dts_Thread_MT(shao_ng_dts_Config.MTLevel);

// Request direct I/O if so wanted
//
   if (shao_ng_dts_Config.Options & shao_ng_dts_IDIO) {fp->FSys()->DirectIO(1);
       DEBUG("Direct input requested.");}

// Open the input file and set starting offset
//
   if (!(inFile = fp->FSys()->Open(fp->pathname,O_RDONLY,Mode,fp->fileargs)))
      {shao_ng_dts_Emsg("SendFile", errno, Act, fp->pathname);
       exit(2);
      }
   if (fp->targetsz && ((retc = inFile->Seek(fp->targetsz)) < 0))
      return shao_ng_dts_Emsg("SendFile",retc,"setting read offset for",fp->pathname);

// If compression is wanted, set up the compression objects
//
   if (shao_ng_dts_Config.Options & shao_ng_dts_COMPRESS 
   && !(cxp = setup_CX(1, inFile->ioFD()))) return -ECANCELED;

// Start a thread for each data link we have
//
   for (i = 0; i < dlcount; i++)
       {if ((retc = shao_ng_dts_Thread_Start(shao_ng_dts_Buff2Net, 
                                (void *)data_link[i], &tid))<0)
           {shao_ng_dts_Emsg("SendFile",retc,"starting net thread for",fp->pathname);
            _exit(100);
           }
        link_tid[i] = tid;
        if (i >= iocount) {DEBUG("Thread " <<tid <<" assigned to data clocker");}
           else {DEBUG("Thread " <<tid <<" assigned to stream " <<i);}
       }

// Start the parent monitor
//
   Parent_Monitor.Start();

// Start the Transfer Time Limit
//
   if (shao_ng_dts_Config.TimeLimit)
      {TLimit = new shao_ng_dts_ProcMon();
       TLimit->Start(shao_ng_dts_Config.TimeLimit, &shao_ng_dts_BPool);
      }

// Read the whole file
//
   if (shao_ng_dts_Config.Options & shao_ng_dts_COMPRESS) retc=inFile->Read_All(shao_ng_dts_APool,1);
      else retc = inFile->Read_All(shao_ng_dts_BPool, shao_ng_dts_Config.Bfact);
   DEBUG("File read ended; rc=" <<retc);

// Wait for compression thread to end
//
   if (shao_ng_dts_Config.Options & shao_ng_dts_COMPRESS)
      {if (tretc = (long)shao_ng_dts_Thread_Wait(cxp->TID)) retc = 128;
       DEBUG("File compression ended; rc=" <<tretc);
       delete cxp;
      }

// Make sure each link thread has terminated normally.
//
   for (i = 0; i < iocount; i++)
       {if (tretc = (long)shao_ng_dts_Thread_Wait(link_tid[i])) retc = 128;
        DEBUG("Thread " <<link_tid[i] <<" stream " <<i <<" ended; rc=" <<tretc);
       }

// All done
//
   if (TLimit) delete TLimit;
   Parent_Monitor.Stop();
   delete inFile;
   close(1); close(2);
   DEBUG("Process " <<getpid() <<" exiting with rc=" <<retc);
   exit(retc);
   return(retc);  // some compilers insist on a return in int functions
}

/******************************************************************************/
/*                       P r i v a t e   M e t h o d s                        */
/******************************************************************************/
/******************************************************************************/
/*                                c h k W s z                                 */
/******************************************************************************/
  
void shao_ng_dts_Node::chkWsz(int fd, int Final)
{
   int wbsz = shao_ng_dts_Net.getWBSize(fd, shao_ng_dts_Config.Options & shao_ng_dts_SRC);
   const char *fmode = (shao_ng_dts_Config.Options & shao_ng_dts_SRC ? "send"   : "recv");
   const char *smode = (shao_ng_dts_Config.Options & shao_ng_dts_SRC ? "Source" : "Target");
   const char *Wtype;
   char mbuff[256];
   int n;

// Figure out window type
//
   if (Final) Wtype = "a final";
      else    Wtype = (shao_ng_dts_Net.AutoTune() ? "initial" : "a fixed");

// Issue message
//
   n = sprintf(mbuff, "%s %s using %s %s window of %d\n",
                      smode, shao_ng_dts_Config.MyHost, Wtype, fmode, wbsz);
   write(STDERR_FILENO, mbuff, n);
}

/******************************************************************************/
/*                             I n c o m m i n g                              */
/******************************************************************************/
  
int shao_ng_dts_Node::Incomming(shao_ng_dts_Protocol *protocol)
{
    int i, retc, minport, maxport;
    shao_ng_dts_Link *link;

// Find the port number we should use for incomming connections
//
   shao_ng_dts_Net.findPort(minport, maxport);

// Set up the default ports first if we didn't find specified ones
//
   if (minport || maxport
   || ((retc = shao_ng_dts_Net.Bind(shao_ng_dts_DFLTMINPORT, shao_ng_dts_DFLTMAXPORT, 1, -1)) < 0))
   if ((retc = shao_ng_dts_Net.Bind(minport, maxport,
               shao_ng_dts_Config.bindtries, shao_ng_dts_Config.bindwait)) < 0)
      return retc;

// Report the port number we have chosen
//
   protocol->setCBPort(retc);

// Establish all of the connections
//
   while(dlcount < shao_ng_dts_Config.Streams)
       {if (!(link = shao_ng_dts_Net.Accept())) break;
        if (!(retc = protocol->Login(link, 1))) 
           {link->LinkNum = dlcount; data_link[dlcount++] = link;}
           else if (retc < 0) {delete link; break;}
       }

// Unbind the network and make sure we have all of the agreed upon links
//
   shao_ng_dts_Net.unBind();
   if (dlcount < shao_ng_dts_Config.Streams) return Recover("Accept");
   iocount = dlcount;

// Initialize the buddy pipeline; a patented way of ensuring maximum parallelism
//
   if (dlcount > 1 && (shao_ng_dts_Config.Options & (shao_ng_dts_SRC|shao_ng_dts_ORDER)))
      {i = dlcount-1;
       data_link[i]->setBuddy(data_link[0]);
       while(i--) data_link[i]->setBuddy(data_link[i+1]);
       shao_ng_dts_Link::setNudge();
      }

// Determine what the actual window size is (only if verbose)
//
   if (shao_ng_dts_Config.Options & shao_ng_dts_BLAB) chkWsz(data_link[0]->FD());
   return 0;
}
  
/******************************************************************************/
/*                              O u t g o i n g                               */
/******************************************************************************/
  
int shao_ng_dts_Node::Outgoing(shao_ng_dts_Protocol *protocol)
{
   int i, retc;
   shao_ng_dts_Link *link;

// Establish the control connection first
//
   if (link = shao_ng_dts_Net.Connect(shao_ng_dts_Config.CBhost, shao_ng_dts_Config.CBport, 3))
      if ((retc = protocol->Login(link, 0)) < 0)
         {delete link; link = 0;}

// Now establish all data channels if we have a control channel
//
   if (link)
      {pthread_t tid, T_id[shao_ng_dts_MAXSTREAMS];

       // Start threads for data connections
       //
       for (i = 0; i < shao_ng_dts_Config.Streams; i++)
           {if ((retc=shao_ng_dts_Thread_Start(shao_ng_dts_Connect,(void *)protocol,&tid))<0)
               {shao_ng_dts_Emsg("Outgoing", retc, "starting connect thread");
                _exit(100);
               }
            T_id[i] = tid;
#ifdef CRAY_X1E
            usleep(1);
#endif
           }
       for (i = 0; i < shao_ng_dts_Config.Streams; i++)
           {if (!(link = (shao_ng_dts_Link *)shao_ng_dts_Thread_Wait(T_id[i]))) break;
            link->LinkNum = dlcount; data_link[dlcount++] = link;
           }
       }
   iocount = dlcount;

// Make sure we have all of the required links
//
   if (dlcount < shao_ng_dts_Config.Streams) return Recover("Connect");

// Determine what the actual window size is (only if verbose)
//
   if (shao_ng_dts_Config.Options & shao_ng_dts_BLAB) chkWsz(data_link[0]->FD());

// Initialize the buddy pipeline; a patented way of ensuring maximum parallelism
//
   if (dlcount > 1 && (shao_ng_dts_Config.Options & (shao_ng_dts_SRC|shao_ng_dts_ORDER)))
      {i = dlcount-1;
       data_link[i]->setBuddy(data_link[0]);
       while(i--) data_link[i]->setBuddy(data_link[i+1]);
       shao_ng_dts_Link::setNudge();
      }
   return 0;
}
  
/******************************************************************************/
/*                               R e c o v e r                                */
/******************************************************************************/

int shao_ng_dts_Node::Recover(const char *who)
{
    char mbuff[256];
    sprintf(mbuff, "%d of %d data streams.", dlcount, shao_ng_dts_Config.Streams);
    shao_ng_dts_Fmsg(who, "Unable to allocate more than", mbuff);
    while(dlcount) data_link[--dlcount]->Close();
    return -1;
}
 
/******************************************************************************/
/*                                R e p o r t                                 */
/******************************************************************************/
  
void shao_ng_dts_Node::Report(double ttime, shao_ng_dts_FileSpec *fp, shao_ng_dts_File *ioFile,
                       shao_ng_dts_ZCX *cxp)
{
float  cratio;
double xtime, xrate;
long long xbytes, cxbytes;
const char *xType;
char buff[128], Line[2048];
int n;

// Calculate compression ratio
//
   xbytes = ioFile->Stats(xtime);
   if (cxp)
      {if (!(cxbytes = cxp->Bytes())) cratio = 0.0;
          else cratio = ((float)(xbytes*10/cxbytes))/10.0;
       sprintf(buff, " compressed %.1f", cratio);
      } else *buff = 0;

// Print the summary
//
   xrate = ((double)xbytes)/ttime*1000.0; xType = shao_ng_dts_Config::Scale(xrate);
   n = sprintf(Line, "File %s created; %lld bytes at %.1f %sB/s%s\n",
               fp->targpath, xbytes, xrate, xType, buff);
   write(STDERR_FILENO, Line, n);
   if (!(shao_ng_dts_Config.Options & shao_ng_dts_BLAB)) return;

// Tell user how many reorder events there were
//
   n = sprintf(Line, "%d buffers used with %d reorders; peaking at %d.\n",
              shao_ng_dts_BPool.BuffCount(), ioFile->bufreorders, ioFile->maxreorders);
   write(STDERR_FILENO, Line, n);
}
 
/******************************************************************************/
/*                     s e t u p _ C o m p r e s s i o n                      */
/******************************************************************************/
  
shao_ng_dts_ZCX *shao_ng_dts_Node::setup_CX(int deflating, int iofd)
{
   int retc, clvl;
   shao_ng_dts_BuffPool *ibp, *rbp, *obp;
   shao_ng_dts_ZCX *cxp;
   pthread_t tid;

// Initialize the processing parameters
//
   if (deflating)
      {ibp = &shao_ng_dts_APool;
       rbp = &shao_ng_dts_APool;
       obp = &shao_ng_dts_BPool;
       clvl = (shao_ng_dts_Config.Complvl ? shao_ng_dts_Config.Complvl : 1);
      } else {
       ibp = &shao_ng_dts_CPool;
       rbp = &shao_ng_dts_BPool;
       obp = &shao_ng_dts_APool;
       clvl = 0;
      }

// Allocate buffers in the A pool
//
   if (shao_ng_dts_APool.Allocate(shao_ng_dts_Config.BNum, shao_ng_dts_Config.RWBsz, !deflating))
      return 0;

// Allocate a new compression/expansion object
//
   cxp = new shao_ng_dts_ZCX(ibp, rbp, obp, clvl, iofd,
             (int)(shao_ng_dts_Config.Options & (clvl ? shao_ng_dts_LOGCMP : shao_ng_dts_LOGEXP)));

// Start the compression/expansion thread
//
   if ((retc = shao_ng_dts_Thread_Start(shao_ng_dts_doCX, (void *)cxp, &tid))<0)
      {shao_ng_dts_Emsg("File", retc, "starting", 
                 (char *)(deflating ? "compression" : "expansion"),
                 (char *)" thread.");
       _exit(100);
      }
   cxp->TID = tid;
   DEBUG("Thread " <<tid <<" assigned to cx stage.");

// Return the compression object
//
   return cxp;
}
 
/******************************************************************************/
/*                                 U s a g e                                  */
/******************************************************************************/
  
int shao_ng_dts_Node::Usage(const char *who, char *buff, int blen)
{
      int   Tsec, Ssec, Usec;

      Tsec = shao_ng_dts_OS.Usage(Ssec, Usec);

      return snprintf(buff, blen, "%s cpu=%.3f (sys=%.3f usr=%.3f).\n", who,
             ((float)Tsec)/1000.0, ((float)Ssec)/1000.0, ((float)Usec)/1000.0);
}
