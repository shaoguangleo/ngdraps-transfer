#ifndef __shao_ng_dts_ENDIAN_H
#define __shao_ng_dts_ENDIAN_H
/******************************************************************************/
/*                                                                            */
/*                         shao_ng_dts_ E n d i a n . h                          */
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

// Include stdlib and sys/param so that ENDIAN macros are defined properly
//
#include <stdlib.h>
#include <sys/param.h>
  
#if defined(_BIG_ENDIAN) || defined(__BIG_ENDIAN__) || \
   defined(__IEEE_BIG_ENDIAN) || \
   (defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN)
#define shao_ng_dts_BIG_ENDIAN
#endif
#endif
