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

Este es un complemento para obs-studio que codificará la salida y publicará un flujo RTSP.

**Sistemas operativos compatibles** : Windows 10, Windows 11, Linux y macOS

**Versiones de OBS Studio compatibles** : 30.0.0+

[![Estado de empaque](https://repology.org/badge/vertical-allrepos/obs-rtspserver.svg)](https://repology.org/project/obs-rtspserver/versions)

# Instalación
## Windows
Puede utilizar el instalador para instalar. Puede encontrar el programa de instalación aquí [Página de lanzamiento](https://github.com/iamscottxu/obs-rtspserver/releases).

Si desea utilizar un archivo comprimido para la instalación manual, puede descomprimirlo (por ejemplo, obs-rtspserver-v2.0.1-windows.zip) y guardarlo en su carpeta de instalación obs-studio.

## macOS
Puede utilizar el instalador .pkg para instalar. Puede encontrar el programa de instalación aquí [Página de lanzamiento](https://github.com/iamscottxu/obs-rtspserver/releases).

## Linux (solamente x64)
* Descarge la última versión: [Página de lanzamiento](https://github.com/iamscottxu/obs-rtspserver/releases).
* mkdir -p $HOME/.config/obs-studio/plugins
* Untar, por Ej.: tar -xzvf obs-rtspserver-v2.0.1-linux.tar.gz -C $HOME/.config/obs-studio/plugins/

### ArchLinux Paquete AUR
obs-rtspserver también está disponible como un [paquete AUR](https://aur.archlinux.org/packages/?O=0&K=obs-rtspserver).
Si está usando [yay] (https://github.com/Jguer/yay) puede instalarlo con el siguiente comando:

```shell
yay -S obs-rtspserver
```

# Build
* Instale cmake, visual studio (solo para Windows) y qt.
* Descargue y configure el código fuente de obs-studio.
* Copie el código fuente en (código fuente obs-studio) / plugins / obs-rtspserver /
* Agregue `add_subdirectory (obs-rtspserver)` a (código fuente de obs-studio)/plugins/CMakeLists.txt.
* Inicie la compilación de obs-rtspserver.

# No se encontró el complemento en el menú
Vea también: [https://github.com/iamscottxu/obs-rtspserver/issues/1](https://github.com/iamscottxu/obs-rtspserver/issues/1).

# Licencias
* [RtspServer](https://github.com/PHZ76/RtspServer/) - [MIT License](https://github.com/PHZ76/RtspServer/blob/master/LICENSE)
* [Qt5](https://www.qt.io/) - [GPL version 2](https://doc.qt.io/qt-5/licensing.html)
* [libb64](https://sourceforge.net/projects/libb64/) - [Public domain dedication](https://sourceforge.net/p/libb64/git/ci/master/tree/LICENSE)
