#ifndef __shao_ng_dts_TIMER_H__
#define __shao_ng_dts_TIMER_H__
/******************************************************************************/
/*                                                                            */
/*                          shao_ng_dts_ T i m e r . h                           */
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

#include <time.h>
#include <sys/time.h>

// This include file describes the oo elapsed time interval interface.
//

class shao_ng_dts_Timer {

public:

       int  Format(char *tbuff);

inline void Report(unsigned int  &Total_Time) // Report in milliseconds
                   {Total_Time = static_cast<unsigned int>(TotalTime/1000);}

inline void Report(long     long &Total_Time) // Report in microseconds
                   {Total_Time = TotalTime;}

inline void Report(double        &Total_Time) // Report in millisecond
                   {Total_Time = static_cast<double>(TotalTime)/1000.0;}

inline void Reset()   {TotalTime = 0; gettimeofday(&StopWatch, 0);}

inline void Start()   {gettimeofday(&StopWatch, 0);}

       void Stop();

       void Wait(int       milliseconds);

       void Wait(long long microseconds);

       shao_ng_dts_Timer() {Reset();}
      ~shao_ng_dts_Timer() {}

private:
      long long      TotalTime;     // Total time so far in microseconds
      struct timeval StopWatch;     // Current running clock
};
#endif
