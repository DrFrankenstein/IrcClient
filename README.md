# Generic IRC Client
Windows: [![Build status](https://ci.appveyor.com/api/projects/status/e60tep2i17k220ii?svg=true)](https://ci.appveyor.com/project/DrFrankenstein/ircclient)
Linux: [![Build Status](https://travis-ci.org/DrFrankenstein/IrcClient.svg)](https://travis-ci.org/DrFrankenstein/IrcClient)

---

This is a simple (currently work-in-progress) IRC client that I (@DrFrankenstein) am making for myself. I figured it would be more interesting than loading a bunch of scripts into whichever client I'm using to add the features that I want. It will, in turn, be scriptable using JavaScript. Eventually.

It has no name beyond "Generic IRC Client" (you can abbreviate as "Generic") and no target audience beyond people who use IRC on a GUI system.

At this point, the app is more a construction site than something usable. I apologize for the mess. It correctly connects to an IRC server, parses messages and responds to PING and some stuff. You can interact with the server manually but the chat UI is not complete yet.

## Build
### Dependencies

This application requires (version numbers are based on docs, let me know if these don't work):
* [Qt](http://www.qt.io), version 5.4 or newer
* [Boost](http://www.boost.org), version 1.43 (probably, based on the docs) or newer.
* A C++ toolchain with decent C++11 support.
  * CL version 18 or newer (Visual C++ 2013; it doesn't seem to ship with the latest Windows SDK, but [Visual Studio Community](https://www.visualstudio.com/products/visual-studio-community-vs.aspx) is free), or
  * GCC 4.6 or newer, or
  * clang 3.1 or newer, or
  * any other recent compiler.

Optionally:
* [git](https://git-scm.com), to easily download and update this repository.

### Downloading
If you use git, run the following command:
```
git clone https://github.com/DrFrankenstein/IrcClient.git
```
Otherwise, use the "Download ZIP" button on [this repository's page](https://github.com/DrFrankenstein/IrcClient).

### Building

Run `IrcClient.pro` through `qmake`, then build using your favourite toolchain. Detailed steps for each Windows and Linux follow.

#### Windows
##### Visual Studio / Visual C++ Build Tools
After installing Qt and adding its `bin` directory to your `PATH` environment variable, run the following in the Visual Studio Command Prompt:
```bat
qmake IrcClient.pro "INCLUDEPATH+=X:\path_to_boost"
nmake
```
Alternatively, you can use `qmake` to generate solution/project files to use with MsBuild or Visual Studio.

##### MinGW / Cygwin
After installing Qt and adding its `bin` directory to your shell's `PATH` variable, run the following in your `bash` or other shell:
```bash
qmake IrcClient.pro "INCLUDEPATH+=X:\path_to_boost"
make
```

#### Linux
After installing your Qt and Boost dev packages, run the following:
```bash
qmake IrcClient.pro
make
```
