# DDE Polkit Agent

DDE Polkit Agent is the [polkit](https://www.freedesktop.org/software/polkit/docs/latest/polkit.8.html) agent used in Deepin Desktop Environment.

## Dependencies

### Build dependencies

* qmake (>= 5.3)
* libpolkit-qt5
* [dtk](https://github.com/linuxdeepin/libdtk)

### Runtime dependencies

* polkit
* Qt5 (>= 5.3)
  * Qt5-Gui
  * Qt5-Widgets
  * Qt5-DBus
  * Qt5-X11extras

## Installation

### Build from source code

1. Make sure you have installed all dependencies.

2. Build:
```
$ cd dde-polkit-agent
$ mkdir build
$ cd build
$ qmake ..
$ make
```

3. Install:
```
$ sudo make install
```

## Usage

DDE Polkit Agent works as an standard polkit agent, see https://www.freedesktop.org/software/polkit/docs/latest/polkit.8.html for 
more about how to use polkit.

## Getting help

You may also find these channels useful if you encounter any other issues:

* [Gitter](https://gitter.im/orgs/linuxdeepin/rooms)
* [IRC Channel](https://webchat.freenode.net/?channels=deepin)
* [Official Forum](https://bbs.deepin.org/)
* [Wiki](http://wiki.deepin.org/)

## License

DDE Polkit Agent is licensed under [GPLv3](https://github.com/linuxdeepin/developer-center/wiki/LICENSE).
