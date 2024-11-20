/******************************************************************************/
/*                                                                            */
/*                        shao_ng_dts_ F S _ N u l l . C                         */
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
  
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include "shao_ng_dts_File.h"
#include "shao_ng_dts_IO_Null.h"
#include "shao_ng_dts_FS_Null.h"
#include "shao_ng_dts_Platform.h"
#include "shao_ng_dts_System.h"

/******************************************************************************/
/*                        G l o b a l   O b j e c t s                         */
/******************************************************************************/

extern shao_ng_dts_System shao_ng_dts_OS;

#define MAXLL  (long long)~(((unsigned long long)1)<<(sizeof(long long)*8-1))
//                                                            1234567890
#define isDEVNULL(x) (!strcmp(x, "/dev/null") || !strncmp(x, "/dev/null/", 10))
#define isDEVZERO(x) (!strcmp(x, "/dev/zero") || !strncmp(x, "/dev/zero/", 10))
  
/******************************************************************************/
/*                            A p p l i c a b l e                             */
/******************************************************************************/
  
int shao_ng_dts_FS_Null::Applicable(const char *path)
{

// This filesystem is applicable if the path is one of the special /dev
// devices. Otherwise, it is not applicable.
//
   if (!isDEVNULL(path)
   &&  strcmp(path, "/dev/zero") 
   &&  strcmp(path, "/dev/random")) return 0;

// Save the path to this filesystem if we don't have one. This is a real
// kludgy short-cut since in shao_ng_dts we only have a single output destination.
//
   if (!fs_path)
      {fs_path = strdup(path);
       memset((void *)&fs_id, 0, sizeof(fs_id));
      }
   return 1;
}

/******************************************************************************/
/*                               g e t S i z e                                */
/******************************************************************************/
  
long long shao_ng_dts_FS_Null::getSize(int fd, long long *bsz)
{
// For null file systems blocksize is 8K (nothing special)
//
   if (bsz) *bsz = 8192;
   return (fd ? 0 : MAXLL);
}

/******************************************************************************/
/*                                  O p e n                                   */
/******************************************************************************/

shao_ng_dts_File *shao_ng_dts_FS_Null::Open(const char *fn, int opts, int mode, const char *fa)
{
    shao_ng_dts_IO *iob;
    int FD;

// For null filesystems, we simply create a dummy IO object to provide
// eof on a read and throw-away on a write.
//
        if (!mode && isDEVZERO(fn)) iob =  new shao_ng_dts_IO_Null(0);
   else if ( mode && isDEVNULL(fn)) iob =  new shao_ng_dts_IO_Null(-1);
   else {if ((FD = (mode ? open(fn,opts,mode) : open(fn,opts))) < 0)
            return (shao_ng_dts_File *)0;
         iob =  new shao_ng_dts_IO(FD);
        }
   return new shao_ng_dts_File(fn, iob, (shao_ng_dts_FileSystem *)this);
}
 
/******************************************************************************/
/*                                  S t a t                                   */
/******************************************************************************/
  
int shao_ng_dts_FS_Null::Stat(const char *path, shao_ng_dts_FileInfo *sbuff)
{

// Return -ENOENT if we have /dev/null/x or anything that is handled by us
//
   if (strcmp(path, "/dev/null")   && strcmp(path, "/dev/zero")
   &&  strcmp(path, "/dev/random") && strcmp(path, "/dev")) return -ENOENT;
   if (!sbuff) return 0;

// Construct info out of thin air!
//
   sbuff->fileid = 0;
   sbuff->mode   = 0;
   sbuff->size   = (strcmp(path, "/dev/zero") ? 0 : MAXLL);
   sbuff->atime  = 0;
   sbuff->ctime  = 0;
   sbuff->mtime  = 0;
   sbuff->Otype  = (isDEVNULL(path) ? 'd' : 'f');

// Supply group name as "other"
//
   if (sbuff->Group) free(sbuff->Group);
   sbuff->Group = strdup("other");

// All done
//
   return 0;
}
