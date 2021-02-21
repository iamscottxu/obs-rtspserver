![CI Release](https://github.com/iamscottxu/obs-rtspserver/workflows/CI%20Release/badge.svg)

# OBS-RTSPServer

Dit is een plug-in voor obs-studio die de uitvoer codeert en een RTSP-stream publiceert.

**Ondersteunde besturingssystemen** : Windows 7 , Windows 8 , Windows 10, Linux en macOS

**Ondersteunde OBS Studio-versies** : 24.0.0+

# Installatie
## Windows
U kunt het installatieprogramma gebruiken om het te installeren. Je vindt het installatieprogramma hier [release-pagina](https://github.com/iamscottxu/obs-rtspserver/releases).

Als je een gecomprimeerd bestand wilt gebruiken voor handmatige installatie, kun je het uitpakken (bijv. obs-rtspserver-v1.3.0-windows.zip) en het in je obs-studio installatiemap plaatsen.

## macOS
U kunt het .pkg installatieprogramma gebruiken om het te installeren. Je vindt het installatieprogramma hier [release-pagina](https://github.com/iamscottxu/obs-rtspserver/releases).

## Linux (alleen x64)
* Download de laatste versie: [Release Page] (https://github.com/iamscottxu/obs-rtspserver/releases).
* mkdir -p $HOME/.config/obs-studio/plugins
* Untar, bijv.: tar -xzvf obs-rtspserver-v1.3.0-linux.tar.gz -C $HOME/.config/obs-studio/plugins/

[![Verpakkingsstatus](https://repology.org/badge/vertical-allrepos/obs-rtspserver.svg)](https://repology.org/project/obs-rtspserver/versions)

### ArchLinux AUR Pakket
obs-rtspserver is ook beschikbaar als een [AUR-Pakket] (https://aur.archlinux.org/packages/obs-rtspserver-bin/).
Als je [yay] (https://github.com/Jguer/yay) gebruikt, kun je het installeren met het volgende commando:

```shell
yay -S obs-rtspserver-bin
```

# Build
* Installeer cmake, visual studio (alleen voor Windows) en qt.
* Download en configureer de broncode van obs-studio.
* Kopieer de broncode naar (obs-studio source code) / plugins / obs-rtspserver /
* Voeg 'add_subdirectory (obs-rtspserver)' toe aan (obs-studio source code) /plugins/obs-rtspserver/CMakeLists.txt.
* Start de obs-rtspserver Build.

# De plugin werd niet gevonden in het menu
Zie ook: [https://github.com/iamscottxu/obs-rtspserver/issues/1](https://github.com/iamscottxu/obs-rtspserver/issues/1).

# Licenties
* [RtspServer](https://github.com/PHZ76/RtspServer/) - [MIT License](https://github.com/PHZ76/RtspServer/blob/master/LICENSE)
* [Qt5](https://www.qt.io/) - [GPL version 2](https://doc.qt.io/qt-5/licensing.html)
