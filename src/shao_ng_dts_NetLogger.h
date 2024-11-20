#ifndef __shao_ng_dts_NETLOGGER_H__
#define __shao_ng_dts_NETLOGGER_H__
/******************************************************************************/
/*                                                                            */
/*                      shao_ng_dts_ N e t L o g g e r . h                       */
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

#ifdef NETLOGGER
#include "NetLogger.h"
  
class shao_ng_dts_NetLogger
{
public:

int  Close() {if (nl_handle) return NetLoggerClose(nl_handle); return -1;}

int  Emit(const char *key, const char *data, const char *fmt, ...);

int  Flush() {if (nl_handle) return NetLoggerFlush(nl_handle); return -1;}

int  Open(const char *pgm, char *url=0, int opts=0);

int  setOpts(int opts=0) {return openopts |= opts;}

     shao_ng_dts_NetLogger() {nl_handle = (NLhandle *)0; openopts = 0;}
    ~shao_ng_dts_NetLogger() {Close();}

private:

int       openopts;
NLhandle *nl_handle;
};
#else
  
class shao_ng_dts_NetLogger
{
public:

int  Close() {return 0;}

int  Emit(const char *key, const char *data, const char *fmt, ...) {return 0;}

int  Flush() {return 0;}

int  Open(const char *pgm, char *url=0, int opts=0) {return 0;}

int  setOpts(int opts=0) {return 0;}

     shao_ng_dts_NetLogger() {}
    ~shao_ng_dts_NetLogger() {}

};

#define NL_APPEND 0
#define NL_MEM    0
#endif
#endif
