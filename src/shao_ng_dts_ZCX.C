/******************************************************************************/
/*                                                                            */
/*                            shao_ng_dts_ Z C X . C                             */
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
  
extern "C"
{
#include <zlib.h>
}
#include "shao_ng_dts_BuffPool.h"
#include "shao_ng_dts_Emsg.h"
#include "shao_ng_dts_Headers.h"
#include "shao_ng_dts_NetLogger.h"
#include "shao_ng_dts_Platform.h"
#include "shao_ng_dts_ZCX.h"

/******************************************************************************/
/*                        G l o b a l   O b j e c t s                         */
/******************************************************************************/
  
extern shao_ng_dts_NetLogger shao_ng_dts_NetLog;

/******************************************************************************/
/*                     L o c a l   D e f i n i t i o n s                      */
/******************************************************************************/

#define LOGSTART \
        if (LogIDbeg) {xfrseek = (Clvl ? inbytes : outbytes); LOGIT(LogIDbeg);}

#define LOGEND \
        if (LogIDend) LOGIT(LogIDend)

#define LOGIT(a) \
        shao_ng_dts_NetLog.Emit(a, (char *)"", "shao_ng_dts.FD=%d shao_ng_dts.SK=%lld",iofd,xfrseek)
  
/******************************************************************************/
/*                           C o n s t r u c t o r                            */
/******************************************************************************/
  
shao_ng_dts_ZCX::shao_ng_dts_ZCX(shao_ng_dts_BuffPool *ib, shao_ng_dts_BuffPool *rb, shao_ng_dts_BuffPool *ob,
          int clvl, int xfd, int logit)
{  
    Ibuff  = ib;
    Rbuff  = rb;
    Obuff  = ob;
    Clvl   = clvl;
    iofd   = xfd;
    cbytes = 0; 
    TID    = 0;

    if (!logit) LogIDbeg = LogIDend = 0;
       else    {LogIDbeg = (clvl ? (char *)"STARTCMP" : (char *)"STARTEXP");
                LogIDend = (clvl ? (char *)"ENDCMP"   : (char *)"ENDEXP");
               }
}

/******************************************************************************/
/*                               P r o c e s s                                */
/******************************************************************************/
  
int shao_ng_dts_ZCX::Process()
{
     z_stream ZStream;
     uInt outsz = (uInt)Obuff->DataSize();
     shao_ng_dts_Buffer *ibp, *obp;
     int rc = 0, ZFlag = 0;
     long long inbytes = 0, outbytes = 0, xfrseek = 0;

// Initialize the compression stream
//
   ZStream.zalloc    = Z_NULL;
   ZStream.zfree     = Z_NULL;
   ZStream.opaque    = Z_NULL;
   ZStream.msg       = 0;
   ZStream.data_type = Z_BINARY;

   if (Clvl)rc = deflateInit(&ZStream, Clvl);
      else rc = inflateInit(&ZStream);
   if (rc != Z_OK) return Zfailure(rc, "initializing", ZStream.msg);

// Get the initial inbuff and outbuff
//
   if (!(obp = Obuff->getEmptyBuff())) return ENOBUFS;
   ZStream.next_out  = (Bytef *)obp->data;
   ZStream.avail_out = outsz;

   if (!(ibp = Ibuff->getFullBuff())) return ENOBUFS;
   ZStream.next_in  = (Bytef *)ibp->data;
   if (!(ZStream.avail_in = (uInt)ibp->blen)) rc = Z_STREAM_END;
   inbytes = ibp->blen;

// Perform compression/expansion
//
   while(rc != Z_STREAM_END)
        {LOGSTART;
         if (Clvl) rc = deflate(&ZStream, ZFlag);
            else   rc = inflate(&ZStream, ZFlag);
         LOGEND;
         if (rc != Z_OK && rc != Z_STREAM_END)
            return Zfailure(rc, "performing", ZStream.msg);

         if (!ZStream.avail_in && !ZFlag)
            {Rbuff->putEmptyBuff(ibp);
             if (!(ibp = Ibuff->getFullBuff())) return ENOBUFS;
             ZStream.next_in  = (Bytef *)ibp->data;
             if (!(ZStream.avail_in = (uInt)ibp->blen)) ZFlag = Z_FINISH;
                else inbytes += ibp->blen;
            }

         if (!ZStream.avail_out)
            {obp->blen = outsz;
             obp->boff = outbytes; outbytes += outsz;
             Obuff->putFullBuff(obp);
             if (!(obp = Obuff->getEmptyBuff())) return ENOBUFS;
             ZStream.next_out  = (Bytef *)obp->data;
             ZStream.avail_out = outsz;
            }
         cbytes = Clvl ? outbytes : inbytes - ZStream.avail_in;
        }

// If we have gotten here then all went well so far flush output
//
   if (obp->blen = outsz - ZStream.avail_out)
      {obp->boff = outbytes; outbytes += obp->blen;
       Obuff->putFullBuff(obp);
       if (!(obp = Obuff->getEmptyBuff())) return ENOBUFS;
      }

// Complete compression/expansion processing
//
   if (Clvl)
      {if ((rc = deflateEnd(&ZStream)) != Z_OK)
          return Zfailure(rc, "ending", ZStream.msg);
      } else {
       if ((rc = inflateEnd(&ZStream)) != Z_OK)
          return Zfailure(rc, "ending", ZStream.msg);
      }

// Record the total number of compressed bytes we've processed
//
   cbytes = Clvl ? outbytes : inbytes;

// Free up buffers
//
   Rbuff->putEmptyBuff(ibp);
   obp->blen = 0;
   obp->boff = outbytes;
   Obuff->putFullBuff(obp);
   return 0;
}

/******************************************************************************/
/*                              Z f a i l u r e                               */
/******************************************************************************/
  
int shao_ng_dts_ZCX::Zfailure(int zerr, const char *oper, char *Zmsg)
{
    char *txt2 = (Clvl ? (char *)"compression" : (char *)"expansion");
    switch(zerr)
          {case Z_ERRNO:         zerr = errno;      break;
           case Z_STREAM_ERROR:  zerr = ENOSR;      break;
           case Z_DATA_ERROR:    zerr = EOVERFLOW;  break;
           case Z_MEM_ERROR:     zerr = ENOMEM;     break;
           case Z_BUF_ERROR:     zerr = EBADSLT;    break;
           case Z_VERSION_ERROR: zerr = EPROTO;     break;
           default:              zerr = (zerr < 0 ? 10000-zerr : 20000+zerr);
          }

    Ibuff->Abort();
    if (Ibuff != Rbuff) Rbuff->Abort();
    Obuff->Abort();

    if (Zmsg) return shao_ng_dts_Fmsg("Zlib", Zmsg, oper, txt2);
    return shao_ng_dts_Emsg("Zlib", zerr, oper,  txt2, (Zmsg ? Zmsg : ""));
}
