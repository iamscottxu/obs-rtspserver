![Latest Release](https://img.shields.io/github/v/release/iamscottxu/obs-rtspserver.svg)
![CI Release](https://github.com/iamscottxu/obs-rtspserver/workflows/CI%20Release/badge.svg)
![Contributors](https://img.shields.io/github/contributors/iamscottxu/obs-rtspserver.svg)
![Total Downloads](https://img.shields.io/github/downloads/iamscottxu/obs-rtspserver/total.svg)
![License](https://img.shields.io/github/license/iamscottxu/obs-rtspserver.svg)


🇨🇳 [简体中文](//github.com/iamscottxu/obs-rtspserver/blob/master/README_zh-CN.md)
🇭🇰 [繁體中文](//github.com/iamscottxu/obs-rtspserver/blob/master/README_zh-TW.md)
🇯🇵 [日本語](//github.com/iamscottxu/obs-rtspserver/blob/master/README_ja-JP.md)
🇰🇷 [한국어](//github.com/iamscottxu/obs-rtspserver/blob/master/README_ko-KR.md)
🇪🇦 [Español](//github.com/iamscottxu/obs-rtspserver/blob/master/README_es-ES.md)
🇫🇷 [Français](//github.com/iamscottxu/obs-rtspserver/blob/master/README_fr-FR.md)
🇮🇹 [Italiano](//github.com/iamscottxu/obs-rtspserver/blob/master/README_it-IT.md)
🇩🇪 [Deutsch](//github.com/iamscottxu/obs-rtspserver/blob/master/README_de-DE.md)
🇳🇱 [Nederlands](//github.com/iamscottxu/obs-rtspserver/blob/master/README_nl-NL.md)
🇷🇺 [Русский](//github.com/iamscottxu/obs-rtspserver/blob/master/README_ru-RU.md)

<font size="5">[Help translate obs-rtspserver!](https://www.transifex.com/scott-xu/obs-rtspserver)</font>

# OBS-RTSPServer

Dit is een plug-in voor obs-studio die de uitvoer codeert en een RTSP-stream publiceert.

**Ondersteunde besturingssystemen** : Windows 10, Windows 11, Linux and macOS

**Ondersteunde OBS Studio-versie**: 30.0.0+

[![Verpakkingsstatus](https://repology.org/badge/vertical-allrepos/obs-rtspserver.svg)](https://repology.org/project/obs-rtspserver/versions)

# Installatie
## Windows
U kunt het installatieprogramma gebruiken om het te installeren. Je vindt het installatieprogramma hier [release-pagina](https://github.com/iamscottxu/obs-rtspserver/releases).

Als je een gecomprimeerd bestand wilt gebruiken voor handmatige installatie, kun je het uitpakken (bijv. obs-rtspserver-v2.0.5-windows.zip) en het in je obs-studio installatiemap plaatsen.

### winget Pakket
Als de versie van het besturingssysteem Windows 10 1709 of later is en [app-installer](https://www.microsoft.com/store/productId/9NBLGGH4NNS1) is geïnstalleerd, voer dan gewoon dit uit om het te installeren:

```powershell
winget install iamscottxu.obs-rtspserver
```

## macOS
U kunt de .pkg-installateur gebruiken om te installeren en de installateur kan worden gevonden op [Release-pagina](https://github.com/iamscottxu/obs-rtspserver/releases) als u macOS gebruikt.

## Linux (Alleen x64)
### Ubuntu/Debian DEB-pakket
Download het deb-pakket van de [Releasepagina](https://github.com/iamscottxu/obs-rtspserver/releases) en installeer het.

```bash
wget -O obs-rtspserver-linux.deb https://github.com/iamscottxu/obs-rtspserver/releases/download/{version}/obs-rtspserver-{version}-linux.deb
apt install -y obs-rtspserver-linux.deb
```
* Vervang {versie} door de laatste versie van de release, bijvoorbeeld: v2.2.0

### Red-Hat RPM-pakket
Download het rpm-pakket van de [Releasepagina](https://github.com/iamscottxu/obs-rtspserver/releases) en installeer het.

```bash
wget -O obs-rtspserver-linux.rpm https://github.com/iamscottxu/obs-rtspserver/releases/download/{version}/obs-rtspserver-{version}-linux.rpm
rpm -ivh obs-rtspserver-linux.rpm
```
* Vervang {versie} door de laatste versie van de release, bijvoorbeeld: v2.2.0

### ArchLinux AUR-pakket
obs-rtspserver is ook beschikbaar als een [AUR-pakket](https://aur.archlinux.org/packages/?O=0&K=obs-rtspserver)
Als je [yay](https://github.com/Jguer/yay) gebruikt, voer dan gewoon dit uit om het te installeren:

```bash
yay -S obs-rtspserver
```

### Andere
Download het tar.gz-archief van de [Releasepagina](https://github.com/iamscottxu/obs-rtspserver/releases) en pak het uit naar "/".

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
* Vervang {versie} door de laatste versie van de release, bijvoorbeeld: v2.2.0


# Bouwen
* Installeer cmake, visual studio (alleen Windows) en qt.
* Download en configureer de broncode van obs-studio.
* Kopieer de broncode naar (obs-studio broncode)/plugins/obs-rtspserver/
* Voeg `add_subdirectory(obs-rtspserver)` toe aan (obs-studio broncode)/plugins/CMakeLists.txt
* Bouw obs-rtspserver.

# Veelgestelde vragen
* [Kan de plugin niet vinden in het menu](https://github.com/iamscottxu/obs-rtspserver/wiki/FAQ#cant-find-the-plugin-in-the-menu)

# Licentie
* [RtspServer](https://github.com/PHZ76/RtspServer/) - [MIT License](https://github.com/PHZ76/RtspServer/blob/master/LICENSE)
* [Qt5](https://www.qt.io/) - [GPL version 2](https://doc.qt.io/qt-5/licensing.html)
* [libb64](https://sourceforge.net/projects/libb64/) - [Public domain dedication](https://sourceforge.net/p/libb64/git/ci/master/tree/LICENSE)
