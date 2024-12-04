cmake_minimum_required(VERSION 3.10)

add_library(bolo STATIC alliance.cc
                        building.cc
                        explosions.cc
                        floodfill.cc
                        grass.cc
                        labels.cc
                        mines.cc
                        minesexp.cc
                        rubble.cc
                        swamp.cc
                        treegrow.cc
                        util.cc)
