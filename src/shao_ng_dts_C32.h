#ifndef __shao_ng_dts_C32_H__
#define __shao_ng_dts_C32_H__
/******************************************************************************/
/*                                                                            */
/*                            shao_ng_dts_ C 3 2 . h                             */
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

#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <inttypes.h>

#include "shao_ng_dts_ChkSum.h"
#include "shao_ng_dts_Endian.h"

typedef unsigned int uint32;
  
class shao_ng_dts_C32 : public shao_ng_dts_ChkSum
{
public:

void  Init() {C32Result = CRC32_XINIT; TotLen = 0;}

void  Update(const char *Buff, int BLen);

int   csSize() {return sizeof(C32Result);}

char *Final(char **Text=0)
               {char buff[sizeof(long long)];
                long long tLcs = TotLen;
                int i = 0;
                if (tLcs)
                   {while(tLcs) {buff[i++] = tLcs & 0xff ; tLcs >>= 8;}
                    Update(buff, i);
                   }
                TheResult = C32Result ^ CRC32_XOROT;
#ifndef shao_ng_dts_BIG_ENDIAN
                TheResult = htonl(TheResult);
#endif
                if (Text) *Text = x2a((char *)&TheResult);
                return (char *)&TheResult;
               }

const char *Type() {return "c32";}

            shao_ng_dts_C32() {Init();}
virtual    ~shao_ng_dts_C32() {}

private:
static const uint CRC32_XINIT = 0;
static const uint CRC32_XOROT = 0xffffffff;
static       uint crctable[256];
             uint C32Result;
             uint TheResult;
       long  long TotLen;
};
#endif
