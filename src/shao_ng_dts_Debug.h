#ifndef __shao_ng_dts_DEBUG_H__
#define __shao_ng_dts_DEBUG_H__
/******************************************************************************/
/*                                                                            */
/*                          shao_ng_dts_ D e b u g . h                           */
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

#include "shao_ng_dts_Headers.h"

struct shao_ng_dts_Debug
       {int   Trace;
        char *Who;
        int   mypid;

        shao_ng_dts_Debug() {Trace = 0; Who = (char *)"CTL"; mypid = (int)getpid();}
       ~shao_ng_dts_Debug() {}
      };

#ifndef shao_ng_dts_CONFIG_DEBUG
extern shao_ng_dts_Debug shao_ng_dts_Debug;
#endif
  
#define DEBUGON    (shao_ng_dts_Debug.Trace)

#define DEBUG(x)  {if (shao_ng_dts_Debug.Trace) \
                      cerr << "shao_ng_dts_" <<shao_ng_dts_Debug.Who <<' ' <<shao_ng_dts_Debug.mypid <<": " <<x <<endl <<flush;}
#endif
