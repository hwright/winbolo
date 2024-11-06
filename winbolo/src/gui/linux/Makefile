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


CFLAGS = -g -std=c11
BOLO_CFLAGS = ${CFLAGS} `sdl-config --cflags` `gtk-config --cflags` `pkg-config minizip --cflags` -Wall  -Wno-missing-braces
LDFLAGS = `sdl-config --libs` `gtk-config --libs` `glib-config --libs gthread` `pkg-config minizip --libs`
#-s
#CFLAGS = -s -O3 -ffast-math -fomit-frame-pointer -mpentium -march=pentium `sdl-config --cflags` `gtk-config --cflags` `glib-config --cflags` 
BOLODIR = ../../bolo/
GUIDIR = ../
WBNETDIR = ../../winbolonet/
GUIDIRLINUX = ./
SERVERDIR = ../../server/
COMPRESSDIR = ../../lzw/
APPNAME = linbolo
BDDIR = ../../server/BigDigits/

# added for BigDigits
BDINCLUDES = bigd.h bigdigits.h bigdtypes.h bigdRand.h bigdigitsRand.h

all: linbolo

clean:
	rm *.o
	rm linbolo
	

linbolo:    global.o bases.o util.o crc.o pillbox.o lgm.o shells.o tank.o \
	allience.o bolo_map.o building.o explosions.o floodfill.o \
	gametype.o grass.o labels.o messages.o mines.o minesexp.o \
	netmt.o netpnb.o players.o rubble.o backend.o netplayers.o \
	screenbrainmap.o screenbullet.o screencalc.o screenlgm.o \
	screentank.o scroll.o sounddist.o starts.o swamp.o \
	tankexp.o treegrow.o playersrejoin.o network.o screen.o \
	main.o draw.o lang.o netclient.o clientmutex.o framemutex.o \
	dcodlzw.o ecodlzw.o input.o cursor.o sound.o preferences.o \
	dialogsysteminfo.o dialognetworkinfo.o dialoggameinfo.o \
	dialogmessages.o dialogopening.o dialoggamesetup.o \
	dialogudpsetup.o dialogtrackersetup.o dialogabout.o winbolonetthread.o \
	messagebox.o dialogpassword.o dialogsetname.o dialogalliance.o \
	currentgames.o dialoggamefinder.o brainshandler.o dialogkeysetup.o \
	log.o winbolonet.o udppackets.o winbolonetevents.o  \
	http.o dnslookups.o dialogwinbolonet.o mathWinbolo.o \
	servercore.o servermessages.o debug_file_output.o \
	servernet.o servertransport.o threads.o \
	bigd.o bigdigits.o bigdigitsRand.o bigdRand.o 
	${CC} \
	global.o bases.o util.o crc.o pillbox.o lgm.o \
	shells.o tank.o allience.o bolo_map.o building.o \
	explosions.o floodfill.o gametype.o grass.o labels.o \
	messages.o mines.o minesexp.o  netmt.o  backend.o \
	netpnb.o players.o rubble.o screenbrainmap.o netplayers.o \
	screenbullet.o screencalc.o screenlgm.o screentank.o \
	scroll.o sounddist.o starts.o swamp.o tankexp.o \
	treegrow.o playersrejoin.o network.o screen.o \
	lang.o main.o draw.o netclient.o clientmutex.o framemutex.o \
	dcodlzw.o ecodlzw.o input.o cursor.o sound.o \
	dialogsysteminfo.o dialognetworkinfo.o dialoggameinfo.o \
	dialogmessages.o dialogopening.o dialoggamesetup.o winbolonetthread.o \
	dialogudpsetup.o dialogtrackersetup.o dialogabout.o preferences.o \
	messagebox.o dialogpassword.o dialogsetname.o dialogalliance.o \
	currentgames.o dialoggamefinder.o brainshandler.o dialogkeysetup.o \
	log.o http.o winbolonet.o winbolonetevents.o udppackets.o \
	dnslookups.o dialogwinbolonet.o \
	bigd.o bigdigits.o bigdigitsRand.o bigdRand.o \
	mathWinbolo.o debug_file_output.o \
	servercore.o servermessages.o \
	servernet.o servertransport.o threads.o \
	-o ${APPNAME} ${LDFLAGS} -lSDL_ttf -lSDL_mixer \
	-L -L/usr/X11/lib -lX11


winbolonet.o: ${WBNETDIR}winbolonet.c ${WBNETDIR}winbolonet.h
	${CC} ${BOLO_CFLAGS} -c ${WBNETDIR}winbolonet.c

winbolonetthread.o: ${WBNETDIR}winbolonetthread.c ${WBNETDIR}winbolonetthread.h
	${CC} ${BOLO_CFLAGS} -c ${WBNETDIR}winbolonetthread.c

http.o: ${WBNETDIR}http.c ${WBNETDIR}http.h
	${CC} ${BOLO_CFLAGS} -c ${WBNETDIR}http.c

winbolonetevents.o: ${WBNETDIR}winbolonetevents.c ${WBNETDIR}winbolonetevents.h
	${CC} ${BOLO_CFLAGS} -c ${WBNETDIR}winbolonetevents.c

debug_file_output.o: ${BOLODIR}debug_file_output.c ${BOLODIR}debug_file_output.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}debug_file_output.c


main.o: main.c 
	${CC} ${BOLO_CFLAGS} -c main.c

dialogkeysetup.o: dialogkeysetup.c dialogkeysetup.h 
	${CC} ${BOLO_CFLAGS} -c dialogkeysetup.c

dnslookups.o: dnslookups.c 
	${CC} ${BOLO_CFLAGS} -c dnslookups.c

brainshandler.o: brainshandler.c
	${CC} ${BOLO_CFLAGS} -c brainshandler.c

preferences.o: preferences.c preferences.h
	${CC} ${BOLO_CFLAGS} -c preferences.c

currentgames.o: ../win32/currentgames.c ../currentgames.h
	${CC} ${BOLO_CFLAGS} -c ../win32/currentgames.c

dialogalliance.o: dialogalliance.c dialogalliance.h
	${CC} ${BOLO_CFLAGS} -c dialogalliance.c

dialoggamefinder.o: dialoggamefinder.c dialoggamefinder.h
	${CC} ${BOLO_CFLAGS} -c dialoggamefinder.c
	
dialogpassword.o: dialogpassword.c
	${CC} ${BOLO_CFLAGS} -c dialogpassword.c

dialogsetname.o: dialogsetname.c dialogsetname.h
	${CC} ${BOLO_CFLAGS} -c dialogsetname.c

dialoggamesetup.o: dialoggamesetup.c dialoggamesetup.h
	${CC} ${BOLO_CFLAGS} -c dialoggamesetup.c

dialogtrackersetup.o: dialogtrackersetup.c dialogtrackersetup.h
	${CC} ${BOLO_CFLAGS} -c dialogtrackersetup.c

dialogudpsetup.o: dialogudpsetup.c
	${CC} ${BOLO_CFLAGS} -c dialogudpsetup.c

messagebox.o: messagebox.c messagebox.h
	${CC} ${BOLO_CFLAGS} -c messagebox.c

dialogsysteminfo.o: dialogsysteminfo.c dialogsysteminfo.h
	${CC} ${BOLO_CFLAGS} -c dialogsysteminfo.c

dialogabout.o: dialogabout.c dialogabout.h
	${CC} ${BOLO_CFLAGS} -c dialogabout.c

dialogopening.o: dialogopening.c
	${CC} ${BOLO_CFLAGS} -c dialogopening.c

dialogmessages.o: dialogmessages.c dialogmessages.h
	${CC} ${BOLO_CFLAGS} -c dialogmessages.c

dialognetworkinfo.o: dialognetworkinfo.c dialognetworkinfo.h
	${CC} ${BOLO_CFLAGS} -c dialognetworkinfo.c

dialogwinbolonet.o: dialogwinbolonet.c dialogwinbolonet.h
	${CC} ${BOLO_CFLAGS} -c dialogwinbolonet.c

dialoggameinfo.o: dialoggameinfo.c dialoggameinfo.h
	${CC} ${BOLO_CFLAGS} -c dialoggameinfo.c

draw.o: draw.c draw.h
	${CC} ${BOLO_CFLAGS} -c draw.c

input.o: input.c input.h
	${CC} ${BOLO_CFLAGS} -c input.c

sound.o: sound.c sound.h
	${CC} ${BOLO_CFLAGS} -c sound.c

cursor.o: cursor.c cursor.h
	${CC} ${BOLO_CFLAGS} -c cursor.c

lang.o: ${GUIDIRLINUX}lang.c
	${CC} ${CFLGS} -c ${GUIDIRLINUX}lang.c

netclient.o: netclient.c
	${CC} ${BOLO_CFLAGS} -c netclient.c
dcodlzw.o: ${COMPRESSDIR}dcodlzw.c
	${CC} ${CFLAGS} -c ${COMPRESSDIR}dcodlzw.c

ecodlzw.o: ${COMPRESSDIR}ecodlzw.c
	${CC} ${CFLAGS} -c ${COMPRESSDIR}ecodlzw.c

framemutex.o: framemutex.c framemutex.h
	${CC} ${BOLO_CFLAGS} -c framemutex.c

clientmutex.o: clientmutex.c ${GUIDIR}clientmutex.h
	${CC} ${BOLO_CFLAGS} -c clientmutex.c
global.o: ${BOLODIR}global.c ${BOLODIR}global.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}global.c

util.o:	${BOLODIR}util.c ${BOLODIR}util.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}util.c

crc.o: ${BOLODIR}crc.c ${BOLODIR}crc.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}crc.c

backend.o: ${BOLODIR}backend.c ${BOLODIR}backend.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}backend.c


log.o: ${BOLODIR}log.c ${BOLODIR}log.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}log.c

udppackets.o: ${BOLODIR}udppackets.c ${BOLODIR}udppackets.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}udppackets.c

bases.o: ${BOLODIR}bases.h ${BOLODIR}bases.c
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}bases.c

lgm.o: ${BOLODIR}lgm.c ${BOLODIR}lgm.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}lgm.c

pillbox.o: ${BOLODIR}pillbox.c ${BOLODIR}pillbox.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}pillbox.c

shells.o: ${BOLODIR}shells.c ${BOLODIR}shells.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}shells.c

tank.o: ${BOLODIR}tank.c ${BOLODIR}tank.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}tank.c

allience.o: ${BOLODIR}allience.c ${BOLODIR}allience.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}allience.c

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

messages.o: ${BOLODIR}messages.c ${BOLODIR}messages.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}messages.c
	
mines.o: ${BOLODIR}mines.c ${BOLODIR}mines.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}mines.c

minesexp.o: ${BOLODIR}minesexp.c ${BOLODIR}minesexp.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}minesexp.c

netmt.o: ${BOLODIR}netmt.c ${BOLODIR}netmt.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}netmt.c
	
netpnb.o: ${BOLODIR}netpnb.c ${BOLODIR}netpnb.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}netpnb.c

players.o: ${BOLODIR}players.c ${BOLODIR}players.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}players.c
	
rubble.o: ${BOLODIR}rubble.c ${BOLODIR}rubble.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}rubble.c
	
screenbrainmap.o: ${BOLODIR}screenbrainmap.c ${BOLODIR}screenbrainmap.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}screenbrainmap.c
	
screenbullet.o: ${BOLODIR}screenbullet.c ${BOLODIR}screenbullet.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}screenbullet.c
	
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

tankexp.o: ${BOLODIR}tankexp.c ${BOLODIR}tankexp.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}tankexp.c

treegrow.o: ${BOLODIR}treegrow.c ${BOLODIR}treegrow.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}treegrow.c

playersrejoin.o: ${BOLODIR}playersrejoin.c ${BOLODIR}playersrejoin.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}playersrejoin.c

network.o: ${BOLODIR}network.c ${BOLODIR}network.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}network.c

screen.o: ${BOLODIR}screen.c ${BOLODIR}screen.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}screen.c

mathWinbolo.o: ${BOLODIR}mathWinbolo.c ${BOLODIR}mathWinbolo.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}mathWinbolo.c

netplayers.o: ${BOLODIR}netplayers.c ${BOLODIR}netplayers.h
	${CC} ${BOLO_CFLAGS} -c ${BOLODIR}netplayers.c



# bigdigits
bigd.o: $(BDDIR)bigd.h
	$(CC) $(BDFLAGS) -c $(BDDIR)bigd.c

bigdigits.o: $(BDDIR)bigdigits.h
	$(CC) $(BDFLAGS) -c $(BDDIR)bigdigits.c

bigdigitsRand.o: $(BDDIR)bigdigitsRand.h
	$(CC) $(BDFLAGS) -c $(BDDIR)bigdigitsRand.c

bigdRand.o: $(BDDIR)bigdRand.h
	$(CC) $(BDFLAGS) -c $(BDDIR)bigdRand.c

# Server
servercore.o: ${SERVERDIR}servercore.c ${SERVERDIR}servercore.h
	${CC} ${BOLO_CFLAGS} -c ${SERVERDIR}servercore.c

servermessages.o: ${SERVERDIR}servermessages.c ${BOLODIR}messages.h
	${CC} ${BOLO_CFLAGS} -c ${SERVERDIR}servermessages.c
servernet.o: ${SERVERDIR}servernet.c ${SERVERDIR}servernet.h ${SERVERDIR}rsaalgorithm.h
	${CC}  ${BOLO_CFLAGS} -c ${SERVERDIR}servernet.c
servertransport.o: ${SERVERDIR}servertransport.c ${SERVERDIR}servertransport.h
	${CC}  ${BOLO_CFLAGS} -c ${SERVERDIR}servertransport.c

threads.o: ${SERVERDIR}threads.c ${SERVERDIR}threads.h
	${CC}  ${BOLO_CFLAGS} -c ${SERVERDIR}threads.c

