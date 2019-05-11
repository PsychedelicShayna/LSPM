# Description
(U)niveral (E)ncrypted (P)assword (M)anager is an open source password manager built in Qt, and uses OpenSSL's AES-CBC encryption.

There are many password managers avalible, however I don't really trust any of them. Passwords always get stored on some server somewhere, and how their encryption works is entirely unknown. They could just store encryption keys, who knows. Anyways, a while back I created a very poor password manager in C#, which I never released publicly, then I wrote a sequel to it, also, never releasing it publicly. This is the third iteration, written in C++ and Qt instead of C#/Winforms. The code itself is quite chaotic as of the first commit, and a lot needs to be re-structured over time, however it's in a working state. Some bugs exist here and there, but it's a fully functional password manager. 

This project uses Qt, OpenSSL, and the Nlohmann Json library. The latter being included, the former two however aren't. If you plan on compiling this by yourself, you'll need a Qt environment, and the OpenSSL library installed. I deleted the QMake file early on (check history if you really want to see it) as it's largely inapplicable, and is too contextual to my local environment for people to be able to use.

Rather than using a server (which is the primary thing I'm trying to avoid) this stores everything locally in "UEPM" files, which are just regular old JSON files, but with optional AES-CBC 128, 192, or 256 encryption on top. 

It also contains an integrated password generator, and the support for theming (any old QSS file will do). 

Pre-built binaries will be released shortly. 
