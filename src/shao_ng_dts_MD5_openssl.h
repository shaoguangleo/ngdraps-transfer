#ifndef __shao_ng_dts_MD5_openssl_H__
#define __shao_ng_dts_MD5_openssl_H__
/******************************************************************************/
/*                                                                            */
/*                            shao_ng_dts_ M D 5 . h                             */
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

typedef unsigned int uint32;

/******************************************************************************/
/*                  M D 5 C o n t e x t   S t r u c t u r e                   */
/******************************************************************************/
  

/******************************************************************************/
/*                             M D 5   C l a s s                              */
/******************************************************************************/

#include <stdio.h>

#include "shao_ng_dts_Headers.h"
#include "shao_ng_dts_ChkSum.h"
#include "openssl/md5.h"
  
class shao_ng_dts_MD5_openssl : public shao_ng_dts_ChkSum
{
public:

int   Chk_MD5(char *buff, int  blen, char MD5Val[16])
             {MD5Init(&MyContext);
              MD5Update(&MyContext, (unsigned char *)buff, (unsigned)blen);
              MD5Final(MyDigest, &MyContext);
              return memcmp((const void *)MD5Val, (const void *)MyDigest,
                            (size_t)sizeof(MyDigest));
             }

void  Set_MD5(char *buff, int  blen, char MD5Val[16])
             {MD5Init(&MyContext);
              MD5Update(&MyContext, (unsigned char *)buff, (unsigned)blen);
              MD5Final((unsigned char *)MD5Val, &MyContext);
             }

char *csCurr(char **Text=0)
             {MD5_CTX currCTX = MyContext;
              MD5Final(MyDigest, &currCTX);
              if (Text) *Text = x2a((char *)MyDigest);
              return (char *)MyDigest;
             }

int   csSize() {return sizeof(MyDigest);}

char *csVal();

void  Init() {MD5Init(&MyContext);}

void  Update(const char *Buff, int BLen)
            {MD5Update(&MyContext, (unsigned char *)Buff, (unsigned)BLen);}

char *Final(char **Text=0)
            {MD5Final(MyDigest, &MyContext);
             if (Text) *Text = x2a((char *)MyDigest);
             return (char *)MyDigest;
            }

const char *Type() {return "md5sl";}

      shao_ng_dts_MD5_openssl() {MD5Init(&MyContext);}
     ~shao_ng_dts_MD5_openssl() {}

private:

MD5_CTX       MyContext;
unsigned char MyDigest[16];

void byteReverse(unsigned char *buf, unsigned longs);
void MD5Init(MD5_CTX *context);
void MD5Update(MD5_CTX *context, unsigned char const *buf,
               unsigned len);
void MD5Final(unsigned char digest[16], MD5_CTX *context);
void MD5Transform(uint32 buf[4], uint32 const in[16]);
};
#endif
