#ifndef __shao_ng_dts_FS_NULL_H__
#define __shao_ng_dts_FS_NULL_H__
/******************************************************************************/
/*                                                                            */
/*                        shao_ng_dts_ F S _ N u l l . h                         */
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
  
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "shao_ng_dts_FileSystem.h"

class shao_ng_dts_FS_Null : public shao_ng_dts_FileSystem
{
public:

int        Applicable(const char *path);

int        Enough(long long bytes, int numfiles=1) {return 1;}

long long  getSize(int fd, long long *bsz=0);

shao_ng_dts_File *Open(const char *fn, int opts, int mode=0, const char *fa=0);

int        MKDir(const char *path, mode_t mode) {return 0;}

int        RM(const char *path) {return 0;}

int        setGroup(const char *path, const char *Group) {return 0;}

int        setMode(const char *path, mode_t mode) {return 0;}

int        setTimes(const char *path, time_t atime, time_t mtime) {return 0;}

int        Stat(const char *path, shao_ng_dts_FileInfo *finfo=0);

int        Stat(const char *path, const char *dent, int fd,
                int nolnks=1, shao_ng_dts_FileInfo *finfo=0) {return -ENOENT;}

           shao_ng_dts_FS_Null() {}
          ~shao_ng_dts_FS_Null() {}

protected:
};
#endif
