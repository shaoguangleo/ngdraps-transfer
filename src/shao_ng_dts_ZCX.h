#ifndef __shao_ng_dts_ZCX_H__
#define __shao_ng_dts_ZCX_H__
/******************************************************************************/
/*                                                                            */
/*                            shao_ng_dts_ Z C X . h                             */
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

class shao_ng_dts_BuffPool;

class shao_ng_dts_ZCX
{
public:

long long    Bytes() {return cbytes;}

int          Process(void);

pthread_t    TID;

      shao_ng_dts_ZCX(shao_ng_dts_BuffPool *ib, shao_ng_dts_BuffPool *rb, shao_ng_dts_BuffPool *ob,
               int clvl, int xfd, int logit);
     ~shao_ng_dts_ZCX() {}

private:

int Zfailure(int zerr, const char *oper, char *Zmsg);

shao_ng_dts_BuffPool   *Ibuff;
shao_ng_dts_BuffPool   *Rbuff;
shao_ng_dts_BuffPool   *Obuff;
long long        cbytes;
int              Clvl;
int              iofd;
char            *LogIDbeg;
char            *LogIDend;
};
#endif
