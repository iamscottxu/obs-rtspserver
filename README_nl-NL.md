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

<font size="5">[Help translate obs-rtspserver!](https://www.transifex.com/scott-xu/obs-rtspserver)</font>

# OBS-RTSPServer

Dit is een plug-in voor obs-studio die de uitvoer codeert en een RTSP-stream publiceert.

**Ondersteunde besturingssystemen** : Windows 10, Windows 11, Linux and macOS

**Ondersteunde OBS Studio-versies** : 30.0.0+

[![Verpakkingsstatus](https://repology.org/badge/vertical-allrepos/obs-rtspserver.svg)](https://repology.org/project/obs-rtspserver/versions)

# Installatie
## Windows
U kunt het installatieprogramma gebruiken om het te installeren. Je vindt het installatieprogramma hier [release-pagina](https://github.com/iamscottxu/obs-rtspserver/releases).

Als je een gecomprimeerd bestand wilt gebruiken voor handmatige installatie, kun je het uitpakken (bijv. obs-rtspserver-v2.0.1-windows.zip) en het in je obs-studio installatiemap plaatsen.

## macOS
U kunt het .pkg installatieprogramma gebruiken om het te installeren. Je vindt het installatieprogramma hier [release-pagina](https://github.com/iamscottxu/obs-rtspserver/releases).

## Linux (alleen x64)
* Download de laatste versie: [Release Page] (https://github.com/iamscottxu/obs-rtspserver/releases).
* mkdir -p $HOME/.config/obs-studio/plugins
* Untar, bijv.: tar -xzvf obs-rtspserver-v2.0.1-linux.tar.gz -C $HOME/.config/obs-studio/plugins/

### ArchLinux AUR Pakket
obs-rtspserver is ook beschikbaar als een [AUR-Pakket] (https://aur.archlinux.org/packages/?O=0&K=obs-rtspserver).
Als je [yay] (https://github.com/Jguer/yay) gebruikt, kun je het installeren met het volgende commando:

```shell
yay -S obs-rtspserver
```

# Build
* Installeer cmake, visual studio (alleen voor Windows) en qt.
* Download en configureer de broncode van obs-studio.
* Kopieer de broncode naar (obs-studio source code) / plugins / obs-rtspserver /
* Voeg 'add_subdirectory (obs-rtspserver)' toe aan (obs-studio source code)/plugins/CMakeLists.txt.
* Start de obs-rtspserver Build.

# De plugin werd niet gevonden in het menu
Zie ook: [https://github.com/iamscottxu/obs-rtspserver/issues/1](https://github.com/iamscottxu/obs-rtspserver/issues/1).

# Licenties
* [RtspServer](https://github.com/PHZ76/RtspServer/) - [MIT License](https://github.com/PHZ76/RtspServer/blob/master/LICENSE)
* [Qt5](https://www.qt.io/) - [GPL version 2](https://doc.qt.io/qt-5/licensing.html)
* [libb64](https://sourceforge.net/projects/libb64/) - [Public domain dedication](https://sourceforge.net/p/libb64/git/ci/master/tree/LICENSE)
