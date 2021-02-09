![CI Release](https://github.com/iamscottxu/obs-rtspserver/workflows/CI%20Release/badge.svg)

# OBS-RTSPServer

Este es un complemento para obs-studio que codificará la salida y publicará un flujo RTSP.

**Sistemas operativos compatibles** : Windows 7 , Windows 8 , Windows 10 , Linux y macOS

**Versiones de OBS Studio compatibles** : 24.0.0+

# Instalación
## Windows
Puede utilizar el instalador para instalar. Puede encontrar el programa de instalación aquí [Página de lanzamiento](https://github.com/iamscottxu/obs-rtspserver/releases).

Si desea utilizar un archivo comprimido para la instalación manual, puede descomprimirlo (por ejemplo, obs-rtspserver-v1.2.2-windows.zip) y guardarlo en su carpeta de instalación obs-studio.

## Linux (solamente x64)
* Descarge la última versión: [Página de lanzamiento](https://github.com/iamscottxu/obs-rtspserver/releases).
* mkdir -p $HOME/.config/obs-studio/plugins
* Untar, por Ej.: tar -xzvf obs-rtspserver-v1.2.2-linux.tar.gz -C $HOME/.config/obs-studio/plugins/

[![Estado de empaque](https://repology.org/badge/vertical-allrepos/obs-rtspserver.svg)](https://repology.org/project/obs-rtspserver/versions)

### ArchLinux Paquete AUR
obs-rtspserver también está disponible como un [paquete AUR](https://aur.archlinux.org/packages/obs-rtspserver-bin/).
Si está usando [yay] (https://github.com/Jguer/yay) puede instalarlo con el siguiente comando:

```shell
yay -S obs-rtspserver-bin
```

# Build
* Instale cmake, visual studio (solo para Windows) y qt.
* Descargue y configure el código fuente de obs-studio.
* Copie el código fuente en (código fuente obs-studio) / plugins / obs-rtspserver /
* Agregue `add_subdirectory (obs-rtspserver)` a (código fuente de obs-studio) /plugins/obs-rtspserver/CMakeLists.txt.
* Inicie la compilación de obs-rtspserver.

# No se encontró el complemento en el menú
Vea también: [https://github.com/iamscottxu/obs-rtspserver/issues/1](https://github.com/iamscottxu/obs-rtspserver/issues/1).

# Licencias
* [RtspServer](https://github.com/PHZ76/RtspServer/) - [MIT License](https://github.com/PHZ76/RtspServer/blob/master/LICENSE)
* [Qt5](https://www.qt.io/) - [GPL version 2](https://doc.qt.io/qt-5/licensing.html)
