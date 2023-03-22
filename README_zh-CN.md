# OBS-RTSPServer

这是一个 OBS Studio 插件，此插件可对输出进行编码并发布 RTSP 流。

**支持的平台** : Windows 10、Windows 11、Linux和macOS

**支持的 OBS Studio 版本** : 28.0.0+

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
### Ubuntu/Debian DEB 软件包
在[发布页面](https://github.com/iamscottxu/obs-rtspserver/releases)下载 deb 软件包并安装。

```bash
wget -O obs-rtspserver-linux.deb https://github.com/iamscottxu/obs-rtspserver/releases/download/{version}/obs-rtspserver-{version}-linux.deb
apt install -y obs-rtspserver-linux.deb
```
* 将 {version} 替换成最新发布版本号，例如：v2.2.0

### Red-Hat RPM 软件包
在[发布页面](https://github.com/iamscottxu/obs-rtspserver/releases)下载 rpm 软件包并安装。

```bash
wget -O obs-rtspserver-linux.rpm https://github.com/iamscottxu/obs-rtspserver/releases/download/{version}/obs-rtspserver-{version}-linux.rpm
rpm -ivh obs-rtspserver-linux.rpm
```
* 将 {version} 替换成最新发布版本号，例如：v2.2.0

### ArchLinux AUR 软件包
obs-rtspserver也可以作为[AUR软件包](https://aur.archlinux.org/packages/?O=0&K=obs-rtspserver)提供。如果您使用[yay](https://github.com/Jguer/yay)，请运行以下命令进行安装：

```bash
yay -S obs-rtspserver
```

### Other
在[发布页面](https://github.com/iamscottxu/obs-rtspserver/releases)下载 tar.gz 压缩包并解压到 "/"。

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
* 将 {version} 替换成最新发布版本号，例如：v2.2.0


# 生成
* 安装 cmake 、 visual studio （仅 Windows ）和 qt ；
* 下载并配置 OBS Studio 的源代码；
* 将源代码复制到 (OBS Studio 源代码目录)/plugins/obs-rtspserver/ 中；
* 添加 `add_subdirectory(obs-rtspserver)` 到 (OBS Studio 源代码目录)/plugins/CMakeLists.txt 中；
* 生成 obs-rtspserver 。

# 常见问题
* [在菜单中找不到插件](https://github.com/iamscottxu/obs-rtspserver/wiki/%E5%B8%B8%E8%A7%81%E9%97%AE%E9%A2%98#%E5%9C%A8%E8%8F%9C%E5%8D%95%E4%B8%AD%E6%89%BE%E4%B8%8D%E5%88%B0%E6%8F%92%E4%BB%B6)

# 许可信息
* [RtspServer](https://github.com/PHZ76/RtspServer/) - [MIT License](https://github.com/PHZ76/RtspServer/blob/master/LICENSE)
* [Qt5](https://www.qt.io/) - [GPL version 2](https://doc.qt.io/qt-5/licensing.html)
* [libb64](https://sourceforge.net/projects/libb64/) - [Public domain dedication](https://sourceforge.net/p/libb64/git/ci/master/tree/LICENSE)
