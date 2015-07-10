# Generic IRC Client
[![Build Status](https://travis-ci.org/DrFrankenstein/IrcClient.svg)](https://travis-ci.org/DrFrankenstein/IrcClient) (#5)

---

This is a simple (currently work-in-progress) IRC client that I (@DrFrankenstein) am making for myself. I figured it would be more interesting than loading a bunch of scripts into whichever client I'm using to add the features that I want. It will, in turn, be scriptable using JavaScript. Eventually.

It has no name beyond "Generic IRC Client" (you can abbreviate as "Generic") and no target audience beyond people who use IRC on a GUI system.

At this point, the app is more a construction site than something usable. I apologize for the mess. It correctly connects to an IRC server, parses messages and responds to PING and some stuff. You can interact with the server manually but the chat UI is not complete yet.

## Building

Make sure the latest Qt 5 is installed on top of your favourite C++ toolchain, then run IrcClient.pro through `qmake` and build. You'll need a compiler that supports C++11.

A basic, typical build on Linux (or Windows with MinGW or Cygwin) would be something like:

```bash
git clone https://github.com/DrFrankenstein/IrcClient.git
qmake IrcClient.pro
make
```

On Windows with Visual Studio (you need to have git installed and in your `PATH`):

```bat
git clone https://github.com/DrFrankenstein/IrcClient.git
qmake IrcClient.pro
nmake
```

You can also generate a solution to use with MsBuild and/or Visual Studio. Generic IRC Client is developed with Visual Studio 2013.
