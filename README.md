![CI Windows Release](https://github.com/iamscottxu/obs-rtspserver/workflows/CI%20Windows%20Release/badge.svg) 

# OBS-RTSPServer

This is a plugin for obs-studio, encoding the output and publish rtsp stream.

**Supported Platforms** : Windows 7 , Windows 8 and Windows 10

**Supported OBS Studio version** : 24.0.0+

# Install
You can use installer to install and the installer can be found in [Release Page](https://github.com/iamscottxu/obs-rtspserver/releases).

If you want to use compressed file to install manually, you can unzip obs-rtspserver.x.x.x-windows.zip and put it to your obs-studio install folder.

# Build
* Install cmake, visual studio and qt.
* Download and configure the source code of obs-studio.
* Copy source code to (obs-studio source code)/plugins/obs-rtspserver/
* Add `add_subdirectory(obs-rtspserver)` to (obs-studio source code)/plugins/obs-rtspserver/CMakeLists.txt
* Compile obs-rtspserver.
