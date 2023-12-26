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

Ceci est un plugin pour obs-studio, encoder et publier dans un flux RTSP.

**Systèmes d'exploitation pris en charge** : Windows 10, Windows 11, Linux and macOS

**Version OBS Studio prise en charge** : 30.0.0+

[![Statut de l'emballage](https://repology.org/badge/vertical-allrepos/obs-rtspserver.svg)](https://repology.org/project/obs-rtspserver/versions)

# Installer
## Windows
Vous pouvez utiliser le programme d'installation pour l'installation et le programme d'installation se trouve dans [Page de version](https://github.com/iamscottxu/obs-rtspserver/releases) si vous utilisez Windows.

Si vous souhaitez utiliser un fichier compressé pour l'installer manuellement, vous pouvez le décompresser (par exemple : obs-rtspserver-v2.0.5-windows.zip) et le placer dans votre dossier d'installation obs-studio.

### winget Package
Si la version du système d'exploitation est Windows 10 1709 ou ultérieure et que [app-installer](https://www.microsoft.com/store/productId/9NBLGGH4NNS1) a été installé, il suffit d'exécuter ceci pour l'installer :

```powershell
winget install iamscottxu.obs-rtspserver
```

## MacOS
Vous pouvez utiliser l'installateur .pkg pour installer et l'installateur peut être trouvé dans [Release Page](https://github.com/iamscottxu/obs-rtspserver/releases) si vous utilisez macOS.

## Linux (Seulement x64)
### ArchLinux AUR Paquet
Téléchargez le paquet deb depuis la [Page de sortie](https://github.com/iamscottxu/obs-rtspserver/releases) et installez-le.

```bash
wget -O obs-rtspserver-linux.deb https://github.com/iamscottxu/obs-rtspserver/releases/download/{version}/obs-rtspserver-{version}-linux.deb
apt install -y obs-rtspserver-linux.deb
```
* Remplacez {version} par la dernière version de sortie, par exemple : v2.2.0

### Package Red-Hat RPM
Téléchargez le package rpm depuis la [Page de sortie](https://github.com/iamscottxu/obs-rtspserver/releases) et installez-le.

```bash
wget -O obs-rtspserver-linux.rpm https://github.com/iamscottxu/obs-rtspserver/releases/download/{version}/obs-rtspserver-{version}-linux.rpm
rpm -ivh obs-rtspserver-linux.rpm
```
* Remplacez {version} par la dernière version de sortie, par exemple : v2.2.0

### ArchLinux AUR Package
obs-rtspserver est également disponible en tant que [paquet AUR](https://aur.archlinux.org/packages/?O=0&K=obs-rtspserver)
Si vous utilisez [yay](https://github.com/Jguer/yay), exécutez simplement ceci pour l'installer :

```bash
yay -S obs-rtspserver
```

### Autre
Téléchargez l'archive tar.gz depuis la [Page de publication](https://github.com/iamscottxu/obs-rtspserver/releases) et décompressez-la dans "/".

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
* Remplacez {version} par la dernière version de sortie, par exemple : v2.2.0


# Construire
* Installer cmake, visual studio (uniquement sur Windows) et qt.
* Téléchargez et configurez le code source d'obs-studio.
* Copiez le code source vers (code source obs-studio)/plugins/obs-rtspserver/
* Ajoutez `add_subdirectory(obs-rtspserver)` à (obs-studio source code)/plugins/CMakeLists.txt
* Construisez obs-rtspserver.

# FAQ
* [Impossible de trouver le plugin dans le menu](https://github.com/iamscottxu/obs-rtspserver/wiki/FAQ#cant-find-the-plugin-in-the-menu)

# Licence
* [RtspServer](https://github.com/PHZ76/RtspServer/) - [MIT License](https://github.com/PHZ76/RtspServer/blob/master/LICENSE)
* [Qt5](https://www.qt.io/) - [GPL version 2](https://doc.qt.io/qt-5/licensing.html)
* [libb64](https://sourceforge.net/projects/libb64/) - [Public domain dedication](https://sourceforge.net/p/libb64/git/ci/master/tree/LICENSE)
