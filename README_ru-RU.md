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

<font size="5">[Помогите перевести obs-rtspserver!](https://www.transifex.com/scott-xu/obs-rtspserver)</font>

# OBS-RTSPServer

Это плагин для obs-studio, кодирующий и публикующий поток RTSP.

**Поддерживаемые платформы**: Windows 10, Windows 11, Linux и macOS

**Поддерживаемая версия OBS Studio**: 30.0.0+

[![Статус упаковки](https://repology.org/badge/vertical-allrepos/obs-rtspserver.svg)](https://repology.org/project/obs-rtspserver/versions)

# Установить
## Windows
Установщик можно найти на [странице релизов](https://github.com/iamscottxu/obs-rtspserver/releases).

Если вы хотите использовать сжатый файл для ручной установки, вы можете распаковать его (например: obs-rtspserver-v2.0.5-windows.zip) и поместить его в папку установки вашей программы obs-studio.

### winget Package
Если версия операционной системы Windows 10 1709 или более поздняя и [app-installer](https://www.microsoft.com/store/productId/9NBLGGH4NNS1) был установлен, просто запустите это, чтобы установить его.

```powershell
winget install iamscottxu.obs-rtspserver
```

## MacOS
Вы можете использовать установщик .pkg для установки, и установщик можно найти на [странице релизов](https://github.com/iamscottxu/obs-rtspserver/releases), если вы используете macOS.

## Linux (Только x64)
### Ubuntu/Debian DEB Package
Скачайте deb-пакет с [страницы релизов](https://github.com/iamscottxu/obs-rtspserver/releases) и установите его.

```bash
wget -O obs-rtspserver-linux.deb https://github.com/iamscottxu/obs-rtspserver/releases/download/{version}/obs-rtspserver-{version}-linux.deb
apt install -y obs-rtspserver-linux.deb
```
* Замените {version} на последнюю версию релиза, например: v2.2.0

### Red-Hat RPM Package
Скачайте пакет rpm с [страницы релизов](https://github.com/iamscottxu/obs-rtspserver/releases) и установите его.

```bash
wget -O obs-rtspserver-linux.rpm https://github.com/iamscottxu/obs-rtspserver/releases/download/{version}/obs-rtspserver-{version}-linux.rpm
rpm -ivh obs-rtspserver-linux.rpm
```
* Замените {version} на последнюю версию релиза, например: v2.2.0

### ArchLinux AUR Package
obs-rtspserver также доступен в виде [AUR Package](https://aur.archlinux.org/packages/?O=0&K=obs-rtspserver)
Если вы используете [yay](https://github.com/Jguer/yay), просто выполните следующую команду для его установки:

```bash
yay -S obs-rtspserver
```

### Другое
Скачайте архив tar.gz с [страницы релизов](https://github.com/iamscottxu/obs-rtspserver/releases) и распакуйте его в корневой каталог "/".

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
* Замените {version} на последнюю версию релиза, например: v2.2.0


# Создать
* Установите cmake, visual studio (только для Windows) и qt.
* Скачайте и настройте исходный код obs-studio.
* Скопируйте исходный код в (исходный код obs-studio)/plugins/obs-rtspserver/
* Добавьте `add_subdirectory(obs-rtspserver)` в (исходный код obs-studio)/plugins/CMakeLists.txt
* Соберите obs-rtspserver.

# ЧАВО
* [Не удается найти плагин в меню](https://github.com/iamscottxu/obs-rtspserver/wiki/FAQ#cant-find-the-plugin-in-the-menu)

# Лицензия
* [RtspServer](https://github.com/PHZ76/RtspServer/) - [MIT License](https://github.com/PHZ76/RtspServer/blob/master/LICENSE)
* [Qt5](https://www.qt.io/) - [GPL version 2](https://doc.qt.io/qt-5/licensing.html)
* [libb64](https://sourceforge.net/projects/libb64/) - [Public domain dedication](https://sourceforge.net/p/libb64/git/ci/master/tree/LICENSE)
