/******************************************************************************/
/*                                                                            */
/*                     shao_ng_dts_ F i l e S y s t e m . C                      */
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
  
#include "shao_ng_dts_Headers.h"
#include "shao_ng_dts_FS_Null.h"
#include "shao_ng_dts_FS_Pipe.h"
#include "shao_ng_dts_FS_Unix.h"

// This file holds the function that returns an applicable filesystem object
// for any given path (or null if none can be found). We do this by simply
// statically instantiating all of the supported filesystems and returning
// the first one that claims to be applicable to the given path.

/******************************************************************************/
/*                    shao_ng_dts_ g e t F i l e S y s t e m                     */
/******************************************************************************/
  
shao_ng_dts_FileSystem *shao_ng_dts_FileSystem::getFS(const char *path, int Opts)
{

   static shao_ng_dts_FS_Null shao_ng_dts_NULL;
   static shao_ng_dts_FS_Pipe shao_ng_dts_PIPE;
   static shao_ng_dts_FS_Unix shao_ng_dts_UFS;

// Simply try each supported filesystem (add more as they are defined)
//
   if (shao_ng_dts_NULL.Applicable(path))  return (shao_ng_dts_FileSystem *)&shao_ng_dts_NULL;
   if (Opts & getFS_Pipe)           return (shao_ng_dts_FileSystem *)&shao_ng_dts_PIPE;
   if (shao_ng_dts_UFS.Applicable(path))   return (shao_ng_dts_FileSystem *)&shao_ng_dts_UFS;

// All done, nothing is applicable
//
   return (shao_ng_dts_FileSystem *)0;
}
