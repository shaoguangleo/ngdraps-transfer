/******************************************************************************/
/*                                                                            */
/*                           shao_ng_dts_ L i n k . C                            */
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
  
#include <sys/types.h>
#include <stdlib.h>
#include <strings.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include "shao_ng_dts_BuffPool.h"
#include "shao_ng_dts_Config.h"
#include "shao_ng_dts_Debug.h"
#include "shao_ng_dts_Emsg.h"
#include "shao_ng_dts_Link.h"
#include "shao_ng_dts_Network.h"
#include "shao_ng_dts_Platform.h"
#include "shao_ng_dts_Timer.h"

/******************************************************************************/
/*                     E x t e r n a l   P o i n t e r s                      */
/******************************************************************************/
  
extern shao_ng_dts_BuffPool shao_ng_dts_BPool;
  
extern shao_ng_dts_Config   shao_ng_dts_Config;

extern shao_ng_dts_Network  shao_ng_dts_Net;

/******************************************************************************/
/*                      S t a t i c   V a r i a b l e s                       */
/******************************************************************************/
  
int shao_ng_dts_Link::Nudge = 0;
int shao_ng_dts_Link::Wait  = 0;

/******************************************************************************/
/*                           C o n s t r u c t o r                            */
/******************************************************************************/

shao_ng_dts_Link::shao_ng_dts_Link(int fd, const char *lname)
          : LinkNum(0), Buddy(0),  Rendezvous(0), IOB(fd), Lname(strdup(lname))
{
   csObj = (shao_ng_dts_Config.csOpts & shao_ng_dts_csLink
         ? shao_ng_dts_ChkSum::Alloc(shao_ng_dts_Config.csType?shao_ng_dts_Config.csType:shao_ng_dts_csMD5)
         : 0);
}
  
/******************************************************************************/
/*                              B u f f 2 N e t                               */
/******************************************************************************/
  
int shao_ng_dts_Link::Buff2Net()
{
    shao_ng_dts_Buffer *outbuff;
    ssize_t wrsz, wlen = 0;
    const ssize_t hdrsz = (ssize_t)sizeof(shao_ng_dts_Header);
    int retc = 0, NotDone = 1, csLen = (csObj ? csObj->csSize() : 0);
    struct iovec iov[2] = {0, hdrsz, 0, 0};

// Establish logging options
//
   if (shao_ng_dts_Config.Options & shao_ng_dts_LOGOUT) IOB.Log(0, "NET");

// Do this until an error of eof
//
   while(NotDone)
      {
      // Obtain a buffer
      //
         if (!(outbuff = shao_ng_dts_BPool.getFullBuff()))
            {NotDone = -1; retc = ENOBUFS; break;}

      // Compose the header and see if control operation required
      //
         if (outbuff->blen <= 0)
            {if ((NotDone = Control_Out(outbuff)) < 0) {retc = 255; break;}}
            else shao_ng_dts_BPool.Encode(outbuff, SHAO_NG_DTS_IO);

      // Check if we should generate a checksum
      //
         if (csObj && outbuff->blen) memcpy(outbuff->bHdr.cksm,
            csObj->Calc(outbuff->data,outbuff->blen), csLen);

      // Write all of the data (header & data are sequential)
      //
         iov[0].iov_base = (char *)&outbuff->bHdr;
         iov[1].iov_base =  outbuff->data; iov[1].iov_len = outbuff->blen;
         wrsz = (ssize_t)outbuff->blen + hdrsz;
         if ((wlen = IOB.Write(iov, 2)) != wrsz) break;

      // Queue buffer for re-use
      //
         if (NotDone) shao_ng_dts_BPool.putEmptyBuff(outbuff);
            else      shao_ng_dts_BPool.putFullBuff(outbuff);
         outbuff = 0;

      // Tell our buddy that it's ok to continue then do a rendezvous
      //
         if (Nudge) {Buddy->Rendezvous.Post(); if (Wait) Rendezvous.Wait();}
      }

// Check how we ended this loop
//
   if (outbuff) shao_ng_dts_BPool.putEmptyBuff(outbuff);
   if (NotDone > 0 && !wlen)
      {shao_ng_dts_BPool.Abort();
       if (wlen < 0)  retc=shao_ng_dts_Emsg("Link",-wlen,"writing data for",Lname);
          else if (wlen > 0) {shao_ng_dts_Fmsg("Link","Data lost on link", Lname);
                              retc = 100;
                             }
      } else if (NotDone) shao_ng_dts_BPool.Abort();

// All done
//
   if (Nudge) {Wait = 0; Buddy->Rendezvous.Post();}
   return (retc < 0 ? -retc : retc);
}

/******************************************************************************/
/*                              N e t 2 B u f f                               */
/******************************************************************************/
  
int shao_ng_dts_Link::Net2Buff()
{
    static const char *Etxt[] = {"",
                                 "Invalid header length",   // 1
                                 "Invalid buffer length",   // 2
                                 "Invalid data length",     // 3
                                 "Invalid checksum",        // 4
                                 "Invalid hdr checksum"     // 5
                                };
    enum err_type {NONE = 0, IHL = 1, IBL = 2, IDL = 3, ICS = 4, IHS = 5};
    err_type ecode = NONE;
    shao_ng_dts_Buffer *inbuff;
    ssize_t rdsz, rlen = 0;
    ssize_t hdrsz = (ssize_t)sizeof(shao_ng_dts_Header);
    int  maxrdsz  = shao_ng_dts_BPool.DataSize();
    int i, notdone = 1, csLen = (csObj ? csObj->csSize() : 0);

// Establish logging options
//
   if (shao_ng_dts_Config.Options & shao_ng_dts_LOGIN) IOB.Log("NET", 0);

// Do this until an error of eof
//
   while(notdone)
      {
      // Obtain a buffer
      //
         if (!(inbuff = shao_ng_dts_BPool.getEmptyBuff()))
            {rlen = ENOBUFS; notdone = 0; break;}

      // Read the header information into the header buffer
      //
         if ((rlen = IOB.Read((char *)&inbuff->bHdr, hdrsz)) != hdrsz)
            {if (rlen > 0) {ecode = IHL; rlen = EINVAL;} break;}

      // Decode the header and make sure it decoded correctly
      //
         if (!shao_ng_dts_BPool.Decode(inbuff)) {ecode = IHS; break;}

      // Make sure the read length does not overflow our buffer
      //
         if ((rdsz = inbuff->blen) > maxrdsz) {ecode = IBL; break;}

      // Read data into the buffer and do checksum if needed
      //
         if (rdsz)
            {if ((rlen = IOB.Read(inbuff->data, rdsz)) != rdsz)
                {if (rlen > 0) ecode = IDL; break;}
             if (csObj && memcmp(csObj->Calc(inbuff->data,inbuff->blen),
                                 inbuff->bHdr.cksm,csLen)) {ecode = ICS; break;}
            }

      // Check if this is a control operation or data operation
      //
         if (inbuff->bHdr.cmnd == (char)SHAO_NG_DTS_IO)
            shao_ng_dts_BPool.putFullBuff(inbuff);
            else if ((notdone = Control_In(inbuff)) <= 0) break;

      // Tell our buddy that it's ok to continue then do a rendezvous
      //
         if (Nudge) {Buddy->Rendezvous.Post(); if (Wait) Rendezvous.Wait();}
      }

// If we ended the loop with an error, abort the buffer pool to force all
// threads dependent on the queue to abnormally terminate. Otherwise, post
// the buddy thread twice since that is the most that it may need to read.
//

   if (Nudge) {Wait = 0; i = shao_ng_dts_Config.Streams;
               do {Buddy->Rendezvous.Post();} while(i--);
              }
   if (notdone)
      {shao_ng_dts_BPool.Abort();
      if (ecode != NONE)
         {shao_ng_dts_Fmsg("Net2Buff", Etxt[(int)ecode], "from", Lname);
          return 128;
         }
      if (rlen >=0) return EPIPE;
      shao_ng_dts_Emsg("Net2Buff", rlen, "reading data from", Lname);
      return -rlen;
      }
   return 0;
}

/******************************************************************************/
/*                       P r i v a t e   M e t h o d s                        */
/******************************************************************************/
/******************************************************************************/
/*                            C o n t r o l _ I n                             */
/******************************************************************************/

int shao_ng_dts_Link::Control_In(shao_ng_dts_Buffer *bp)
{
    int  newsz;
    shao_ng_dts_Header *hp = &bp->bHdr;

// Check if this is a vanilla close request.
//
   if (hp->cmnd == (char)SHAO_NG_DTS_CLOSE)
      {DEBUG("Close request received on link " <<LinkNum);
       bp->blen = 0;
       shao_ng_dts_BPool.putFullBuff(bp);
       return 0;
      }

// Check if this is a checksum close request.
//
   if (hp->cmnd == (char)SHAO_NG_DTS_CLCKS)
      {DEBUG("Checksum close request received on link " <<LinkNum);
       if (shao_ng_dts_Config.csOpts & shao_ng_dts_csSend)
          {DEBUG("Setting checksum from link " <<LinkNum);
           shao_ng_dts_Config.setCS(bp->bHdr.cksm);
           bp->blen = 0;
          }
       shao_ng_dts_BPool.putFullBuff(bp);
       return 0;
      }

// Check if this is an abort equest. If so, queue the message and terminate
//
   if (hp->cmnd == (char)SHAO_NG_DTS_ABORT)
      {DEBUG("Abort request received on link " <<LinkNum);
       shao_ng_dts_BPool.putFullBuff(bp);
       return -1;
      }

// Unknown command here
//
   return shao_ng_dts_Emsg("Control_In", EINVAL, "invalid command code", Lname);
}

/******************************************************************************/
/*                           C o n t r o l _ O u t                            */
/******************************************************************************/
  
int shao_ng_dts_Link::Control_Out(shao_ng_dts_Buffer *bp)
{

// We are called because the buffer length is <= 0 which is the action code.
//

// EOF condition
//
   if ((!(bp->blen) && bp->boff >= 0) || bp->blen == -SHAO_NG_DTS_CLOSE)
      {shao_ng_dts_BPool.Encode(bp, SHAO_NG_DTS_CLOSE);
       DEBUG("Sending close request on link " <<LinkNum);
       return 0;
      }

// CLOSE with checksum
//
   if (bp->blen == -SHAO_NG_DTS_CLCKS)
      {bp->blen = 0;
       shao_ng_dts_BPool.Encode(bp, SHAO_NG_DTS_CLCKS);
       DEBUG("Sending close+chksum request on link " <<LinkNum);
       return 0;
      }

// ABORT
//
   bp->blen = 0;
   shao_ng_dts_BPool.Encode(bp, SHAO_NG_DTS_ABORT);
   DEBUG("Sending abort request on link " <<LinkNum);
   return -1;
}
