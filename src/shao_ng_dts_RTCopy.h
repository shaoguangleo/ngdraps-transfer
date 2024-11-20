#ifndef __shao_ng_dts_RTCopy_H__
#define __shao_ng_dts_RTCopy_H__
/******************************************************************************/
/*                                                                            */
/*                         shao_ng_dts_ R T C o p y . h                          */
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

#include <pthread.h>

class shao_ng_dts_FileSystem;
class shao_ng_dts_Semaphore;
  
class shao_ng_dts_RTCopy
{
public:

void     *Lock(shao_ng_dts_Semaphore *semP);

long long Prep(long long rdsk, int rdsz, ssize_t &rlen);

int       Start(shao_ng_dts_FileSystem *fsp, const char *iofn, int iofd);

void      Stop();

          shao_ng_dts_RTCopy() : FSp(0), xLim(0), Left(0), lkFN(0), lkFD(-1),
                          Blok(0), Grow(0), ioFD(-1), Tid(0) {}
         ~shao_ng_dts_RTCopy() {} // Never gets deleted

private:

shao_ng_dts_FileSystem *FSp;
long long        xLim;
long long        Left;
const char      *lkFN;
int              lkFD;
int              Blok;
int              Grow;
int              ioFD;
pthread_t        Tid;
};
#endif
