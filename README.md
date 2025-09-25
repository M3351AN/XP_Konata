# XP_Konata
A tool to set windows desktop background and system sounds as Izumi Konata
---
## What Is This
This is a tool that automatically sets Windows system sound effects and desktop wallpaper to Izumi Konata. It has been tested on Windows XP and the latest Windows 11.

- XP_Konata.sln is solution for VS2022
- XP_Konata.sln is solution for VS2005 & later
## Features

- With minimal CRT dependencies and other magic, you can compile programs that work on Windows XP on the latest VS2022 version.
- It has been tested on Windows 11 24H2, Windows 10 22H2\Windows XP SP3. In theory, you can run it on all x86/64 architecture Windows from Windows XP SP3 to the latest Windows 11.（Not tested on systems older than Windows XP SP3）

## KnownIssues

- On Windows XP, since the wallpaper only supports BMP format by default, you need to manually click "Apply" in the settings to make it take effect correctly
- On Windows Vista and later systems, the boot sound effect requires modifying imageres.dll and is not integrated into the current version.
- On Windows 8 and later systems, Windows has removed the shutdown sound effect, so the shutdown sound effect customization will not work.

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
