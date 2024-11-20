#ifndef __shao_ng_dts_IO_PIPE_H__
#define __shao_ng_dts_IO_PIPE_H__
/******************************************************************************/
/*                                                                            */
/*                          shao_ng_dts_ I O _ P i p e                           */
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

#include "shao_ng_dts_IO.h"
  
class shao_ng_dts_IO_Pipe : public virtual shao_ng_dts_IO
{
public:
        int     Close();

        int     Seek(long long offv) {return 0;}

        ssize_t Write(char *buff, size_t wrsz)
                     {return shao_ng_dts_IO::Write(buff, wrsz);}

        ssize_t Write(char *buff, size_t wrsz, off_t offs)
                     {return shao_ng_dts_IO::Write(buff, wrsz);}

        ssize_t Write(const struct iovec *iovp, int iovn)
                     {return shao_ng_dts_IO::Write(iovp, iovn);}

             shao_ng_dts_IO_Pipe(int fd, pid_t pnum=0) : shao_ng_dts_IO(fd), thePid(pnum) {}
            ~shao_ng_dts_IO_Pipe() {}

private:
pid_t thePid;
};
#endif