#
# $Id$
#
# Copyright (c) 1998-2008 John Morrison.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.


# Makefile for LinBolo Server
# Author: John Morrison 

# Edited by Kruz for Minhiriath
# Date: 31-12-2008

########## VARIABLES ##########
# Executables
REM = rm

# Flags for gcc and rm
REMFLAGS = -f
LDFLAGS = `sdl-config --libs` `pkg-config minizip --libs` -lm
CFLAGS =
BOLO_CFLAGS = $(CFLAGS) `sdl-config --cflags` `pkg-config minizip --cflags` -DHAVE_C99INCLUDES -DHAVE_SYS_TYPES -Wall -Wno-missing-braces
BDFLAGS = -DHAVE_C99INCLUDES -std=c99 -pedantic -Wpointer-arith -Wstrict-prototypes -O2

# directories
BOLODIR = ../bolo/
GUIDIR = ../gui/
WBNETDIR = ../winbolonet/
GUIDIRLINUX = ../gui/linux/
SERVERDIR = ./
COMPRESSDIR = ../lzw/
BDDIR = BigDigits/

# added for BigDigits
BDINCLUDES = bigd.h bigdigits.h bigdtypes.h bigdRand.h bigdigitsRand.h

# Output Executable
SERVERNAME = linbolods

########## OUTPUT ##########
all: $(SERVERNAME)

b: global.o bases.o util.o crc.o pillbox.o lgm.o shells.o tank.o \
	allience.o bolo_map.o building.o explosions.o floodfill.o \
	gametype.o grass.o labels.o messages.o mines.o minesexp.o \
	netmt.o netplayers.o netpnb.o players.o rubble.o \
	screenbrainmap.o screenbullet.o screencalc.o screenlgm.o \
	screentank.o scroll.o sounddist.o starts.o swamp.o \
	tankexp.o treegrow.o dcodlzw.o ecodlzw.o playersrejoin.o \
	lang.o winbolonetevents.o winbolonetthread.o backend.o \
	mathWinbolo.o debug_file_output.o

server: servercore.o servermessages.o serverfrontend.o servermain.o \
	servernet.o servertransport.o threads.o log.o udppackets.o \
	winbolonet.o http.o preferences.o

bigdigits: bigd.o bigdigits.o bigdigitsRand.o bigdRand.o
	
$(SERVERNAME): b server bigdigits
	$(CC) \
	global.o bases.o util.o crc.o pillbox.o lgm.o shells.o tank.o \
	allience.o bolo_map.o building.o explosions.o floodfill.o \
	gametype.o grass.o labels.o mines.o minesexp.o  netmt.o netplayers.o \
	netpnb.o players.o rubble.o screenbrainmap.o  starts.o swamp.o \
	tankexp.o playersrejoin.o treegrow.o servercore.o servermessages.o \
	serverfrontend.o servermain.o servernet.o servertransport.o threads.o \
	dcodlzw.o ecodlzw.o lang.o log.o udppackets.o winbolonetevents.o \
	winbolonet.o http.o screenbullet.o preferences.o winbolonetthread.o \
	backend.o bigd.o bigdigits.o bigdigitsRand.o bigdRand.o \
	mathWinbolo.o debug_file_output.o $(LDFLAGS) -o $(SERVERNAME)

tidy: 
	$(REM) $(REMFLAGS) *.o

clean: tidy 
	$(REM) $(REMFLAGS) $(SERVERNAME)

########## OBJECTS ##########

# bigdigits
bigd.o: $(BDDIR)bigd.h
	$(CC) $(BDFLAGS) -c $(BDDIR)bigd.c

bigdigits.o: $(BDDIR)bigdigits.h
	$(CC) $(BDFLAGS) -c $(BDDIR)bigdigits.c

bigdigitsRand.o: $(BDDIR)bigdigitsRand.h
	$(CC) $(BDFLAGS) -c $(BDDIR)bigdigitsRand.c

bigdRand.o: $(BDDIR)bigdRand.h
	$(CC) $(BDFLAGS) -c $(BDDIR)bigdRand.c

#
# gui lang
#
lang.o: ${GUIDIRLINUX}lang.c ${GUIDIR}lang.h
	${CC} ${BOLO_CFLAGS} -c ${GUIDIRLINUX}lang.c
preferences.o: ${GUIDIRLINUX}preferences.c ${GUIDIRLINUX}preferences.h
	${CC} ${BOLO_CFLAGS} -c ${GUIDIRLINUX}preferences.c

# winbolonet
winbolonet.o: ${WBNETDIR}winbolonet.c ${WBNETDIR}winbolonet.h
	${CC} ${BOLO_CFLAGS} -c ${WBNETDIR}winbolonet.c
winbolonetevents.o: ${WBNETDIR}winbolonetevents.c ${WBNETDIR}winbolonetevents.h
	${CC} ${BOLO_CFLAGS} -c ${WBNETDIR}winbolonetevents.c
winbolonetthread.o: ${WBNETDIR}winbolonetthread.c ${WBNETDIR}winbolonetthread.h
	${CC} ${BOLO_CFLAGS} -c ${WBNETDIR}winbolonetthread.c


http.o: ${WBNETDIR}http.c ${WBNETDIR}http.h
	${CC} ${BOLO_CFLAGS} -c ${WBNETDIR}http.c

# Bolo
global.o: ${BOLODIR}global.c ${BOLODIR}global.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}global.c

backend.o: ${BOLODIR}backend.c 
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}backend.c

util.o: ${BOLODIR}util.c ${BOLODIR}util.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}util.c

crc.o: ${BOLODIR}crc.c ${BOLODIR}crc.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}crc.c

shells.o: ${BOLODIR}shells.c ${BOLODIR}shells.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}shells.c

screenbullet.o: ${BOLODIR}screenbullet.c ${BOLODIR}screenbullet.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}screenbullet.c

playersrejoin.o: ${BOLODIR}playersrejoin.c ${BOLODIR}playersrejoin.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}playersrejoin.c

bases.o: ${BOLODIR}bases.h ${BOLODIR}bases.c
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}bases.c
bolo_map.o: ${BOLODIR}bolo_map.c ${BOLODIR}bolo_map.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}bolo_map.c
building.o: ${BOLODIR}building.c ${BOLODIR}building.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}building.c
explosions.o: ${BOLODIR}explosions.c ${BOLODIR}explosions.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}explosions.c
floodfill.o: ${BOLODIR}floodfill.c ${BOLODIR}floodfill.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}floodfill.c
gametype.o: ${BOLODIR}gametype.c ${BOLODIR}gametype.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}gametype.c
grass.o: ${BOLODIR}grass.c ${BOLODIR}grass.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}grass.c
labels.o: ${BOLODIR}labels.c ${BOLODIR}labels.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}labels.c
lgm.o: ${BOLODIR}lgm.c ${BOLODIR}lgm.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}lgm.c
log.o: ${BOLODIR}log.c ${BOLODIR}log.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}log.c
mathWinbolo.o: ${BOLODIR}mathWinbolo.c ${BOLODIR}mathWinbolo.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}mathWinbolo.c

messages.o: ${BOLODIR}messages.c ${BOLODIR}messages.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}messages.c

mines.o: ${BOLODIR}mines.c ${BOLODIR}mines.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}mines.c
minesexp.o: ${BOLODIR}minesexp.c ${BOLODIR}minesexp.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}minesexp.c
netmt.o: ${BOLODIR}netmt.c ${BOLODIR}netmt.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}netmt.c

netplayers.o: ${BOLODIR}netplayers.c ${BOLODIR}netplayers.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}netplayers.c

netpnb.o: ${BOLODIR}netpnb.c ${BOLODIR}netpnb.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}netpnb.c

network.o: ${BOLODIR}network.c ${BOLODIR}network.h ${SERVERDIR}rsaalgorithm.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}network.c
pillbox.o: ${BOLODIR}pillbox.c ${BOLODIR}pillbox.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}pillbox.c
players.o: ${BOLODIR}players.c ${BOLODIR}players.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}players.c

rubble.o: ${BOLODIR}rubble.c ${BOLODIR}rubble.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}rubble.c

screenbrainmap.o: ${BOLODIR}screenbrainmap.c ${BOLODIR}screenbrainmap.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}screenbrainmap.c

screencalc.o: ${BOLODIR}screencalc.c ${BOLODIR}screencalc.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}screencalc.c

screenlgm.o: ${BOLODIR}screenlgm.c ${BOLODIR}screenlgm.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}screenlgm.c
screentank.o: ${BOLODIR}screentank.c ${BOLODIR}screentank.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}screentank.c
scroll.o: ${BOLODIR}scroll.c ${BOLODIR}scroll.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}scroll.c

sounddist.o: ${BOLODIR}sounddist.c ${BOLODIR}sounddist.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}sounddist.c
starts.o: ${BOLODIR}starts.c ${BOLODIR}starts.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}starts.c
swamp.o: ${BOLODIR}swamp.c ${BOLODIR}swamp.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}swamp.c
tank.o: ${BOLODIR}tank.c ${BOLODIR}tank.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}tank.c
allience.o: ${BOLODIR}allience.c ${BOLODIR}allience.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}allience.c
tankexp.o: ${BOLODIR}tankexp.c ${BOLODIR}tankexp.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}tankexp.c
treegrow.o: ${BOLODIR}treegrow.c ${BOLODIR}treegrow.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}treegrow.c
udppackets.o: ${BOLODIR}udppackets.c ${BOLODIR}udppackets.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}udppackets.c
debug_file_output.o: ${BOLODIR}debug_file_output.c ${BOLODIR}debug_file_output.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}debug_file_output.c

#
# LZW
#
dcodlzw.o: ${COMPRESSDIR}dcodlzw.c
	${CC} ${CFLAGS} -c ${COMPRESSDIR}dcodlzw.c
ecodlzw.o: ${COMPRESSDIR}ecodlzw.c
	${CC} ${CFLAGS} -c ${COMPRESSDIR}ecodlzw.c

# Server 
servercore.o: ${SERVERDIR}servercore.c ${SERVERDIR}servercore.h
	${CC} ${BOLO_CFLAGS} -c ${SERVERDIR}servercore.c

servermessages.o: ${SERVERDIR}servermessages.c ${BOLODIR}messages.h
	 ${CC} ${BOLO_CFLAGS} -c ${SERVERDIR}servermessages.c

serverfrontend.o: ${SERVERDIR}serverfrontend.c 
	${CC} ${BOLO_CFLAGS} -c ${SERVERDIR}serverfrontend.c

servermain.o: ${SERVERDIR}servermain.c
	${CC} ${BOLO_CFLAGS} -c ${SERVERDIR}servermain.c
servernet.o: ${SERVERDIR}servernet.c ${SERVERDIR}servernet.h ${SERVERDIR}rsaalgorithm.h
	${CC}  ${BOLO_CFLAGS} -c ${SERVERDIR}servernet.c
servertransport.o: ${SERVERDIR}servertransport.c ${SERVERDIR}servertransport.h
	${CC}  ${BOLO_CFLAGS} -c ${SERVERDIR}servertransport.c

threads.o: ${SERVERDIR}threads.c ${SERVERDIR}threads.h
	${CC}  ${BOLO_CFLAGS} -c ${SERVERDIR}threads.c

