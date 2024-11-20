#ifndef __shao_ng_dts_X32_H__
#define __shao_ng_dts_X32_H__
/******************************************************************************/
/*                                                                            */
/*                            shao_ng_dts_ A 3 2 . h                             */
/*                                                                            */
/*(c) 2014-2024 by Shanghai Astronomical Observatory , Chinese Academy Of Sciences*/
/*      All Rights Reserved. See shao_ng_dts_Version.C for complete License Terms    */
/*                            All Rights Reserved                             */
/*   Produced by Shaoguang Guo, Shanghai Astronomical Observatory , Chinese Academy Of Sciences*/
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

/* The following implementation of adler32 was derived from zlib and is
                   * Copyright (C) 1995-1998 Mark Adler
   Below are the zlib license terms for this implementation.
*/
  
/* zlib.h -- interface of the 'zlib' general purpose compression library
  version 1.1.4, March 11th, 2002

  Copyright (C) 1995-2002 Jean-loup Gailly and Mark Adler

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Jean-loup Gailly        Mark Adler
  jloup@gzip.org          madler@alumni.caltech.edu


  The data format used by the zlib library is described by RFCs (Request for
  Comments) 1950 to 1952 in the files ftp://ds.internic.net/rfc/rfc1950.txt
  (zlib format), rfc1951.txt (deflate format) and rfc1952.txt (gzip format).
*/

#define DO1(buf,i)  {unSum1 += buf[i]; unSum2 += unSum1;}
#define DO2(buf,i)  DO1(buf,i); DO1(buf,i+1);
#define DO4(buf,i)  DO2(buf,i); DO2(buf,i+2);
#define DO8(buf,i)  DO4(buf,i); DO4(buf,i+4);
#define DO16(buf)   DO8(buf,0); DO8(buf,8);

class shao_ng_dts_A32 : public shao_ng_dts_ChkSum
{
public:

void  Init() {unSum1 = AdlerStart; unSum2 = 0;}

void  Update(const char *Buff, int BLen)
             {unsigned char *buff = (unsigned char *)Buff;
              while(BLen >= AdlerNMax)
                  {BLen -= AdlerNMax; n = AdlerNCnt;
                   do {DO16(buff); buff += 16;} while(--n);
                   unSum1 %= AdlerBase; unSum2 %= AdlerBase;
                  }
            if (BLen)             // avoid modulos if none remaining
               {while(BLen >= 16) {BLen -= 16; DO16(buff); buff += 16;}
                while(BLen--) {unSum1 += *buff++; unSum2 += unSum1;}
                unSum1 %= AdlerBase; unSum2 %= AdlerBase;
               }
            }

int   csSize() {return sizeof(AdlerResult);}

char *Final(char **Text=0)
            {AdlerResult = (unSum2 << 16) + unSum1;
#ifndef shao_ng_dts_BIG_ENDIAN
             AdlerResult = htonl(AdlerResult);
#endif
             if (Text) *Text = x2a((char *)&AdlerResult);
             return (char *)&AdlerResult;
            }

const char *Type() {return "a32";}

            shao_ng_dts_A32() {Init();}
virtual    ~shao_ng_dts_A32() {}

private:
static const uint AdlerBase  = 0xFFF1;
static const uint AdlerStart = 0x0001;
static const uint AdlerNMax  = 5552;
/* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */
static const uint AdlerNCnt  = AdlerNMax/16;
             uint AdlerResult;
             uint unSum1;
             uint unSum2;
              int n;
};
#endif
