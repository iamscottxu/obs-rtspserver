![CI Release](https://github.com/iamscottxu/obs-rtspserver/workflows/CI%20Release/badge.svg)

# OBS-RTSPServer

Dies ist ein Plugin für obs-studio, das die Ausgabe codiert und ein RTSP-Stream veröffentlicht.

**Unterstützte Betriebssysteme** : Windows 7 , Windows 8 , Windows 10 und Linux

**Unterstützte OBS Studio Versionen** : 24.0.0+

# Installation
## Windows
Sie können das Installationsprogramm zum Installieren verwenden. Das Installationsprogramm finden Sie hier [Release Seite](https://github.com/iamscottxu/obs-rtspserver/releases).

Wenn Sie eine komprimierte Datei zur manuellen Installation verwenden möchten, können Sie sie entpacken (z. B. obs-rtspserver-v1.2.0-windows.zip) und in Ihrem obs-studio-Installationsordner ablegen.

## Linux (nur x64)
* Laden Sie die neueste Version: [Release Seite](https://github.com/iamscottxu/obs-rtspserver/releases).
* mkdir -p $HOME/.config/obs-studio/plugins
* Untar, z.B.: tar -xzvf obs-rtspserver-v1.2.0-linux.tar.gz -C $HOME/.config/obs-studio/plugins/

### ArchLinux AUR Paket
obs-rtspserver ist auch als [AUR Paket](https://aur.archlinux.org/packages/obs-rtspserver-bin/) verfügbar.
Wenn Sie [yay](https://github.com/Jguer/yay) benutzen, können Sie es mit folgendem Befehl installieren:

```shell
yay -S obs-rtspserver-bin
```

# Build
* Installieren Sie cmake, visual studio(nur bei windows) und qt.
* Downloaden und konfigurieren Sie den Quellcode von obs-studio.
* Kopieren Sie den Quellcode nach (obs-studio Quellcode)/plugins/obs-rtspserver/
* Fügen Sie `add_subdirectory(obs-rtspserver)` zu (obs-studio Quellcode)/plugins/obs-rtspserver/CMakeLists.txt hinzu.
* Starten Sie den obs-rtspserver Build.

# Das Plugin im Menü wurde nicht gefunden
Siehe auch: [https://github.com/iamscottxu/obs-rtspserver/issues/1](https://github.com/iamscottxu/obs-rtspserver/issues/1).

# Lizenzen
* [RtspServer](https://github.com/PHZ76/RtspServer/blob/master/LICENSE) - [MIT License](https://github.com/PHZ76/RtspServer/blob/master/LICENSE)
* [Qt5](https://www.qt.io/) - [GPL version 2](https://doc.qt.io/qt-5/licensing.html)
