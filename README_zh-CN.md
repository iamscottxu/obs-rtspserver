![CI Release](https://github.com/iamscottxu/obs-rtspserver/workflows/CI%20Release/badge.svg)

# OBS-RTSPServer

这是一个 OBS Studio 插件，此插件可对输出进行编码并发布 RTSP 流。

**支持的平台** : Windows 7 、 Windows 8 、 Windows 10、 Linux 和 macOS

**支持的 OBS Studio 版本** : 24.0.0+

[![Packaging status](https://repology.org/badge/vertical-allrepos/obs-rtspserver.svg)](https://repology.org/project/obs-rtspserver/versions)

# 安装
## Windows
可以使用安装程序进行安装，安装程序可以在[发布页面](https://github.com/iamscottxu/obs-rtspserver/releases)中找到。

如果要使用压缩文件手动安装，可以解压缩压缩文件（例如： obs-rtspserver-v2.0.1-windows.zip）把它放到你的 OBS Studio 安装文件夹里。

### winget 软件包
如果你使用Windows 10 1709和以后的版本，且已经安装了[app-installer](https://www.microsoft.com/store/productId/9NBLGGH4NNS1)后，可以运行以下命令进行安装：

```powershell
winget install iamscottxu.obs-rtspserver
```

## macOS
如果你使用的是 macOS 操作系统，您可以使用安装程序进行安装，安装程序 .pkg 可以在发布页面中找到。

## Linux (仅 x64)
* 从[发布页面](https://github.com/iamscottxu/obs-rtspserver/releases)下载最新发布版本。
* mkdir -p $HOME/.config/obs-studio/plugins
* 解压压缩包。例如：tar -xzvf obs-rtspserver-v2.0.1-linux.tar.gz -C $HOME/.config/obs-studio/plugins/

### ArchLinux AUR 软件包
obs-rtspserver也可以作为[AUR软件包](https://aur.archlinux.org/packages/?O=0&K=obs-rtspserver)提供。如果您使用[yay](https://github.com/Jguer/yay)，请运行以下命令进行安装：

```shell
yay -S obs-rtspserver
```

# 生成
* 安装 cmake 、 visual studio （仅 Windows ）和 qt ；
* 下载并配置 OBS Studio 的源代码；
* 将源代码复制到 (OBS Studio 源代码目录)/plugins/obs-rtspserver/ 中；
* 添加 `add_subdirectory(obs-rtspserver)` 到 (OBS Studio 源代码目录)/plugins/CMakeLists.txt 中；
* 生成 obs-rtspserver 。

# 我在菜单中找不到插件
请参阅 [https://github.com/iamscottxu/obs-rtspserver/issues/1](https://github.com/iamscottxu/obs-rtspserver/issues/1) 。

请下载 VC++ 运行时并安装。[https://www.microsoft.com/zh-CN/download/details.aspx?id=48145](https://www.microsoft.com/zh-CN/download/details.aspx?id=48145)

# 许可信息
* [RtspServer](https://github.com/PHZ76/RtspServer/) - [MIT License](https://github.com/PHZ76/RtspServer/blob/master/LICENSE)
* [Qt5](https://www.qt.io/) - [GPL version 2](https://doc.qt.io/qt-5/licensing.html)
* [libb64](https://sourceforge.net/projects/libb64/) - [Public domain dedication](https://sourceforge.net/p/libb64/git/ci/master/tree/LICENSE)
