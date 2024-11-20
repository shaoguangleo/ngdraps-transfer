#ifndef __shao_ng_dts_NETSOCKADDR_H__
#define __shao_ng_dts_NETSOCKADDR_H__
/******************************************************************************/
/*                                                                            */
/*                    shao_ng_dts_ N e t S o c k A d d r . h                     */
/*                                                                            */
/*(c) 2014-2024 by Shanghai Astronomical Observatory , Chinese Academy Of Sciences*/
/*      All Rights Reserved. See shao_ng_dts_Version.C for complete License Terms    */
/*                            All Rights Reserved                             */
/*   Produced by Shaoguang Guo Shanghai Astronomical Observatory , Chinese Academy Of Sciences*/
/*                                                                            */
/* shao_ng_dts is free software: you can redistribute it and/or modify it under      */
/* the terms of the GNU Lesser General Public License as published by the     */
/* Free Software Foundation, either version 3 of the License, or (at your     */
/* option) any later version. This file is derived from XrdNetSockAddr.cc part*/
/* of the XRootD software suite which is identically copyrighted and licensed.*/
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
  
#include <sys/socket.h>
#include <netinet/in.h>

//------------------------------------------------------------------------------
//! Define the smallest structure needed to hold ipv6 and ipv4 addresses. Note
//! we do not want to use sockadddr_storage for this case because it is sized
//! to handle all possible network addresses and winds up being outlandishly
//! bigger than we really need (e.g. 256 bytes in Solaris and 128 in Linux).
//------------------------------------------------------------------------------

union shao_ng_dts_NetSockAddr {struct sockaddr_in6 v6;
                        struct sockaddr_in  v4;
                        struct sockaddr     Addr;
                       };
#endif
