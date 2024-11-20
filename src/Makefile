# These are the default value for all archetures. The makefile appears
# cumbersome but only because we wanted to have one makefile that would
# work in a variety of disparate architecture and various release levels.
# Additionally, this make file is easily modifiable for test environments.
#

BINDIR     = ../bin/$(OSVER)

OBJDIR     = ../obj/$(OSVER)

ENVCFLAGS  =    -Dunix -D_BSD -D_ALL_SOURCE

ENVINCLUDE = -I. -I/usr/local/include -I/usr/include 

AIXLIBS    =  -q64 -L/usr/lib -lz -lpthreads

BSDLIBS    =  -L/usr/lib -lz -pthread

LNXLIBS32  =  -lnsl -lpthread -lrt -lz -lcrypto
LNXLIBS64  =  -lnsl -lpthread -lrt -lz -lcrypto

MACLIBS    =  -dynamic -L/usr/lib -lz -lpthread -lcrypto

SUNLIBSA   =  /usr/local/lib/libz.a -lposix4 -lsocket -lnsl -lpthread
SUNLIBS    =  -L/usr/lib -lz -lposix4 -lsocket -lnsl -lpthread

MKPARMS  = doitall

NLGR       = -DNL_THREADSAFE

SUN64      = -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
SUNMT      = -D_REENTRANT -DOO_STD -mt
SUNOPT     = -g -fast $(SUNMT) $(SUN64) $(NLGR) -DSUN

SUNCC      = CC
SUNcc      = cc

S86OPT     = -D_REENTRANT -DOO_STD $(NLGR) -DSUN -DSUNX86 -Wno-deprecated
S86OPT     = $(SUNOPT) -DSUNX86 

S86CC      = CC
S86cc      = gcc

SUNGCC64   = -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
SUNGCCMT   = -D_REENTRANT -DOO_STD
SUNGCCOPT  = -L/usr/local/lib -g \
             $(SUNGCCMT) $(SUNGCC64) $(NLGR) -DSUN

SUNGCC     = g++
SUNGcc     = gcc

LNXOPT     = $(SUN64) $(NLGR) -D_REENTRANT -DOO_STD -DLINUX -Wno-deprecated \
             -D_GNU_SOURCE -g
LNXOPT_B   = $(SUN64) $(NLGR) -D_REENTRANT -DOO_STD -DLINUX \
             -D_GNU_SOURCE -g

LNXCC      = g++
LNXcc      = gcc

BSDOPT     = $(SUN64) $(NLGR) -D_REENTRANT -DOO_STD -DFREEBSD -pthread

BSDCC      = g++
BSDcc      = gcc

MACOPT     = $(SUN64) $(NLGR) -D_REENTRANT -DOO_STD -DMACOS -Wno-deprecated -g

MACCC      = g++
MACcc      = gcc

AIXCC      = xlC
AIXcc      = xlc

AIXOPT     = -q64 $(SUN64) $(NLGR) -D_REENTRANT -DOO_STD -DAIX -g

SOURCE =      \
     shao_ng_dts.C \
     shao_ng_dts_Args.C \
     shao_ng_dts_BuffPool.C \
     shao_ng_dts_C32.C \
     shao_ng_dts_ChkSum.C \
     shao_ng_dts_Config.C \
     shao_ng_dts_Emsg.C \
     shao_ng_dts_File.C \
     shao_ng_dts_FileSpec.C \
     shao_ng_dts_FileSystem.C \
     shao_ng_dts_FS_Null.C \
     shao_ng_dts_FS_Pipe.C \
     shao_ng_dts_FS_Unix.C \
     shao_ng_dts_IO.C \
     shao_ng_dts_IO_Null.C \
     shao_ng_dts_IO_Pipe.C \
     shao_ng_dts_Link.C \
     shao_ng_dts_LogFile.C \
     shao_ng_dts_MD5.C \
     shao_ng_dts_MD5_openssl.C \
     shao_ng_dts_NetAddr.C \
     shao_ng_dts_NetAddrInfo.C \
     shao_ng_dts_NetLogger.C \
     shao_ng_dts_Network.C \
     shao_ng_dts_Node.C \
     shao_ng_dts_ProcMon.C \
     shao_ng_dts_ProgMon.C \
     shao_ng_dts_Protocol.C \
     shao_ng_dts_Pthread.C \
     shao_ng_dts_RTCopy.C \
     shao_ng_dts_Stream.C \
     shao_ng_dts_System.C \
     shao_ng_dts_Timer.C \
     shao_ng_dts_Version.C \
     shao_ng_dts_ZCX.C \
     NetLogger.c

OBJECT =      \
     $(OBJDIR)/shao_ng_dts.o \
     $(OBJDIR)/shao_ng_dts_Args.o \
     $(OBJDIR)/shao_ng_dts_BuffPool.o \
     $(OBJDIR)/shao_ng_dts_C32.o \
     $(OBJDIR)/shao_ng_dts_ChkSum.o \
     $(OBJDIR)/shao_ng_dts_Config.o \
     $(OBJDIR)/shao_ng_dts_Emsg.o \
     $(OBJDIR)/shao_ng_dts_File.o \
     $(OBJDIR)/shao_ng_dts_FileSpec.o \
     $(OBJDIR)/shao_ng_dts_FileSystem.o \
     $(OBJDIR)/shao_ng_dts_FS_Null.o \
     $(OBJDIR)/shao_ng_dts_FS_Pipe.o \
     $(OBJDIR)/shao_ng_dts_FS_Unix.o \
     $(OBJDIR)/shao_ng_dts_IO.o \
     $(OBJDIR)/shao_ng_dts_IO_Null.o \
     $(OBJDIR)/shao_ng_dts_IO_Pipe.o \
     $(OBJDIR)/shao_ng_dts_Link.o \
     $(OBJDIR)/shao_ng_dts_LogFile.o \
     $(OBJDIR)/shao_ng_dts_MD5.o \
     $(OBJMD5) \
     $(OBJDIR)/shao_ng_dts_NetLogger.o \
     $(OBJDIR)/shao_ng_dts_NetAddr.o \
     $(OBJDIR)/shao_ng_dts_NetAddrInfo.o \
     $(OBJDIR)/shao_ng_dts_Network.o \
     $(OBJDIR)/shao_ng_dts_Node.o \
     $(OBJDIR)/shao_ng_dts_ProcMon.o \
     $(OBJDIR)/shao_ng_dts_ProgMon.o \
     $(OBJDIR)/shao_ng_dts_Protocol.o \
     $(OBJDIR)/shao_ng_dts_Pthread.o \
     $(OBJDIR)/shao_ng_dts_RTCopy.o \
     $(OBJDIR)/shao_ng_dts_Stream.o \
     $(OBJDIR)/shao_ng_dts_System.o \
     $(OBJDIR)/shao_ng_dts_Timer.o \
     $(OBJDIR)/shao_ng_dts_Version.o \
     $(OBJDIR)/shao_ng_dts_ZCX.o \
     $(OBJDIR)/NetLogger.o

TARGET  = $(BINDIR)/shao_ng_dts


all:
	@make `uname` OSVER=`../MakeSname` 
	@echo Make done.

doitall: $(TARGET)

clean:
	@make cleanall OSVER=`../MakeSname`

cleanall:
	@rm -f $(OBJECT) $(BINDIR)/core $(TARGET) $(OBJDIR)/*

usage:
	@echo "Usage: make [echo | usage] [OPT=-DSUN6] [Sungcc]"

echo:		
	@for f in $(SOURCE); do \
	echo $$f ;\
	done

AIX:
	@make $(MKPARMS) \
	CC=$(AIXCC) \
	BB=$(AIXcc) \
	BFLAGS="$(ENVCFLAGS) $(AIXOPT)" \
	CFLAGS="$(ENVCFLAGS) $(AIXOPT)" \
	INCLUDE="$(ENVINCLUDE)"  \
	LIBS="$(AIXLIBS)"

FreeBSD:
	@make $(MKPARMS)  \
	CC=$(BSDCC) \
	BB=$(BSDcc) \
	CFLAGS="$(ENVCFLAGS) $(BSDOPT) $(OPT)" \
	INCLUDE="$(ENVINCLUDE)" \
	LIBS="$(BSDLIBS)"

Linux:
	@make makeLinux`/bin/uname -m` \
	MD5HEADER=shao_ng_dts_MD5_openssl.h \
	OBJMD5="$(OBJDIR)/shao_ng_dts_MD5_openssl.o"

makeLinuxi386:
	@make $(MKPARMS)  \
	CC=$(LNXCC) \
	BB=$(LNXcc) \
	CFLAGS="$(ENVCFLAGS) $(LNXOPT)" \
	BFLAGS="$(ENVCFLAGS) $(LNXOPT_B)" \
	INCLUDE="$(ENVINCLUDE)" \
	LIBS="$(LNXLIBS32)"

makeLinuxi686:
	@make $(MKPARMS)  \
	CC=$(LNXCC) \
	BB=$(LNXcc) \
	CFLAGS="$(ENVCFLAGS) $(LNXOPT)" \
	BFLAGS="$(ENVCFLAGS) $(LNXOPT_B)" \
	INCLUDE="$(ENVINCLUDE)" \
	LIBS="$(LNXLIBS32)"

makeLinuxx86_64:
	@make $(MKPARMS)  \
	CC=$(LNXCC) \
	BB=$(LNXcc) \
	CFLAGS="$(ENVCFLAGS) $(LNXOPT)" \
	BFLAGS="$(ENVCFLAGS) $(LNXOPT_B)" \
	INCLUDE="$(ENVINCLUDE)" \
	LIBS="$(LNXLIBS64)"

Darwin:
	@MACOSX_DEPLOYMENT_TARGET=10.5;\
	make $(MKPARMS)  \
	CC=$(MACCC) \
	BB=$(MACcc) \
	CFLAGS="$(ENVCFLAGS) $(MACOPT) $(OPT)" \
	INCLUDE="$(ENVINCLUDE)" \
	LIBS="$(MACLIBS)" \
	MD5HEADER=shao_ng_dts_MD5_openssl.h \
	OBJMD5="$(OBJDIR)/shao_ng_dts_MD5_openssl.o"


UNICOS/mp:
	@make $(MKPARMS)  \
	CC=$(LNXCC) \
	BB=$(LNXcc) \
	CFLAGS="$(ENVCFLAGS) $(LNXOPT)" \
	BFLAGS="$(ENVCFLAGS) $(LNXOPT_B)" \
	INCLUDE="$(ENVINCLUDE)" \
	LIBS="$(LNXLIBS)"

SunOS:
	@make $(MKPARMS)  \
	CC=$(SUNCC) \
	BB=$(SUNcc) \
	CFLAGS="$(ENVCFLAGS) $(SUNOPT) $(OPT)" \
	INCLUDE="$(ENVINCLUDE)" \
	LIBS="$(SUNLIBS)"

SunX86:
	@make $(MKPARMS)  \
	CC=$(S86CC) \
	BB=$(S86cc) \
	CFLAGS="$(ENVCFLAGS) $(S86OPT) $(OPT)" \
	INCLUDE="$(ENVINCLUDE)" \
	LIBS="$(SUNLIBS)"

Sungcc:
	@make $(MKPARMS)  \
	CC=$(SUNGCC) \
	BB=$(SUNGcc) \
	CFLAGS="$(ENVCFLAGS) $(SUNGCCOPT) $(OPT)" \
	INCLUDE="$(ENVINCLUDE)" \
	LIBS="$(SUNLIBS)" \
	OSVER=`../MakeSname`

$(TARGET): $(OBJECT)
	@echo Creating executable $(TARGET) ...
	@$(CC) $(OBJECT) $(LIBS) -o $(TARGET)

$(OBJDIR)/shao_ng_dts.o: shao_ng_dts.C shao_ng_dts_Args.h shao_ng_dts_Config.h shao_ng_dts_Debug.h shao_ng_dts_FileSpec.h \
                  shao_ng_dts_LogFile.h shao_ng_dts_Node.h shao_ng_dts_Protocol.h shao_ng_dts_System.h \
                  shao_ng_dts_Headers.h
	@echo Compiling shao_ng_dts.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/shao_ng_dts_Args.o: shao_ng_dts_Args.C shao_ng_dts_Args.h shao_ng_dts_Config.h shao_ng_dts_Stream.h
	@echo Compiling shao_ng_dts_Args.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/shao_ng_dts_BuffPool.o: shao_ng_dts_BuffPool.C shao_ng_dts_BuffPool.h shao_ng_dts_Debug.h \
                           shao_ng_dts_Emsg.h shao_ng_dts_Pthread.h shao_ng_dts_Headers.h
	@echo Compiling shao_ng_dts_BuffPool.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/shao_ng_dts_C32.o: shao_ng_dts_C32.C shao_ng_dts_C32.h shao_ng_dts_ChkSum.h shao_ng_dts_Endian.h
	@echo Compiling shao_ng_dts_C32.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/shao_ng_dts_ChkSum.o: shao_ng_dts_ChkSum.C shao_ng_dts_ChkSum.h shao_ng_dts_Endian.h \
                         shao_ng_dts_A32.h shao_ng_dts_A32_zlib.h shao_ng_dts_C32.h shao_ng_dts_MD5.h $(MD5HEADER)
	@echo Compiling shao_ng_dts_ChkSum.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/shao_ng_dts_Config.o: shao_ng_dts_Config.C shao_ng_dts_Config.h shao_ng_dts_Args.h \
                         shao_ng_dts_Debug.h shao_ng_dts_Emsg.h shao_ng_dts_FileSpec.h shao_ng_dts_LogFile.h \
                         shao_ng_dts_NetLogger.h shao_ng_dts_Network.h shao_ng_dts_Platform.h \
                         shao_ng_dts_Stream.h shao_ng_dts_System.h shao_ng_dts_Version.h \
                         shao_ng_dts_Headers.h  shao_ng_dts_Pthread.h
	@echo Compiling shao_ng_dts_Config.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/shao_ng_dts_Emsg.o: shao_ng_dts_Emsg.C shao_ng_dts_Emsg.h shao_ng_dts_Debug.h shao_ng_dts_Platform.h \
                       shao_ng_dts_Headers.h
	@echo Compiling shao_ng_dts_Emsg.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/shao_ng_dts_File.o: shao_ng_dts_File.C shao_ng_dts_File.h shao_ng_dts_IO.h shao_ng_dts_BuffPool.h \
                       shao_ng_dts_Config.h shao_ng_dts_Debug.h shao_ng_dts_Emsg.h shao_ng_dts_Pthread.h \
                       shao_ng_dts_Platform.h shao_ng_dts_Headers.h shao_ng_dts_ChkSum.h \
                       shao_ng_dts_RTCopy.h
	@echo Compiling shao_ng_dts_File.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/shao_ng_dts_FileSpec.o: shao_ng_dts_FileSpec.C shao_ng_dts_FileSpec.h shao_ng_dts_Config.h \
                           shao_ng_dts_Emsg.h shao_ng_dts_FileSystem.h shao_ng_dts_FS_Unix.h \
                           shao_ng_dts_Pthread.h
	@echo Compiling shao_ng_dts_FileSpec.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/shao_ng_dts_FileSystem.o: shao_ng_dts_FileSystem.C shao_ng_dts_FileSystem.h \
                             shao_ng_dts_FS_Null.h shao_ng_dts_FS_Pipe.h shao_ng_dts_FS_Unix.h
	@echo Compiling shao_ng_dts_FileSystem.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/shao_ng_dts_FS_Null.o: shao_ng_dts_FS_Null.C shao_ng_dts_FS_Null.h \
                          shao_ng_dts_IO_Null.h shao_ng_dts_FileSystem.h shao_ng_dts_System.h \
                          shao_ng_dts_Platform.h shao_ng_dts_Pthread.h
	@echo Compiling shao_ng_dts_FS_Null.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/shao_ng_dts_FS_Pipe.o: shao_ng_dts_FS_Pipe.C  shao_ng_dts_FS_Pipe.h    shao_ng_dts_FS_Unix.h\
                          shao_ng_dts_IO_Pipe.h  shao_ng_dts_FileSystem.h shao_ng_dts_System.h \
                          shao_ng_dts_Platform.h shao_ng_dts_Pthread.h    shao_ng_dts_Emsg.h
	@echo Compiling shao_ng_dts_FS_Pipe.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/shao_ng_dts_FS_Unix.o: shao_ng_dts_FS_Unix.C shao_ng_dts_FS_Unix.h shao_ng_dts_FileSystem.h \
                          shao_ng_dts_Config.h  shao_ng_dts_IO.h      shao_ng_dts_IO_Null.h \
                          shao_ng_dts_Platform.h shao_ng_dts_System.h shao_ng_dts_Pthread.h
	@echo Compiling shao_ng_dts_FS_Unix.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/shao_ng_dts_IO.o: shao_ng_dts_IO.C shao_ng_dts_IO.h shao_ng_dts_NetLogger.h shao_ng_dts_Timer.h
	@echo Compiling shao_ng_dts_IO.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/shao_ng_dts_IO_Null.o: shao_ng_dts_IO_Null.C shao_ng_dts_IO_Null.h shao_ng_dts_IO.h
	@echo Compiling shao_ng_dts_IO_Null.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/shao_ng_dts_IO_Pipe.o: shao_ng_dts_IO_Pipe.C shao_ng_dts_IO_Pipe.h shao_ng_dts_IO.h \
                          shao_ng_dts_System.h
	@echo Compiling shao_ng_dts_IO_Pipe.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/shao_ng_dts_Link.o:  shao_ng_dts_Link.C shao_ng_dts_Link.h shao_ng_dts_File.h shao_ng_dts_IO.h \
                        shao_ng_dts_BuffPool.h shao_ng_dts_ChkSum.h  shao_ng_dts_Config.h \
                        shao_ng_dts_Debug.h shao_ng_dts_Emsg.h \
                        shao_ng_dts_Link.h shao_ng_dts_Network.h shao_ng_dts_Platform.h \
                        shao_ng_dts_Timer.h
	@echo Compiling shao_ng_dts_Link.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/shao_ng_dts_LogFile.o:  shao_ng_dts_LogFile.C shao_ng_dts_LogFile.h shao_ng_dts_Pthread.h \
                           shao_ng_dts_Debug.h shao_ng_dts_Emsg.h shao_ng_dts_Platform.h \
                           shao_ng_dts_Headers.h shao_ng_dts_Timer.h
	@echo Compiling shao_ng_dts_LogFile.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/shao_ng_dts_MD5.o: shao_ng_dts_MD5.C shao_ng_dts_MD5.h shao_ng_dts_ChkSum.h shao_ng_dts_Endian.h
	@echo Compiling shao_ng_dts_MD5.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/shao_ng_dts_MD5_openssl.o: shao_ng_dts_MD5_openssl.C shao_ng_dts_MD5_openssl.h shao_ng_dts_ChkSum.h shao_ng_dts_Endian.h
	@echo Compiling shao_ng_dts_MD5_openssl.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/shao_ng_dts_NetLogger.o:  shao_ng_dts_NetLogger.C shao_ng_dts_NetLogger.h NetLogger.h
	@echo Compiling shao_ng_dts_Netlogger.c
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/shao_ng_dts_NetAddr.o:  shao_ng_dts_NetAddr.C shao_ng_dts_NetAddr.h shao_ng_dts_NetAddrInfo.h \
                           shao_ng_dts_NetSockAddr.h
	@echo Compiling shao_ng_dts_NetAddr.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/shao_ng_dts_NetAddrInfo.o:  shao_ng_dts_NetAddrInfo.C shao_ng_dts_NetAddrInfo.h \
                               shao_ng_dts_NetSockAddr.h
	@echo Compiling shao_ng_dts_NetAddrInfo.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/shao_ng_dts_Network.o:  shao_ng_dts_Network.C shao_ng_dts_Network.h shao_ng_dts_Debug.h \
                           shao_ng_dts_Emsg.h shao_ng_dts_Link.h shao_ng_dts_Pthread.h \
                           shao_ng_dts_NetAddr.h shao_ng_dts_NetAddrInfo.h  shao_ng_dts_Platform.h
	@echo Compiling shao_ng_dts_Network.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/shao_ng_dts_Node.o: shao_ng_dts_Node.C shao_ng_dts_Node.h shao_ng_dts_Config.h shao_ng_dts_Emsg.h \
                       shao_ng_dts_Debug.h shao_ng_dts_File.h shao_ng_dts_FileSpec.h shao_ng_dts_Link.h \
                       shao_ng_dts_BuffPool.h shao_ng_dts_Network.h shao_ng_dts_Protocol.h \
                       shao_ng_dts_ProcMon.h shao_ng_dts_ProgMon.h shao_ng_dts_Stream.h \
                       shao_ng_dts_System.h shao_ng_dts_ZCX.h shao_ng_dts_Headers.h
	@echo Compiling shao_ng_dts_Node.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/shao_ng_dts_ProcMon.o: shao_ng_dts_ProcMon.C shao_ng_dts_ProcMon.h shao_ng_dts_Debug.h \
                          shao_ng_dts_Emsg.h shao_ng_dts_File.h shao_ng_dts_Pthread.h shao_ng_dts_Headers.h
	@echo Compiling shao_ng_dts_ProcMon.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/shao_ng_dts_ProgMon.o: shao_ng_dts_ProgMon.C shao_ng_dts_ProgMon.h shao_ng_dts_Config.h \
                          shao_ng_dts_Debug.h shao_ng_dts_File.h shao_ng_dts_Headers.h \
                          shao_ng_dts_Pthread.h shao_ng_dts_Timer.h shao_ng_dts_ZCX.h
	@echo Compiling shao_ng_dts_ProgMon.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/shao_ng_dts_Protocol.o: shao_ng_dts_Protocol.C shao_ng_dts_Protocol.h shao_ng_dts_Config.h  \
                           shao_ng_dts_Node.h shao_ng_dts_FileSpec.h shao_ng_dts_FileSystem.h  \
                           shao_ng_dts_IO.h shao_ng_dts_File.h shao_ng_dts_Link.h shao_ng_dts_Debug.h \
                           shao_ng_dts_Pthread.h shao_ng_dts_Stream.h shao_ng_dts_Emsg.h \
                           shao_ng_dts_Network.h shao_ng_dts_Version.h shao_ng_dts_Headers.h
	@echo Compiling shao_ng_dts_Protocol.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/shao_ng_dts_Pthread.o: shao_ng_dts_Pthread.C shao_ng_dts_Pthread.h
	@echo Compiling shao_ng_dts_Pthread.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/shao_ng_dts_RTCopy.o: shao_ng_dts_RTCopy.C shao_ng_dts_RTCopy.h shao_ng_dts_Config.h \
                         shao_ng_dts_Emsg.h shao_ng_dts_FileSystem.h shao_ng_dts_Pthread.h
	@echo Compiling shao_ng_dts_RTCopy.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/shao_ng_dts_Stream.o: shao_ng_dts_Stream.C shao_ng_dts_Stream.h shao_ng_dts_Config.h shao_ng_dts_Debug.h \
                         shao_ng_dts_Emsg.h shao_ng_dts_System.h
	@echo Compiling shao_ng_dts_Stream.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/shao_ng_dts_System.o: shao_ng_dts_System.C shao_ng_dts_System.h shao_ng_dts_Config.h \
                         shao_ng_dts_Debug.h shao_ng_dts_Emsg.h shao_ng_dts_Platform.h shao_ng_dts_Pthread.h
	@echo Compiling shao_ng_dts_System.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/shao_ng_dts_Timer.o: shao_ng_dts_Timer.C shao_ng_dts_Timer.h
	@echo Compiling shao_ng_dts_Timer.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/shao_ng_dts_Version.o: shao_ng_dts_Version.C shao_ng_dts_Version.h shao_ng_dts_Config.h shao_ng_dts_Emsg.h
	@echo Compiling shao_ng_dts_Version.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/shao_ng_dts_ZCX.o: shao_ng_dts_ZCX.C shao_ng_dts_ZCX.h shao_ng_dts_BuffPool.h shao_ng_dts_Emsg.h
	@echo Compiling shao_ng_dts_ZCX.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/NetLogger.o: NetLogger.c NetLogger.h
	@echo Compiling NetLogger.c
	@$(BB) -c $(BFLAGS) $(INCLUDE) $(*F).c -o $(OBJDIR)/$(*F).o
