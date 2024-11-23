cmake_minimum_required(VERSION 3.10)

add_library(bolo STATIC building.cc
                        floodfill.cc
                        grass.cc
                        minesexp.cc
                        rubble.cc
                        swamp.cc
                        treegrow.cc
                        util.cc)
