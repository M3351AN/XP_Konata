# XP_Konata
なんかさ、切羽詰まってくると無性に掃除とかしたくならない？

<img width="414" height="290" alt="image" src="https://github.com/user-attachments/assets/2495aea3-8d41-4d6f-a999-753c8f5baca2" />

---
## What Is This
This is a tool that automatically sets Windows system sound effects and desktop wallpaper to Izumi Konata. It has been tested on Windows 98 SE and the latest Windows 11.

- XP_Konata_VC6.dsw is workspace for VC6
- XP_Konata_v141_xp.sln is solution for VS2017&later with v141_xp toolset
## Differences from V143 Branch(v0.xxxx)

- V143 branch We try to use the latest v143 toolset to build, so we are very careful in using WinApi, and even need to exclude CRT dependency, so the function is not perfect.

Almost rebased so

- We no longer inline resource files in headers, so VC6 finally doesn't explode the stack and throw C1063.
- We can now build a basic window program.
- We now automatically back up the user's original settings during set, allowing users to reset.
- Maybe we can try something more radical? Idk, but still not in modern C++...
## Features

- VC6 version build has been tested on Windows 11 24H2, Windows 10 22H2, Windows XP SP3, Windows 98 SE. In theory, you can run it on all x86/64 architecture Windows from Windows 98 SE to the latest Windows 11.（Not tested on systems older than Windows 98 SE）
- v141_xp version build has been tested on Windows 11 24H2, Windows 10 22H2, Windows XP SP3. In theory, you can run it on all x86/64 architecture Windows from Windows XP SP3 to the latest Windows 11.（Cant exec on systems older than Windows XP）
- Initial compatibility with changes in boot sound behavior caused by Windows version changes (to be improved)
## KnownIssues

- On Windows Vista and later systems, the boot sound effect requires modifying imageres.dll we use the logon sound effect instead. And we patch imageres.dll only on Windows 8 and later systems.
- On Windows 8 and later systems, Windows has removed the shutdown/logon/logoff sound effect, so the shutdown sound effect customization will not work.
- On very old computers, performance may be SIGNIFICANTLY degraded or settings may not be successful due to large image resolutions.

## Credits

- [richgel999(Rich Geldreich)](https://github.com/richgel999/) for his [miniz](https://github.com/richgel999/miniz)
- [ORelio](https://github.com/ORelio/) for his [Sound-Manager](https://github.com/ORelio/Sound-Manager)

## License

This project is licensed under [**TOSSRCU**](LICENSE).
```diff
+ You are free to:
	• Use: Utilize the software for any purpose not explicitly restricted
	• Copy: Reproduce the software without limitation
	• Modify: Create derivative works through remixing/transforming
	• Merge: Combine with other materials
	• Publish: Display/communicate the software publicly
	• Distribute: Share copies of the software

+ Under the following terms:
	• Attribution: Must include copyright notice and this license in all copies
	• Waifu Clause: Don't consider the author as your waifu

- You are not allowed to:
	• Sublicense: Cannot grant sublicenses for original/modified material

```
