/******************************************************************************/
/*                                                                            */
/*                         shao_ng_dts_ C h k S u m . C                          */
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
  
#include "shao_ng_dts_Config.h"
#include "shao_ng_dts_ChkSum.h"
#include "shao_ng_dts_A32.h"
#include "shao_ng_dts_A32_zlib.h"
#include "shao_ng_dts_C32.h"
#include "shao_ng_dts_MD5.h"
#include "shao_ng_dts_MD5_openssl.h"

/******************************************************************************/
/*                                 A l l o c                                  */
/******************************************************************************/

shao_ng_dts_ChkSum *shao_ng_dts_ChkSum::Alloc(int csType)
{
// Return correct object
//
   switch(csType)
         {case shao_ng_dts_csA32: return (shao_ng_dts_ChkSum *)new shao_ng_dts_A32_zlib;
          case shao_ng_dts_csC32: return (shao_ng_dts_ChkSum *)new shao_ng_dts_C32;
#if defined(LINUX) || defined(MACOS)
          case shao_ng_dts_csMD5: return (shao_ng_dts_ChkSum *)new shao_ng_dts_MD5_openssl;
#else
          case shao_ng_dts_csMD5: return (shao_ng_dts_ChkSum *)new shao_ng_dts_MD5;
#endif
          default:         break;
         }

// Unknown checksum
//
   return 0;
}

/******************************************************************************/
/*                                   x 2 a                                    */
/******************************************************************************/
  
char *shao_ng_dts_ChkSum::x2a(char *inCS)
{
   static char hv[] = "0123456789abcdef";
   int i, j = 0, csLen = csSize();

// Truncate the checksum if need be
//
   if (csLen*2 >= sizeof(csBuff)) csLen = sizeof(csBuff)*2-1;

// Convert checksum to text
//
     for (i = 0; i < csLen; i++) {
         csBuff[j++] = hv[(inCS[i] >> 4) & 0x0f];
         csBuff[j++] = hv[ inCS[i]       & 0x0f];
         }
     csBuff[j] = '\0';
     return csBuff;
}
