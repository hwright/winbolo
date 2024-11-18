cmake_minimum_required(VERSION 3.10)

add_subdirectory(../../lzw lzw)
add_subdirectory(../../server/BigDigits bd)

add_executable(linbolo 
               brainshandler.c
               clientmutex.c
               cursor.c
               dialogabout.c
               dialogalliance.c
               dialoggamefinder.c
               dialoggameinfo.c
               dialoggamesetup.c
               dialogkeysetup.c
               dialogmessages.c
               dialognetworkinfo.c
               dialogopening.c
               dialogpassword.c
               dialogsetname.c
               dialogsysteminfo.c
               dialogtrackersetup.c
               dialogudpsetup.c
               dialogwinbolonet.c
               dnslookups.c
               draw.c
               framemutex.c
               input.c
               lang.c
               main.c
               messagebox.c
               preferences.c
               netclient.c
               sound.c
               ../../bolo/allience.c
               ../../bolo/backend.c
               ../../bolo/bases.c
               ../../bolo/bolo_map.c
               ../../bolo/building.c
               ../../bolo/crc.c
               ../../bolo/debug_file_output.c
               ../../bolo/explosions.c
               ../../bolo/floodfill.c
               ../../bolo/gametype.c
               ../../bolo/grass.c
               ../../bolo/labels.c
               ../../bolo/lgm.c
               ../../bolo/log.c
               ../../bolo/mathWinbolo.c
               ../../bolo/messages.c
               ../../bolo/mines.c
               ../../bolo/minesexp.c
               ../../bolo/netmt.c
               ../../bolo/netplayers.c
               ../../bolo/netpnb.c
               ../../bolo/network.c
               ../../bolo/pillbox.c
               ../../bolo/players.c
               ../../bolo/playersrejoin.c
               ../../bolo/rubble.c
               ../../bolo/screen.c
               ../../bolo/screenbrainmap.c
               ../../bolo/screenbullet.c
               ../../bolo/screencalc.c
               ../../bolo/screenlgm.c
               ../../bolo/screentank.c
               ../../bolo/scroll.c
               ../../bolo/shells.c
               ../../bolo/sounddist.c
               ../../bolo/starts.c
               ../../bolo/swamp.c
               ../../bolo/tank.c
               ../../bolo/tankexp.c
               ../../bolo/treegrow.c
               ../../bolo/udppackets.c
               ../../bolo/util.c
               ../../server/servercore.c
               ../../server/servertransport.c
               ../../server/servermessages.c
               ../../server/servernet.c
               ../../server/threads.c
               ../win32/currentgames.c
               ../../winbolonet/http.c
               ../../winbolonet/winbolonetevents.c
               ../../winbolonet/winbolonetthread.c
               ../../winbolonet/winbolonet.c)

target_compile_options(linbolo PRIVATE -Wno-compound-token-split-by-macro)

include_directories(${MINIZIP_INCLUDE_DIRS})
include_directories(${SDL_INCLUDE_DIRS})
include_directories(${GTK_INCLUDE_DIRS})
target_link_libraries(linbolo PRIVATE
                     bd
                     lzw
                     SDL_ttf
                     SDL_mixer
                     ${GTK_LIBRARIES}
                     ${MINIZIP_LIBRARIES}
                     ${SDL_LIBRARIES}
                     gmodule
                     gthread)

