#VirtualAA2

VirtualAA2 is package-manager/mod installer for AA2. What sets it apart from existing mod installers is that mods can be activated/deactivated without modifying any files by using file system virtualization.

The project is ambitious and has the following goals:
- Mods may not modify game files, it should be easy and quick to add/remove mods, without any fear of possible braking anything
- The package manager should be intelligent enough to detect conflicts and understand dependencies, to avoid (or at least warn) semi-broken setups. (Ever tried adding a character, and not noticing you are missing some mod?)
- It should be able to fix bad design decisions in how the game stores its files, and reduce the installation size significantly

##File system virtualization
VirtualAA2 is based on file system virtualization. This makes it possible to create a folder (or partition) which does not take up space on the disk drive, but looks and behaves like a normal folder. The contents are generated when accessed as needed and streamed directly to the application. VirtualAA2 creates a virtual folder containing the entire AA2 installation and can this way add and remove mods dynamically without changing any data on the disk.

######Why do this?
AA2 uses `.pp` archive files which contains many smaller files. Mods which added or modified files in these archives requires you to modify the entire archive. This can be slow, but most importantly it pollutes the original game files with no easy way to tell which are the originals, which are mods, and what have been overwritten.
Furthermore, it opens up for a lot of possibilities not possible otherwise without modifying the game code itself.

######Reducing installation size
All the game files are stored without compression, which bloats the file size. Even worse, a lot of content in a file is often replicated partially in other files. Because the folder is virtualized, VirtuallAA2 can optimize and compress the game files, and decompress them on the fly without the game knowing any better.

######Easing mod development
Mods no longer needs to be packaged to `.pp` archives which makes it a lot easier to try out your modifications.

##Done

- Game is running through a virtualized folder
- PP files can be combined virtually

##In progress
- Compression
- Deduplication in `.xx` files

##TODO
- Combination rules, don't let mods overwrite files unless they are supposed to
- Deduplication in `.xa` files
- Parse character `.png` files and check if all dependencies are meet
- Support revisions of save files (save-cramming)
- Support config files, allow the package to contain the default configuation, and save all active configuration files in a special folder

##Big issues
- VirtualizedAA2 needs to be restarted to add/disable mods, which is a design decision to avoid complicated multi-threading code. Might be changed in the future, but not considered for now.
- Packages are not read-only, you can still modify the files due to a bug
- Game not loading at full speed, due to an issue in Dokan which is being worked on
- BSOD are possible if you kill VirtualizedAA2 while the game is running, or other bugs in Dokan