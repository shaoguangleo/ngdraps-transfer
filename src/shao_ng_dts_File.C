/******************************************************************************/
/*                                                                            */
/*                           shao_ng_dts_ F i l e . C                            */
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
  
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include "shao_ng_dts_Platform.h"
#include "shao_ng_dts_ChkSum.h"
#include "shao_ng_dts_Config.h"
#include "shao_ng_dts_Debug.h"
#include "shao_ng_dts_Emsg.h"
#include "shao_ng_dts_File.h"
#include "shao_ng_dts_Headers.h"
#include "shao_ng_dts_RTCopy.h"

#ifdef FREEBSD
#undef ENODATA
#define ENODATA ENOATTRR
#endif

/******************************************************************************/
/*                         L o c a l   C l a s s e s                          */
/******************************************************************************/
  
class shao_ng_dts_FileChkSum
{
public:

shao_ng_dts_File       *csFile;
shao_ng_dts_BuffPool   *otPool;
shao_ng_dts_BuffPool    csPool;
shao_ng_dts_ChkSum     *csObj;
int              csVer;
int              csRC;

     shao_ng_dts_FileChkSum(shao_ng_dts_BuffPool *obP,shao_ng_dts_File *fP,int csT,int csV,int n=0)
                    : csFile(fP), otPool(obP), csObj(shao_ng_dts_ChkSum::Alloc(csT)),
                      csVer(csV), csRC(n) {}
    ~shao_ng_dts_FileChkSum() {if (csObj) delete csObj;}
};

/******************************************************************************/
/*                         G l o b a l   V a l u e s                          */
/******************************************************************************/
  
extern shao_ng_dts_BuffPool shao_ng_dts_BPool;

extern shao_ng_dts_BuffPool shao_ng_dts_CPool;

extern shao_ng_dts_Config   shao_ng_dts_Config;

extern shao_ng_dts_RTCopy   shao_ng_dts_RTCopy;
 
/******************************************************************************/
/*            E x t e r n a l   T h r e a d   I n t e r f a c e s             */
/******************************************************************************/
  
extern "C"
{
void *shao_ng_dts_FileCSX(void *pp)
{
   shao_ng_dts_FileChkSum *csP = (shao_ng_dts_FileChkSum *)pp;
   shao_ng_dts_Buffer *bP;
   int csLen = csP->csObj->csSize(), csCopy = csP->csVer;

// Simple loop to do checksumming
//
   while((bP = csP->csPool.getFullBuff()) && bP->blen)
        {csP->csObj->Update(bP->data, bP->blen);
         if (csCopy) memcpy(bP->bHdr.cksm, csP->csObj->csCurr(), csLen);
         csP->otPool->putFullBuff(bP);
        }

// Send checksum to the target if so needed
//
   if ((shao_ng_dts_Config.csOpts & shao_ng_dts_csSend) && bP && bP->boff >= 0)
      {bP->blen = -SHAO_NG_DTS_CLCKS;
       memcpy(bP->bHdr.cksm, csP->csObj->csCurr(), csLen);
      }

// Either we aborted or ended. Make sure to cascade null buffer to signal eof.
//
   if (bP) csP->otPool->putFullBuff(bP);
   return 0;
}

void *shao_ng_dts_FileCSY(void *pp)
{
   shao_ng_dts_FileChkSum *csP = (shao_ng_dts_FileChkSum *)pp;

   csP->csRC = csP->csFile->Passthru(csP->otPool, &csP->csPool, csP, csP->csRC);
   return 0;
}
}

/******************************************************************************/
/*                           C o n s t r u c t o r                            */
/******************************************************************************/

shao_ng_dts_File::shao_ng_dts_File(const char *path, shao_ng_dts_IO *iox,
                     shao_ng_dts_FileSystem *fsp, int secSize)
{
   nextbuff    = 0;
   nextoffset  = 0;
   lastoff     = 0;
   blockSize   = secSize;
   curq        = 0;
   iofn        = strdup(path);
   IOB         = iox;
   FSp         = fsp;
   bufreorders = 0;
   maxreorders = 0;
   PaceTime    = 0;
   rtCopy      = (shao_ng_dts_Config.Options & shao_ng_dts_RTCSRC ? 1 : 0);
}

/******************************************************************************/
/* Private:                    g e t B u f f e r                              */
/******************************************************************************/

shao_ng_dts_Buffer *shao_ng_dts_File::getBuffer(long long offset)
{
   shao_ng_dts_Buffer *bp, *pp=0;

// Find a buffer
//
   if (bp = nextbuff)
      while(bp && bp->boff != offset) {pp = bp; bp = bp->next;}

// If we found a buffer, unchain it
//
   if (bp) {curq--;
            if (pp) pp->next = bp->next;
                else nextbuff = bp->next;
//if (!curq) {DEBUG("Queue has been emptied at offset " <<offset);}
           }

// Return what we have
//
   return bp;
}

/******************************************************************************/
/* Public:                   P a s s t h r o u g h                            */
/******************************************************************************/
  
int shao_ng_dts_File::Passthru(shao_ng_dts_BuffPool *iBP, shao_ng_dts_BuffPool *oBP, 
                        shao_ng_dts_FileChkSum *csP, int nstrms)
{
    shao_ng_dts_Buffer *outbuff;
    shao_ng_dts_ChkSum *csObj;
    long long Offset = nextoffset;
    int csLen, csVer, numadd, maxbufs, maxadds = nstrms;
    int rc = 0, unordered = !(shao_ng_dts_Config.Options & shao_ng_dts_ORDER);

// Determine if we will be piggy-backing checksumming here
//
   if (csP && (csObj = csP->csObj))
      {csVer = csP->csVer; csLen = csObj->csSize();}
      else csVer = csLen = 0;

// Record the maximum number of buffers we have here
//
   maxbufs = iBP->BuffCount();
   numadd  = nstrms+1;

// Read all of the data until eof (note that we are single threaded)
//
   while(nstrms)
      {
      // Obtain a full buffer
      //
         if (!(outbuff = iBP->getFullBuff())) break;

     // Check if this is an eof marker
     //
     //  cerr <<nstrms <<" Passt " <<outbuff->blen <<'@' <<outbuff->boff <<endl;
        if (!(outbuff->blen))
           {iBP->putEmptyBuff(outbuff); nstrms--; continue;}

      // Do an unordered write if allowed
      //
         if (unordered) {oBP->putFullBuff(outbuff); continue;}

      // Check if this buffer is in the correct sequence
      //
         if (outbuff->boff != Offset)
            {if (outbuff->boff < 0) {rc = -ESPIPE; break;}
             outbuff->next = nextbuff;
             nextbuff = outbuff;
             bufreorders++;
             if (++curq > maxreorders) 
                {maxreorders = curq;
                 DEBUG("Buff disorder " <<curq <<" rcvd " <<outbuff->boff <<" want " <<Offset);
                }
             if (curq >= maxbufs-nstrms)
                {if (!(--maxadds)) {rc = -ENOBUFS; break;}
                 DEBUG("Too few buffs; adding " <<numadd <<" more; " <<maxadds <<" tries left.");
                 shao_ng_dts_BPool.Allocate(numadd);
                 maxbufs += numadd;
                }
             continue;
            }

      // Pass through any queued buffers
      //
      do {Offset += outbuff->blen;
          if (csObj)
             {csObj->Update(outbuff->data, outbuff->blen);
              if (csVer && memcmp(outbuff->bHdr.cksm, csObj->csCurr(), csLen))
                 {char buff[32];
                  sprintf(buff, "%lld", outbuff->boff);
                  shao_ng_dts_Fmsg("Write",iofn, "xfr checksum error at offset",buff);
                  rc = -EILSEQ; nstrms = 0; break;
                 }
             }
          oBP->putFullBuff(outbuff);
         } while(nextbuff && (outbuff = getBuffer(Offset)));
      }

// Check if we should print an error here
//
   if (rc && rc != -EILSEQ) shao_ng_dts_Emsg("Write",rc, "unable to write", iofn);

// Queue an empty buffer indicating eof or abort the stream
//
   if (!rc && (outbuff = iBP->getEmptyBuff()))
      {outbuff->blen = 0; outbuff->boff = Offset;
       oBP->putFullBuff(outbuff);
      } else {
       if (!rc) rc = -ENOBUFS;
       oBP->Abort(); iBP->Abort();
      }

// All done
//
   return rc;
}
  
/******************************************************************************/
/* Public:                      R e a d _ A l l                               */
/******************************************************************************/
  
int shao_ng_dts_File::Read_All(shao_ng_dts_BuffPool &inPool, int Vn)
{
    shao_ng_dts_FileChkSum *csP = 0;
    shao_ng_dts_BuffPool *outPool;
    shao_ng_dts_Buffer   *bP;
    pthread_t tid;
    int ec, rc = 0;

// Get the size of the file
//
   if ((bytesLeft = FSp->getSize(IOB->FD())) < 0)
      {shao_ng_dts_Emsg("Read", static_cast<int>(-bytesLeft), "stat", iofn);
       inPool.Abort(); return 200;
      }

// Adjust bytes left based on where we will be reading from
//
   bytesLeft -= nextoffset;
   if (bytesLeft < 0)
      {shao_ng_dts_Emsg("Read", ESPIPE, "stat", iofn);
       inPool.Abort(); return 200;
      }

// If this is a real-time copy operation, start the rtcopy object
//
   if (rtCopy && !shao_ng_dts_RTCopy.Start(FSp, iofn, IOB->FD()))
      {inPool.Abort(); return 200;
      }

// Set up checksumming. We would prefer to do this in the calling thread but
// this is easier. One day we will generalize buffer piping.
//
   if (shao_ng_dts_Config.csOpts & shao_ng_dts_csVerIn)
      {csP = new shao_ng_dts_FileChkSum(&inPool, this, shao_ng_dts_Config.csType,
                                 shao_ng_dts_Config.csOpts & shao_ng_dts_csVerIO);
       if ((rc = shao_ng_dts_Thread_Start(shao_ng_dts_FileCSX, (void *)csP, &tid)) < 0)
           {shao_ng_dts_Emsg("File", rc, "starting file checksum thread.");
            delete csP;
            if (rtCopy) shao_ng_dts_RTCopy.Stop();
            inPool.Abort(); return 201;
           }
       outPool =  &csP->csPool;
      } else outPool = &inPool;

// Establish logging options
//
   if (shao_ng_dts_Config.Options & shao_ng_dts_LOGRD) IOB->Log("DISK", 0);

// Determine what kind of reading we will do here and do it
//
// cerr <<"BLOCKSIZE " <<blockSize <<endl;
        if (shao_ng_dts_Config.Options & shao_ng_dts_XPIPE)
                        rc = Read_Pipe  (&inPool, outPool);
   else if (blockSize ) rc = Read_Direct(&inPool, outPool);
   else rc = (Vn > 1       ? Read_Vector(&inPool, outPool, Vn)
                           : Read_Normal(&inPool, outPool));

// Delete the real-time copy object if we have one to kill possible thread
//
   if (rtCopy) shao_ng_dts_RTCopy.Stop();

// Check if we ended because with an error
//
   if (rc && rc != -ENOBUFS)
      {const char *Act=(shao_ng_dts_Config.Options & shao_ng_dts_XPIPE ? "piping":"writing");
       shao_ng_dts_Emsg("Read", -rc, Act, iofn);
      }

// Now close the input file, make sure no errors occur here
//
   if ((ec = IOB->Close()))
      if (!rc) {shao_ng_dts_Emsg("Read", -ec, "closing", iofn); rc = ec;}

// Prepare an empty buffer to shutdown the buffer pipeline. The offet indicates
// how much data should have been sent and received. A negative offset implies
// that we encountered an error.
//
   if (!(bP = inPool.getEmptyBuff())) return 255;
   bP->blen = 0;
   bP->boff = (rc ? -1 : nextoffset);

// Verify check sum if so wanted. Note that is link-level check summing is in
//
   if (csP)
      {csP->csPool.putFullBuff(bP);
       shao_ng_dts_Thread_Wait(tid);
       if (!rc && *shao_ng_dts_Config.csString)
          {char *csTxt, *csVal = csP->csObj->Final(&csTxt);
           if (memcmp(csVal, shao_ng_dts_Config.csValue, csP->csObj->csSize()))
              {shao_ng_dts_Fmsg("Read", iofn, "source checksum", shao_ng_dts_Config.csString,
                         "does not match", csTxt);
               rc = EILSEQ;
              } else {DEBUG(csP->csObj->Type() <<": " <<csTxt <<' ' <<iofn);}
          }
       delete csP;
      } else inPool.putFullBuff(bP);

// All done
//
   DEBUG("EOF offset=" <<nextoffset <<" rc=" <<rc <<" fn=" <<iofn);
   return rc;
}

/******************************************************************************/
/* Private:                  R e a d _ D i r e c t                            */
/******************************************************************************/

int shao_ng_dts_File::Read_Direct(shao_ng_dts_BuffPool *iBP, shao_ng_dts_BuffPool *oBP)
{
    shao_ng_dts_Buffer  *bP;
    ssize_t rlen;
    int Trunc = 0, rdsz = iBP->DataSize();

// Initialize transfer rate limiting if so desired
//
   if (shao_ng_dts_Config.Xrate) Read_Wait(rdsz);

// Simply read one buffer at a time, that's the fastest way to do this
//
// cerr <<"DIRECT READ SIZE=" <<rdsz <<endl;
   while(bytesLeft > 0)
      {
      // Obtain buffer
      //
         if (!(bP = iBP->getEmptyBuff())) return -ENOBUFS;

      // Check if we are reading the last segment. Inflate it to the block
      // size value (we have room in the buffer) and indicate trunc() needed.
      // Otherwise, do a normal full-sized read.
      //
         if (bytesLeft < rdsz)
            {rdsz = bytesLeft + (blockSize - 1) & ~(blockSize - 1);
             if ((rlen = IOB->Read(bP->data, rdsz)) < bytesLeft)
                {iBP->putEmptyBuff(bP); break;}
             Trunc = 1; rlen = bytesLeft;
            } else {
             if ((rlen = IOB->Read(bP->data, rdsz)) <= 0)
                {iBP->putEmptyBuff(bP); break;}
            }

      // Queue a filled buffer for further processing
      //
         bytesLeft -= rlen;
         bP->boff   = nextoffset; nextoffset += rlen;
         bP->blen   = rlen;
         oBP->putFullBuff(bP);

      // Limit Transfer rate if need be
      //
         if (PaceTime && bytesLeft > 0) Read_Wait();
      }
  
// All done
//
   if (bytesLeft) return (rlen ? static_cast<int>(rlen) : -ENODATA);
   return 0;
}

/******************************************************************************/
/* Private:                  R e a d _ N o r m a l                            */
/******************************************************************************/

int shao_ng_dts_File::Read_Normal(shao_ng_dts_BuffPool *iBP, shao_ng_dts_BuffPool *oBP)
{
    shao_ng_dts_Buffer  *bP;
    ssize_t rlen;
    int rdsz = iBP->DataSize();

// Initialize transfer rate limiting if so desired
//
   if (shao_ng_dts_Config.Xrate) Read_Wait(rdsz);

// Simply read one buffer at a time, that's the fastest way to do this
//
// cerr <<"NORMAL READ SIZE=" <<rdsz <<endl;
   do {
      // Do real-time copy if need be
      //
         if (rtCopy && (bytesLeft=shao_ng_dts_RTCopy.Prep(nextoffset,rdsz,rlen)) <= 0)
            break;

      // Obtain buffer
      //
         if (!(bP = iBP->getEmptyBuff())) return -ENOBUFS;

      // Read data into the buffer
      //
         if ((rlen = IOB->Read(bP->data, rdsz)) <= 0)
            {iBP->putEmptyBuff(bP); break;}

      // Queue a filled buffer for further processing
      //
         bytesLeft -= rlen;
         bP->boff   =  nextoffset; nextoffset += rlen;
         bP->blen   = rlen;
         oBP->putFullBuff(bP);

      // Limit Transfer rate if need be
      //
         if (PaceTime && rlen == rdsz && bytesLeft > 0) Read_Wait();

      } while(rlen == rdsz && bytesLeft > 0);

// All done
//
   if (bytesLeft) return (rlen ? static_cast<int>(rlen) : -ENODATA);
   return 0;
}

/******************************************************************************/
/* Private:                    R e a d _ P i p e                              */
/******************************************************************************/

int shao_ng_dts_File::Read_Pipe(shao_ng_dts_BuffPool *iBP, shao_ng_dts_BuffPool *oBP)
{
    shao_ng_dts_Buffer  *bP;
    ssize_t rlen;
    int rdsz = iBP->DataSize();

// Initialize transfer rate limiting if so desired
//
   if (shao_ng_dts_Config.Xrate) Read_Wait(rdsz);

// Simply read one buffer at a time, that's the fastest way to do this
//
// cerr <<"PIPE   READ SIZE=" <<rdsz <<endl;
   do {
      // Obtain buffer
      //
         if (!(bP = iBP->getEmptyBuff())) return -ENOBUFS;

      // Read data into the buffer
      //
         if ((rlen = IOB->Read(bP->data, rdsz)) <= 0)
            {iBP->putEmptyBuff(bP); break;}

      // Queue a filled buffer for further processing
      //
         bP->boff   =  nextoffset; nextoffset += rlen;
         bP->blen   = rlen;
         oBP->putFullBuff(bP);

      // Limit Transfer rate if need be
      //
         if (PaceTime && rlen == rdsz) Read_Wait();
      } while(rlen == rdsz);

// All done
//
   return 0;
}

/******************************************************************************/
/* Private:                  R e a d _ V e c t o r                            */
/******************************************************************************/

int shao_ng_dts_File::Read_Vector(shao_ng_dts_BuffPool *iBP, shao_ng_dts_BuffPool *oBP, int vNum)
{
    shao_ng_dts_Buffer *ioBuff[IOV_MAX];
    struct iovec ioVec [IOV_MAX];
    ssize_t blen, rlen;
    int rdsz = iBP->DataSize(), numV = (vNum > IOV_MAX ? IOV_MAX : vNum);
    int i, ivN, eof = 0;

// Initialize transfer rate limiting if so desired
//
   if (shao_ng_dts_Config.Xrate) Read_Wait(rdsz*numV);

// Simply read one buffer at a time, that's the fastest way to do this
//
// cerr <<"VECTOR READ SIZE=" <<rdsz <<" v=" <<numV <<endl;
   while(!eof)
      {
      // Obtain buffers
      //
         for (ivN = 0; ivN < numV && bytesLeft > 0; ivN++)
             {if (!(ioBuff[ivN] = iBP->getEmptyBuff())) return -ENOBUFS;
              ioVec[ivN].iov_base = (caddr_t)(ioBuff[ivN]->data);
              ioVec[ivN].iov_len  = (rdsz < bytesLeft ? rdsz : bytesLeft);
              bytesLeft -= rdsz;
             }

      // Read data into the buffer
      //
         if ((rlen = IOB->Read((const struct iovec *)&ioVec, ivN)) <= 0)
            {for (i = 0; i < ivN; i++) iBP->putEmptyBuff(ioBuff[i]);
             eof = !rlen; break;
            }

      // Queue a filled buffers for further processing
      //
         for (i = 0; i < ivN && rlen; i++)
             {blen = (rlen >= rdsz ? rdsz : rlen);
              ioBuff[i]->boff = nextoffset; nextoffset += blen;
              ioBuff[i]->blen = blen;       rlen       -= blen;
              oBP->putFullBuff(ioBuff[i]);
             }

      // Return any empty buffers
      //
         eof = i < ivN || bytesLeft <= 0;
         while(i < ivN) iBP->putEmptyBuff(ioBuff[i++]);

      // Limit Transfer rate if need be
      //
         if (PaceTime && !eof) Read_Wait();
      }

// All done
//
   if (!eof) return (rlen ? static_cast<int>(rlen) : -ENODATA);
   return 0;
}

/******************************************************************************/
/*                             R e a d _ W a i t                              */
/******************************************************************************/
  
void shao_ng_dts_File::Read_Wait(int rdsz)
{
   static const double usPerSec = 1000000.0;
   double nBlk;

// Calculate number of micro seconds each read should take to achive the
// desired transfer rate. This may cause us to not limit the transfer at all.
//
   nBlk = static_cast<double>(shao_ng_dts_Config.Xrate)
        / static_cast<double>(rdsz);
   PaceTime = static_cast<long long>(usPerSec / nBlk);
   DEBUG("Pacing " <<rdsz <<" * " <<nBlk <<" @ " <<PaceTime <<"us = "
         <<shao_ng_dts_Config.Xrate <<"/sec");
   Ticker.Reset();
}

/******************************************************************************/

void shao_ng_dts_File::Read_Wait()
{
   long long Etime;

// Calculate how time was spent reading the data. Wait if the read was faster
// than we need it to be to achieve the maximum transfer rate.
//
   Ticker.Stop(); Ticker.Report(Etime);
   if (Etime < PaceTime) Ticker.Wait((long long)((PaceTime - Etime)));
   Ticker.Reset();
}

/******************************************************************************/
/* Private:                    v e r C h k S u m                              */
/******************************************************************************/
  
int shao_ng_dts_File::verChkSum(shao_ng_dts_FileChkSum *csP)
{

// If there was a transmission error, simply return the rc
//
   if (csP->csRC) return csP->csRC;

// Check if we should verify this against a known value
//
   if (shao_ng_dts_Config.csOpts & shao_ng_dts_csVerOut)
      {int   csLen         = csP->csObj->csSize();
       char *csTxt, *csVal = csP->csObj->Final(&csTxt);
       if (*shao_ng_dts_Config.csString)
          {if (memcmp(csVal, shao_ng_dts_Config.csValue, csP->csObj->csSize()))
              {shao_ng_dts_Fmsg("Write", iofn, "target checksum", shao_ng_dts_Config.csString,
                         "does not match", csTxt);
               return -EILSEQ;
              }
          } else {
           memcpy(shao_ng_dts_Config.csValue, csVal, csLen);
           strcpy(shao_ng_dts_Config.csString, csTxt);
          }
      }

// All done
//
   DEBUG(csP->csObj->Type() <<": '" <<shao_ng_dts_Config.csString <<"' " <<iofn);
   return 0;
}

/******************************************************************************/
/* Public:                     W r i t e _ A l l                              */
/******************************************************************************/
  
int shao_ng_dts_File::Write_All(shao_ng_dts_BuffPool &inPool, int nstrms)
{
    shao_ng_dts_FileChkSum *csP = 0;
    shao_ng_dts_BuffPool   *iBP;
    pthread_t tid;
    int ec, rc, csType;

// If we have no IOB, then do a simple in-line passthru
//
   if (!IOB) return Passthru(&inPool, &shao_ng_dts_CPool, 0, nstrms);

// Establish checksum options as well as ordering options. Note that we do
// not support checksums in unordered streams and should have been prohibited.
//
   csType = shao_ng_dts_Config.csOpts & shao_ng_dts_csVerOut ? shao_ng_dts_Config.csType:shao_ng_dts_csNOP;
   if (shao_ng_dts_Config.csOpts & shao_ng_dts_csVerOut || shao_ng_dts_Config.Options & shao_ng_dts_ORDER)
      {csP = new shao_ng_dts_FileChkSum(&inPool, this, csType,
                                 shao_ng_dts_Config.csOpts & shao_ng_dts_csVerIO,nstrms);
       nstrms = 1;
       if ((rc = shao_ng_dts_Thread_Start(shao_ng_dts_FileCSY, (void *)csP, &tid)) < 0)
           {shao_ng_dts_Emsg("File", rc, "starting file checksum thread.");
            if (csP) delete csP;
            if (IOB) IOB->Close();
            return 201;
           } else iBP = &(csP->csPool);
      }      else iBP = &inPool;

// Establish logging options
//
   if (shao_ng_dts_Config.Options & shao_ng_dts_LOGRD) IOB->Log(0, "DISK");

// Determine what kind of writing we will do here and do it
//
   rc = (blockSize ? Write_Direct(iBP, &inPool, nstrms)
                   : Write_Normal(iBP, &inPool, nstrms));

// Check if we ended because of an error or end of file
//
   if (rc < 0 && rc != -ENOBUFS)
      {const char *Act=(shao_ng_dts_Config.Options & shao_ng_dts_XPIPE ? "piping":"writing");
       shao_ng_dts_Emsg("Write", -rc, Act, iofn);
       inPool.Abort();
      }

// Check if we should verify a checksum
//
   if (csP)
      {shao_ng_dts_Thread_Wait(tid);
       if (!rc && csP->csObj) rc = verChkSum(csP);
       delete csP;
      }

// If checksums are being printed, send off ours if we have it
//
   if (shao_ng_dts_Config.csOpts & shao_ng_dts_csPrint && *shao_ng_dts_Config.csString)
      cout <<"200 cks: " <<shao_ng_dts_Config.csString <<' ' <<iofn <<endl;

// Check if we should fsync this file
//
   if (!rc && IOB && (shao_ng_dts_Config.Options & shao_ng_dts_FSYNC)
   && (rc = FSp->Fsync((shao_ng_dts_Config.Options & shao_ng_dts_DSYNC ? iofn:0),IOB->FD())))
      shao_ng_dts_Emsg("Write", -rc, "synchronizing", iofn);

// Close the output file and make sure it's ok
//
   if (IOB && (ec = IOB->Close()))
      if (!rc) {shao_ng_dts_Emsg("Write", -ec, "closing", iofn); rc = ec;}

// All done
//
   return rc;
}

/******************************************************************************/
/* Private:                 W r i t e _ D i r e c t                           */
/******************************************************************************/

int shao_ng_dts_File::Write_Direct(shao_ng_dts_BuffPool *iBP, shao_ng_dts_BuffPool *oBP, int nstrms)
{
   shao_ng_dts_Buffer *bP;
   ssize_t tlen, wlen = -1, sMask = blockSize - 1;

// Read all of the data until eof (note that we are single threaded)
//
// cerr <<"Write Direct! " <<blockSize <<endl;
   while(nstrms)
      {
      // Obtain a full buffer
      //
         if (!(bP = iBP->getFullBuff())) return -ENOBUFS;

      // Check if this is an eof marker
      //
         if (!(bP->blen)) {iBP->putEmptyBuff(bP); nstrms--; continue;}

      // Make sure we are writing a sector's worth of data
      //
         if (!(bP->blen & sMask)) {wlen = bP->blen; tlen = 0;}
            else {wlen = (bP->blen + blockSize) & ~sMask;
                  tlen =  bP->blen + bP->boff;
                 }

      // Do a normal write
      //
      // cerr <<"Dwrt " <<bP->blen <<'@' <<bP->boff <<endl;
         if ((wlen = IOB->Write(bP->data, wlen, bP->boff)) <= 0) break;

      // Truncate to the correct size if need be and recycle the buffer
      //
      // if (tlen) cerr <<"TRUNC to " <<tlen <<endl;
         if (tlen && ftruncate(IOB->FD(), tlen)) return -errno;
         oBP->putEmptyBuff(bP);
      }

// All done
//
   if (!nstrms) return  0;
   if (!wlen)   return -1;
   return static_cast<int>(wlen);
}

/******************************************************************************/
/* Private:                 W r i t e _ N o r m a l                           */
/******************************************************************************/

int shao_ng_dts_File::Write_Normal(shao_ng_dts_BuffPool *iBP, shao_ng_dts_BuffPool *oBP, int nstrms)
{
   shao_ng_dts_Buffer *bP;
   ssize_t wlen = -1;

// Read all of the data until eof (note that we are single threaded)
//
   while(nstrms)
      {
      // Obtain a full buffer
      //
         if (!(bP = iBP->getFullBuff())) return -ENOBUFS;

      // Check if this is an eof marker
      //
      // cerr <<nstrms <<" Write " <<bP->blen <<'@' <<bP->boff <<endl;
         if (!(bP->blen)) {iBP->putEmptyBuff(bP); nstrms--; continue;}

      // Do a normal write
      //
         if ((wlen=IOB->Write(bP->data, bP->blen, bP->boff)) <= 0) break;
         oBP->putEmptyBuff(bP);
      }

// All done
//
   if (!nstrms) return  0;
   if (!wlen)   return -1;
   return static_cast<int>(wlen);
}
