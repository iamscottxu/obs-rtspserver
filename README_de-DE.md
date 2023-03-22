# OBS-RTSPServer

Dies ist ein Plugin für obs-studio, das die Ausgabe codiert und ein RTSP-Stream veröffentlicht.

**Unterstützte Betriebssysteme** : Windows 10, Windows 11, Linux und macOS

**Unterstützte OBS Studio Versionen** : 28.0.0+

[![Paketstatus](https://repology.org/badge/vertical-allrepos/obs-rtspserver.svg)](https://repology.org/project/obs-rtspserver/versions)

# Installation
## Windows
Sie können das Installationsprogramm zum Installieren verwenden. Das Installationsprogramm finden Sie hier [Release Seite](https://github.com/iamscottxu/obs-rtspserver/releases).

Wenn Sie eine komprimierte Datei zur manuellen Installation verwenden möchten, können Sie sie entpacken (z. B. obs-rtspserver-v2.0.1-windows.zip) und in Ihrem obs-studio-Installationsordner ablegen.

## macOS
Sie können das .pkg Installationsprogramm zum Installieren verwenden. Das Installationsprogramm finden Sie hier [Release Seite](https://github.com/iamscottxu/obs-rtspserver/releases).

## Linux (nur x64)
* Laden Sie die neueste Version: [Release Seite](https://github.com/iamscottxu/obs-rtspserver/releases).
* mkdir -p $HOME/.config/obs-studio/plugins
* Untar, z.B.: tar -xzvf obs-rtspserver-v2.0.1-linux.tar.gz -C $HOME/.config/obs-studio/plugins/

### ArchLinux AUR Paket
obs-rtspserver ist auch als [AUR Paket](https://aur.archlinux.org/packages/?O=0&K=obs-rtspserver) verfügbar.
Wenn Sie [yay](https://github.com/Jguer/yay) benutzen, können Sie es mit folgendem Befehl installieren:

```shell
yay -S obs-rtspserver
```

# Build
* Installieren Sie cmake, visual studio(nur bei windows) und qt.
* Downloaden und konfigurieren Sie den Quellcode von obs-studio.
* Kopieren Sie den Quellcode nach (obs-studio Quellcode)/plugins/obs-rtspserver/
* Fügen Sie `add_subdirectory(obs-rtspserver)` zu (obs-studio Quellcode)/plugins/CMakeLists.txt hinzu.
* Starten Sie den obs-rtspserver Build.

# Das Plugin im Menü wurde nicht gefunden
Siehe auch: [https://github.com/iamscottxu/obs-rtspserver/issues/1](https://github.com/iamscottxu/obs-rtspserver/issues/1).

# Lizenzen
* [RtspServer](https://github.com/PHZ76/RtspServer/) - [MIT License](https://github.com/PHZ76/RtspServer/blob/master/LICENSE)
* [Qt5](https://www.qt.io/) - [GPL version 2](https://doc.qt.io/qt-5/licensing.html)
* [libb64](https://sourceforge.net/projects/libb64/) - [Public domain dedication](https://sourceforge.net/p/libb64/git/ci/master/tree/LICENSE)
