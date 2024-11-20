#ifndef __shao_ng_dts_BUFFPOOL_H__
#define __shao_ng_dts_BUFFPOOL_H__
/******************************************************************************/
/*                                                                            */
/*                       shao_ng_dts_ B u f f P o o l . h                        */
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

#include <stdlib.h>
#include <strings.h>
#include "shao_ng_dts_Pthread.h"

class shao_ng_dts_Buffer;
  
/******************************************************************************/
/*                   C l a s s   shao_ng_dts_ B u f f P o o l                    */
/******************************************************************************/
  
class shao_ng_dts_BuffPool
{
public:
void         Abort();

int          Allocate(int buffnum, int bsize, int sink, int ovhd=0);
int          Allocate(int buffnum);

int          BuffCount() {return numbuf;}

int          DataSize() {return datasz;}

int          Decode(shao_ng_dts_Buffer *bp);

void         Encode(shao_ng_dts_Buffer *bp, char xcmnd);

shao_ng_dts_Buffer *getEmptyBuff();

void         putEmptyBuff(shao_ng_dts_Buffer *buff);

shao_ng_dts_Buffer *getFullBuff();

void         putFullBuff(shao_ng_dts_Buffer *buff);

             shao_ng_dts_BuffPool(const char *id="net");
            ~shao_ng_dts_BuffPool();

private:

shao_ng_dts_Mutex EmptyPool;
shao_ng_dts_Mutex FullPool;
shao_ng_dts_Semaphore EmptyBuffs;
shao_ng_dts_Semaphore FullBuffs;

int         numbuf;
int         datasz;
int         buffsz;
int         RU486;
const char *pname;

shao_ng_dts_Buffer *next_full;
shao_ng_dts_Buffer *last_full;
shao_ng_dts_Buffer *last_empty;
};

/******************************************************************************/
/*                     C l a s s   shao_ng_dts_ H e a d c s                      */
/******************************************************************************/

struct shao_ng_dts_Headcs
      {union {long long lVal[2];
              int       iVal[2];
              short     sVal[2];
              char      cVal[2];
             };
      };

/******************************************************************************/
/*                     C l a s s   shao_ng_dts_ H e a d e r                      */
/******************************************************************************/

  
struct shao_ng_dts_Header
      {char cmnd;          // Command
       char hdcs;          // Header checksum
       char flgs;          // Flags
       char rsv1;          // Reserved
       char blen[4];       // int       buffer length
       char boff[8];       // Long long buffer offset
       char cksm[16];      // MD5 check sum (optional)
       shao_ng_dts_Header() {bzero(cksm, sizeof(cksm));}
      ~shao_ng_dts_Header() {}
       };

// Valid commands
//
#define SHAO_NG_DTS_IO 0x00
#define SHAO_NG_DTS_CLCKS    0x03
#define SHAO_NG_DTS_CLOSE    0x04
#define SHAO_NG_DTS_ABORT    0xFF

// Valid flags in flgs
//
#define shao_ng_dts_HDCS     0x80

/******************************************************************************/
/*                     C l a s s   shao_ng_dts_ B u f f e r                      */
/******************************************************************************/
  
class shao_ng_dts_Buffer
{
public:
shao_ng_dts_Buffer  *next;
long long     boff;
int           blen;
int           rsvd;
shao_ng_dts_Header   bHdr;
char         *data;

inline void   Recycle() {Owner->putEmptyBuff(this);}

              shao_ng_dts_Buffer(shao_ng_dts_BuffPool *oP, char *bP=0)
                         : next(0), boff(0),   blen(0), rsvd(0),
                           data(0), Owner(oP), Buff(bP) {}
             ~shao_ng_dts_Buffer() {if (Buff) free(Buff);}

private:
shao_ng_dts_BuffPool *Owner;
char          *Buff;
};
#endif
