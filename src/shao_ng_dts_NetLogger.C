#ifdef NETLOGGER
/******************************************************************************/
/*                                                                            */
/*                      shao_ng_dts_ N e t L o g g e r . C                       */
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
#include <stdarg.h>
#include "shao_ng_dts_Headers.h"

#include "shao_ng_dts_NetLogger.h"
  
/******************************************************************************/
/*                                  E m i t                                   */
/******************************************************************************/
  
int shao_ng_dts_NetLogger::Emit(const char *key, const char *data, const char *fmt, ...)
{
    char buff[1024];
    va_list arg_pointer;
    va_start(arg_pointer, fmt);

    vsnprintf(buff, sizeof(buff), fmt, arg_pointer);
    return NetLoggerWrite(nl_handle, (char *)key, (char *)data, buff) == 1;

    va_end(arg_pointer);
}

/******************************************************************************/
/*                                  O p e n                                   */
/******************************************************************************/

int shao_ng_dts_NetLogger::Open(const char *pgm, char *url, int opts)
{
    return (nl_handle = NetLoggerOpen((char *)pgm, url,
           (url ? opts | openopts : opts | openopts | NL_ENV))) != 0;
}
#else
int shao_ng_dts_NetLoggerDisabled;
#endif
