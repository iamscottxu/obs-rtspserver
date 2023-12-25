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

# OBS RTSPサーバー

RTSPストリームにエンコードして公開するためのOBS Studioのプラグインです。

**サポートされるプラットフォーム**: Windows 10、Windows 11、LinuxおよびmacOS

**サポートされるOBS Studioのバージョン**: 30.0.0以降

[![Packaging status](https://repology.org/badge/vertical-allrepos/obs-rtspserver.svg)](https://repology.org/project/obs-rtspserver/versions)

# インストール
## Windows
インストーラーは[Release Page](https://github.com/iamscottxu/obs-rtspserver/releases)にあります。

圧縮ファイルを使用して手動インストールする場合は、圧縮ファイル (例: obs-rtspserver-v2.2.1-windows.zip)を解凍して、obs-studioインストールフォルダーに配置します。

### wingetパッケージ
OSのバージョンがWindows 10 1709以降で、[app-installer](https://www.microsoft.com/store/productId/9NBLGGH4NNS1)がインストールされている場合は、下記のようにコマンドを実行してインストールします。

```powershell
winget install iamscottxu.obs-rtspserver
```

## macOS
macOSを使用している場合、.pkgインストーラーを使用してインストールできます。インストーラーは[Release Page](https://github.com/iamscottxu/obs-rtspserver/releases)にあります。

## Linux (x64のみ)
### Ubuntu/Debian DEBパッケージ
[Release Page](https://github.com/iamscottxu/obs-rtspserver/releases)からdebパッケージをダウンロードしてインストールします。

```bash
wget -O obs-rtspserver-linux.deb https://github.com/iamscottxu/obs-rtspserver/releases/download/{version}/obs-rtspserver-{version}-linux.deb
apt install -y obs-rtspserver-linux.deb
```
* {version}は最新のリリースバージョン (例: v2.2.1)に置換してください

### Red-Hat RPMパッケージ
[Release Page](https://github.com/iamscottxu/obs-rtspserver/releases)からrpmパッケージをダウンロードしてインストールします。

```bash
wget -O obs-rtspserver-linux.rpm https://github.com/iamscottxu/obs-rtspserver/releases/download/{version}/obs-rtspserver-{version}-linux.rpm
rpm -ivh obs-rtspserver-linux.rpm
```
* {version}は最新のリリースバージョン (例: v2.2.1)に置換してください

### ArchLinux AURパッケージ
obs-rtspserverは[AUR Package](https://aur.archlinux.org/packages/?O=0&K=obs-rtspserver)にあります。
[yay](https://github.com/Jguer/yay)を使用する場合は、下記のようにコマンドを実行してインストールします。

```bash
yay -S obs-rtspserver
```

### その他
[Release Page](https://github.com/iamscottxu/obs-rtspserver/releases)からtar.gzアーカイブをダウンロードし、「/」に解凍します。

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
* {version}は最新のリリースバージョン (例: v2.2.1)に置換してください


# ビルド
* CMake、Visual Studio (Windowsのみ)およびQtをインストールします
* obs-studioのソースコードをダウンロードして設定します
* ソースコードを(obs-studio source code)/plugins/obs-rtspserver/にコピーします
* `add_subdirectory(obs-rtspserver)`を(obs-studio source code)/plugins/CMakeLists.txtに追加します
* obs-rtspserverをビルドします

# FAQ
* [Can't find the plugin in the menu](https://github.com/iamscottxu/obs-rtspserver/wiki/FAQ#cant-find-the-plugin-in-the-menu)

# License
* [RtspServer](https://github.com/PHZ76/RtspServer/) - [MIT License](https://github.com/PHZ76/RtspServer/blob/master/LICENSE)
* [Qt5](https://www.qt.io/) - [GPL version 2](https://doc.qt.io/qt-5/licensing.html)
* [libb64](https://sourceforge.net/projects/libb64/) - [Public domain dedication](https://sourceforge.net/p/libb64/git/ci/master/tree/LICENSE)
