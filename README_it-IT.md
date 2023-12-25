# OBS-RTSPServer

Questo è un plugin per obs-studio che codificherà l'output e pubblicherà un flusso RTSP.

**Sistemi operativi supportati** : Windows 10, Windows 11, Linux e macOS

**Versioni di OBS Studio supportate** : 30.0.0+

[![Stato del pacchetto](https://repology.org/badge/vertical-allrepos/obs-rtspserver.svg)](https://repology.org/project/obs-rtspserver/versions)

# Installazione
## Windows
È possibile utilizzare il programma di installazione per l'installazione. Puoi trovare il programma di installazione qui [Pagina di rilascio](https://github.com/iamscottxu/obs-rtspserver/releases).

Se desideri utilizzare un file compresso per l'installazione manuale, puoi decomprimerlo (ad esempio obs-rtspserver-v2.0.7-windows.zip) e salvarlo nella cartella di installazione di obs-studio.

## macOS
È possibile utilizzare il programma di installazione .pkg per l'installazione. Puoi trovare il programma di installazione qui [Pagina di rilascio](https://github.com/iamscottxu/obs-rtspserver/releases).

## Linux (solo x64)
* Scarica l'ultima versione: [Pagina di rilascio](https://github.com/iamscottxu/obs-rtspserver/releases).
* mkdir -p $HOME/.config/obs-studio/plugins
* Untar, z.B.: tar -xzvf obs-rtspserver-v2.0.7-linux.tar.gz -C $HOME/.config/obs-studio/plugins/

### ArchLinux Pacchetto AUR
obs-rtspserver è disponibile anche come [Pacchetto AUR](https://aur.archlinux.org/packages/?O=0&K=obs-rtspserver).
Se stai usando [yay](https://github.com/Jguer/yay) puoi installarlo con il seguente comando:

```shell
yay -S obs-rtspserver
```

# Costruisci
* Installa cmake, visual studio (solo su Windows) e qt.
* Scarica e configura il codice sorgente da obs-studio.
* Copia il codice sorgente in (codice sorgente obs-studio)/plugins/obs-rtspserver/
* Aggiungi `add_subdirectory (obs-rtspserver)` a (codice sorgente obs-studio)/plugins/CMakeLists.txt.
* Avvia la build obs-rtspserver.

# Il plugin nel menu non è stato trovato
[https://github.com/iamscottxu/obs-rtspserver/issues/1](https://github.com/iamscottxu/obs-rtspserver/issues/1).

# Licenze software
* [RtspServer](https://github.com/PHZ76/RtspServer/) - [MIT License](https://github.com/PHZ76/RtspServer/blob/master/LICENSE)
* [Qt5](https://www.qt.io/) - [GPL version 2](https://doc.qt.io/qt-5/licensing.html)
* [libb64](https://sourceforge.net/projects/libb64/) - [Public domain dedication](https://sourceforge.net/p/libb64/git/ci/master/tree/LICENSE)
