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

<font size="5">[Ayuda a traducir obs-rtspserver!](https://www.transifex.com/scott-xu/obs-rtspserver)</font>

# OBS-RTSPServer

Este es un complemento para obs-studio que codificará la salida y publicará un flujo RTSP.

**Sistemas operativos compatibles** : Windows 10, Windows 11, Linux y macOS

**Versión de OBS Studio compatible**: 30.0.0+

[![Packaging status](https://repology.org/badge/vertical-allrepos/obs-rtspserver.svg)](https://repology.org/project/obs-rtspserver/versions)

# Instalar
## Windows
El instalador se puede encontrar en [Página de lanzamiento](https://github.com/iamscottxu/obs-rtspserver/releases).

Si quieres usar un archivo comprimido para instalar manualmente, puedes descomprimirlo (por ejemplo: obs-rtspserver-v2.0.5-windows.zip) y colocarlo en la carpeta de instalación de tu obs-studio.

### winget Paquete
Si la versión del sistema operativo es Windows 10 1709 o posterior y se ha instalado [app-installer](https://www.microsoft.com/store/productId/9NBLGGH4NNS1), simplemente ejecuta esto para instalarlo:

```powershell
winget install iamscottxu.obs-rtspserver
```

## MacOS
Puede utilizar el instalador .pkg para instalar y el instalador se puede encontrar en [Página de lanzamiento](https://github.com/iamscottxu/obs-rtspserver/releases) si utiliza macOS.

## Linux (Solo x64)
### Paquete DEB de Ubuntu/Debian
Descarga el paquete deb desde la [Página de lanzamiento](https://github.com/iamscottxu/obs-rtspserver/releases) e instálalo.

```bash
wget -O obs-rtspserver-linux.deb https://github.com/iamscottxu/obs-rtspserver/releases/download/{version}/obs-rtspserver-{version}-linux.deb
apt install -y obs-rtspserver-linux.deb
```
* Reemplace {version} con la última versión de lanzamiento, por ejemplo: v2.2.0

### Paquete Red-Hat RPM
Descargue el paquete rpm desde la [Página de lanzamiento](https://github.com/iamscottxu/obs-rtspserver/releases) e instálelo.

```bash
wget -O obs-rtspserver-linux.rpm https://github.com/iamscottxu/obs-rtspserver/releases/download/{version}/obs-rtspserver-{version}-linux.rpm
rpm -ivh obs-rtspserver-linux.rpm
```
* Reemplace {version} con la última versión de lanzamiento, por ejemplo: v2.2.0

### ArchLinux AUR Paquete
obs-rtspserver también está disponible como un [Paquete AUR](https://aur.archlinux.org/packages/?O=0&K=obs-rtspserver)
Si usas [yay](https://github.com/Jguer/yay) simplemente ejecuta esto para instalarlo:

```bash
yay -S obs-rtspserver
```

### Otros
Descargue el archivo tar.gz desde la [Página de lanzamiento](https://github.com/iamscottxu/obs-rtspserver/releases) y descomprímalo en "/".

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
* Reemplace {version} con la última versión de lanzamiento, por ejemplo: v2.2.0


# Construir
* Instala cmake, visual studio (solo en Windows) y qt.
* Descarga y configura el código fuente de obs-studio.
* Copiar el código fuente a (código fuente de obs-studio)/plugins/obs-rtspserver/
* Añade `add_subdirectory(obs-rtspserver)` a (código fuente de obs-studio)/plugins/CMakeLists.txt
* Construye obs-rtspserver.

# Preguntas frecuentes
* [No puedo encontrar el complemento en el menú](https://github.com/iamscottxu/obs-rtspserver/wiki/FAQ#cant-find-the-plugin-in-the-menu)

# Licencia
* [RtspServer](https://github.com/PHZ76/RtspServer/) - [MIT License](https://github.com/PHZ76/RtspServer/blob/master/LICENSE)
* [Qt5](https://www.qt.io/) - [GPL version 2](https://doc.qt.io/qt-5/licensing.html)
* [libb64](https://sourceforge.net/projects/libb64/) - [Public domain dedication](https://sourceforge.net/p/libb64/git/ci/master/tree/LICENSE)
