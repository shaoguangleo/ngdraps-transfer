#ifndef __shao_ng_dts_ChkSum_H__
#define __shao_ng_dts_ChkSum_H__
/******************************************************************************/
/*                                                                            */
/*                         shao_ng_dts_ C h k S u m . h                          */
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
  
class shao_ng_dts_ChkSum
{
public:

static shao_ng_dts_ChkSum *Alloc(int csType);

// The below calculates a one-time checksum
//
virtual       char *Calc(const char *Buff, int BLen)
                        {Init(); Update(Buff, BLen); return Final();}

// The below returns the current checksum value (defaults to final)
//
virtual       char *csCurr(char **Text=0) {return Final(Text);}

// csLen() returns the length of a checksum
//
virtual       int   csSize() = 0;

// At the end call Final that will return the actual checksum
//
virtual       char *Final(char **Text=0) = 0;

// Init() to initialize data structures (always called by constructor)
//
virtual       void  Init() = 0;

// Call this to get the character name of the checksum object
//
virtual const char *Type() = 0;

// Continue calling Update() to compute running checksum
//
virtual       void  Update(const char *Buff, int BLen) = 0;

                    shao_ng_dts_ChkSum() {}
virtual            ~shao_ng_dts_ChkSum() {}

protected:
char *x2a(char *inX);

char csBuff[40];
};
#endif
