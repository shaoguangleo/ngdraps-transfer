#ifndef __shao_ng_dts_IO_H__
#define __shao_ng_dts_IO_H__
/******************************************************************************/
/*                                                                            */
/*                               shao_ng_dts_ I O                                */
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/uio.h>
#include "shao_ng_dts_Timer.h"

// This is a standard I/O class that is inherited by the Unix File and Net
// classes. For strange filesystems, define a new filesystem type that
// overrides the methods provided in this class. The I/O class is returned
// by the FileSystem class and is particular to each filesystem.
//
  
class shao_ng_dts_IO
{
public:

virtual int          Close();

        int          FD() {return iofd;}

virtual long long    ioStats() {return xfrbytes;}

virtual long long    ioStats(double &iotime)
                            {xfrtime.Report(iotime); return xfrbytes;}

virtual void         Log(const char *rk, const char *wk);

virtual int          Seek(long long offv);

virtual ssize_t      Read(char *buff, size_t rdsz);

virtual ssize_t      Read(const struct iovec *iovp, int iovn);

virtual ssize_t      Write(char *buff, size_t wrsz);

virtual ssize_t      Write(char *buff, size_t wrsz, off_t offs);

virtual ssize_t      Write(const struct iovec *iovp, int iovn);

             shao_ng_dts_IO(int fd=-1)
                    : RKeyA(0), RKeyZ(0), WKeyA(0), WKeyZ(0),
                      xfrbytes(0), xfrseek(0), iofd(fd) {}
virtual     ~shao_ng_dts_IO()          {Close();
                                 if (RKeyA) free(RKeyA);
                                 if (RKeyZ) free(RKeyZ);
                                 if (WKeyA) free(WKeyA);
                                 if (WKeyZ) free(WKeyZ);
                                }
protected:

char      *RKeyA, *RKeyZ, *WKeyA, *WKeyZ;
shao_ng_dts_Timer xfrtime;
long long  xfrbytes;
long long  xfrseek;
int        iofd;
};
#endif
