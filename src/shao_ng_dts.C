/******************************************************************************/
/*                                                                            */
/*                                shao_ng_dts. C                                 */
/*                                                                            */
/*(c) 2010-14 by the Board of Trustees of the Leland Stanford, Jr., University*//*      All Rights Reserved. See shao_ng_dts_Version.C for complete License Terms    */
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
  
/* shao_ng_dts provides a secure fast parallel copy utility. See shao_ng_dts_Config.C for
   the actual list of options. The general syntax is:

   shao_ng_dts [options] inspec outspec
*/
/******************************************************************************/
/*                         i n c l u d e   f i l e s                          */
/******************************************************************************/
  
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <sys/param.h>

#include "shao_ng_dts_Args.h"
#include "shao_ng_dts_Config.h"
#include "shao_ng_dts_Debug.h"
#include "shao_ng_dts_FileSpec.h"
#include "shao_ng_dts_Headers.h"
#include "shao_ng_dts_LogFile.h"
#include "shao_ng_dts_Node.h"
#include "shao_ng_dts_ProcMon.h"
#include "shao_ng_dts_Protocol.h"
#include "shao_ng_dts_System.h"
#include "shao_ng_dts_Timer.h"
  
/******************************************************************************/
/*                    L O C A L   D E F I N I T I O N S                       */
/******************************************************************************/

#define Same(s1, s2) (s1 == s2 || (s1 && s2 && !strcmp(s1, s2)))

/******************************************************************************/
/*                      G l o b a l   V a r i a b l e s                       */
/******************************************************************************/

extern shao_ng_dts_Config   shao_ng_dts_Config;

extern shao_ng_dts_BuffPool shao_ng_dts_BuffPool;
  
extern shao_ng_dts_System   shao_ng_dts_OS;

/******************************************************************************/
/*                                  m a i n                                   */
/******************************************************************************/
  
int main(int argc, char *argv[], char *envp[])
{
   shao_ng_dts_Node     *Source, *Sink;
   shao_ng_dts_Protocol  Protocol;
   shao_ng_dts_FileSpec *fsp, *psp, *sfs, *tfs;
   int retc;
   int        TotFiles = 0;
   long long  TotBytes = 0;
   double     xRate;
   shao_ng_dts_Timer Elapsed_Timer;
   const char *xType;

// Process configuration file
//
   shao_ng_dts_OS.EnvP = envp;
   if (shao_ng_dts_Config.ConfigInit(argc, argv)) exit(1);

// Process the arguments
//
   shao_ng_dts_Config.Arguments(argc, argv);

// Process final source/sink actions here
//
   if (shao_ng_dts_Config.Options & (shao_ng_dts_SRC | shao_ng_dts_SNK))
      {int retc;
       shao_ng_dts_ProcMon theAgent;
       theAgent.Start(shao_ng_dts_OS.getGrandP());
           {shao_ng_dts_Node     SS_Node;
            retc = (shao_ng_dts_Config.Options & shao_ng_dts_SRC
                 ?  Protocol.Process(&SS_Node)
                 :  Protocol.Request(&SS_Node));
           }
       exit(retc);
      }

// Do some debugging here
//
   Elapsed_Timer.Start();
   if (shao_ng_dts_Debug.Trace > 2) shao_ng_dts_Config.Display();

// Allocate the source and sink node and common protocol
//
   Source = new shao_ng_dts_Node;
   Sink   = new shao_ng_dts_Node;
   tfs    = shao_ng_dts_Config.snkSpec;

// Allocate the log file
//
   if (shao_ng_dts_Config.Logfn)
      {shao_ng_dts_Config.MLog = new shao_ng_dts_LogFile();
       if (shao_ng_dts_Config.MLog->Open(shao_ng_dts_Config.Logfn)) exit(5);
      }

// Grab all source files for each particular user/host and copy them
//
   retc = 0;
   while(!retc && (psp = shao_ng_dts_Config.srcSpec))
      {fsp = psp->next;
       while(fsp && Same(fsp->username, psp->username)
             && Same(fsp->hostname, psp->hostname)) 
            {psp = fsp; fsp = fsp->next;}
       psp->next = 0;
       sfs = shao_ng_dts_Config.srcSpec;
       shao_ng_dts_Config.srcSpec = fsp;
         if (shao_ng_dts_Config.Options & shao_ng_dts_CON2SRC)
            retc = Protocol.Schedule(Source, sfs, 
                                     (char *)shao_ng_dts_Config.SrcXeq,
                                     (char *)shao_ng_dts_Config.SrcArg,
                                     Sink,   tfs,
                                     (char *)shao_ng_dts_Config.SnkXeq,
                                     (char *)shao_ng_dts_Config.SnkArg, Sink);
            else
            retc = Protocol.Schedule(Sink,   tfs,
                                     (char *)shao_ng_dts_Config.SnkXeq,
                                     (char *)shao_ng_dts_Config.SnkArg,
                                     Source, sfs,
                                     (char *)shao_ng_dts_Config.SrcXeq,
                                     (char *)shao_ng_dts_Config.SrcArg, Sink);
       TotFiles += Sink->TotFiles;
       TotBytes += Sink->TotBytes;
      }

// All done
//
   delete Source;
   delete Sink;

// Report final statistics if wanted
//
   DEBUG("Ending; rc=" <<retc <<" files=" <<TotFiles <<" bytes=" <<TotBytes);
   if (shao_ng_dts_Config.Options & shao_ng_dts_VERBOSE
   && !retc && TotFiles && TotBytes)
      {double ttime;
       char buff[128];
       Elapsed_Timer.Stop();
       Elapsed_Timer.Report(ttime);
       xRate = ((double)TotBytes)/ttime*1000.0; xType = shao_ng_dts_Config::Scale(xRate);
       sprintf(buff, "%.1f %sB/s", xRate, xType);
       cerr <<TotFiles <<(TotFiles != 1 ? " files" : " file");
       cerr <<" copied at effectively " <<buff <<endl;
      }
   if (shao_ng_dts_Config.MLog) delete shao_ng_dts_Config.MLog;
   exit(retc);
}
