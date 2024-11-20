#ifndef __shao_ng_dts_FILESPEC_H__
#define __shao_ng_dts_FILESPEC_H__
/******************************************************************************/
/*                                                                            */
/*                       shao_ng_dts_ F i l e S p e c . h                        */
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
#include <sys/types.h>
#include <utime.h>
#include "shao_ng_dts_FileSystem.h"
  
// Parse a spec formed as: [username@][hostname:]fname
//
class shao_ng_dts_FileSpec
{
public:

shao_ng_dts_FileSpec        *next;
char                 *username;
char                 *hostname;
char                 *pathname;
char                 *filename;
char                 *filereqn;
char                 *fileargs;
char                 *targpath;
char                 *targetfn;
long long             targetsz;
char                 *targsigf;
int                   seqno;
struct shao_ng_dts_FileInfo  Info;

int              Compose(long long did, char *dpath, int dplen, char *fname);

int              Create_Path();

int              Decode(char *buff, char *xName=0);

int              Encode(char *buff, size_t blen);

void             ExtendFileSpec(shao_ng_dts_FileSpec* headp, int &numF);

int              Finalize(int retc=0);

shao_ng_dts_FileSystem *FSys() {return FSp;}

void             Parse(char *spec, int isPipe=0);

int              setMode(mode_t Mode);

int              setStat();

int              Stat(int complain=1);

int              WriteSigFile();

int              Xfr_Done();

     shao_ng_dts_FileSpec(shao_ng_dts_FileSystem *fsp=0, char *hname = 0, char *uname=0)
                  : next(0), username(uname), hostname(hname), pathname(0),
                    filename(0), filereqn(0), fileargs(0),
                    targpath(0), targetfn(0), targetsz(0), targsigf(0),
                    fspec(0), fspec1(0), fspec2(0), FSp(fsp) {}
    ~shao_ng_dts_FileSpec() {if (fspec)    free(fspec);
                      if (fspec1)   free(fspec1);
                      if (fspec2)   free(fspec2);
                      if (targpath) free(targpath);
                      if (targsigf) free(targsigf);
                     }

private:

char            *fspec;
char            *fspec1;
char            *fspec2;
shao_ng_dts_FileSystem *FSp;
int              Xfr_Fixup();
void             BuildPaths();

};
#endif
