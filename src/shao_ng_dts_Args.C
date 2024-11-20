/******************************************************************************/
/*                                                                            */
/*                           shao_ng_dts_ A r g s . C                            */
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
  
#include <stdlib.h>
#include <strings.h>
#include "shao_ng_dts_Args.h"
#include "shao_ng_dts_Config.h"
#include "shao_ng_dts_Headers.h"

/******************************************************************************/
/*                        G l o b a l   O b j e c t s                         */
/******************************************************************************/

extern shao_ng_dts_Config shao_ng_dts_Config;

/******************************************************************************/
/*                         L o c a l   C l a s s e s                          */
/******************************************************************************/

class shao_ng_dts_Opt
{
public:

int  operator==(char *optarg)
       {int i = strlen(optarg);
        return i <= Optmaxl && i >= Optminl &&
               !strncmp(Optword, optarg, i);
       }

char *operator%(char *optarg)
        {int i = strlen(optarg);
         shao_ng_dts_Opt *p = this;
         do if (i <= p->Optmaxl && i >= p->Optminl &&
               !strncmp(p->Optword, optarg, i)) return p->Optvalu;
            while(p = p->Optnext);
         return 0;
        }

friend void shao_ng_dts_Args::Option(const char *opw, int minl, char optv, char opta);

      shao_ng_dts_Opt(const char *opw, int minl, char optv, char opta)
              {Optword = strdup(opw);
               Optminl = minl; Optmaxl = strlen(opw);
               Optvalu[0] = optv; Optvalu[1] = opta;
               Optnext = 0;
              }
     ~shao_ng_dts_Opt()
              {if (Optword) free(Optword);
               if (Optnext) delete Optnext;
              }
private:
shao_ng_dts_Opt *Optnext;
char     *Optword;
int       Optmaxl;
int       Optminl;
char      Optvalu[2];
};
  
/******************************************************************************/
/*              C o n s t r u c t o r   &   D e s t r u c t o r               */
/******************************************************************************/
  
shao_ng_dts_Args::shao_ng_dts_Args(char *etxt)
             {MOA = inStream = Argc = Aloc = 0; vopts = curopt = 0; endopts = 1;
              optp = 0; epfx = strdup(etxt);
             }

shao_ng_dts_Args::~shao_ng_dts_Args()
             {if (vopts) free(vopts);
              if (optp) delete optp;
              if (inStream) arg_stream.Detach();
              free(epfx);
             }

/******************************************************************************/
/*                               g e t a r g s                                */
/******************************************************************************/
  
char *shao_ng_dts_Args::getarg(int newln)
{

// Process argument array
//
   if (!inStream)
      {if (Aloc >= Argc) return (char *)0;
       argval = Argv[Aloc++];
       return argval;
      }

// Process stream or line arguments
//
   if (newln) {if ((argval = arg_stream.GetLine()) && *argval)  return argval;}
      else do {if ((argval = arg_stream.GetToken()))            return argval;
              }   while(arg_stream.GetLine());

// All done, force continual end of arguments
//
   arg_stream.Detach();
   argval   = 0;
   inStream = 0;
   Argc     = 0;
   Aloc     = 0;
   return (char *)0;
}

/******************************************************************************/
/*                                g e t o p t                                 */
/******************************************************************************/
  
char shao_ng_dts_Args::getopt()
{
   char optval, optbuff[2] = {0,0}, *optspec, *arglist, *theOpt = optbuff;

// Check if we really have any more options
//
   if (endopts) return '\0';

// Get next option from whatever source we have
//
   if (curopt && curopt[1]) curopt++;
      else if (inStream)
              {if (MOA) arglist = arg_stream.GetLine();
                  else  arglist = (char *)"";
               if (arglist && (curopt = arg_stream.GetToken(&arglist)))
                  {if (*curopt != '-') {arg_stream.RetToken(); curopt = 0;}
                      else curopt++;
                  } else curopt = 0;
              }
              else if (Aloc >= Argc || *Argv[Aloc] != '-') curopt = 0;
                      else curopt = Argv[Aloc++]+1;

// Check if we really have an option here
//
   if (!curopt) {endopts = 1; return '\0';}
   if (!*curopt)
      {cerr <<epfx <<"Option letter missing after '-'." <<endl;
       endopts = 1;
       return '?';
      }

// Check for extended options or single letter option
//
        if (optp && strlen(curopt) > 2 && *curopt == '-'
        && (optspec = *optp%(curopt+1)))
           {theOpt = curopt; curopt = 0;
            if (!(optspec = index(vopts, *optspec)))
               {cerr <<epfx <<"Invalid option, '-" <<theOpt <<"'." <<endl;
                endopts = 1;
                return '?';
               }
           }
   else if (!(optspec = index(vopts, *curopt))
        || *curopt == ':' || *curopt == '.')
           {cerr <<epfx <<"Invalid option, '-" <<*curopt <<"'." <<endl;
            endopts = 1;
            return '?';
           }
   else *optbuff = *curopt;

// Check if this option requires an argument
//
   if (optspec[1] != ':' && optspec[1] != '.') return *optspec;

// Get the argument from whatever source we have
//
   if (inStream) argval = (MOA ? arglist : arg_stream.GetToken());
      else argval = (Aloc < Argc ? Argv[Aloc++] : 0);

// If we have a valid argument, then we are all done
//
   if (argval)
      if (!*argval) argval = 0;
         else if (*argval != '-') return *optspec;

// If argument is optional, let it go
//
   if (optspec[1] == '.')
      {if (argval && *argval == '-')
          if (inStream) arg_stream.RetToken();
             else Aloc--;
       argval = 0;
       return *optspec;
      }

// Complain about a missing argument
//
   cerr <<epfx <<"Value not specified for '-" <<theOpt <<"'." << endl;
   endopts = 1;
   return '?';
}
 
/******************************************************************************/
/*                                O p t i o n                                 */
/******************************************************************************/

void shao_ng_dts_Args::Option(const char *opw, int minl, char opv, char opta)
{
     shao_ng_dts_Opt *p = new shao_ng_dts_Opt(opw, minl, opv, opta);
     p->Optnext = optp;
     optp = p;
}
  
/******************************************************************************/
/*                               O p t i o n s                                */
/******************************************************************************/

void shao_ng_dts_Args::Options(char *opts, int fd, int moa)
{
   inStream = 1; 
   arg_stream.Attach(fd);
   if (vopts) free(vopts);
   vopts = strdup(opts);
   curopt = 0;
   endopts = (moa ? 0 : !arg_stream.GetLine());
   MOA = moa;
}

void shao_ng_dts_Args::Options(char *opts, int argc, char **argv)
{
   inStream = 0; 
   Argc = argc; Argv = argv; Aloc = 0;
   if (vopts) free(vopts);
   vopts = strdup(opts);
   curopt = 0; endopts = 0;
   endopts = !argc;
}
