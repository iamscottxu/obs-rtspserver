![Latest Release](https://img.shields.io/github/v/release/iamscottxu/obs-rtspserver.svg)
![CI Release](https://github.com/iamscottxu/obs-rtspserver/workflows/CI%20Release/badge.svg)
![Contributors](https://img.shields.io/github/contributors/iamscottxu/obs-rtspserver.svg)
![Total Downloads](https://img.shields.io/github/downloads/iamscottxu/obs-rtspserver/total.svg)
![License](https://img.shields.io/github/license/iamscottxu/obs-rtspserver.svg)


🇨🇳 [简体中文](//github.com/iamscottxu/obs-rtspserver/blob/master/README_zh-CN.md)
🇯🇵 [日本語](//github.com/iamscottxu/obs-rtspserver/blob/master/README_ja-JP.md)
🇰🇷 [한국어](//github.com/iamscottxu/obs-rtspserver/blob/master/README_ko-KR.md)
🇪🇦 [Español](//github.com/iamscottxu/obs-rtspserver/blob/master/README_es-ES.md)
🇫🇷 [Français](//github.com/iamscottxu/obs-rtspserver/blob/master/README_fr-FR.md)
🇮🇹 [Italiano](//github.com/iamscottxu/obs-rtspserver/blob/master/README_it-IT.md)
🇩🇪 [Deutsch](//github.com/iamscottxu/obs-rtspserver/blob/master/README_de-DE.md)
🇳🇱 [Nederlands](//github.com/iamscottxu/obs-rtspserver/blob/master/README_nl-NL.md)


# OBS-RTSPServer

This is a plugin for obs-studio, encode and publish to a RTSP stream.

**Supported Platforms** : Windows 7 , Windows 8 , Windows 10, Linux and macOS

**Supported OBS Studio version** : 24.0.0+

[![Packaging status](https://repology.org/badge/vertical-allrepos/obs-rtspserver.svg)](https://repology.org/project/obs-rtspserver/versions)

# Install
## Windows
The installer can be found in [Release Page](https://github.com/iamscottxu/obs-rtspserver/releases).

If you want to use compressed file to install manually, you can unzip it (e.g.: obs-rtspserver-v2.0.5-windows.zip) and put it to your obs-studio install folder.

### winget Package
If the operating system version is Windows 10 1709 or later and [app-installer](https://www.microsoft.com/store/productId/9NBLGGH4NNS1) has been installed, just run this to install it:

```powershell
winget install iamscottxu.obs-rtspserver
```

## MacOS
You can use the .pkg installer to install and the installer can be found in [Release Page](https://github.com/iamscottxu/obs-rtspserver/releases) if use macOS.

## Linux (Only x64)
### Ubuntu/Debian DEB Package
Download the deb package from the [Release Page](https://github.com/iamscottxu/obs-rtspserver/releases) and install it.

```bash
wget -O obs-rtspserver-linux.deb https://github.com/iamscottxu/obs-rtspserver/releases/download/{version}/obs-rtspserver-{version}-linux.deb
apt install -y obs-rtspserver-linux.deb
```
* Replace {version} with last release version, e.g.: v2.2.0

### Red-Hat RPM Package
Download the rpm package from the [Release Page](https://github.com/iamscottxu/obs-rtspserver/releases) and install it.

```bash
wget -O obs-rtspserver-linux.rpm https://github.com/iamscottxu/obs-rtspserver/releases/download/{version}/obs-rtspserver-{version}-linux.rpm
rpm -ivh obs-rtspserver-linux.rpm
```
* Replace {version} with last release version, e.g.: v2.2.0

### ArchLinux AUR Package
obs-rtspserver is also available as an [AUR Package](https://aur.archlinux.org/packages/?O=0&K=obs-rtspserver)
If you use [yay](https://github.com/Jguer/yay) just run this to install it:

```bash
yay -S obs-rtspserver
```

### Other
Download the tar.gz archive from the [Release Page](https://github.com/iamscottxu/obs-rtspserver/releases) and unpack to "/".

```bash
wget -O obs-rtspserver-linux.tar.gz https://github.com/iamscottxu/obs-rtspserver/releases/download/{version}/obs-rtspserver-{version}-linux.tar.gz
#For all user
tar -xzvf obs-rtspserver-linux.tar.gz -C /
#For local user
mkdir -p ~/.config/obs-studio/plugins/obs-rtspserver/bin/64bit/
mkdir -p ~/.config/obs-studio/plugins/obs-rtspserver/data/
mkdir -p ~/obs-rtspserver-linux
tar -xzvf obs-rtspserver-linux.tar.gz -C ~/obs-rtspserver-linux/
mv ~/obs-rtspserver-linux/usr/lib/obs-plugins/obs-rtspserver.so ~/.config/obs-studio/plugins/obs-rtspserver/bin/64bit/obs-rtspserver.so
mv ~/obs-rtspserver-linux/usr/share/obs/obs-plugins/obs-rtspserver/locale ~/.config/obs-studio/plugins/obs-rtspserver/data/locale
rm -rf ~/obs-rtspserver-linux
```
* Replace {version} with last release version, e.g.: v2.2.0


# Build
* Install cmake, visual studio(only windows) and qt.
* Download and configure the source code of obs-studio.
* Copy source code to (obs-studio source code)/plugins/obs-rtspserver/
* Add `add_subdirectory(obs-rtspserver)` to (obs-studio source code)/plugins/CMakeLists.txt
* Build obs-rtspserver.

# FAQ
* [Can't find the plugin in the menu](https://github.com/iamscottxu/obs-rtspserver/wiki/FAQ#cant-find-the-plugin-in-the-menu)

# License
* [RtspServer](https://github.com/PHZ76/RtspServer/) - [MIT License](https://github.com/PHZ76/RtspServer/blob/master/LICENSE)
* [Qt5](https://www.qt.io/) - [GPL version 2](https://doc.qt.io/qt-5/licensing.html)
* [libb64](https://sourceforge.net/projects/libb64/) - [Public domain dedication](https://sourceforge.net/p/libb64/git/ci/master/tree/LICENSE)
