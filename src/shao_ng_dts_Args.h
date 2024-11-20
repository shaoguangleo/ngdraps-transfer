#ifndef __shao_ng_dts_ARGS_H__
#define __shao_ng_dts_ARGS_H__
/******************************************************************************/
/*                                                                            */
/*                           shao_ng_dts_ A r g s . h                            */
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

#include <stdlib.h>
#include <string.h>
#include "shao_ng_dts_Stream.h"

class shao_ng_dts_Opt;
  
class shao_ng_dts_Args
{
public:

char *getarg(int newln=0);

char  getopt();

void  Option(const char *optw, int minl, char optv, char opta); // For extended -- options

void  Options(char *opts, int fd, int moa=0);

void  Options(char *opts, int argc, char **argv);

      shao_ng_dts_Args(char *etxt=(char *)"Args: ");

    ~shao_ng_dts_Args();

char *argval;

private:

char            *epfx;
shao_ng_dts_Opt        *optp;
char            *vopts;
char            *curopt;
int              MOA;
int              inStream;
int              endopts;
int              Argc;
int              Aloc;
char           **Argv;
shao_ng_dts_Stream      arg_stream;
};
#endif
