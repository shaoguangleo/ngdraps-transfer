/******************************************************************************/
/*                                                                            */
/*                        shao_ng_dts_ I O _ P i p e . C                         */
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
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "shao_ng_dts_Headers.h"
#include "shao_ng_dts_IO_Pipe.h"
#include "shao_ng_dts_System.h"

/******************************************************************************/
/*                        G l o b a l   O b j e c t s                         */
/******************************************************************************/
  
extern shao_ng_dts_System shao_ng_dts_OS;
  
/******************************************************************************/
/*                                 C l o s e                                  */
/******************************************************************************/
  
int shao_ng_dts_IO_Pipe::Close()
{
   int oifd, rc, tc;

// If we are already closed, return
//
   if (iofd < 0) return 0;

// Close the file descriptor
//
   oifd = iofd; iofd = -1;
   do {rc = close(oifd);} while(rc < 0 && errno == EINTR);
   if (rc) rc = errno;

// Check if we should wait for the process to end
//
   tc = (thePid ? shao_ng_dts_OS.Waitpid(thePid) : 0);

// Return the result
//
   if (tc < 0) return tc;
   if (tc > 0) return -EPIPE;
   return rc;
}
