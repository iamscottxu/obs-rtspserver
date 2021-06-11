![CI Release](https://github.com/iamscottxu/obs-rtspserver/workflows/CI%20Release/badge.svg)

# OBS-RTSPServer

Ceci est un plugin pour obs-studio, encoder et publier dans un flux RTSP.

**Systèmes d'exploitation pris en charge** : Windows 7 , Windows 8 , Windows 10, Linux and macOS

**Version OBS Studio prise en charge** : 24.0.0+

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

[![Statut de l'emballage](https://repology.org/badge/vertical-allrepos/obs-rtspserver.svg)](https://repology.org/project/obs-rtspserver/versions)

### ArchLinux AUR Paquet
obs-rtspserver est également disponible en tant que [AUR Paquet](https://aur.archlinux.org/packages/obs-rtspserver-bin/)
Si vous utilisez [yay](https://github.com/Jguer/yay) exécutez simplement ceci pour l'installer :

```shell
yay -S obs-rtspserver-bin
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
