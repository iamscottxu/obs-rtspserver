![CI Release](https://github.com/iamscottxu/obs-rtspserver/workflows/CI%20Release/badge.svg)

🇨🇳 [简体中文](//github.com/iamscottxu/obs-rtspserver/blob/master/README_zh-cn.md)
🇩🇪 [Deutsch](//github.com/iamscottxu/obs-rtspserver/blob/master/README_de-de.md)

# OBS-RTSPServer

This is a plugin for obs-studio, encoding the output and publish rtsp stream.

**Supported Platforms** : Windows 7 , Windows 8 , Windows 10 and Linux

**Supported OBS Studio version** : 24.0.0+

# Install
## Windows
You can use installer to install and the installer can be found in [Release Page](https://github.com/iamscottxu/obs-rtspserver/releases) if use windows.

If you want to use compressed file to install manually, you can unzip it (e.g.: obs-rtspserver-v1.1.1-windows.zip) and put it to your obs-studio install folder.

## Linux (Only x64)
* Download the latest release from the [Release Page](https://github.com/iamscottxu/obs-rtspserver/releases).
* mkdir -p $HOME/.config/obs-studio/plugins
* Untar, e.g.: tar -xzvf obs-rtspserver-v1.1.1-linux.tar.gz -C $HOME/.config/obs-studio/plugins/

# Build
* Install cmake, visual studio(only windows) and qt.
* Download and configure the source code of obs-studio.
* Copy source code to (obs-studio source code)/plugins/obs-rtspserver/
* Add `add_subdirectory(obs-rtspserver)` to (obs-studio source code)/plugins/obs-rtspserver/CMakeLists.txt
* Build obs-rtspserver.

# Can't found the plugin in menu
See also [https://github.com/iamscottxu/obs-rtspserver/issues/1](https://github.com/iamscottxu/obs-rtspserver/issues/1).

# License
* [RtspServer](https://github.com/PHZ76/RtspServer/blob/master/LICENSE) - [MIT License](https://github.com/PHZ76/RtspServer/blob/master/LICENSE)
* [Qt5](https://www.qt.io/) - [GPL version 2](https://doc.qt.io/qt-5/licensing.html)
