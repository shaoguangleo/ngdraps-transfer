#ifndef __shao_ng_dts_Network_H__
#define __shao_ng_dts_Network_H__
/******************************************************************************/
/*                                                                            */
/*                        shao_ng_dts_ N e t w o r k . h                         */
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
#include <sys/socket.h>
#include "shao_ng_dts_Link.h"
#include "shao_ng_dts_Pthread.h"

// The shao_ng_dts_Network class defines a generic network where we can define common
// tcp/ip operations. This class is used by the global network object.
//
class shao_ng_dts_Network
{
public:

shao_ng_dts_Link   *Accept();

int          AutoTune() {return ATune;}

int          Bind(int minport, int maxport, int tries=1, int timeout=-1);

shao_ng_dts_Link   *Connect(char *host, int port, int retries=0, int rwait=1);

void         findPort(int &minport, int &maxport);

void         Flow(int isSRC);

char        *FullHostName(char *host=0, int asipaddr=0);

void         IPV4();

int          MaxWSize(int isSink);

int          MaxSSize() {return maxSegment;}

int          getWBSize(int xfd, int srwant);

int          QoS(int newQoS=-1);

static int   setPorts(int pnum1, int pnum2);

int          setWindow(int wsz, int noAT=0);

void         unBind() {if (iofd >= 0) {close(iofd); iofd = Portnum = -1;}}

             shao_ng_dts_Network();
            ~shao_ng_dts_Network() {unBind();}

private:

static int pFirst;
static int pLast;

int        accWait;
int        ATune;
int        iofd;
int        maxRcvBuff;
int        maxSndBuff;
int        maxSegment;
int        netQoS;
int        Portnum;
int        protID;
int        Sender;
int        Window;
int        WinSOP;

//int   getHostAddr(char *hostname, struct sockaddr_in &InetAddr);
//char *getHostName(struct sockaddr_in &addr);
int   Retry(int retries, int rwait);
void  setOpts(const char *who, int iofd);
int   setSegSz(const char *who, int iofd);
};
#endif
