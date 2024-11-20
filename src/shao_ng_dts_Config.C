/******************************************************************************/
/*                                                                            */
/*                         shao_ng_dts_ C o n f i g . C                          */
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

/*
   The routines in this file handle oofs() initialization. They get the
   configuration values either from configuration file or oofs_config.h (in that
   order of precedence).

   These routines are thread-safe if compiled with:
   AIX: -D_THREAD_SAFE
   SUN: -D_REENTRANT
*/
  
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <netdb.h>
#include <signal.h>
#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/uio.h>

#define  shao_ng_dts_CONFIG_DEBUG
#define  shao_ng_dts_IOMANIP
#include "shao_ng_dts_Platform.h"
#include "shao_ng_dts_Args.h"
#include "shao_ng_dts_Config.h"
#include "shao_ng_dts_Debug.h"
#include "shao_ng_dts_Emsg.h"
#include "shao_ng_dts_Headers.h"
#include "shao_ng_dts_NetLogger.h"
#include "shao_ng_dts_Network.h"
#include "shao_ng_dts_Platform.h"
#include "shao_ng_dts_Stream.h"
#include "shao_ng_dts_System.h"

#include "shao_ng_dts_Version.h"

/******************************************************************************/
/*                               d e f i n e s                                */
/******************************************************************************/

#define TS_Xeq(x,m)   if (!strcmp(x,var)) return m(Config);

#define TS_Str(x,m)   if (!strcmp(x,var)) {free(m); m = strdup(val); return 0;}

#define TS_Chr(x,m)   if (!strcmp(x,var)) {m = val[0]; return 0;}

#define TS_Bit(x,m,v) if (!strcmp(x,var)) {m |= v; return 0;}

/******************************************************************************/
/*                        G l o b a l   O b j e c t s                         */
/******************************************************************************/

       shao_ng_dts_Config    shao_ng_dts_Config;

       shao_ng_dts_Debug     shao_ng_dts_Debug;

       shao_ng_dts_NetLogger shao_ng_dts_NetLog;

extern shao_ng_dts_Network   shao_ng_dts_Net;

extern shao_ng_dts_System    shao_ng_dts_OS;

extern shao_ng_dts_Version   shao_ng_dts_Version;

extern const char    *shao_ng_dts_License;

       const char    *shao_ng_dts_HostName = "localhost";

namespace
{
       char lclHost[] = {'l','o','c','a','l','h','o','s','t','\0'};
};
  
/******************************************************************************/
/*                           C o n s t r u c t o r                            */
/******************************************************************************/

shao_ng_dts_Config::shao_ng_dts_Config()
{
   SrcBuff   = 0;
   SrcBase   = 0;
   SrcUser   = 0;
   SrcHost   = 0;
   SrcBlen   = 0;
   srcPath   = 0;
   srcSpec   = 0;
   srcLast   = 0;
   snkSpec   = 0;
   SynSpec   = 0;
   CBhost    = 0;
   CBport    = 0;
   CopyOpts  = 0;
   CopyOSrc  = 0;
   CopyOTrg  = 0;
   LogSpec   = 0;
   PorSpec   = 0;
   RepSpec   = 0;
   bindtries = 1;
   bindwait  = 0;
   Options   = 0;
   Mode      = S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH;
   ModeD     = 0;
   ModeDC    = Mode   |S_IXUSR|S_IXGRP|S_IXOTH;
   BAdd      = 0;
   Bfact     = 0;
   BNum      = 0;
   Streams   = 4;
   Xrate     = 0;
   Complvl   = 1;
   Progint   = 0;
   RWBsz     = 0;
// SrcXeq    = strdup("ssh -x -a -oFallBackToRsh=no -oServerAliveInterval=10 "
   SrcXeq    = strdup("ssh -x -a -oFallBackToRsh=no "
                      "%4 %I -l %U %H shao_ng_dts");
// SnkXeq    = strdup("ssh -x -a -oFallBackToRsh=no -oServerAliveInterval=10 "
   SnkXeq    = strdup("ssh -x -a -oFallBackToRsh=no "
                      "%4 %I -l %U %H shao_ng_dts");
   Logurl    = 0;
   Logfn     = 0;
   MLog      = 0;
   MyAddr    = lclHost;
   MyHost    = lclHost;
   MyUser    = shao_ng_dts_OS.UserName();
   MyProg    = 0;
   SecToken  = Rtoken();
   MaxSegSz  = shao_ng_dts_Net.MaxSSize();
   Wsize     = 131072;
   MaxWindow = 0;
   lastseqno = 0;
   SrcArg    = "SRC";
   SnkArg    = "SNK";
   CKPdir    = 0;
   IDfn      = 0;
   TimeLimit = 0;
   MTLevel   = 0;
   csOpts    = 0;
   csSize    = 16;
   csType    = shao_ng_dts_csMD5;
   csSpec    = 0;
  *csString  = 0;
   csPath    = 0;
   csFD      = -1;
   rtCheck   = 3;
   rtLimit   = 0;
   rtLockd   = -1;
   rtLockf   = 0;
   ubSpec[0] = ' '; ubSpec[1] = ' '; ubSpec[2] = 0;
   upSpec[0] = ' '; upSpec[1] = ' '; upSpec[2] = 0;
}

/******************************************************************************/
/*                            D e s t r u c t o r                             */
/******************************************************************************/

shao_ng_dts_Config::~shao_ng_dts_Config()
{
   shao_ng_dts_FileSpec *nsp, *fsp;

// Delete all file spec objects chained from us
//
   fsp = srcPath;
   while(fsp) {nsp = fsp->next; delete fsp; fsp = nsp;}
   fsp = srcSpec;
   while(fsp) {nsp = fsp->next; delete fsp; fsp = nsp;}
   fsp = snkSpec;
   while(fsp) {nsp = fsp->next; delete fsp; fsp = nsp;}

// Delete all character strings
//
   if (SrcBuff)  free(SrcBuff);
   if (SrcXeq)   free(SrcXeq);
   if (SnkXeq)   free(SnkXeq);
   if (Logurl)   free(Logurl);
   if (Logfn)    free(Logfn);
   if (LogSpec)  free(LogSpec);
   if (PorSpec)  free(PorSpec);
   if (CBhost)   free(CBhost);
   if (CopyOpts) free(CopyOpts);
   if (SecToken) free(SecToken);
   if (MyHost && MyHost != lclHost) free(MyHost);
   if (MyUser && MyUser != lclHost) free(MyUser);
   if (MyProg)   free(MyProg);
   if (CKPdir)   free(CKPdir);
   if (csSpec)   free(csSpec);
   if (rtLockf)  free(rtLockf);
}
/******************************************************************************/
/*                             A r g u m e n t s                              */
/******************************************************************************/

#define Add_Opt(x) {cbp[0]=' '; cbp[1]='-'; cbp[2]=x; cbp[3]='\0'; cbp+=3;}
#define Add_Num(x) {cbp[0]=' '; cbp=n2a(x,&cbp[1]);}
#define Add_Nul(x) {cbp[0]=' '; cbp=n2a(x,&cbp[1],"=%d");}
#define Add_Nup(x) {cbp[0]=' '; cbp=n2a(x,&cbp[1],"+%d");}
#define Add_Oct(x) {cbp[0]=' '; cbp=n2a(x,&cbp[1],"%o");}
#define Cat_Oct(x) {            cbp=n2a(x,&cbp[0],"%o");}
#define Add_Str(x) {cbp[0]=' '; strcpy(&cbp[1], x); cbp+=strlen(x)+1;}

#define shao_ng_dts_VALIDOPTS (char *)"-a.B:b:C:c.d:DeE:fFhi:I:kKl:L:m:nN:oOpP:q:rR.s:S:t:T:u:U:vVw:W:x:y:zZ:4.$#"
#define shao_ng_dts_SSOPTIONS shao_ng_dts_VALIDOPTS "MH:Y:"

#define Hmsg1(a)   {shao_ng_dts_Fmsg("Config", a);    help(1);}
#define Hmsg2(a,b) {shao_ng_dts_Fmsg("Config", a, b); help(1);}

/******************************************************************************/

void shao_ng_dts_Config::Arguments(int argc, char **argv, int cfgfd)
{
   shao_ng_dts_FileSpec *lfsp;
   int n, retc, xTrace = 1, infiles = 0, notctl = 0, rwbsz = 0;
   int mspec = 0, isProg = 0;
   char *Slash, *inFN=0, c, cbhname[MAXHOSTNAMELEN+1];
   shao_ng_dts_Args arglist((char *)"shao_ng_dts: ");

// Make sure we have at least one argument
//
   if (argc < 2) Hmsg1("Copy arguments not specified.");
   notctl = (Options & (shao_ng_dts_SRC | shao_ng_dts_SNK));

// Establish valid options and the source of those options
//
   if (notctl)             arglist.Options(shao_ng_dts_SSOPTIONS, STDIN_FILENO);
      else {if (cfgfd < 0) arglist.Options(shao_ng_dts_VALIDOPTS, argc-1, ++argv);
               else        arglist.Options(shao_ng_dts_SSOPTIONS, cfgfd, 1);
            setOpts(arglist);
           }

// Process the options
//
   while (c=arglist.getopt())
     { switch(c)
       {
       case 'a': Options |= shao_ng_dts_APPEND | shao_ng_dts_ORDER;
                 if (CKPdir) {free(CKPdir); CKPdir = 0;}
                 if (arglist.argval) CKPdir = strdup(arglist.argval);
                 break;
       case 'B': if (a2sz("block size", arglist.argval,
                         rwbsz, 1024, ((int)1)<<30))
                    Cleanup(1, argv[0], cfgfd);
                 break;
       case 'b': retc = (*arglist.argval == '+'
                      ? a2n("additional buff",arglist.argval,BAdd ,1,4096)
                      : a2n("blocking factor",arglist.argval,Bfact,1,IOV_MAX));
                 if (retc) Cleanup(1, argv[0], cfgfd);
                 break;
       case 'c': if (arglist.argval)
                    {if (a2n("compression level",arglist.argval,Complvl,0,9))
                        Cleanup(1, argv[0], cfgfd);
                    } else Complvl = 1;
                 if (Complvl) Options |=  shao_ng_dts_COMPRESS;
                    else      Options &= ~shao_ng_dts_COMPRESS;
                 break;
       case 'C': if (Configure(arglist.argval)) Cleanup(1, argv[0], cfgfd);
                 break;
       case 'd': if (SrcBuff) free(SrcBuff);
                 if (Options & (shao_ng_dts_SRC | shao_ng_dts_SNK))
                    {SrcBase = SrcBuff = strdup(arglist.argval);
                     SrcBlen = strlen(SrcBase);
                     SrcUser = SrcHost = 0;
                    } else ParseSB(arglist.argval);
                 break;
       case 'D': Options |= shao_ng_dts_TRACE;
                 break;
       case 'e': csOpts  |= shao_ng_dts_csDashE|shao_ng_dts_csLink;
                 break;
       case 'E': if (csSpec) free(csSpec);
                 csSpec = strdup(arglist.argval);
                 break;
       case 'f': Options |= shao_ng_dts_FORCE;
                 break;
       case 'F': Options |= shao_ng_dts_NOSPCHK;
                 break;
       case 'h': help(0);
                 break;
       case 'H': if (CBhost) free(CBhost);
                 if ((CBport = HostAndPort("callback",arglist.argval,
                                   cbhname, sizeof(cbhname))) < 0) exit(1);
                 CBhost = strdup(cbhname);
                 break;
       case 'i': if (IDfn) free(IDfn);
                 IDfn = strdup(arglist.argval);
                 break;
       case 'I': if (inFN) free(inFN);
                 inFN = strdup(arglist.argval);
                 break;
       case 'k': Options |= shao_ng_dts_KEEP | shao_ng_dts_ORDER;
                 break;
       case 'K': Options |= shao_ng_dts_NOUNLINK;
                 break;
       case 'l': if (Logfn) free(Logfn);
                 Logfn = strdup(arglist.argval);
                 break;
       case 'L': if (LogOpts(arglist.argval)) Cleanup(1, argv[0], cfgfd);
                 break;
       case 'm': if ((Slash = index(arglist.argval, '/')))
                    {*Slash = '\0';
                     if (a2o("dirmode", arglist.argval, ModeD, 1, 07777))
                        Cleanup(1, argv[0], cfgfd);
                     if (*(Slash+1) && a2o("mode", Slash+1, Mode, 1, 07777))
                        Cleanup(1, argv[0], cfgfd);
                    } else if (a2o("mode", arglist.argval, Mode, 1, 07777))
                              Cleanup(1, argv[0], cfgfd);
                 mspec = 1;
                 break;
       case 'M': Options |= shao_ng_dts_OUTDIR;
                 break;
       case 'n': Options |= shao_ng_dts_NODNS;
                 break;
       case 'N': if (Unpipe(arglist.argval)) Cleanup(1, argv[0], cfgfd);
                 break;
       case 'o': Options |= shao_ng_dts_ORDER;
                 break;
       case 'O': Options |= shao_ng_dts_OMIT;
                 break;
       case 'p': Options |= shao_ng_dts_PCOPY;
                 break;
       case 'P': if (a2n("seconds",arglist.argval,Progint,shao_ng_dts_MINPMONSEC,-1))
                    Cleanup(1, argv[0], cfgfd);
                 break;
       case 'q': if (a2n("qos",arglist.argval,n,0, 255))
                    Cleanup(1, argv[0], cfgfd);
                 shao_ng_dts_Net.QoS(n);
                 break;
       case 'r': Options |= shao_ng_dts_RECURSE;
                 break;
       case 'R': Options |= (shao_ng_dts_RTCOPY | shao_ng_dts_NOFSZCHK);
                 if (rtSpec) free(rtSpec);
                 rtSpec = (arglist.argval ? strdup(arglist.argval) : 0);
                 break;
       case 's': if (a2n("streams", arglist.argval,
                         Streams,1,shao_ng_dts_MAXSTREAMS)) Cleanup(1, argv[0], cfgfd);
                 break;
       case 'S': if (SrcXeq) free(SrcXeq);
                 SrcXeq = strdup(arglist.argval);
                 break;
       case 't': if (a2sz("time limit", arglist.argval,
                         TimeLimit,1,((int)1)<<30)) Cleanup(1, argv[0], cfgfd);
                 break;
       case 'T': if (SnkXeq) free(SnkXeq);
                 SnkXeq = strdup(arglist.argval);
                 break;
       case 'v': Options |= shao_ng_dts_VERBOSE;
                 if (xTrace < 2) xTrace = 2;
                 break;
       case 'V': Options |= (shao_ng_dts_BLAB | shao_ng_dts_VERBOSE); xTrace = 3;
                 break;
       case 'u': if (Unbuff(arglist.argval)) Cleanup(1, argv[0], cfgfd);
                 break;
       case 'U': if (a2sz("forced window size", arglist.argval,
                         Wsize, 8192, ((int)1)<<30)) Cleanup(1, argv[0], cfgfd);
                 MaxWindow = Wsize;
                 break;
       case 'W': // Backward compatibility
       case 'w': if (*arglist.argval == '=')
                    {Options |= shao_ng_dts_MANTUNE; arglist.argval++;}
                 if (a2sz("window size", arglist.argval,
                         Wsize, 8192, ((int)1)<<30)) Cleanup(1, argv[0], cfgfd);
                 break;
       case 'x': if (a2sz("transfer rate", arglist.argval,
                         Xrate,1024,((int)1)<<30)) Cleanup(1, argv[0], cfgfd);
                 break;
       case 'Y': if (SecToken) free(SecToken);
                 SecToken = strdup(arglist.argval);
                 break;
       case 'y': Options &= ~shao_ng_dts_DSYNC;
                      if (!strcmp("d", arglist.argval)) Options |= shao_ng_dts_FSYNC;
                 else if (!strcmp("dd",arglist.argval)) Options |= shao_ng_dts_DSYNC;
                 else{shao_ng_dts_Fmsg("Config","Invalid sync option -",arglist.argval);
                      Cleanup(1, argv[0], cfgfd);
                     }
                 SynSpec = strdup(arglist.argval);
                 break;
       case 'z': Options |= shao_ng_dts_CON2SRC;
                 break;
       case 'Z': if (!setPorts(arglist.argval)) Cleanup(1, argv[0], cfgfd);
                 break;
       case '4': if (notctl) Options |= shao_ng_dts_IPV4;
                    else if (setIPV4(arglist.argval)) Cleanup(1,argv[0],cfgfd);
                 break;
       case '$': cout <<shao_ng_dts_License <<endl;
                 exit(0);
                 break;
       case '#': cout <<shao_ng_dts_Version.VData <<endl;
                 exit(0);
                 break;
       case '-': break;
       default:  if (!notctl)
                    {if (cfgfd < 0) help(255);
                     Cleanup(1, argv[0], cfgfd);
                    }
       }
     }

// If we were processing the configuration file, return
//
   if (!notctl && cfgfd >= 0) return;

// Set the correct ip stack to use (do this before any resolution)
//
   if (Options & shao_ng_dts_IPV4) shao_ng_dts_Net.IPV4();

// If we should use the DNS then get our real hostname
//
   MyAddr = shao_ng_dts_Net.FullHostName((char *)0, 1);
   if (Options & shao_ng_dts_NODNS) MyHost = MyAddr;
      else MyHost = shao_ng_dts_Net.FullHostName((char *)0);
   shao_ng_dts_HostName = MyHost;

// If there is a checksum specification, process it now
//
   if (csSpec && EOpts(csSpec)) Cleanup(1, argv[0], cfgfd);

// If there is a realtime specification, process it now
//
   if (rtSpec && ROpts(rtSpec)) Cleanup(1, argv[0], cfgfd);

// Enforce the order option if we need to compute checksum at the destination
//
   if ((csOpts & shao_ng_dts_csVerOut) || (Options & shao_ng_dts_COMPRESS))
      Options |= shao_ng_dts_ORDER;

// Check for options mutually exclsuive with '-a'
//
   if ((Options & shao_ng_dts_APPEND) && (*csString || (csOpts & shao_ng_dts_csPrint)))
      {shao_ng_dts_Fmsg("Config", "-a and -E ...= are mutually exclusive.");
       Cleanup(1, argv[0], cfgfd);
      }

// Check for options mutually exclusive with '-N'
//
   if (Options & shao_ng_dts_XPIPE)
      {int isBad = 0;
       if (Options & shao_ng_dts_APPEND)
          isBad = shao_ng_dts_Fmsg("Config", "-N xx and -a are mutually exclusive.");
       if (SrcBuff)
          isBad = shao_ng_dts_Fmsg("Config", "-N xx and -d are mutually exclusive.");
       if (Options & shao_ng_dts_FORCE && Options & shao_ng_dts_OPIPE)
          isBad = shao_ng_dts_Fmsg("Config", "-N o  and -f are mutually exclusive.");
       if (Options & shao_ng_dts_NOUNLINK)
          isBad = shao_ng_dts_Fmsg("Config", "-N xx and -K are mutually exclusive.");
       if (mspec && Options & shao_ng_dts_OPIPE)
          isBad = shao_ng_dts_Fmsg("Config", "-N o  and -m are mutually exclusive.");
       if (Options & shao_ng_dts_PCOPY && Options & shao_ng_dts_OPIPE)
          isBad = shao_ng_dts_Fmsg("Config", "-N o  and -p are mutually exclusive.");
       if (Options & shao_ng_dts_RECURSE)
          isBad = shao_ng_dts_Fmsg("Config", "-N xx and -r are mutually exclusive.");
       if (Options & shao_ng_dts_RTCOPY)
          isBad = shao_ng_dts_Fmsg("Config", "-N xx and -R are mutually exclusive.");
       if (Options & (shao_ng_dts_IDIO | shao_ng_dts_ODIO))
          isBad = shao_ng_dts_Fmsg("Config", "-N xx and -u are mutually exclusive.");
       if (isBad) Cleanup(1, argv[0], cfgfd);
      }

// Check for options mutually exclusice with '-r'
//
   if ((Options & shao_ng_dts_RECURSE) && *csString)
      {shao_ng_dts_Fmsg("Config", "'-E ...=' and -r are mutually exclusive.");
           Cleanup(1, argv[0], cfgfd);
      }

// Check for options mutually exclusive with '-k'
//
   if ((Options & shao_ng_dts_KEEP) && (Options & shao_ng_dts_NOUNLINK))
      {shao_ng_dts_Fmsg("Config", "-k and -K are mutually exclusive.");
       Cleanup(1, argv[0], cfgfd);
      }

// Check for options mutually exclusive with '-R'
//
   if ((Options & shao_ng_dts_RTCOPY) && (Options & shao_ng_dts_IDIO))
      {shao_ng_dts_Fmsg("Config", "-R and '-u s' are mutually exclusive.");
       Cleanup(1, argv[0], cfgfd);
      }

// Get all of the filenames in the input file list (only on agent)
//
   if (inFN)
      {int infd;
       shao_ng_dts_Stream inStream;
       char *lp;
       if ((infd = open(inFN, O_RDONLY)) < 0)
          {shao_ng_dts_Emsg("Config", errno, "opening", inFN); exit(2);}
       inStream.Attach(infd);
       while ((lp = inStream.GetLine()) && (*lp != '\0'))
             {lfsp = srcLast;
              srcLast = new shao_ng_dts_FileSpec;
              if (lfsp) lfsp->next = srcLast;
                 else srcSpec = srcLast;
              srcLast->Parse(lp);
              if (srcLast->username && !srcLast->hostname)
                  Hmsg2("Missing host name for user", srcLast->username);
              if (srcLast->hostname && !srcLast->pathname)
                  Hmsg1("Piped I/O incompatible with hostname specification.");
              infiles++;
             }
       inStream.Close(); free(inFN);
      }

// Determine if we are going to be parsinga program specification
//
   if (notctl) isProg = ((Options & shao_ng_dts_SRC) && (Options & shao_ng_dts_IPIPE))
                     || ((Options & shao_ng_dts_SNK) && (Options & shao_ng_dts_OPIPE));

// Get all of the file names
//
   while(arglist.getarg(notctl))
        {lfsp = srcLast;
         srcLast = new shao_ng_dts_FileSpec;
         if (lfsp) lfsp->next = srcLast;
            else srcSpec = srcLast;
         srcLast->Parse(arglist.argval, isProg);
         if (srcLast->username && !srcLast->hostname)
            Hmsg2("Missing host name for user", srcLast->username);
         if (srcLast->hostname && !srcLast->pathname)
            Hmsg1("Piped I/O incompatible with hostname specification.");
         infiles++;
        }

// Perform sanity checks based on who we are
//
        if (Options & shao_ng_dts_SRC)
           {if (!srcSpec)
               {shao_ng_dts_Fmsg("Config", "Source file not specified."); exit(3);}
            Options &= ~(shao_ng_dts_RTCSNK|shao_ng_dts_OPIPE|shao_ng_dts_DSYNC);
           }
   else if (Options & shao_ng_dts_SNK)
           {if (infiles > 1)
               {shao_ng_dts_Fmsg("Config", "Ambiguous sink data target."); exit(3);}
            if (!(snkSpec = srcSpec))
               {shao_ng_dts_Fmsg("Config", "Target file not specified."); exit(3);}
            srcSpec = srcLast = 0;
            if (Options & shao_ng_dts_IPIPE) Options |= shao_ng_dts_NOFSZCHK;
            if (Options & shao_ng_dts_OPIPE) Options &=~shao_ng_dts_DSYNC;
            Options &= ~(shao_ng_dts_RTCSRC|shao_ng_dts_IPIPE);
           }
   else    {     if (infiles == 0) Hmsg1("Copy source not specified.")
            else if (infiles == 1) Hmsg1("Copy target not specified.")
            else if (infiles >  2)
                    {if (!(Options & shao_ng_dts_IPIPE)) Options |= shao_ng_dts_OUTDIR;
                        else {upSpec[1] = 0;
                              shao_ng_dts_Fmsg("Only one source allowed with '-N '",
                                        upSpec, "'.");
                              exit(3);
                             }
                    }
            snkSpec  = srcLast;
            lfsp->next = 0;
            srcLast = lfsp;
           }

// For output pipes we do direct processing and must order output.
//
   if (Options & shao_ng_dts_OPIPE)
      {Options &= ~(shao_ng_dts_OUTDIR   | shao_ng_dts_RELATIVE | shao_ng_dts_DSYNC);
       Options |=  (shao_ng_dts_NOFSZCHK | shao_ng_dts_KEEP | shao_ng_dts_ORDER);
      }

// Set appropriate debugging level
//
   if (Options & shao_ng_dts_TRACE) shao_ng_dts_Debug.Trace = xTrace;

// Do Source/Sink configuration or assemble the final options
//
   if (notctl) Config_Xeq(rwbsz);
      else     Config_Ctl(rwbsz);

// Get the correct directory creation mode setting
//
   if (!ModeD) ModeD = ModeDC;
      else if ((ModeD & S_IRWXU) == S_IRWXU) ModeDC = ModeD;

// Set checkpoint directory if we do not have one here and must have one
//
   if (!CKPdir && Options & shao_ng_dts_APPEND && Options & shao_ng_dts_SNK)
      {const char *ckpsfx = "/.shao_ng_dts";
       char *homedir = shao_ng_dts_OS.getHomeDir();
       CKPdir = (char *)malloc(strlen(homedir) + sizeof(ckpsfx) + 1);
       strcpy(CKPdir, homedir); strcat(CKPdir, ckpsfx);
       if (mkdir(CKPdir, 0755) && errno != EEXIST)
          {shao_ng_dts_Emsg("Config",errno,"creating restart directory",CKPdir);
           exit(100);
          }
       DEBUG("Restart directory is " <<CKPdir);
      }

// Compute proper MT level
//
   MTLevel = Streams + (Complvl ?  (Options & shao_ng_dts_SNK  ? 3 : 2) : 1)
                     + (Progint && (Options & shao_ng_dts_SNK) ? 1 : 0) + 2;
   DEBUG("Optimum MT level is " <<MTLevel);

// All done
//
   return;
}

/******************************************************************************/
/*                                  h e l p                                   */
/******************************************************************************/

#define H(x)    cout <<x <<endl;
#define I(x)    cout <<'\n' <<x <<endl;

/******************************************************************************/
  
void shao_ng_dts_Config::help(int rc)
{
	printf("\n");
	printf
	    (" ========================================================================\n");
	printf
	    (" ==                   Transfer Big Data Utilities                      ==\n");
	printf
	    (" ==            VLBI LAB Of Shanghai Astronomical Observatory           ==\n");
	printf
	    (" ==              Chinese Academy Of Sciences, 2014-2024                ==\n");
	printf
	    (" ========================================================================\n\n\n");

	sleep(2);
H("Usage:   shao_ng_dts [Options] [Inspec] Outspec")
I("Options: [-a [dir]] [-b [+]bf] [-B bsz] [-c [lvl]] [-C cfn] [-D] [-d path]")
H("         [-e] [-E csa] [-f] [-F] [-h] [-i idfn] [-I slfn] [-k] [-K]")
H("         [-L opts[@logurl]] [-l logf] [-m mode] [-n] [-N nio] [-o] [-O] [-p]")
H("         [-P sec] [-r] [-R [args]] [-q qos] [-s snum] [-S srcxeq] [-T trgxeq]")
H("         [-t sec] [-v] [-V] [-u loc] [-U wsz] [-w [=]wsz] [-x rate] [-y] [-z]")
H("         [-Z pnf[:pnl]] [-4 [loc]] [-$] [-#] [--]")
I("I/Ospec: [user@][host:]file")
if (rc) exit(rc);
I("Function: Secure and fast copy utility.")
I("-a dir  append mode to restart a previously failed copy.")
H("-b bf   sets the read blocking factor (default is 1).")
H("-b +bf  adds additional output buffers to mitigate ordering stalls.")
H("-B bsz  sets the read/write I/O buffer size (default is wsz).")
H("-c lvl  compress data before sending across the network (default lvl is 1).")
H("-C cfn  process the named configuration file at time of encounter.")
H("-d path requests relative source path addressing and target path creation.")
H("-D      turns on debugging.")
H("-e      error check data for transmission errors using md5 checksum.")
H("-E csa  specify checksum alorithm and optionally report or verify checksum.")
H("        csa: [%]{a32|c32|md5}[=[<value> | <outfile>]]")
H("-f      forces the copy by first unlinking the target file before copying.")
H("-F      does not check to see if there is enough space on the target node.")
H("-h      print help information.")
H("-i idfn is the name of the ssh identify file for source and target.")
H("-I slfn is the name of the file that holds the list of files to be copied.")
H("        With -I no source files need be specified on the command line.")
H("-k      keep the destination file even when the copy fails.")
H("-K      do not rm the file when -f specified, only truncate it.")
H("-l logf logs standard error to the specified file.")
H("-L args sets the logginng level and log message destination.")
H("-m mode target file mode as [dmode/][fmode] but one mode must be present.")
H("        Default dmode is 0755 and fmode is 0644 or it comes via -p option.")
H("-n      do not use DNS to resolve IP addresses to host names.")
H("-N nio  enable named pipe processing; nio specifies input and output state:")
H("        i -> input pipe or program, o -> output pipe or program")
H("-s snum number of network streams to use (default is 4).")
H("-o      enforces output ordering (writes in ascending offset order).")
H("-O      omits files that already exist at the target node (useful with -r).")
H("-p      preserve source mode, ownership, and dates.")
H("-P sec  produce a progress message every sec seconds (15 sec minimum).")
H("-q lvl  specifies the quality of service for routers that support QOS.")
H("-r      copy subdirectories and their contents (actual files only).")
H("-R args enables real-time copy where args specific handling options.")
H("-S cmd  command to start shao_ng_dts on the source node.")
H("-T cmd  command to start shao_ng_dts on the target node.")
H("-t sec  sets the time limit for the copy to complete.")
H("-v      verbose mode (provides per file transfer rate).")
H("-V      very verbose mode (excruciating detail).")
H("-u loc  use unbuffered I/O at source or target, if possible.")
H("        loc: s | t | st")
H("-U wsz  unnegotiated window size (sets maximum and default for all parties).")
H("-w wsz  desired window size for transmission (the default is 128K).")
H("        Prefixing wsz with '=' disables autotuning in favor of a fixed wsz.")
H("-x rate is the maximum transfer rate allowed in bytes, K, M, or G.")
H("-y what perform fsync before closing the output file when what is 'd'.")
H("        When what is 'dd' then the file and directory are fsynced.")
H("-z      use reverse connection protocol (i.e., target to source).")
H("-Z      use port range pn1:pn2 for accepting data transfer connections.")
H("-4      use only IPV4 stack; optionally, at specified location.")
H("-$      print the license and exit.")
H("-#      print the version and exit.")
H("--      allows an option with a defaulted optional arg to appear last.")
I("user    the user under which the copy is to be performed. The default is")
H("        to use the current login name.")
H("host    the location of the file. The default is to use the current host.")
H("Inspec  the name of the source file(s) (also see -I).")
H("Outspec the name of the target file or directory (required if >1 input file.\n")
I(shao_ng_dts_Version.Version)
exit(rc);
}
  
/******************************************************************************/
/*                            C o n f i g I n i t                             */
/******************************************************************************/
  
int shao_ng_dts_Config::ConfigInit(int argc, char **argv)
{
/*
  Function: Establish default values using a configuration file.

  Input:    None.

  Output:   0 upon success or !0 otherwise.
*/
   char *homedir, *cfn = (char *)"/.shao_ng_dts.cf";
   int  retc;
   char *ConfigFN;

// Ignore sigpipe
//
   signal(SIGPIPE, SIG_IGN);

// Make sure we have at least one argument to determine who we are
//
   if (argc >= 2)
           if (!strcmp(argv[1], SrcArg))
            {Options |= shao_ng_dts_SRC; shao_ng_dts_Debug.Who = (char *)"SRC"; return 0;}
      else if (!strcmp(argv[1], SnkArg))
            {Options |= shao_ng_dts_SNK; shao_ng_dts_Debug.Who = (char *)"SNK"; return 0;}
      else MyProg = strdup(argv[0]);

// Use the config file, if present
//
   if (ConfigFN = getenv("shao_ng_dts_CONFIGFN"))
      {if (retc = Configure(ConfigFN)) return retc;}
      else {
      // Use configuration file in the home directory, if any
      //
      struct stat buf;
      homedir = shao_ng_dts_OS.getHomeDir();
      ConfigFN = (char *)malloc(strlen(homedir) + strlen(cfn) + 1);
      strcpy(ConfigFN, homedir); strcat(ConfigFN, cfn);
      if (stat(ConfigFN, &buf))
         {retc = 0; free(ConfigFN); ConfigFN = 0;}
         else if (retc = Configure(ConfigFN)) return retc;
     }

// Establish the FD limit
//
   {struct rlimit rlim;
    if (getrlimit(RLIMIT_NOFILE, &rlim) < 0)
       shao_ng_dts_Emsg("Config",-errno,"getting FD limit");
       else {rlim.rlim_cur=(rlim.rlim_max == RLIM_INFINITY ? 255:rlim.rlim_max);
             if (setrlimit(RLIMIT_NOFILE, &rlim) < 0)
                 shao_ng_dts_Emsg("config", errno, "setting FD limit");
            }
   }

// All done
//
   return retc;
}
  
/******************************************************************************/
/*                             C o n f i g u r e                              */
/******************************************************************************/
  
int shao_ng_dts_Config::Configure(const char *cfgFN)
{
/*
  Function: Establish default values using a configuration file.

  Input:    None.

  Output:   0 upon success or !0 otherwise.
*/
   static int depth = 3;
          int  cfgFD;

// Check if we are recursing too much
//
   if (!(depth--))
      return shao_ng_dts_Fmsg("Config", "Too many embedded configuration files.");

// Try to open the configuration file.
//
   if ( (cfgFD = open(cfgFN, O_RDONLY, 0)) < 0)
      return shao_ng_dts_Emsg("Config",errno,"opening config file",cfgFN);

// Process the arguments from the config file
//
   Arguments(2, (char **)&cfgFN, cfgFD);
   return 0;
}

/******************************************************************************/
/*                               D i s p l a y                                */
/******************************************************************************/
  
void shao_ng_dts_Config::Display()
{

// Display the option values (used during debugging)
//
   cerr <<"badd " <<BAdd <<endl;
   cerr <<"bfact " <<Bfact <<endl;
   cerr <<"bsize " <<RWBsz <<endl;
   cerr <<"bindtries " <<bindtries <<" bindwait " <<bindwait <<endl;
   if (CBhost)
   cerr <<"cbhost:port " <<CBhost <<":" <<CBport <<endl;
   if (CKPdir)
   cerr <<"ckpdir " <<CKPdir <<endl;
   if (Complvl)
   cerr <<"compress " <<Complvl <<endl;
   if (CopyOpts)
   cerr <<"copyopts " <<CopyOpts <<endl;
   if (IDfn)
   cerr <<"idfn " <<IDfn <<endl;
   if (Logfn)
   cerr <<"logfn " <<Logfn <<endl;
   cerr <<"mode " <<oct <<Mode <<dec <<endl;
   cerr <<"myhost " <<MyHost <<endl;
   cerr <<"myuser " <<MyUser <<endl;
   cerr <<"ssectoken " <<SecToken <<endl;
   cerr <<"streams " <<Streams <<endl;
   cerr <<"wsize " <<Wsize <<" maxwindow " <<MaxWindow  <<" maxss " <<MaxSegSz <<endl;
   cerr <<"srcxeq " <<SrcXeq <<endl;
   cerr <<"trgxeq " <<SnkXeq <<endl;
   if (Xrate)
   cerr <<"xfr " <<Xrate <<endl;
}

/******************************************************************************/
/*                                 S c a l e                                  */
/******************************************************************************/

const char *shao_ng_dts_Config::Scale(double &xVal)
{
    static const double Kilo = 1024.0;

    if (xVal < Kilo) return "";
    xVal /= Kilo;
    if (xVal < Kilo) return "K";
    xVal /= Kilo;
    if (xVal < Kilo) return "M";
    xVal /= Kilo;
    if (xVal < Kilo) return "G";
    xVal /= Kilo;
    if (xVal < Kilo) return "T";
    xVal /= Kilo;
    if (xVal < Kilo) return "E";
    xVal /= Kilo;
    if (xVal < Kilo) return "Z";
    xVal /= Kilo;
    if (xVal < Kilo) return "Y";
    return "";
}
 
/******************************************************************************/
/*                                 W A M s g                                  */
/******************************************************************************/
  
void shao_ng_dts_Config::WAMsg(const char *who, const char *act, int newsz)
{
    char buff[128];
    sprintf(buff, "to %d", newsz);
    shao_ng_dts_Fmsg(who, act, buff, "bytes.");
}

/******************************************************************************/
/*                     p r i v a t e   f u n c t i o n s                      */
/******************************************************************************/
/******************************************************************************/
/*                            C o n f i g _ C t l                             */
/******************************************************************************/

void shao_ng_dts_Config::Config_Ctl(int rwbsz)
{
   char cbuff[4096], *cbp = cbuff;
   int n;

// If a checksum output file exists, open it now
//
   if (csPath)
      {if ((csFD = open(csPath, O_CREAT|O_APPEND|O_WRONLY, 0644)) < 0)
          {shao_ng_dts_Emsg("Config", errno, "opening checksum file", csPath);
           exit(2);
          }
      }

// Now generate all the options we will send to the source and sink
//
   if (Options & shao_ng_dts_APPEND)    Add_Opt('a');
   if (CKPdir)                                 Add_Str(CKPdir);
   if (BAdd)                    {Add_Opt('b'); Add_Nup(BAdd);}
   if (Bfact)                   {Add_Opt('b'); Add_Num(Bfact);}
   if (rwbsz)                   {Add_Opt('B'); Add_Num(rwbsz);}
   if (Options & shao_ng_dts_COMPRESS) {Add_Opt('c'); Add_Num(Complvl);}
   if (SrcBase)                 {Add_Opt('d'); Add_Str(SrcBase);}
   if (Options & shao_ng_dts_TRACE)     Add_Opt('D');
   if (csOpts  & shao_ng_dts_csDashE)   Add_Opt('e');
   if (Options & shao_ng_dts_FORCE)     Add_Opt('f');
   if (Options & shao_ng_dts_NOSPCHK)   Add_Opt('F');
   if (Options & shao_ng_dts_KEEP)      Add_Opt('k');
   if (Options & shao_ng_dts_NOUNLINK)  Add_Opt('K');
   if (LogSpec)                 {Add_Opt('L'); Add_Str(LogSpec);}
   if (!(Options & shao_ng_dts_XPIPE)) {Add_Opt('m');
   if (ModeD)                   {              Add_Oct(ModeD); *cbp++ ='/';
                                               Cat_Oct(Mode);
                                }
      else                                     Add_Oct(Mode);
                                }
   if (Options & shao_ng_dts_OUTDIR || (Options & shao_ng_dts_RELATIVE && SrcBase))
                                 Add_Opt('M');
   if (Options & shao_ng_dts_NODNS)     Add_Opt('n');
   if (Options & shao_ng_dts_XPIPE)    {Add_Opt('N'); Add_Str(upSpec);}
   if (Options & shao_ng_dts_ORDER)     Add_Opt('o');
   if (Options & shao_ng_dts_OMIT)      Add_Opt('O');
   if (Options & shao_ng_dts_PCOPY)     Add_Opt('p');
   if (Progint)                 {Add_Opt('P'); Add_Num(Progint);}
   if ((n = shao_ng_dts_Net.QoS()))    {Add_Opt('q'); Add_Num(n); }
   if (Options & shao_ng_dts_RECURSE)   Add_Opt('r');
   if (Options & shao_ng_dts_RTCOPY)   {Add_Opt('R');
                                 if (rtSpec) Add_Str(rtSpec);
                                }
   if (Streams)                 {Add_Opt('s'); Add_Num(Streams);}
   if (TimeLimit)               {Add_Opt('t'); Add_Num(TimeLimit);}
   if (Options & shao_ng_dts_VERBOSE)   Add_Opt('v');
   if (Options & shao_ng_dts_BLAB)      Add_Opt('V');
   if (MaxWindow)               {Add_Opt('U'); Add_Num(MaxWindow);}
   else
   if (Wsize)                   {Add_Opt('W');
                                 if (Options & shao_ng_dts_MANTUNE) {Add_Nul(Wsize);}
                                    else      {Add_Num(Wsize);}
                                }
                                 Add_Opt('Y'); Add_Str(SecToken);
   if (Xrate)                   {Add_Opt('x'); Add_Num(Xrate);}
   if (SynSpec)                 {Add_Opt('y'); Add_Str(SynSpec);}
   if (Options & shao_ng_dts_CON2SRC)   Add_Opt('z');
   if (csSpec)                  {Add_Opt('E'); Add_Str(csSpec);}
   if (Options & (shao_ng_dts_IDIO | shao_ng_dts_ODIO))
                                {Add_Opt('u'); Add_Str(ubSpec);}
   if (PorSpec)                 {Add_Opt('Z'); Add_Str(PorSpec);}
   CopyOpts = strdup(cbuff);
}
  
/******************************************************************************/
/*                            C o n f i g _ X e q                             */
/******************************************************************************/

void shao_ng_dts_Config::Config_Xeq(int rwbsz)
{
   long long memreq;

// Set network flow. This is indepedent of connection direction.
//
   shao_ng_dts_Net.Flow(Options & shao_ng_dts_SRC);

// Start up the logging if we need to
//
   if (Options & shao_ng_dts_LOGGING && !shao_ng_dts_NetLog.Open("shao_ng_dts", Logurl))
      {shao_ng_dts_Fmsg("Config", "Unable to initialize netlogger.");
       exit(4);
      }

// Get maximum window size if not specified
//
   if (!MaxWindow) MaxWindow = shao_ng_dts_Net.MaxWSize(Options & shao_ng_dts_SNK);

// Do penultimate adjustement to the window size
//
   if (Wsize > MaxWindow)
      {Wsize = MaxWindow;
       if (Options & shao_ng_dts_BLAB && Options & shao_ng_dts_SRC)
          WAMsg("Config", "Window size reduced", Wsize);
      }
   shao_ng_dts_Net.setWindow(Wsize, Options & shao_ng_dts_MANTUNE);

// Set the initial I/O buffer size
//
   setRWB(rwbsz);

// Set the blocking factor to one if it has not been specified or if direct
// or real-time input is in effect.
//
   if (!Bfact || (Options & (shao_ng_dts_IDIO | shao_ng_dts_RTCOPY))) Bfact = 1;

// Compute the number of buffers we will obtain
//
   if (Options & shao_ng_dts_SRC) BNum = (Streams > Bfact ? Streams : Bfact)*3;
      else {BNum = (Streams > Bfact ? Streams : Bfact)
                 * (Options & shao_ng_dts_ORDER ? 9 : 3) + BAdd;
            if (Options & shao_ng_dts_ORDER && !BAdd)
               {int n = MaxWindow/RWBsz;
                BNum += (n > 100 ? 100 : (n < Streams*9 ? Streams*9 : n));
               }
           }
   DEBUG("rwbsz=" <<RWBsz <<" BNum=" <<BNum);

// Check if we have exceeded memory limitations
//
   memreq = RWBsz * BNum + (Options & shao_ng_dts_COMPRESS ? Bfact * RWBsz : 0);
   if (memreq > shao_ng_dts_OS.FreeRAM/4)
      {char mbuff[80];
       sprintf(mbuff, "I/O buffers (%" FMTLL "dK) > 25%% of available free memory (%" FMTLL "dK);",
                      memreq/1024, shao_ng_dts_OS.FreeRAM/1024);
       shao_ng_dts_Fmsg("Config", (Options & shao_ng_dts_SRC ? "Source" : "Sink"), mbuff,
                           "copy may be slow");
      }
}
  
/******************************************************************************/
/*                                R t o k e n                                 */
/******************************************************************************/
  
char *shao_ng_dts_Config::Rtoken()
{
   int mynum = (int)getpid();
   struct timeval tod;
   char mybuff[sizeof(mynum)*2+1];

// Get current time of day and add into the random equation
//
   gettimeofday(&tod, 0);
   mynum = mynum ^ tod.tv_sec ^ tod.tv_usec ^ (tod.tv_usec<<((mynum & 0x0f)+1));

// Convert to a printable string
//
   tohex((char *)&mynum, sizeof(mynum), mybuff);
   return strdup(mybuff);
}

/******************************************************************************/
/*                                  a 2 x x                                   */
/******************************************************************************/

int shao_ng_dts_Config::a2sz(const char *etxt, char *item, int  &result,
                      int  minv, int  maxv)
{   int i = strlen(item)-1; char cmult = item[i];
    int  val, qmult = 1;
    char *endC;
         if (cmult == 'k' || cmult == 'K') qmult = 1024;
    else if (cmult == 'm' || cmult == 'M') qmult = 1024*1024;
    else if (cmult == 'g' || cmult == 'G') qmult = 1024*1024*1024;
    if (qmult > 1) item[i] = '\0';
    val  = strtol(item, &endC, 10) * qmult;
    if (*endC || (maxv != -1 && (val > maxv || val < 1)) || val < minv)
       {if (qmult > 1) item[i] = cmult;
        return shao_ng_dts_Fmsg("Config", "Invalid", etxt, "-",item);
       }
    if (qmult > 1) item[i] = cmult;
    result = val;
    return 0;
}

int shao_ng_dts_Config::a2tm(const char *etxt, char *item, int  &result,
                      int  minv, int  maxv)
{   int i = strlen(item)-1; char cmult = item[i];
    int  val, qmult = 1;
    char *endC;
         if (cmult == 's' || cmult == 'S') qmult =  1;
    else if (cmult == 'm' || cmult == 'M') qmult = 60;
    else if (cmult == 'h' || cmult == 'H') qmult = 60*60;
    if (qmult > 1) item[i] = '\0';
    val  = strtol(item, &endC, 10) * qmult;
    if (*endC || (maxv != -1 && (val > maxv || val < 1)) || val < minv)
       {if (qmult > 1) item[i] = cmult;
        return shao_ng_dts_Fmsg("Config", "Invalid", etxt, "-",item);
       }
    if (qmult > 1) item[i] = cmult;
    result = val;
    return 0;
}

int shao_ng_dts_Config::a2ll(const char *etxt, char *item, long long &result,
                      long long minv, long long maxv)
{   long long val;
    char *endC;
    val  = strtoll(item, &endC, 10);
    if (*endC || (maxv != -1 && val > maxv) || val  < minv)
       return shao_ng_dts_Fmsg("Config", "Invalid", etxt, "-", item);
    result = val;
    return 0;
}

int shao_ng_dts_Config::a2n(const char *etxt, char *item, int  &result,
                     int  minv, int  maxv)
{   int  val;
    char *endC;
    val  = strtol(item, &endC, 10);
    if (*endC || (maxv != -1 && val > maxv) || val  < minv)
       return shao_ng_dts_Fmsg("Config", "Invalid", etxt, "-", item);
    result = val;
    return 0;
}

int shao_ng_dts_Config::a2o(const char *etxt, char *item, int  &result,
                     int  minv, int  maxv)
{   int  val;
    char *endC;
    val  = strtol(item, &endC, 8);
    if (*endC || (maxv != -1 && val > maxv) || val < minv)
       return shao_ng_dts_Fmsg("Config", "Invalid", etxt, "-", item);
    result = val;
    return 0;
}

int shao_ng_dts_Config::a2x(char *result, char *item, int ilen)
{   int  xVal, i, j = 0;

    for (i = 0; i < ilen; i++)
        {if (isdigit(item[i])) xVal = item[i] - int('0');
            else if (item[i] >= 'a' && item[i] <= 'f')
                    xVal = item[i] - int('a') + 10;
                    else return 1;
         if (i & 0x01) result[j++] |= xVal;
            else result[j] = xVal << 4;
        }
    return 0;
}

/******************************************************************************/
/*                                   n 2 a                                    */
/******************************************************************************/

char *shao_ng_dts_Config::n2a(int  val, char *buff, const char *fmt)
      {return buff + sprintf(buff, fmt, val);}

char *shao_ng_dts_Config::n2a(long long  val, char *buff, const char *fmt)
      {return buff + sprintf(buff, fmt, val);}

/******************************************************************************/
/*                               P a r s e S B                                */
/******************************************************************************/
  
void shao_ng_dts_Config::ParseSB(char *spec)
{
   char *up = 0, *hp = 0, *cp, *sp;
   int i = strlen(spec);

// Make sure spec ends with a slash
//
   if (spec[i-1] == '/') SrcBuff = strdup(spec);
      else {SrcBuff = (char *)malloc(i+2);
            strcpy(SrcBuff, spec); strcpy(SrcBuff+i, "/");
           }

// Prepare to parse the spec
//
   sp = cp = SrcBuff;
   while(*cp)
      {     if (*cp == '@' && !SrcUser)
               {SrcUser = sp; *cp = '\0'; sp = cp+1;}
       else if (*cp == ':' && !SrcHost)
               {SrcHost = sp; *cp = '\0'; sp = cp+1;}
       else if (*cp == '/') break;
       cp++;
      }
   SrcBase = sp;

// Ignore null users and hosts
//
   if (SrcUser && !(*SrcUser)) SrcUser = 0;
   if (SrcHost && !(*SrcHost)) SrcHost = 0;
}
  
/******************************************************************************/
/*                                 E O p t s                                  */
/******************************************************************************/
  
int shao_ng_dts_Config::EOpts(char *opts)
{
   int csLen, csPrt = 0, csSrc = 0;
   char *Equal, *csArg = 0;

// Reset the checksum option area
//
   csFD = -1; *csString = 0;
   if (csPath) {free(csPath); csPath = 0;}

// If the type starts with a dot then source computation is indicated
//
   if (*opts == '%')
      {csSrc = 1; opts++;
       if (!(*opts))
          {shao_ng_dts_Fmsg("Config", "Checksum type not specified."); return -1;}
      }

// Find the equals and see what follows
//
   if ((Equal = index(opts, '=')))
      {*Equal = 0; csArg = Equal+1;
       if (!isxdigit(*csArg))
          {if (*csArg) csPath = strdup(csArg); csPrt = 1; csArg = 0;}
      } else csOpts = shao_ng_dts_csDashE|shao_ng_dts_csLink;

// Verify that this is a supported checksum
//
   csLen = strlen(opts);
   if (csLen >= (int)sizeof(csName)) *csName = 0;
      else strcpy(csName, opts);
        if (!strcmp("a32", csName)) {csType = shao_ng_dts_csA32; csSize =  4;}
   else if (!strcmp("c32", csName)) {csType = shao_ng_dts_csC32; csSize =  4;}
   else if (!strcmp("md5", csName)) {csType = shao_ng_dts_csMD5; csSize = 16;}
   else {shao_ng_dts_Fmsg("Config", "Invalid checksum type -", opts); return -1;}

// Verify the checksum value if one was actually specified
//
   if (csArg)
      {csLen = strlen(csArg);
       if (csSize*2 != csLen || a2x(csValue, csArg, csLen))
          {shao_ng_dts_Fmsg("Config","Invalid",csName,"checksum value -",csArg);
           return -1;
          }
       strcpy(csString, csArg);
      }

// Indicate where the checksum generation will occur
//
   if (Equal)
      {if (csOpts & shao_ng_dts_csLink)
          if (Options & shao_ng_dts_ORDER || !csSrc)
                                    csOpts  =  shao_ng_dts_csVerAll|shao_ng_dts_csDashE;
             else                   csOpts |=  shao_ng_dts_csVerIn;
          else if (csSrc)           csOpts  =  shao_ng_dts_csVerIn;
                  else              csOpts  =  shao_ng_dts_csVerOut;
      }

// Establish print and echo options
//
   if (csPrt)
      csOpts |= shao_ng_dts_csPrint | (csOpts & shao_ng_dts_csVerOut ? 0 : shao_ng_dts_csSend);

// All done
//
   if (Equal) *Equal = '=';
   return 0;
}

/******************************************************************************/
/*                               L o g O p t s                                */
/******************************************************************************/
  
int shao_ng_dts_Config::LogOpts(char *opts)
{
    char *opt = opts;
    int myopts = 0, nlopts = 0;

    while(*opt && *opt != '@')
         {switch(*opt)
                {case 'a': nlopts |= NL_APPEND;    break;
                 case 'b': nlopts |= NL_MEM;       break;
                 case 'c': myopts |= shao_ng_dts_LOGCMP;  break;
                 case 'i': myopts |= shao_ng_dts_LOGIN;   break;
                 case 'o': myopts |= shao_ng_dts_LOGOUT;  break;
                 case 'r': myopts |= shao_ng_dts_LOGRD;   break;
                 case 'w': myopts |= shao_ng_dts_LOGWR;   break;
                 case 'x': myopts |= shao_ng_dts_LOGEXP;  break;
                 case '@':                         break;
                 default:  shao_ng_dts_Fmsg("Config","Invalid logging options -",opts);
                           return -1;
                }
          opt++;
         }

    if (!myopts) {shao_ng_dts_Fmsg("Config", "no logging events selected.");
                  return -1;
                 }

    Options &= ~shao_ng_dts_LOGGING;
    Options |= myopts;
    shao_ng_dts_NetLog.setOpts(nlopts);
    if (Logurl) {free(Logurl); Logurl = 0;}
    LogSpec = strdup(opts);
    if (*opt == '@' && opt[1]) {opt++; Logurl = strdup(opt);}

    return 0;
}

/******************************************************************************/
/*                          H o s t A n d   P o r t                           */
/******************************************************************************/
  
int shao_ng_dts_Config::HostAndPort(const char *what, char *path, char *buff, int bsz)
{   int hlen, pnum = 0;
    char *hn;

 // Extract the host name from the path
 //
    if (*path == '[')
       {if (!(hn = index(path, ']')))
           {shao_ng_dts_Fmsg("Config", what, "invalid ipv6 address in", path);
            return -1;
           }
        hn++;
       } else if (!(hn = index(path, ':'))) hn = path + strlen(path);

    if (!(hlen = hn - path))
       {shao_ng_dts_Fmsg("Config", what, "host not specified in", path);
        return -1;
       }
    if (hlen >= bsz)
       {shao_ng_dts_Fmsg("Config", what, "hostname too long in", path);
        return -1;
       }
    if (path != buff) strncpy(buff, path, hlen);

 // Extract the port number from the path
 //
    if (*hn == ':' && hn++ && *hn)
       {errno = 0;
        pnum = strtol(hn, (char **)NULL, 10);
        if (!pnum && errno || pnum > 65535)
           {shao_ng_dts_Fmsg("Config",what,"port invalid -", hn); return -1;}
       }

// All done.
//
    buff[hlen] = '\0';
    return pnum;
}
  
/******************************************************************************/
/*                                 R O p t s                                  */
/******************************************************************************/
  
int shao_ng_dts_Config::ROpts(char *opts)
{
   struct auto_RO {char *S; auto_RO(char *x) : S(strdup(x)) {}
                           ~auto_RO() {free(S);}
                  } Arg(opts);
   char *rNext, *rOpts = Arg.S;

// Set values to defaults
//
   rtCheck   = 3;
   rtLimit   = 0;
   if (rtLockf) {free(rtLockf); rtLockf = 0;}
   Options &= ~(shao_ng_dts_RTCBLOK | shao_ng_dts_RTCHIDE);

// Grab all items in opts (these may be separated by a comma)
//
   while(rOpts && *rOpts)
        {if ((rNext = index(rOpts, ','))) while(*rNext == ',') *rNext++ = '\0';
         switch(*rOpts)
               {case 'c': if (*(rOpts+1) != '=' 
                          ||  (rtCheck = atoi(rOpts+2)) <= 0)
                             return ROptsErr(rOpts);
                          break;
                case 'b': Options |= shao_ng_dts_RTCBLOK;
                          break;
                case 'h': Options |= shao_ng_dts_RTCHIDE;
                          break;
                case 'i': if (*(rOpts+1) != '='
                          ||  (rtLimit = atoi(rOpts+2)) <= 0)
                             return ROptsErr(rOpts);
                          break;
                case 'v': Options |= shao_ng_dts_RTCVERC;
                          break;
                case '/':
                case '.': rtLockf = strdup(rOpts);
                          break;
                default:  return ROptsErr(rOpts);
               }
          rOpts = rNext;
         }

// All done
//
   return 0;
}

/******************************************************************************/
/*                              R O p t s E r r                               */
/******************************************************************************/
  
int shao_ng_dts_Config::ROptsErr(char *eTxt)
{
    shao_ng_dts_Fmsg("Config", "Invalid -R argument -", eTxt);
    return -1;
}
 
/******************************************************************************/
/*                                 s e t C S                                  */
/******************************************************************************/
  
void shao_ng_dts_Config::setCS(char *inCS)
{
// Set checksum
//
   memcpy(csValue,inCS,csSize);
   tohex(inCS,csSize,csString);
}
  
/******************************************************************************/
/*                               s e t I P V 4                                */
/******************************************************************************/
  
int shao_ng_dts_Config::setIPV4(char *opts)
{
    static char ipv4opt[] = {' ', '-', '4', '\0'};
    char *opt = opts;

    if (opts && *opts)
       while(*opt)
            {switch(*opt)
                   {case 't': CopyOTrg = ipv4opt;   break;
                    case 's': CopyOSrc = ipv4opt;   break;
                    case 'c': Options |= shao_ng_dts_IPV4; break;
                    default:  shao_ng_dts_Fmsg("Config","Invalid ipv4 options -",opts);
                              return -1;
                   }
             opt++;
            } else {CopyOSrc = CopyOTrg = ipv4opt; Options |= shao_ng_dts_IPV4;}
    return 0;
}
  
/******************************************************************************/
/*                               s e t O p t s                                */
/******************************************************************************/
  
void shao_ng_dts_Config::setOpts(shao_ng_dts_Args &Args)
{
     Args.Option("append",     1, 'a', '.');
     Args.Option("buffers",    1, 'b', ':');
     Args.Option("buffsz",     6, 'B', ':');
     Args.Option("compress",   1, 'c', '.');
     Args.Option("config",     6, 'C', ':');
     Args.Option("dirbase",    3, 'd', ':');
     Args.Option("debug",      5, 'D', 0);
// e
     Args.Option("checksum",   5, 'E', ':');
     Args.Option("force",      1, 'f', 0);
     Args.Option("nofschk",    4, 'F', ':');
     Args.Option("help",       1, 'h', ':');
     Args.Option("idfile",     1, 'i', ':');
     Args.Option("infiles",    2, 'I', ':');
     Args.Option("ipv4",       4, '4', '.');
     Args.Option("keep",       1, 'k', 0);
// K
     Args.Option("license",    7, '$', 0);
     Args.Option("logfile",    1, 'l', ':');
// L
     Args.Option("mode",       1, 'm', ':');
     Args.Option("nodns",      1, 'n', 0);
     Args.Option("pipe",       4, 'N', ':');
     Args.Option("order",      1, 'o', 0);
     Args.Option("omit",       4, 'O', 0);
     Args.Option("preserve",   1, 'p', 0);
     Args.Option("progress",   4, 'P', ':');
     Args.Option("qos",        3, 'q', ':');
     Args.Option("recursive",  1, 'r', 0);
     Args.Option("realtime",   4, 'R', ':');
     Args.Option("streams",    1, 's', ':');
// S
     Args.Option("timelimit",  1, 't', ':');
// T
     Args.Option("verbose",    1, 'v', 0);
     Args.Option("vverbose",   2, 'V', 0);
     Args.Option("version",    7, '#', 0);
     Args.Option("unbuffered", 1, 'u', ':');
// U
     Args.Option("windowsz",   1, 'w', ':');
     Args.Option("xfrrate",    1, 'x', ':');
// Y
     Args.Option("sync",       4, 'y', 0);
     Args.Option("reverse",    3, 'z', 0);
     Args.Option("port",       4, 'Z', ':');
}

/******************************************************************************/
/*                              s e t P o r t s                               */
/******************************************************************************/

int shao_ng_dts_Config::setPorts(char *pspec)
{
   char *Colon, buff[256];
   int pnum1, pnum2;

// Find colon and separate numbers
//
   if ((Colon = index(pspec, ':')))
      {if (*(Colon+1)) *Colon = 0;
          else {*Colon = ':';
                shao_ng_dts_Fmsg("Config", "Invalid port range -", pspec);
                return 0;
               }
      }

// Convert first port
//
   if (a2n("port number", pspec, pnum1, 1, 65535)) return 0;

// Get second port, if specified and verify range
//
   if (Colon)
      {*Colon = ':';
       if (a2n("port number", Colon+1, pnum2, 1, 65535)) return 0;
      } else pnum2 = pnum1;

// Set the port range
//
   if (!shao_ng_dts_Network::setPorts(pnum1, pnum2))
      {shao_ng_dts_Fmsg("Config", "Invalid port range -", pspec);
       return 0;
      }

// Reformat the port range
//
   if (Colon) sprintf(buff, "%d:%d", pnum1, pnum2);
      else    sprintf(buff, "%d",    pnum1);
   if (PorSpec) free(PorSpec);
   PorSpec = strdup(buff);
   return 1;
}
  
/******************************************************************************/
/*                                s e t R W B                                 */
/******************************************************************************/

void shao_ng_dts_Config::setRWB(int rwbsz)
{
   static const int MaxRWB = 524288;
   static const int xyzRWB = 418816;

// Now compute the possible R/W buffer size
//
   if (rwbsz) RWBsz = rwbsz;
      else RWBsz = (Wsize > xyzRWB ? MaxRWB : Wsize + Wsize/4);
   RWBsz = (RWBsz < shao_ng_dts_OS.PageSize ? shao_ng_dts_OS.PageSize
                                     : RWBsz/shao_ng_dts_OS.PageSize*shao_ng_dts_OS.PageSize);

   if (rwbsz && RWBsz != rwbsz && (Options & shao_ng_dts_BLAB))
      WAMsg("Config", "Buffer size changed", RWBsz);
}

/******************************************************************************/
/*                                 t o h e x                                  */
/******************************************************************************/

char *shao_ng_dts_Config::tohex(char *inbuff, int inlen, char *outbuff) {
     static char hv[] = "0123456789abcdef";
     int i, j = 0;
     for (i = 0; i < inlen; i++) {
         outbuff[j++] = hv[(inbuff[i] >> 4) & 0x0f];
         outbuff[j++] = hv[ inbuff[i]       & 0x0f];
         }
     outbuff[j] = '\0';
     return outbuff;
     }

/******************************************************************************/
/*                                U n b u f f                                 */
/******************************************************************************/
  
int shao_ng_dts_Config::Unbuff(char *opts)
{
    char *opt = opts;

    while(*opt)
         {switch(*opt)
                {case 't': Options |= shao_ng_dts_ODIO; ubSpec[0] = 't'; break;
                 case 's': Options |= shao_ng_dts_IDIO; ubSpec[1] = 's'; break;
                 default:  shao_ng_dts_Fmsg("Config","Invalid unbuffered options -",opts);
                           return -1;
                }
          opt++;
         }
    return 0;
}

/******************************************************************************/
/*                                U n p i p e                                 */
/******************************************************************************/
  
int shao_ng_dts_Config::Unpipe(char *opts)
{
    char *opt = opts;

    while(*opt)
         {switch(*opt)
                {case 'i': Options |= shao_ng_dts_IPIPE;
                           upSpec[0] = 'i'; break;
                 case 'o': Options |= shao_ng_dts_OPIPE;
                           upSpec[1] = 'o'; break;
                 default:  shao_ng_dts_Fmsg("Config","Invalid named pipe options -",opts);
                           return -1;
                }
          opt++;
         }
    return 0;
}

/******************************************************************************/
/*                               C l e a n u p                                */
/******************************************************************************/
  
void shao_ng_dts_Config::Cleanup(int rc, char *cfgfn, int cfgfd)
{
if (cfgfd >= 0 && cfgfn)
   if (rc > 1)
        shao_ng_dts_Fmsg("Config","Check config file",cfgfn,"for conflicts.");
   else shao_ng_dts_Fmsg("Config", "Error occured processing config file", cfgfn);
exit(rc);
}
