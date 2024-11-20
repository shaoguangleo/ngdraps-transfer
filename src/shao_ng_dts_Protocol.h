#ifndef __shao_ng_dts_PROTOCOL_H__
#define __shao_ng_dts_PROTOCOL_H__
/******************************************************************************/
/*                                                                            */
/*                       shao_ng_dts_ P r o t o c o l . h                        */
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
#include "shao_ng_dts_Config.h"
#include "shao_ng_dts_FileSpec.h"
#include "shao_ng_dts_FileSystem.h"
#include "shao_ng_dts_Link.h"

class shao_ng_dts_Node;
  
class shao_ng_dts_Protocol
{
public:

int    setCBPort(int pnum);

int    Schedule(shao_ng_dts_Node *Fnode, shao_ng_dts_FileSpec *Ffs, char *Fcmd, char *Ftype,
                shao_ng_dts_Node *Lnode, shao_ng_dts_FileSpec *Lfs, char *Lcmd, char *Ltype,
                shao_ng_dts_Node *Tnode);

int    Login(shao_ng_dts_Link *Net, int incomming)
            {if (incomming) return Process_login(Net);
             return Request_login(Net);
            }

int    Process(shao_ng_dts_Node *Node);

int    Request(shao_ng_dts_Node *Node);

       shao_ng_dts_Protocol() {Local = 0; Remote = 0; tdir = 0;}
      ~shao_ng_dts_Protocol() {if (Remote) delete Remote;}

private:

shao_ng_dts_Node       *Local;
shao_ng_dts_Node       *Remote;
shao_ng_dts_FileSystem *fs_obj;
char            *tdir;
long long        tdir_id;

int   AdjustWS(char *wp, char *bp, int Final);
int   getCBPort(shao_ng_dts_Node *Node);
void  getEnd(shao_ng_dts_Node *Node);
void  putCSV(char *Host, char *csFn, char *csVal, int csVsz);

int   Process_exit();
int   Process_flist();
int   Process_get();
int   Process_login(shao_ng_dts_Link *Net);

int   Request_exit(int retc);
int   Request_flist(long long &totsz);
int   Request_get(shao_ng_dts_FileSpec *fp);
int   Request_login(shao_ng_dts_Link *Net);

int   SendArgs(shao_ng_dts_Node *Node, shao_ng_dts_FileSpec *fsp,
               char *cbhost, int cbport, char *addOpt=0);
};
#endif
