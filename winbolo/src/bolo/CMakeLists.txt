cmake_minimum_required(VERSION 3.10)

add_library(bolo STATIC building.cc
                        grass.cc
                        rubble.cc
                        swamp.cc
                        treegrow.cc
                        util.cc)
