cmake_minimum_required(VERSION 3.10)

find_package(GTK2 2.0 REQUIRED gtk)

add_subdirectory(../../lzw lzw)
add_subdirectory(../../server/BigDigits bd)

add_executable(linbolo 
               brainshandler.cc
               cursor.cc
               dialogabout.cc
               dialogalliance.cc
               dialoggamefinder.cc
               dialoggameinfo.cc
               dialoggamesetup.cc
               dialogkeysetup.cc
               dialogmessages.cc
               dialognetworkinfo.cc
               dialogopening.cc
               dialogpassword.cc
               dialogsetname.cc
               dialogsysteminfo.cc
               dialogtrackersetup.cc
               dialogudpsetup.cc
               dialogwinbolonet.cc
               dnslookups.cc
               draw.cc
               framemutex.cc
               frontend.cc
               input.cc
               lang.cc
               main.cc
               messagebox.cc
               preferences.cc
               netclient.cc
               sound.cc
               ../clientmutex.cc
               ../../bolo/backend.cc
               ../../bolo/bases.cc
               ../../bolo/bolo_map.cc
               ../../bolo/crc.cc
               ../../bolo/debug_file_output.cc
               ../../bolo/gametype.cc
               ../../bolo/lgm.cc
               ../../bolo/log.cc
               ../../bolo/mathWinbolo.cc
               ../../bolo/messages.cc
               ../../bolo/netmt.cc
               ../../bolo/netplayers.cc
               ../../bolo/netpnb.cc
               ../../bolo/network.cc
               ../../bolo/pillbox.cc
               ../../bolo/players.cc
               ../../bolo/playersrejoin.cc
               ../../bolo/screen.cc
               ../../bolo/screenbrainmap.cc
               ../../bolo/screencalc.cc
               ../../bolo/screentypes.cc
               ../../bolo/scroll.cc
               ../../bolo/shells.cc
               ../../bolo/sounddist.cc
               ../../bolo/starts.cc
               ../../bolo/tank.cc
               ../../bolo/tankexp.cc
               ../../bolo/udppackets.cc
               ../../server/servercore.cc
               ../../server/serverfrontend.cc
               ../../server/servertransport.cc
               ../../server/servermessages.cc
               ../../server/servernet.cc
               ../../server/threads.cc
               ../win32/currentgames.cc
               ../../winbolonet/http.cc
               ../../winbolonet/winbolonetevents.cc
               ../../winbolonet/winbolonetthread.cc
               ../../winbolonet/winbolonet.cc)

target_compile_options(linbolo PRIVATE -Wno-compound-token-split-by-macro)

include_directories(${MINIZIP_INCLUDE_DIRS})
include_directories(${SDL_INCLUDE_DIRS})
include_directories(${GTK_INCLUDE_DIRS})
target_link_libraries(linbolo PRIVATE
                     bd
                     bolo
                     lzw
                     SDL_ttf
                     SDL_mixer
                     ${GTK_LIBRARIES}
                     ${MINIZIP_LIBRARIES}
                     ${SDL_LIBRARIES}
                     ${SDL_image_LIBRARIES}
                     gmodule
                     X11)

