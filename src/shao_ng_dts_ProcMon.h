#ifndef __shao_ng_dts_PROCMON_H__
#define __shao_ng_dts_PROCMON_H__
/******************************************************************************/
/*                                                                            */
/*                        shao_ng_dts_ P r o c M o n . h                         */
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
  
#include "shao_ng_dts_BuffPool.h"
#include "shao_ng_dts_File.h"
#include "shao_ng_dts_Pthread.h"

class shao_ng_dts_Node;

class shao_ng_dts_ProcMon
{
public:

void  Limit();

void  Monitor();

void  Start(pid_t monit=0, shao_ng_dts_Node *Remote=0);
void  Start(int seclim, shao_ng_dts_BuffPool *);

void  Stop();

      shao_ng_dts_ProcMon() : pingNode(0), MonPool(0), mytid(0),
                       alldone(0), TimeLimit(0), monDone(0) {}

     ~shao_ng_dts_ProcMon() {Stop();}

shao_ng_dts_BuffPool *MonPool;

private:

pthread_t      mytid;
int            alldone;
int            TimeLimit;
pid_t          monPID;
shao_ng_dts_CondVar   CondMon;
shao_ng_dts_Semaphore monDone;
shao_ng_dts_Mutex     pingMutex;
shao_ng_dts_Node     *pingNode;
};
#endif
