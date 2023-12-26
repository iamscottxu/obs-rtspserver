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

Questo è un plugin per obs-studio che codificherà l'output e pubblicherà un flusso RTSP.

**Sistemi operativi supportati** : Windows 10, Windows 11, Linux e macOS

**Versione di OBS Studio supportata**: 30.0.0+

[![Stato del pacchetto](https://repology.org/badge/vertical-allrepos/obs-rtspserver.svg)](https://repology.org/project/obs-rtspserver/versions)

# Installazione
## Windows
È possibile utilizzare il programma di installazione per l'installazione. Puoi trovare il programma di installazione qui [Pagina di rilascio](https://github.com/iamscottxu/obs-rtspserver/releases).

Se desideri utilizzare un file compresso per l'installazione manuale, puoi decomprimerlo (ad esempio obs-rtspserver-v2.0.5-windows.zip) e salvarlo nella cartella di installazione di obs-studio.

### winget Pacchetto
Se il sistema operativo è Windows 10 1709 o successivo e [app-installer](https://www.microsoft.com/store/productId/9NBLGGH4NNS1) è stato installato, esegui semplicemente questo per installarlo:

```powershell
winget install iamscottxu.obs-rtspserver
```

## MacOS
È possibile utilizzare il programma di installazione .pkg per l'installazione. Puoi trovare il programma di installazione qui [Pagina di rilascio](https://github.com/iamscottxu/obs-rtspserver/releases).

## Linux (Solo x64)
### Ubuntu/Debian Pacchetto DEB
Scarica il pacchetto deb dalla [Pagina di rilascio](https://github.com/iamscottxu/obs-rtspserver/releases) e installalo.

```bash
wget -O obs-rtspserver-linux.deb https://github.com/iamscottxu/obs-rtspserver/releases/download/{version}/obs-rtspserver-{version}-linux.deb
apt install -y obs-rtspserver-linux.deb
```
* Sostituisci {version} con l'ultima versione rilasciata, ad esempio: v2.2.0

### Red-Hat RPM Package
Scarica il pacchetto rpm dalla [Pagina di rilascio](https://github.com/iamscottxu/obs-rtspserver/releases) e installalo.

```bash
wget -O obs-rtspserver-linux.rpm https://github.com/iamscottxu/obs-rtspserver/releases/download/{version}/obs-rtspserver-{version}-linux.rpm
rpm -ivh obs-rtspserver-linux.rpm
```
* Sostituisci {version} con l'ultima versione rilasciata, ad esempio: v2.2.0

### ArchLinux Pacchetto AUR
obs-rtspserver è disponibile anche come [Pacchetto AUR](https://aur.archlinux.org/packages/?O=0&K=obs-rtspserver).
Se stai usando [yay](https://github.com/Jguer/yay) puoi installarlo con il seguente comando:

```bash
yay -S obs-rtspserver
```

### Altro
Scarica l'archivio tar.gz dalla [Pagina di rilascio](https://github.com/iamscottxu/obs-rtspserver/releases) e decomprimilo in "/".

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
* Sostituisci {version} con l'ultima versione rilasciata, ad esempio: v2.2.0


# Costruisci
* Installa cmake, visual studio (solo su Windows) e qt.
* Scarica e configura il codice sorgente da obs-studio.
* Copia il codice sorgente in (codice sorgente obs-studio)/plugins/obs-rtspserver/
* Aggiungi `add_subdirectory (obs-rtspserver)` a (codice sorgente obs-studio)/plugins/CMakeLists.txt.
* Avvia la build obs-rtspserver.

# FAQ
* [Il plugin nel menu non è stato trovato](https://github.com/iamscottxu/obs-rtspserver/wiki/FAQ#cant-find-the-plugin-in-the-menu)

# Licenze software
* [RtspServer](https://github.com/PHZ76/RtspServer/) - [MIT License](https://github.com/PHZ76/RtspServer/blob/master/LICENSE)
* [Qt5](https://www.qt.io/) - [GPL version 2](https://doc.qt.io/qt-5/licensing.html)
* [libb64](https://sourceforge.net/projects/libb64/) - [Public domain dedication](https://sourceforge.net/p/libb64/git/ci/master/tree/LICENSE)
