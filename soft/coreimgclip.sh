#!/bin/bash
[ -e coreimgclip ] && rm -rf coreimgclip
export PKG_CONFIG_PATH=/usr/local/Cellar/opencv/2.4.13.1/lib/pkgconfig:$PKG_CONFIG_PATH 
g++  CoreImgClip.cpp `pkg-config opencv --libs --cflags opencv` -o coreimgclip