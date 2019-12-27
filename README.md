English | [简体中文](#) | [हिन्दी](#)

<div align="center">
	<img src="icon.png" width="125" height="125">
	<h1>airtrash</h1>
	<p>
		<b>Clone of Apple's AirDrop - easy P2P file transfer powered by stupidity</b>
	</p>
	<br>
</div>

[![GitHub release](https://img.shields.io/github/release/maciejczyzewski/airtrash.svg)](https://github.com/maciejczyzewski/airtrash/releases) [![Total Downloads](https://img.shields.io/github/downloads/maciejczyzewski/airtrash/total.svg)](https://github.com/maciejczyzewski/airtrash/releases)

## 🎯 Goal/Tutorial

> 100 tiny steps to build cross-platform desktop application using Electron/Node.js/C++

It's simple tutorial/guide for **absolute beginners** to present some tips for
creating desktop application. Unlike [@electron/electron-quick-start](https://github.com/electron/electron-quick-start), which presents the typical `hello world`.
This project aims to **focus on real-live scenario**, where we will try to implement
a complete product (**like cross-platform _Apple's AirDrop_ replacement**).

## 🔦 Screenshot

<img src="screen.gif" width="800">

## 💽 Installation

Download from [GitHub Releases](https://github.com/maciejczyzewski/airtrash/releases) and install it.

### from source

To clone and run this repository you'll need [Git](https://git-scm.com) and [Node.js](https://nodejs.org/en/download/) (and [yarn](https://github.com/yarnpkg/yarn)) installed on your computer. From your command line:

```bash
# clone this repository
git clone https://github.com/maciejczyzewski/airtrash
# go into the repository
cd airtrash
# install dependencies
yarn
# run the app
yarn start
```

Note: If you're using Linux Bash for Windows, [see this guide](https://www.howtogeek.com/261575/how-to-run-graphical-linux-desktop-applications-from-windows-10s-bash-shell/) or use `node` from the command prompt.

### macOS

The macOS users can install _airtrash_ using `brew cask`.

```bash
brew update && brew cask install airtrash
```

(nice try, you can't)

## 📃 Steps

Let's begin our journey.

### 1: starting from template

Clone and run for a quick way to see Electron in action. From your command line:

> `yarn` is strongly recommended instead of `npm`.

```bash
# clone this repository
$ git clone https://github.com/electron/electron-quick-start
# go into the repository
$ cd electron-quick-start
# install dependencies
$ yarn
# run the app
$ yarn start
```

You should see:

![](screen-1.png)

And have this file structure:

```bash
.
├── LICENSE.md        # - no one's bothered
├── README.md         # - sometimes good to read
├── index.html        # body: what you see
├── main.js           # heart: electron window
├── package-lock.json # - auto-generated
├── package.json      # configuration/package manager
├── preload.js        # soul: application behavior
└── renderer.js       # - do after rendering

0 directories, 8 files
```

### 2: using [@electron-userland/electron-builder](https://github.com/electron-userland/electron-builder) for packing things

bla bla bla/setting package.json

### 3: adding [@twbs/bootstrap](https://github.com/twbs/bootstrap) to project

how to install via yarn/how to include/settings nodeActived/removing headers

### 4: customizing window/interface

adding icon/buttons inside/drag for macos?

### 5: adding native extension ([@nodejs/nan](https://github.com/nodejs/nan) C++ library)

bla bla bla/binding gyp/src/some methods

### 6: registering methods for native module

bla bla bla/register function and importing from node.js

### 7: we don't have threads, how to handle this

why there are pull.js/push.js/article link

### 8: creating basic interface in `preload.js`

why we don't need frameworks (overkill)/very simple

### 9: idea behind simple P2P (naive but should work)

port serving (my dravings)/push/pull/explore

### 10: pasting from stackoverflow (how to get ip address)

bla bla bla

### 11: TCP/socket buffer size, does it matters?

why we need to iterate trough send/recv

```diff
// babel.config.js
module.exports = {
  presets: [
    ['@babel/preset-env', {targets: {node: 'current'}}],
+    '@babel/preset-typescript',
  ],
};
```

## 🤝 Contribute [![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg?style=flat)](http://makeapullrequest.com)

If you are interested in participating in joint development, PR and Forks are welcome!

## 📜 License

[MIT](LICENSE.md) Copyright (c) Maciej A. Czyzewski
