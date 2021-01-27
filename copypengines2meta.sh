#!/bin/sh


rm -rf meta/engines/*

mkdir -p meta/engines/cmake
cp -r engines/cmake/* meta/engines/cmake
echo "copy engine/cmake ok..."

mkdir -p meta/engines/engine/inc/log
cp -r engines/engine/inc/log/* meta/engines/engine/inc/log
echo "copy engine/inc/log ok..."

mkdir -p meta/engines/lib/linux/Debug
mkdir -p meta/engines/lib/linux/Release
mkdir -p meta/engines/lib/win/Debug
mkdir -p meta/engines/lib/win/Release

cp -r engines/lib/linux/Debug/libelogger.a		 meta/engines/lib/linux/Debug
cp -r engines/lib/linux/Release/libelogger.a	 meta/engines/lib/linux/Release
cp -r engines/lib/win/Debug/elogger.lib			 meta/engines/lib/win/Debug
cp -r engines/lib/win/Debug/elogger.pdb			 meta/engines/lib/win/Debug
cp -r engines/lib/win/Release/elogger.lib		 meta/engines/lib/win/Release
echo "copy engine/lib ok..."

mkdir -p meta/engines/thirds/asio
cp -r engines/thirds/asio/*  meta/engines/thirds/asio
echo "copy engine/thirds ok..."


