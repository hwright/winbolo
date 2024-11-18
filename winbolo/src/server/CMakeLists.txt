cmake_minimum_required(VERSION 3.5)

project(Bolo VERSION 1.90.0)

set(CMAKE_C_STANDARD 11)
set(C_STANDARD_REQUIRED ON)

find_package(PkgConfig)
pkg_check_modules(MINIZIP REQUIRED minizip)
pkg_check_modules(SDL REQUIRED sdl)

add_executable(linbolods
               servercore.c
               serverfrontend.c
               servermain.c
               servermessages.c
               servernet.c
               servertransport.c
               threads.c
               ../bolo/allience.c
               ../bolo/backend.c
               ../bolo/bases.c
               ../bolo/bolo_map.c
               ../bolo/building.c
               ../bolo/crc.c
               ../bolo/debug_file_output.c
               ../bolo/explosions.c
               ../bolo/floodfill.c
               ../bolo/gametype.c
               ../bolo/grass.c
               ../bolo/labels.c
               ../bolo/lgm.c
               ../bolo/log.c
               ../bolo/mathWinbolo.c
               ../bolo/mines.c
               ../bolo/minesexp.c
               ../bolo/netmt.c
               ../bolo/netplayers.c
               ../bolo/netpnb.c
               ../bolo/pillbox.c
               ../bolo/players.c
               ../bolo/playersrejoin.c
               ../bolo/rubble.c
               ../bolo/screenbrainmap.c
               ../bolo/screenbullet.c
               ../bolo/screencalc.c
               ../bolo/shells.c
               ../bolo/starts.c
               ../bolo/swamp.c
               ../bolo/tank.c
               ../bolo/tankexp.c
               ../bolo/treegrow.c
               ../bolo/udppackets.c
               ../bolo/util.c
               ../winbolonet/http.c
               ../winbolonet/winbolonet.c
               ../winbolonet/winbolonetevents.c
               ../winbolonet/winbolonetthread.c
               ../gui/linux/preferences.c
               ../gui/linux/lang.c
               )

include_directories(${MINIZIP_LIBRARIES})
include_directories(${SDL_INCLUDE_DIRS})
target_link_libraries(linbolods PRIVATE
                      bd
                      lzw
                      ${MINIZIP_LIBRARIES}
                      ${SDL_LIBRARIES}
                      m)
