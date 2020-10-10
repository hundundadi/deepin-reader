# deepin_reader

Deepin Document Viewer is a simple PDF reader, supporting bookmarks, highlights and annotations.

## Dependencies

In debian, use below command to install compile dependencies:

`sudo apt install debhelper (>= 11),pkg-config, libspectre-dev, libdjvulibre-dev, qt5-qmake, qt5-default,libtiff-dev, libkf5archive-dev, libdtkwidget-dev,qttools5-dev-tools,qtbase5-private-dev,libjpeg-dev`

## Install

```sh
git clone <url>
cd deepin-reader
git submodule update
qmake
make
sudo make install
```

## Getting help

Any usage issues can ask for help via

* [Gitter](https://gitter.im/orgs/linuxdeepin/rooms)
* [IRC channel](https://webchat.freenode.net/?channels=deepin)
* [Forum](https://bbs.deepin.org)
* [WiKi](http://wiki.deepin.org/)

## Getting involved

We encourage you to report issues and contribute changes

* [Contribution guide for developers](https://github.com/linuxdeepin/developer-center/wiki/Contribution-Guidelines-for-Developers-en). (English)
* [开发者代码贡献指南](https://github.com/linuxdeepin/developer-center/wiki/Contribution-Guidelines-for-Developers) (中文)

## License

Deepin Document Viewer is licensed under [GPLv3](LICENSE).
