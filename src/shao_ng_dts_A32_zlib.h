#ifndef __shao_ng_dts_X32_ZLIB_H__
#define __shao_ng_dts_X32_ZLIB_H__
/******************************************************************************/
/*                                                                            */
/*                            shao_ng_dts_ A 3 2 . h                             */
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
#include "zlib.h"

typedef unsigned int uint32;

class shao_ng_dts_A32_zlib : public shao_ng_dts_ChkSum
{
 public:
    
    void  Init() { adler = adler32(0L, Z_NULL, 0); }

    void  Update(const char *Buff, int BLen) {
        adler = adler32(adler, (const Bytef*)Buff, BLen);
    }
    
    int   csSize() {return sizeof(adler);}

    char *Final(char **Text=0) {
       
#ifndef shao_ng_dts_BIG_ENDIAN
        adler = htonl(adler);
#endif
        if (Text) *Text = x2a((char *)&adler);
        return (char *)&adler;
    }

    const char *Type() {return "a32";}

    shao_ng_dts_A32_zlib() {Init();}
    virtual    ~shao_ng_dts_A32_zlib() {}

 private:
    uLong adler;
    
};
#endif
