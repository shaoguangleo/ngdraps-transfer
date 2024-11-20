#ifndef __shao_ng_dts_LOGFILE_H__
#define __shao_ng_dts_LOGFILE_H__
/******************************************************************************/
/*                                                                            */
/*                        shao_ng_dts_ L o g F i l e . h                         */
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
  
#include <unistd.h>
#include <sys/uio.h>
#include "shao_ng_dts_Timer.h"
#include "shao_ng_dts_Stream.h"
#include "shao_ng_dts_Pthread.h"

class shao_ng_dts_LogFiler;

class shao_ng_dts_LogFile
{
friend class shao_ng_dts_LogFiler;

public:

       int  Open(const char *fname);

       void Monitor(int fdnum, char *fdname);

static void Record(shao_ng_dts_LogFiler *lfP);

            shao_ng_dts_LogFile() : Logfd(-1), Logfn(0), Loggers(0) {}
           ~shao_ng_dts_LogFile();

private:

shao_ng_dts_Mutex     Flog;
int            Logfd;
char          *Logfn;
shao_ng_dts_LogFiler *Loggers;
};
#endif
