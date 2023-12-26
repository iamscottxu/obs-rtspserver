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

<font size="5">[obs-rtspserver를 번역하는 데 도움을 주세요!](https://www.transifex.com/scott-xu/obs-rtspserver)</font>

# OBS-RTSPServer

이 플러그인은 obs-studio에서 RTSP스트림을 인코딩 및 배포하는 플러그인입니다.

**지원하는 운영체제** : Windows 10, Windows 11, Linux and macOS

**지원되는 OBS Studio 버전** : 30.0.0+

[![Packaging status](https://repology.org/badge/vertical-allrepos/obs-rtspserver.svg)](https://repology.org/project/obs-rtspserver/versions)

# 설치
## Windows
설치 파일은 [Release Page](https://github.com/iamscottxu/obs-rtspserver/releases)에서 찾을 수 있습니다.

직접 설치하려면 압축파일을 다운로드 받은 후 (예: obs-rtspserver-v2.0.5-windows.zip) obs-studio가 설치된 폴더에 압축을 푸십시오.

### winget 패키지
Windows 10 1709 이상 버전을 사용하고 있고 [app-installer](https://www.microsoft.com/store/productId/9NBLGGH4NNS1)가 설치되어 있다면 아래 명령어를 실행하여 바로 설치할 수 있습니다:

```powershell
winget install iamscottxu.obs-rtspserver
```

## macOS
macOS에서는 [Release Page](https://github.com/iamscottxu/obs-rtspserver/releases)에서 .pkg 설치파일을 찾아 설치할 수 있습니다.

## Linux (x64만 지원)
### 우분투/데비안 DEB 패키지
deb 패키지를 [릴리스 페이지](https://github.com/iamscottxu/obs-rtspserver/releases)에서 다운로드하여 설치하십시오.

```bash
wget -O obs-rtspserver-linux.deb https://github.com/iamscottxu/obs-rtspserver/releases/download/{version}/obs-rtspserver-{version}-linux.deb
apt install -y obs-rtspserver-linux.deb
```
* {version}를 최신 릴리스 버전으로 대체하십시오. 예: v2.2.0

### 레드햇 RPM 패키지
rpm 패키지를 [릴리스 페이지](https://github.com/iamscottxu/obs-rtspserver/releases)에서 다운로드하여 설치하십시오.

```bash
wget -O obs-rtspserver-linux.rpm https://github.com/iamscottxu/obs-rtspserver/releases/download/{version}/obs-rtspserver-{version}-linux.rpm
rpm -ivh obs-rtspserver-linux.rpm
```
* {version}를 최신 릴리스 버전으로 대체하십시오. 예: v2.2.0

### ArchLinux AUR 패키지
obs-rtspserver는 [AUR 패키지](https://aur.archlinux.org/packages/?O=0&K=obs-rtspserver)로도 사용할 수 있습니다.
만약 [yay](https://github.com/Jguer/yay)를 사용한다면 다음 명령을 실행하여 설치하세요.

```bash
yay -S obs-rtspserver
```

### 다른
"/"에서 [릴리스 페이지](https://github.com/iamscottxu/obs-rtspserver/releases)에서 tar.gz 아카이브를 다운로드하고 압축을 해제하세요.

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
* {version}를 최신 릴리스 버전으로 대체하십시오. 예: v2.2.0


# 빌드
* cmake, visual studio(only windows) 및 qt를 설치하십시오.
* 다운로드하고 obs-studio의 소스 코드를 구성하세요.
* 소스 코드를 (obs-studio 소스 코드)/plugins/obs-rtspserver/에 복사하세요.
* (obs-studio 소스 코드)/plugins/CMakeLists.txt에 `add_subdirectory(obs-rtspserver)`를 추가하세요.
* obs-rtspserver를 빌드하세요.

# 자주 묻는 질문
* [메뉴에서 플러그인을 찾을 수 없습니다](https://github.com/iamscottxu/obs-rtspserver/wiki/FAQ#cant-find-the-plugin-in-the-menu)

# 라이선스
* [RtspServer](https://github.com/PHZ76/RtspServer/) - [MIT License](https://github.com/PHZ76/RtspServer/blob/master/LICENSE)
* [Qt5](https://www.qt.io/) - [GPL version 2](https://doc.qt.io/qt-5/licensing.html)
* [libb64](https://sourceforge.net/projects/libb64/) - [Public domain dedication](https://sourceforge.net/p/libb64/git/ci/master/tree/LICENSE)
