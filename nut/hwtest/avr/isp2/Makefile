#
# Copyright (C) 2001-2006 by egnite Software GmbH. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. All advertising materials mentioning features or use of this
#    software must display the following acknowledgement:
#
#    This product includes software developed by egnite Software GmbH
#    and its contributors.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
# OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
# AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
# THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#
# For additional information see http://www.ethernut.de/
#
# $Log: Makefile,v $
# Revision 1.4  2006/07/21 09:13:59  haraldkipp
# Added creation of *.elf for 'make all'.
#
# Revision 1.3  2004/09/08 10:17:52  haraldkipp
# Dot-slash needed by Linux
#
# Revision 1.2  2004/08/18 15:02:35  haraldkipp
# Application build is no longer fixed in top_srcdir
#
# Revision 1.1  2003/11/03 15:51:31  haraldkipp
# First check in
#
#
#

PROJ    = isp2
ROMDIR  = isp
ROMFILE = urom.c

include ../Makedefs
include ./Makedefs.isp

SRCS =  $(PROJ).c $(ROMFILE)
OBJS =  $(SRCS:.c=.o) sisp.o
LIBS =  $(LIBDIR)/nutinit.o -lnutfs -lnutos -lnutdev -lnutcrt -lnutarch
TARG =  $(PROJ).hex
PARM =  $(PROJ).eep

all: $(OBJS) $(TARG) $(ITARG) $(DTARG) $(ROMDIR)/$(ISPTARG)

$(ROMFILE): $(ISPTARG)
	$(CP) $(ISPTARG) $(ROMDIR)/$(ISPTARG)
	$(CRUROM) -r -o$(ROMFILE) $(ROMDIR)

include ../Makerules
include ./Makerules.isp

clean: cleanisp
	-rm -f $(OBJS)
	-rm -f $(TARG) $(ITARG) $(DTARG)
	-rm -f $(PROJ).eep
	-rm -f $(PROJ).obj
	-rm -f $(PROJ).map
	-rm -f $(SRCS:.c=.lst)
	-rm -f $(SRCS:.c=.bak)
	-rm -f $(ROMFILE)
