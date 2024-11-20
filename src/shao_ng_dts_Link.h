#ifndef __shao_ng_dts_LINK_H__
#define __shao_ng_dts_LINK_H__
/******************************************************************************/
/*                                                                            */
/*                           shao_ng_dts_ L i n k . h                            */
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
#include <sys/types.h>
#include <fcntl.h>
#include "shao_ng_dts_BuffPool.h"
#include "shao_ng_dts_ChkSum.h"
#include "shao_ng_dts_File.h"
#include "shao_ng_dts_IO.h"
#include "shao_ng_dts_Pthread.h"

// The shao_ng_dts_Link class defines the operations on a network link.
//
class shao_ng_dts_Link
{
public:

int          Close() {if (csObj) delete csObj; return IOB.Close();}

int          FD() {return IOB.FD();}

char        *LinkName() {return Lname;}

void         setBuddy(shao_ng_dts_Link *mybuddy) {Buddy = mybuddy;}

static void  setNudge() {Nudge = 1; Wait = 1;}

int          Buff2Net();

int          Net2Buff();

             shao_ng_dts_Link(int newfd=-1, const char *newfn="");
            ~shao_ng_dts_Link() {Close(); if (Lname) free(Lname);}

int          LinkNum;

private:

static int      Nudge;
static int      Wait;

shao_ng_dts_Link      *Buddy;
shao_ng_dts_Semaphore  Rendezvous;
shao_ng_dts_IO         IOB;
shao_ng_dts_ChkSum    *csObj;
char           *Lname;

int        Control_In( shao_ng_dts_Buffer *inbuff);
int        Control_Out(shao_ng_dts_Buffer *outbuff);
};
#endif
