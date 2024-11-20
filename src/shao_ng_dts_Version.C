/******************************************************************************/
/*                                                                            */
/*                        shao_ng_dts_ V e r s i o n . h                         */
/*                                                                            */
/*(c) 2010-14 by the Board of Trustees of the Leland Stanford, Jr., University*//*      All Rights Reserved. Scroll to end for Terms and Conditions of use    */
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
  
#include <strings.h>

#include "shao_ng_dts_Config.h"
#include "shao_ng_dts_Emsg.h"
#include "shao_ng_dts_Version.h"

/******************************************************************************/
/*                                                                            */
/*                        G l o b a l   O b j e c t s                         */
/*                                                                            */
/******************************************************************************/

extern shao_ng_dts_Config  shao_ng_dts_Config;

       shao_ng_dts_Version shao_ng_dts_Version;
  
/******************************************************************************/
/*                           C o n s t r u c t o r                            */
/******************************************************************************/
  
shao_ng_dts_Version::shao_ng_dts_Version()
{
Copyright = "(c) 2014-2024 by Shanghai Astronomical Observatory , Chinese Academy Of Sciences";
Author    = "Shaoguang Guo";
Version   = "Version: 0.9.0";
//           0123456789
VData     = Version+9;
};

/******************************************************************************/
/*                                 C h e c k                                  */
/******************************************************************************/
  
int shao_ng_dts_Version::Check(char *otherver)
{
    return strncmp(otherver, VData, 11);
};

/******************************************************************************/
/*                                V e r i f y                                 */
/******************************************************************************/
  
int shao_ng_dts_Version::Verify(char *node, char *nodever)
{
    int vchk = Check(nodever);
    char *vset;

    if (vchk < 0)         vset = (char *)"an older";
       else if (vchk > 0) vset = (char *)"a newer";
               else return 1;

    shao_ng_dts_Fmsg("Version", "Warning:",node,"is running",vset,"version of shao_ng_dts");

    if (shao_ng_dts_Config.Options & shao_ng_dts_BLAB)
       {shao_ng_dts_Fmsg("Version", node, "running version", nodever);
        shao_ng_dts_Fmsg("Version", shao_ng_dts_Config.MyHost, "running version", VData);
       }
    return 0;
};
 
/******************************************************************************/
/*           L i c e n s e   T e r m   a n d   C o n d i t i o n s            */
/******************************************************************************/
  
const char *shao_ng_dts_License =
"shao_ng_dts is free software: you can redistribute it and/or modify it under\n"
"the terms of the GNU Lesser General Public License as published by the\n"
"Free Software Foundation, either version 3 of the License, or (at your\n"
"option) any later version.\n\n"
"shao_ng_dts is distributed in the hope that it will be useful, but WITHOUT\n"
"ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or\n"
"FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public\n"
"License for more details.\n\n"
"You should have received a copy of the GNU Lesser General Public License\n"
"along with shao_ng_dts in a file called COPYING.LESSER (LGPL license) and file\n"
"COPYING (GPL license).  If not, see <http://www.gnu.org/licenses/>.\n\n"
"The copyright holder's institutional names and contributor's names may not\n"
"be used to endorse or promote products derived from this software without\n"
"specific prior written permission of the institution or contributor.";
