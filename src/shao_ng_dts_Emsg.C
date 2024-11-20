/******************************************************************************/
/*                                                                            */
/*                           shao_ng_dts_ E m s g . C                            */
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
#include "shao_ng_dts_Platform.h"
#include "shao_ng_dts_Headers.h"
#include "shao_ng_dts_Debug.h"
#include "shao_ng_dts_Emsg.h"

/******************************************************************************/

int shao_ng_dts_Emsg(const char *sfx, int ecode, const char *txt1, 
                                   const char *txt2, const char *txt3)
{
    int xcode;
    char ebuff[16], *etxt;

    xcode = (ecode < 0 ? -ecode : ecode);
    etxt = (char *)strerror(xcode);
    if (!strncmp(etxt, "Unknown", 7))
       {snprintf(ebuff, sizeof(ebuff), "Error %d", ecode);
        etxt = ebuff;
       }

    shao_ng_dts_Fmsg(sfx, etxt, txt1, txt2, txt3);

    return (ecode ? ecode : -1);
}

/******************************************************************************/

int shao_ng_dts_Fmsg(const char *sfx, const char *txt1, const char *txt2,
                               const char *txt3, const char *txt4,
                               const char *txt5, const char *txt6)
{
   const char *bV[] = {txt2, txt3, txt4, txt5, txt6};
   char *bP, buff[2048];
   int i;

   bP = buff + (sfx && shao_ng_dts_Debug.Trace
      ? sprintf(buff, "shao_ng_dts_%s.%s: %s", shao_ng_dts_Debug.Who, sfx, txt1)
      : sprintf(buff, "shao_ng_dts: %s", txt1));

   for (i = 0; i < 5 && bV[i]; i++) bP += sprintf(bP, " %s", bV[i]);
   strcpy(bP, "\n");
   cerr <<buff;
   return -1;
}
