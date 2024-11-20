#ifndef __shao_ng_dts_PLATFORM_H
#define __shao_ng_dts_PLATFORM_H
/******************************************************************************/
/*                                                                            */
/*                       shao_ng_dts_ P l a t f o r m . h                        */
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

#define PS_CMD "/bin/ps -o ppid -p %d"

#ifdef AIX
#define S_IAMB  0x1FF
#define GETHOSTBYNAME(hname, rbuff, cbuff, cblen,  rpnt, pretc) \
     (rpnt = gethostbyname(hname))

#define GETHOSTBYADDR(haddr,hlen,htype,rbuff,cbuff,cblen, rpnt,pretc) \
     (rpnt = gethostbyaddr(haddr,hlen,htype))

#define GETSERVBYNAME(name, stype, psrv, buff, blen,  rpnt) \
     (rpnt = getservbyname(name, stype))

#define EBADSLT EFAULT
#endif

#if defined(FREEBSD)
#define SYS_ERR_DEF const char *const
#define SYS_NER_DEF const int
#define IOV_MAX 16
#endif

#if defined(FREEBSD) || defined(MACOS)
#ifndef O_DSYNC
#define O_DSYNC 0
#endif
#endif

#if defined(FREEBSD) || defined(MACOS) || defined(AIX)

#define S_IAMB  0x1FF

#define GETHOSTBYNAME(hname, rbuff, cbuff, cblen,  rpnt, pretc) \
     (rpnt = gethostbyname(hname))

#define GETHOSTBYADDR(haddr,hlen,htype,rbuff,cbuff,cblen, rpnt,pretc) \
     (rpnt = gethostbyaddr(haddr,hlen,htype))

#define GETSERVBYNAME(name, stype, psrv, buff, blen,  rpnt) \
     (rpnt = getservbyname(name, stype))

#endif
  
#ifdef LINUX

// The value should be MAX_IOVEC but it's not defined in Linux, sigh
//
#ifndef IOV_MAX
#define IOV_MAX 16
#endif
#ifndef POLLRDNORM
#define POLLRDNORM 0
#endif
#ifndef POLLRDBAND
#define POLLRDBAND 0
#endif

#define SYS_ERR_DEF const char *const
#define SYS_NER_DEF       int

#define S_IAMB  0x1FF

#define FMTLL "L"

#ifdef CRAY_X1E
#define GETHOSTBYNAME(hname, rbuff, cbuff, cblen,  rpnt, pretc) \
     (rpnt = gethostbyname(hname))

#define GETHOSTBYADDR(haddr,hlen,htype,rbuff,cbuff,cblen, rpnt,pretc) \
     (rpnt = gethostbyaddr(haddr,hlen,htype))

#define GETSERVBYNAME(name, stype, psrv, buff, blen,  rpnt) \
     (rpnt = getservbyname(name, stype))
#else
#define GETHOSTBYNAME(hname, rbuff, cbuff, cblen,  rpnt, pretc) \
     (gethostbyname_r(hname, rbuff, cbuff, cblen, &rpnt, pretc) == 0 && rpnt)

#define GETHOSTBYADDR(haddr,hlen,htype,rbuff,cbuff,cblen, rpnt,pretc) \
     (gethostbyaddr_r(haddr,hlen,htype,rbuff,cbuff,cblen,&rpnt,pretc) == 0 && rpnt)

#define GETSERVBYNAME(name, stype, psrv, buff, blen,  rpnt) \
     (getservbyname_r(name, stype, psrv, buff, blen, &rpnt) == 0 && rpnt)
#endif

#else

#define FMTLL "ll"

#endif

#if !defined(LINUX) && !defined(FREEBSD) && !defined(AIX) && !defined(MACOS)

#define SYS_ERR_DEF char *
#define SYS_NER_DEF int

#define GETHOSTBYNAME(hname, rbuff, cbuff, cblen, rpnt, pretc) \
(rpnt=gethostbyname_r(hname, rbuff, cbuff, cblen,       pretc))

#define GETHOSTBYADDR(haddr, hlen, htype, rbuff, cbuff, cblen, rpnt, pretc) \
(rpnt=gethostbyaddr_r(haddr, hlen, htype, rbuff, cbuff, cblen,       pretc))


#define GETSERVBYNAME(name, stype, psrv, buff, blen, rpnt) \
(rpnt=getservbyname_r(name, stype, psrv, buff, blen))

#endif

typedef       void * gsval_t;
typedef       void * ssval_t;
typedef size_t Size_T;

#ifdef FREEBSD
#define EPROTO  EPROTOTYPE
#define ENOSR   EILSEQ
#endif

#if defined(FREEBSD) || defined(MACOS) || defined(AIX)
#define EBADSLT EFAULT
#endif

#endif
