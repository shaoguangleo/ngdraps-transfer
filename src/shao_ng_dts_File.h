#ifndef __shao_ng_dts_FILE_H__
#define __shao_ng_dts_FILE_H__
/******************************************************************************/
/*                                                                            */
/*                           shao_ng_dts_ F i l e . h                            */
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

#include <string.h>
#include "shao_ng_dts_BuffPool.h"
#include "shao_ng_dts_IO.h"
#include "shao_ng_dts_Pthread.h"
#include "shao_ng_dts_Timer.h"

// The shao_ng_dts_File class describes the set of operations that are needed to copy
// a "file". The actual I/O are determined by the associated filesystem and
// are specified during instantiation.

class  shao_ng_dts_FileChkSum;
class  shao_ng_dts_FileSystem;

class shao_ng_dts_File
{
public:

// Return target file system
//
shao_ng_dts_FileSystem *Fsys() {return FSp;}

// Return FD number
//
int          ioFD() {return IOB->FD();}

// Read a record from a file
//
ssize_t      Get(char *buff, size_t blen) {return IOB->Read(buff, blen);}

// Internal buffer-to-buffer passthrough function
//
int          Passthru(shao_ng_dts_BuffPool *iBP, shao_ng_dts_BuffPool *oBP,
                      shao_ng_dts_FileChkSum *csP, int nstrms);

// Return path to the file
//
char        *Path() {return iofn;}

// Write a record to a file
//
ssize_t      Put(char *buff, size_t blen) {return IOB->Write(buff, blen);}

// Read_All() reads the file until eof and returns 0 (success) or -errno.
//
int          Read_All(shao_ng_dts_BuffPool &buffpool, int Bfact);

// Write_All() writes the file until eof and return 0 (success) or -errno.
//
int          Write_All(shao_ng_dts_BuffPool &buffpool, int nstrms);

// Sets the file pointer to read or write from an offset
//
int          Seek(long long offv) {nextoffset = offv; return IOB->Seek(offv);}

// setSize() sets the expected file size
//
void         setSize(long long top) {lastoff = top;}

// Stats() reports the i/o time and buffer wait time in milliseconds and
//         returns the total number of bytes transfered.
//
long long    Stats(double &iotime) {return IOB->ioStats(iotime);}

long long    Stats()               {return IOB->ioStats();}

             shao_ng_dts_File(const char *path, shao_ng_dts_IO *iox,
                       shao_ng_dts_FileSystem *fsp, int secSize=0);

            ~shao_ng_dts_File() {if (iofn) {free(iofn); iofn = 0;}
                          if (IOB)  {delete IOB; IOB  = 0;}
                         }

int          bufreorders;
int          maxreorders;

private:

shao_ng_dts_Buffer     *nextbuff;
long long        nextoffset;
long long        lastoff;
long long        bytesLeft;
long long        blockSize;
long long        PaceTime;
shao_ng_dts_Timer       Ticker;
int              rtCopy;
int              curq;
shao_ng_dts_IO         *IOB;
shao_ng_dts_FileSystem *FSp;
char            *iofn;

shao_ng_dts_Buffer *getBuffer(long long offset);
int          Read_Direct (shao_ng_dts_BuffPool *inB, shao_ng_dts_BuffPool *otP);
int          Read_Pipe   (shao_ng_dts_BuffPool *inB, shao_ng_dts_BuffPool *otP);
int          Read_Normal (shao_ng_dts_BuffPool *inB, shao_ng_dts_BuffPool *otP);
int          Read_Vector (shao_ng_dts_BuffPool *inB, shao_ng_dts_BuffPool *otP, int vN);
void         Read_Wait   (int rdsz);
void         Read_Wait   ();
int          verChkSum(shao_ng_dts_FileChkSum *csP);
int          Write_Direct(shao_ng_dts_BuffPool *iBP, shao_ng_dts_BuffPool *oBP, int nstrms);
int          Write_Normal(shao_ng_dts_BuffPool *iBP, shao_ng_dts_BuffPool *oBP, int nstrms);
};
#endif
