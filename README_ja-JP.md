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

<font size="5">[obs-rtspserverを翻訳するのを助けてください！](https://www.transifex.com/scott-xu/obs-rtspserver)</font>

# OBS-RTSPServer

これはobs-studioのプラグインであり、RTSPストリームにエンコードして公開するものです。

**サポートされているプラットフォーム**：Windows 10、Windows 11、Linux、およびmacOS

**サポートされているOBS Studioのバージョン** : 30.0.0+

[![Packaging status](https://repology.org/badge/vertical-allrepos/obs-rtspserver.svg)](https://repology.org/project/obs-rtspserver/versions)

# インストール
## Windows
[Release Page](https://github.com/iamscottxu/obs-rtspserver/releases) でインストーラーを見つけることができます。

手動でインストールするために圧縮ファイルを使用したい場合は、それを解凍して（例：obs-rtspserver-v2.0.5-windows.zip）、obs-studioのインストールフォルダに配置してください。

### winget パッケージ
Windows 10 1709以降のオペレーティングシステムバージョンで、[app-installer](https://www.microsoft.com/store/productId/9NBLGGH4NNS1)がインストールされている場合は、次のコマンドを実行してインストールしてください。

```powershell
winget install iamscottxu.obs-rtspserver
```

## MacOS
.macOSを使用する場合、[リリースページ](https://github.com/iamscottxu/obs-rtspserver/releases)で.pkgインストーラを使用してインストールできます。

## Linux (Only x64)
### Ubuntu/Debian DEB パッケージ
debパッケージを[リリースページ](https://github.com/iamscottxu/obs-rtspserver/releases)からダウンロードしてインストールしてください。

```bash
wget -O obs-rtspserver-linux.deb https://github.com/iamscottxu/obs-rtspserver/releases/download/{version}/obs-rtspserver-{version}-linux.deb
apt install -y obs-rtspserver-linux.deb
```
* 「{version}」を最新のリリースバージョンで置き換えてください。例：v2.2.0

### Red-Hat RPM パッケージ
rpmパッケージを[リリースページ](https://github.com/iamscottxu/obs-rtspserver/releases)からダウンロードしてインストールしてください。

```bash
wget -O obs-rtspserver-linux.rpm https://github.com/iamscottxu/obs-rtspserver/releases/download/{version}/obs-rtspserver-{version}-linux.rpm
rpm -ivh obs-rtspserver-linux.rpm
```
* {version}を最新のリリースバージョンであるv2.2.0などで置き換えてください。

### ArchLinux AUR パッケージ
obs-rtspserverは[AURパッケージ](https://aur.archlinux.org/packages/?O=0&K=obs-rtspserver)としても利用可能です。
[yay](https://github.com/Jguer/yay)を使用している場合は、次のコマンドを実行してインストールしてください。

```bash
yay -S obs-rtspserver
```

### 他の
[リリースページ](https://github.com/iamscottxu/obs-rtspserver/releases)からtar.gzアーカイブをダウンロードし、"/"に展開してください。

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
* {text_to_translate}「{version}」を最新のリリースバージョンで置き換えてください。例：v2.2.0


# ビルド
* cmake、visual studio（Windowsのみ）およびqtをインストールしてください。
* ダウンロードして、obs-studioのソースコードを設定してください。
* ソースコードを(obs-studioソースコード)/plugins/obs-rtspserver/にコピーしてください。
* （obs-studioのソースコード）/plugins/CMakeLists.txtに`add_subdirectory(obs-rtspserver)`を追加してください。
* Build obs-rtspserver.

# FAQ
* [メニューでプラグインが見つかりません](https://github.com/iamscottxu/obs-rtspserver/wiki/FAQ#cant-find-the-plugin-in-the-menu)

# ライセンス
* [RtspServer](https://github.com/PHZ76/RtspServer/) - [MIT License](https://github.com/PHZ76/RtspServer/blob/master/LICENSE)
* [Qt5](https://www.qt.io/) - [GPL version 2](https://doc.qt.io/qt-5/licensing.html)
* [libb64](https://sourceforge.net/projects/libb64/) - [Public domain dedication](https://sourceforge.net/p/libb64/git/ci/master/tree/LICENSE)
