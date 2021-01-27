#!/bin/sh


#rm -rf code/engines/*
rm -rf code/engines/cmake
rm -rf code/engines/engine/inc
rm -rf code/engines/lib
#rm -rf code/engines/thirds

mkdir -p code/engines/cmake
cp -r engines/cmake/* code/engines/cmake
echo "copy engine/cmake ok..."

mkdir -p code/engines/engine/inc
cp -r engines/engine/inc/* code/engines/engine/inc
echo "copy engine/inc ok..."

mkdir -p code/engines/lib
cp -r engines/lib/* code/engines/lib
echo "copy engine/lib ok..."

#mkdir -p code/engines/thirds
#cp -r engines/thirds/*  code/engines/thirds
#echo "copy engine/thirds ok..."


