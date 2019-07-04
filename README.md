# Locally Stored Password Manager
This is an offline password manager built in C++ using Qt. This is one of the first projects I've ever made with Qt, and thus, there are some design flaws at the core of the program that if resolved would result in much cleaner, and structured code. The code in this project might change drastically at a certain point, as more time goes on, and I become more familiar with Qt.

### Privacy
Why make this password manager when there are services such as LastPass, DashLane, 1Password, etc? Quite frankly: privacy. The primary point of this project is that nothing is cloud based, everything is stored locally. There isn't a server that holds all of your credentials hostage, nor will there ever be. This is a purely offline password manager that stores all of your credentials into AES encrypted `.vlt` vault files. You can make as many of these files as you want, and do whatever you want with them knowing that the only copy of that file that exists is on your computer, and it's encrypted. 

### Arbitrary Key/Value
Rather than opting for a `username:password` system, where every account has a fixed amount of fields, I made it variable. The lack of this option in other password managers seriously bothered me for a long time. Sometimes, it's not as simple as `username:password`, sometimes you have to store an email, or security questions, or perhaps some kind of recovery code, or even multiple usernames/passwords for alternate accounts, etc. So I made it so that you decide the amount of keys, and their values. You can add any key you like, and assign it to any value you like. 

### Theming 
Since this is Qt, and Qt has the QSS style format, implementing basic theming support was pretty simple. You can apply any Qt stylesheet you want to this password manager, simply place your sytlesheet in the same directory as the binary, and name it style.qss, it will be automatically applied.

### Integrated Password Generator
Like most password managers, this includes a password generator directly built in, two actually. There's a password generator dialog ripped from my [other project](https://github.com/Shayna-x02/Password-Generator) as well as a password manager built into the entry context menu, meaning you can quickly generate passwords on the fly, or go through the dialg, it's up to you.

### Building
If you plan on building this project yourself, you'll need a Qt5/QMake environment, and a copy of OpenSSL. The project file is included. If you're on a platform other than Windows, or if your OpenSSL installation varies from the one I have configured in the project file, make sure to edit it so that it accurately points to your OpenSSL include directory, and library directory, and make file name corrections if necessary. OpenSSL Is the only non-standard dependancy, the rest is cross platform. 

If you're struggling with manually compiling OpenSSL, you can find a [pre-built binary installer for windows here](https://slproweb.com/products/Win32OpenSSL.html)


### Screenshots
_These screenshots were taken with the [QtDark](https://github.com/EClaesson/QTDark) QSS stylesheet applied._

**Main Dialogue**

![](screenshots/Image1.png?raw=true)

**Account Creation Dialogue**

![](screenshots/Image2.png?raw=true)

**Account Loading Dialogue**

![](screenshots/Image3.png?raw=true)

**Password Generator Dialogue** 

![](screenshots/Image4.png?raw=true)
