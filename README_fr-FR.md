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

Ceci est un plugin pour obs-studio, encoder et publier dans un flux RTSP.

**Systèmes d'exploitation pris en charge** : Windows 10, Windows 11, Linux and macOS

**Version OBS Studio prise en charge** : 30.0.0+

[![Statut de l'emballage](https://repology.org/badge/vertical-allrepos/obs-rtspserver.svg)](https://repology.org/project/obs-rtspserver/versions)

# Installer
## Windows
Vous pouvez utiliser le programme d'installation pour l'installation et le programme d'installation se trouve dans [Page de version](https://github.com/iamscottxu/obs-rtspserver/releases) si vous utilisez Windows.

Si vous souhaitez utiliser un fichier compressé pour l'installer manuellement, vous pouvez le décompresser (par exemple : obs-rtspserver-v2.0.5-windows.zip) et le placer dans votre dossier d'installation obs-studio.

## macOS
Vous pouvez utiliser le programme d'installation .pkg pour l'installation et le programme d'installation se trouve sur la [Page de version](https://github.com/iamscottxu/obs-rtspserver/releases) si vous utilisez macOS.

## Linux (Seulement x64)
* Téléchargez la dernière version à partir de la [Page de version](https://github.com/iamscottxu/obs-rtspserver/releases).
* mkdir -p $HOME/.config/obs-studio/plugins
* Décompresser (Untar), par exemple: tar -xzvf obs-rtspserver-v2.0.5-linux.tar.gz -C $HOME/.config/obs-studio/plugins/

### ArchLinux AUR Paquet
obs-rtspserver est également disponible en tant que [AUR Paquet](https://aur.archlinux.org/packages/?O=0&K=obs-rtspserver)
Si vous utilisez [yay](https://github.com/Jguer/yay) exécutez simplement ceci pour l'installer :

```shell
yay -S obs-rtspserver
```

# Build
* Installez cmake, visual studio (uniquement pour Windows) et qt.
* Téléchargez et configurez le code source depuis obs-studio.
* Copiez le code source (obs-studio code source)/plugins/obs-rtspserver/
* Ajouter `add_subdirectory(obs-rtspserver)` -> (obs-studio code source)/plugins/CMakeLists.txt.
* Démarrer la compilation.

# Le plugin dans le menu n'a pas été trouvé
Voir également: [https://github.com/iamscottxu/obs-rtspserver/issues/1](https://github.com/iamscottxu/obs-rtspserver/issues/1).

# Licences
* [RtspServer](https://github.com/PHZ76/RtspServer/) - [MIT License](https://github.com/PHZ76/RtspServer/blob/master/LICENSE)
* [Qt5](https://www.qt.io/) - [GPL version 2](https://doc.qt.io/qt-5/licensing.html)
* [libb64](https://sourceforge.net/projects/libb64/) - [Public domain dedication](https://sourceforge.net/p/libb64/git/ci/master/tree/LICENSE)
