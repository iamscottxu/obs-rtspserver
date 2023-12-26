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

<font size="5">[Helfen Sie bei der Übersetzung von obs-rtspserver!](https://www.transifex.com/scott-xu/obs-rtspserver)</font>

# OBS-RTSPServer

Dies ist ein Plugin für obs-studio, das die Ausgabe codiert und ein RTSP-Stream veröffentlicht.

**Unterstützte Betriebssysteme** : Windows 10, Windows 11, Linux und macOS

**Unterstützte OBS Studio Version**: 30.0.0+

[![Paketstatus](https://repology.org/badge/vertical-allrepos/obs-rtspserver.svg)](https://repology.org/project/obs-rtspserver/versions)

# Installation
## Windows
Der Installer kann auf der [Release-Seite](https://github.com/iamscottxu/obs-rtspserver/releases) gefunden werden.

Wenn Sie eine komprimierte Datei verwenden möchten, um manuell zu installieren, können Sie sie entpacken (z.B.: obs-rtspserver-v2.0.5-windows.zip) und in den Installationsordner von obs-studio legen.

### winget Paket
Wenn die Betriebssystemversion Windows 10 1709 oder neuer ist und [app-installer](https://www.microsoft.com/store/productId/9NBLGGH4NNS1) installiert wurde, führen Sie einfach Folgendes aus, um es zu installieren:

```powershell
winget install iamscottxu.obs-rtspserver
```

## MacOS
Sie können den .pkg-Installer verwenden, um zu installieren, und der Installer kann unter [Release-Seite](https://github.com/iamscottxu/obs-rtspserver/releases) gefunden werden, wenn macOS verwendet wird.

## Untar, z.B.: tar -xzvf obs-rtspserver-v2.0.1-linux.tar.gz -C $HOME/.config/obs-studio/plugins/
### ArchLinux AUR Paket
obs-rtspserver ist auch als [AUR Paket](https://aur.archlinux.org/packages/?O=0&K=obs-rtspserver) verfügbar.
Wenn Sie [yay](https://github.com/Jguer/yay) benutzen, können Sie es mit folgendem Befehl installieren:

```bash
wget -O obs-rtspserver-linux.deb https://github.com/iamscottxu/obs-rtspserver/releases/download/{version}/obs-rtspserver-{version}-linux.deb
apt install -y obs-rtspserver-linux.deb
```
* Build

### Installieren Sie cmake, visual studio(nur bei windows) und qt.
Downloaden und konfigurieren Sie den Quellcode von obs-studio.

Kopieren Sie den Quellcode nach (obs-studio Quellcode)/plugins/obs-rtspserver/
* Fügen Sie `add_subdirectory(obs-rtspserver)` zu (obs-studio Quellcode)/plugins/CMakeLists.txt hinzu.

### Starten Sie den obs-rtspserver Build.
Das Plugin im Menü wurde nicht gefunden

```bash
yay -S obs-rtspserver
```

### Andere
Laden Sie das tar.gz-Archiv von der [Release-Seite](https://github.com/iamscottxu/obs-rtspserver/releases) herunter und entpacken Sie es in "/".

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
* Ersetzen Sie {version} durch die letzte Veröffentlichungsversion, z.B.: v2.2.0


# Bauen
* Install cmake, visual studio(only windows) and qt.
* Laden Sie den Quellcode von obs-studio herunter und konfigurieren Sie ihn.
* Kopieren Sie den Quellcode in (obs-studio Quellcode)/plugins/obs-rtspserver/
* Fügen Sie `add_subdirectory(obs-rtspserver)` zu (obs-studio Quellcode)/plugins/CMakeLists.txt hinzu.
* Build obs-rtspserver.

# FAQ
* [Kann das Plugin im Menü nicht finden](https://github.com/iamscottxu/obs-rtspserver/wiki/FAQ#cant-find-the-plugin-in-the-menu)

# Lizenz
* [RtspServer](https://github.com/PHZ76/RtspServer/) - [MIT License](https://github.com/PHZ76/RtspServer/blob/master/LICENSE)
* [Qt5](https://www.qt.io/) - [GPL version 2](https://doc.qt.io/qt-5/licensing.html)
* [libb64](https://sourceforge.net/projects/libb64/) - [Public domain dedication](https://sourceforge.net/p/libb64/git/ci/master/tree/LICENSE)
