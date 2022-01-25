# OBS-RTSPServer

이 플러그인은 obs-studio에서 RTSP스트림을 인코딩 및 배포하는 플러그인입니다.

**지원하는 운영체제** : Windows 7 , Windows 8 , Windows 10, Linux and macOS

**지원하는 OBS 버전** : 24.0.0+

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
* [Release Page](https://github.com/iamscottxu/obs-rtspserver/releases)에서 최신 릴리즈를 다운로드
* mkdir -p $HOME/.config/obs-studio/plugins
* tar 압축해제, 예: tar -xzvf obs-rtspserver-v2.0.5-linux.tar.gz -C $HOME/.config/obs-studio/plugins/

### ArchLinux AUR 패키지
obs-rtspserver는 [AUR Package](https://aur.archlinux.org/packages/?O=0&K=obs-rtspserver)로도 설치할 수 있습니다
[yay](https://github.com/Jguer/yay)를 사용 중인 경우 아래 명령어를 실행하여 설치할 수 있습니다:

```shell
yay -S obs-rtspserver
```

# 빌드
* cmake, visual studio(Windows의 경우), QT를 설치합니다.
* obs-studio 소스코드를 다운로드 받고 구성합니다.
* 소스코드를 다음 폴더에 복사합니다 : (obs-studio 소스코드)/plugins/obs-rtspserver/
* `add_subdirectory(obs-rtspserver)` 를 (obs-studio source code)/plugins/CMakeLists.txt에 추가합니다.
* obs-rtspserver를 빌드합니다.

# FAQ
* [메뉴에서 플러그인을 찾을 수 없는 경우](https://github.com/iamscottxu/obs-rtspserver/wiki/FAQ#cant-find-the-plugin-in-the-menu)

# 라이센스
* [RtspServer](https://github.com/PHZ76/RtspServer/) - [MIT License](https://github.com/PHZ76/RtspServer/blob/master/LICENSE)
* [Qt5](https://www.qt.io/) - [GPL version 2](https://doc.qt.io/qt-5/licensing.html)
* [libb64](https://sourceforge.net/projects/libb64/) - [Public domain dedication](https://sourceforge.net/p/libb64/git/ci/master/tree/LICENSE)
