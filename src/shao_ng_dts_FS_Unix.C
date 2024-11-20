/******************************************************************************/
/*                                                                            */
/*                        shao_ng_dts_ F S _ U n i x . C                         */
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
  
#ifdef LINUX
#define _XOPEN_SOURCE 600
#endif

#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <string.h>
#include <unistd.h>
#include <utime.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef FREEBSD
#include <sys/statvfs.h>
#endif
#include "shao_ng_dts_File.h"
#include "shao_ng_dts_FS_Unix.h"
#include "shao_ng_dts_Platform.h"
#include "shao_ng_dts_Pthread.h"
#include "shao_ng_dts_System.h"
#include "shao_ng_dts_Debug.h"

#if defined(MACOS) || defined (AIX)
#define S_IAMB      0x1FF
#endif

/******************************************************************************/
/*                        G l o b a l   O b j e c t s                         */
/******************************************************************************/

extern shao_ng_dts_System shao_ng_dts_OS;
  
/******************************************************************************/
/*                            A p p l i c a b l e                             */
/******************************************************************************/
  
int shao_ng_dts_FS_Unix::Applicable(const char *path)
{
#ifdef FREEBSD
   if (!fs_path) fs_path = strdup(path);
#else
   struct statvfs buf;

// To find out whether or not we are applicable, simply do a statvfs on the
// incomming path. If we can do it, then we are a unix filesystem.
//
   if (statvfs(path, &buf)) return 0;

// Set the sector size
//
   secSize = buf.f_frsize;

// Save the path to this filesystem if we don't have one. This is a real
// kludgy short-cut since in shao_ng_dts we only have a single output destination.
//
   if (!fs_path) 
      {fs_path = strdup(path); 
       memcpy((void *)&fs_id, (const void *)&buf.f_fsid, sizeof(fs_id));
      }
#endif
   return 1;
}

/******************************************************************************/
/*                                E n o u g h                                 */
/******************************************************************************/
  
int shao_ng_dts_FS_Unix::Enough(long long bytes, int numfiles)
{
#ifndef FREEBSD
    struct statvfs buf;
    long long free_space;

// Perform a stat call on the filesystem
//
   if (statvfs(fs_path, &buf)) return 0;

// Calculate free space
//
   free_space = (long long)buf.f_bsize * (long long)buf.f_bavail;

// Indicate whether there is enough space here
//
#ifdef LINUX
   if (!(buf.f_files | buf.f_ffree | buf.f_favail)) numfiles = 0;
#endif
   return (free_space > bytes) && buf.f_favail > numfiles;
#else
   return 1;
#endif
}

/******************************************************************************/
/*                                 F s y n c                                  */
/******************************************************************************/
  
int shao_ng_dts_FS_Unix::Fsync(const char *fn, int fd)
{
   int rc = 0;

// First do an fsync on the file
//
   if (fsync(fd)) return -errno;

// If a filename was passed, do an fsync on the directory as well
//
   if (fn)
      {const char *Slash = rindex(fn, '/');
       char dBuff[MAXPATHLEN+8];
       if (Slash)
          {int n = Slash - fn;
           strncpy(dBuff, fn, n); dBuff[n] = 0;
           if ((n = open(dBuff, O_RDONLY)) < 0) return -errno;
           if (fsync(n)) rc = -errno;
           close(n);
          }
      }

// All done
//
   return rc;
}

/******************************************************************************/
/*                               g e t S i z e                                */
/******************************************************************************/
  
long long shao_ng_dts_FS_Unix::getSize(int fd, long long *bsz)
{
   struct stat Stat;

// Get the size of the file
//
   if (fstat(fd, &Stat)) return -errno;
   if (bsz) *bsz = (secSize > 8192 ? 8192 : secSize);
   return Stat.st_size;
}

/******************************************************************************/
/*                                 M K D i r                                  */
/******************************************************************************/

int shao_ng_dts_FS_Unix::MKDir(const char *path, mode_t mode)
{
    if (mkdir(path, mode)) return -errno;

    if (chmod(path, 0755)) return -errno;

    return 0;
}
  
/******************************************************************************/
/*                                  O p e n                                   */
/******************************************************************************/

shao_ng_dts_File *shao_ng_dts_FS_Unix::Open(const char *fn, int opts, int mode, const char *fa)
{
    static const int rwxMask = S_ISUID|S_ISGID|S_ISVTX|S_IRWXU|S_IRWXG|S_IRWXO;
    int FD;
    shao_ng_dts_IO *iob;

// Check for direct I/O
//
#ifdef O_DIRECT
   if (dIO) opts |= O_DIRECT;
#endif

// Open the file
//
   mode &= rwxMask;
   if ((FD = (mode ? open(fn, opts, mode) : open(fn, opts))) < 0) 
      return (shao_ng_dts_File *)0;

// Advise about file access in Linux
//
#ifdef LINUX
   posix_fadvise(FD,0,0,POSIX_FADV_SEQUENTIAL|POSIX_FADV_NOREUSE);
#endif

// Do direct I/O for Solaris
//
#ifdef SUN
   if (dIO && directio(FD, DIRECTIO_ON))
      {DEBUG(strerror(errno) <<" requesting direct i/o for " <<fn); dIO = 0;}
#endif

// Allocate a file object and return that
//
   iob =  new shao_ng_dts_IO(FD);
   return new shao_ng_dts_File(fn, iob, (shao_ng_dts_FileSystem *)this, (dIO ? secSize : 0));
}
  
/******************************************************************************/
/*                                    R M                                     */
/******************************************************************************/
  
int shao_ng_dts_FS_Unix::RM(const char *path)
{
    if (!unlink(path)) return 0;
    return -errno;
}

/******************************************************************************/
/*                              s e t G r o u p                               */
/******************************************************************************/
  
int shao_ng_dts_FS_Unix::setGroup(const char *path, const char *Group)
{
    gid_t gid;

// Convert the group name to a gid
//
   if (!Group || !Group[0]) return 0;
   gid = shao_ng_dts_OS.getGID(Group);

// Set the group code and return
//
   if (chown(path, (uid_t)-1, gid)) return -errno;
   return 0;
}

/******************************************************************************/
/*                               s e t M o d e                                */
/******************************************************************************/
  
int shao_ng_dts_FS_Unix::setMode(const char *path, mode_t mode)
{
    if (chmod(path, mode)) return -errno;
    return 0;
}

/******************************************************************************/
/*                              s e t T i m e s                               */
/******************************************************************************/
  
int shao_ng_dts_FS_Unix::setTimes(const char *path, time_t atime, time_t mtime)
{
    struct utimbuf ftimes;

    ftimes.actime = atime;
    ftimes.modtime= mtime;
    if (utime(path, &ftimes)) return -errno;
    return 0;
}
 
/******************************************************************************/
/*                                  S t a t                                   */
/******************************************************************************/
  
int shao_ng_dts_FS_Unix::Stat(const char *path, shao_ng_dts_FileInfo *sbuff)
{
   struct stat xbuff;

// Perform the stat function
//
   if (stat(path, &xbuff)) return -errno;
   if (!sbuff) return 0;
   return Stat(xbuff, sbuff);
}

/******************************************************************************/
  
int shao_ng_dts_FS_Unix::Stat(const char *path, const char *dent, int fd,
                       int nolnks, shao_ng_dts_FileInfo *sbuff)
{
   struct stat xbuff;

// Perform the stat function
//
#ifdef AT_SYMLINK_NOFOLLOW
   if (fstatat(fd, dent, &xbuff, (nolnks?AT_SYMLINK_NOFOLLOW:0))) return -errno;
   if (nolnks && (xbuff.st_mode & S_IFMT) == S_IFLNK) return -ENOENT;
   if (!sbuff) return 0;
   return Stat(xbuff, sbuff);
#else
   if (nolnks)
      {if (lstat(path, &xbuff)) return -errno;
       if ((xbuff.st_mode & S_IFMT) == S_IFLNK) return -ENOENT;
          else return Stat(xbuff, sbuff);
      }
   if (!sbuff) return 0;
   return Stat(path, sbuff);
#endif
}

/******************************************************************************/
  
int shao_ng_dts_FS_Unix::Stat(struct stat &xbuff, shao_ng_dts_FileInfo *sbuff)
{
   static const int isXeq = S_IXUSR|S_IXGRP|S_IXOTH;

// Copy the stat info into our own structure
//
   sbuff->fileid = ((long long)xbuff.st_dev)<<32 | (long long)xbuff.st_ino;
   sbuff->mode   = xbuff.st_mode & (S_IAMB | 0xF00);
   sbuff->size   = xbuff.st_size;
   sbuff->atime  = xbuff.st_atime;
   sbuff->ctime  = xbuff.st_ctime;
   sbuff->mtime  = xbuff.st_mtime;

// Get type of object
//
//
        if (S_ISREG( xbuff.st_mode)){sbuff->Otype = 'f';
        if (isXeq &  xbuff.st_mode)  sbuff->Xtype = 'x';
                                    }
   else if (S_ISFIFO(xbuff.st_mode)) sbuff->Otype = 'p';
   else if (S_ISDIR( xbuff.st_mode)) sbuff->Otype = 'd';
   else                              sbuff->Otype = '?';

// Convert gid to a group name
//
   if (sbuff->Group) free(sbuff->Group);
   sbuff->Group = shao_ng_dts_OS.getGNM(xbuff.st_gid);

// All done
//
   return 0;
}
