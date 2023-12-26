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

<font size="5">[幫助翻譯 obs-rtspserver!](https://www.transifex.com/scott-xu/obs-rtspserver)</font>

# OBS-RTSPServer

這是一個 OBS Studio 插件,此外掛程式可對輸出進行編碼並發布 RTSP 流。

**支援的平台** : Windows 10、Windows 11、Linux和macOS

**支援的 OBS Studio 版本**：30.0.0+

[![Packaging status](https://repology.org/badge/vertical-allrepos/obs-rtspserver.svg)](https://repology.org/project/obs-rtspserver/versions)

# 安裝
## Windows
可以使用安裝程式進行安裝，安裝程式可以在[發佈頁面](https://github.com/iamscottxu/obs-rtspserver/releases)中找到。

如果要使用壓縮檔案手動安裝，可以解壓縮壓縮檔案（例如：obs-rtspserver-v2.0.5-windows.zip）把它放到你的 OBS Studio 安裝資料夾裡。

### winget 軟體包
如果你使用 Windows 10 1709 和以後的版本，且已經安裝了[app-installer](https://www.microsoft.com/store/productId/9NBLGGH4NNS1)後，可以執行下列指令安裝：

```powershell
winget install iamscottxu.obs-rtspserver
```

## macOS
如果你使用的是 macOS 作業系統，您可以使用安裝程式進行安裝，安裝程式 .pkg 可以在[發佈頁面](https://github.com/iamscottxu/obs-rtspserver/releases)中找到。

## Linux（僅 x64）
### Ubuntu/Debian DEB 軟體包
在[發佈頁面](https://github.com/iamscottxu/obs-rtspserver/releases)下載 deb 軟體包並安裝。

```bash
wget -O obs-rtspserver-linux.deb https://github.com/iamscottxu/obs-rtspserver/releases/download/{version}/obs-rtspserver-{version}-linux.deb
apt install -y obs-rtspserver-linux.deb
```
* 将 {version} 替换成最新发布版本号，例如：v2.2.0

### Red-Hat RPM 軟體包
在[發佈頁面](https://github.com/iamscottxu/obs-rtspserver/releases)下載 rpm 軟體包並安裝。

```bash
wget -O obs-rtspserver-linux.rpm https://github.com/iamscottxu/obs-rtspserver/releases/download/{version}/obs-rtspserver-{version}-linux.rpm
rpm -ivh obs-rtspserver-linux.rpm
```
* 將 {version} 替換成最新發布版本號，例如：v2.2.0

### ArchLinux AUR 軟體包
obs-rtspserver也可以作為[AUR軟體包](https://aur.archlinux.org/packages/?O=0&K=obs-rtspserver)提供。 如果您使用[yay](https://github.com/Jguer/yay)，請執行下列命令進行安裝：

```bash
yay -S obs-rtspserver
```

### 其他
在[發佈頁面](https://github.com/iamscottxu/obs-rtspserver/releases)下載 tar.gz 壓縮套件並解壓縮到 "/"。

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
* 將 {version} 替換成最新發布版本號，例如：v2.2.0


# 產生
* 安裝 cmake 、 visual studio (僅 Windows )和 qt；
* 下載並配置 OBS Studio 的源代碼；
* 將原始碼複製到 (OBS Studio 原始碼目錄)/plugins/obs-rtspserver/ 中；
* 加入 `add_subdirectory(obs-rtspserver)` 到 (OBS Studio 原始碼目錄)/plugins/CMakeLists.txt 中；
* 產生 obs-rtspserver 。

# 常見問題
* [在選單中找不到外掛程式](https://github.com/iamscottxu/obs-rtspserver/wiki/%E5%B8%B8%E8%A7%81%E9%97%AE%E9%A2%98#%E5%9C%A8%E8%8F%9C%E5%8D%95%E4%B8%AD%E6%89%BE%E4%B8%8D%E5%88%B0%E6%8F%92%E4%BB%B6)

# 許可資訊
* [RtspServer](https://github.com/PHZ76/RtspServer/) - [MIT License](https://github.com/PHZ76/RtspServer/blob/master/LICENSE)
* [Qt5](https://www.qt.io/) - [GPL version 2](https://doc.qt.io/qt-5/licensing.html)
* [libb64](https://sourceforge.net/projects/libb64/) - [Public domain dedication](https://sourceforge.net/p/libb64/git/ci/master/tree/LICENSE)
