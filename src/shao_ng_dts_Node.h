#ifndef __shao_ng_dts_NODE_H__
#define __shao_ng_dts_NODE_H__
/******************************************************************************/
/*                                                                            */
/*                           shao_ng_dts_ N o d e . h                            */
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

#include <unistd.h>
#include <sys/types.h>
#include "shao_ng_dts_FileSpec.h"
#include "shao_ng_dts_File.h"
#include "shao_ng_dts_Link.h"
#include "shao_ng_dts_ProcMon.h"
#include "shao_ng_dts_Stream.h"

class  shao_ng_dts_Protocol;
class  shao_ng_dts_ZCX;
  
class shao_ng_dts_Node
{
public:

void   Detach() {NStream.Detach(); dlcount = 0; data_link[0] = 0;}

int    Drain() {return NStream.Drain();}

int    getBuffers(int isTrg, int isLZO=0);

char  *GetLine();

char  *GetToken() {return NStream.GetToken();}

int    LastError() {return NStream.LastError();}

char  *NodeName() {return nodename;}

int    Put(const char *data, int dlen)
          {char *dpnt[] = {(char *)data, 0}; int lpnt[] = {dlen, 0};
           return Put(dpnt, lpnt);
          }
int    Put(char *data[], int dlen[]);

int    Run(char *user, char *host, char *prog, char *parg);

int    RecvFile(shao_ng_dts_FileSpec *fspec, shao_ng_dts_Node *Remote);

int    SendFile(shao_ng_dts_FileSpec *fspec);

int    Start(shao_ng_dts_Protocol *protocol, int incomming)
            {if (incomming) return Incomming(protocol);
                 else       return  Outgoing(protocol);
            }

void   Stop(int Report=0);

int    Wait(shao_ng_dts_Node *other=0);

int       TotFiles;
long long TotBytes;

       shao_ng_dts_Node(shao_ng_dts_Link *netLink=0);
      ~shao_ng_dts_Node() {Stop(); if (nodename) free(nodename);}

private:

shao_ng_dts_ProcMon Parent_Monitor;
shao_ng_dts_Link   *data_link[shao_ng_dts_MAXSTREAMS];
shao_ng_dts_File   *comp1File, *comp2File;
shao_ng_dts_Stream NStream;
char       *nodename;
int         dlcount;
int         iocount;

void       chkWsz(int fd, int Final=0);
int        Incomming(shao_ng_dts_Protocol *protocol);
int         Outgoing(shao_ng_dts_Protocol *protocol);
int        Recover(const char *who);
void       Report(double, shao_ng_dts_FileSpec *, shao_ng_dts_File *, shao_ng_dts_ZCX *);
shao_ng_dts_ZCX  *setup_CX(int deflating, int iofd);
int        Usage(const char *who, char *buff, int blen);
};
#endif
