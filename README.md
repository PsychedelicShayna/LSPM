# Locally Stored Password Manager
This is an open source, cross-platform, offline password manager built in C++ using Qt. This is one of the first projects I've ever made with Qt, and consequently the code has gone through several major revisions as I continued to familiarize myself. As of now, it is stable, and reliable enough for day to day use. Initially, I developed this project with nothing but Windows/MSVC in mind, but the code itself is free from platform specific code, and I have successfully compiled and used it on Linux. The only requirements are Qt, and OpenSSL, both of which are cross platform. 

### Privacy
Why make this password manager when there are services such as LastPass, DashLane, 1Password, etc? Quite frankly: privacy. The primary point of this project is that nothing is cloud based, everything is stored locally. There isn't a server that holds all of your credentials hostage, nor will there ever be. This is a purely offline password manager that stores all of your credentials into AES encrypted `.vlt` vault files. You can make as many of these files as you want, and do whatever you want with them knowing that the only copy of that file that exists is on your computer, and it's encrypted. 

### Arbitrary Key/Value
Rather than opting for a `username:password` system, where every account has a fixed amount of fields, I made it variable. The lack of this option in other password managers seriously bothered me for a long time. Sometimes, it's not as simple as `username:password`, sometimes you have to store an email, or security questions, or perhaps some kind of recovery code, or even multiple usernames/passwords for alternate accounts, etc. So I made it so that you decide the amount of keys, and their values. You can add any key you like, and assign it to any value you like. 

### Theming 
Since this is Qt, and Qt has the QSS style format, implementing basic theming support was pretty simple. You can apply any Qt stylesheet you want to this password manager, simply place your sytlesheet in the same directory as the binary, and name it style.qss, it will be automatically applied.

### Integrated Password Generator
Like most password managers, this includes a password generator directly built in, two actually. There's a password generator dialog ripped from my [other project](https://github.com/Shayna-x02/Password-Generator) as well as a password manager built into the entry context menu, meaning you can quickly generate passwords on the fly, or go through the dialg, it's up to you.

### Config.json
I've recently added support for an optional configuration file, which if present in the binary's directory, will be loaded and parsed. You can use it to specify the path of a default vault file, which the program will attempt to load on startup. You can also use it to specify the path of a stylesheet in QSS format, in order to apply QSS themes, or modify the aesthetics any way you'd like.

In order to use it, create a file named `config.json` in the binary's directory, and use the following format.
```json
{
  "qstylesheet": "styles/black.qss",
  "vault": "vaults/primary_vault.vlt"
}
```

### Building
If you plan on building this project yourselff -- you'll need a Qt5/QMake environment, a copy of OpenSSL's libcrypto static library built for your platform, and a copy of the standard OpenSSL headers. A QMake project file is included, however the OpenSSL include, and library link paths are assumed, so you may need to modify it according to where OpenSSL is stored on your system.

If you're struggling with manually compiling OpenSSL, you can find a [pre-built binary installer for windows here](https://slproweb.com/products/Win32OpenSSL.html)

You can also download [Qt, and Qt creator here](https://www.qt.io/download-open-source) in order to open and compile the project. _Note: This project hasn't been tested, and probably won't work with Qt<5._

### Screenshots
_These screenshots were taken with the [QtDark](https://github.com/EClaesson/QTDark) QSS stylesheet applied._

**Account Creation**

![](screenshots/1.png?raw=true)

**Entry Context Menu**

![](screenshots/2.png?raw=true)

**Password Generator Dialog**

![](screenshots/3.png?raw=true)
