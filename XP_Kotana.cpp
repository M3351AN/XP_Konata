﻿// Copyright (c) 2025 渟雲. All rights reserved.
#include "pch.h"

#if _MSC_VER >= 1910
#include "./helper.h"
#endif

#include "./deep1.h"
#include "./deep2.h"
#include "./deep3.h"
#include "./dsktop.h"
#include "./shutdown.h"
#include "./startup.h"


#define _WIN32_WINNT 0x0501
#define WINVER 0x0501
#define NTDDI_VERSION 0x05010000

static const char* kHello =
    "                                   .i(vcrjrnuuxrnXLZwZULLj,..             "
    "       \n"
    "                        .. ..   ..:rcxjjuvj1<;`'..`I-rQQUOc'              "
    "       \n"
    "                       . //,   .. xUtjcx?,.           ;zQ0Q^^,\".. .  "
    "....        \n"
    "                       . (Qv(i`   lxxL/.             "
    "'\"/Zz<:l\":I;::::\":;:'       \n"
    "                         ^znxvn\\?_]1fLCj//\\|||11]_>;\",!!,. "
    "'\",^`^``^^^``'.       \n"
    "                     '\":l![UYjjxunnxxJXXYXcnrrxxnnuuxf)~;`               "
    "        \n"
    "                 `I~]]?>i)jjrjjjjjjj0vfjjxcvjjjjjjjjjrxx/)?!^             "
    "       \n"
    "              \"<]]_>I:,,,]jrjjjjjjfCJtjjjjffjjjjjjjjjjjjjf|[[[I          "
    "        \n"
    "          . ;[{+;;<l:::::,i{tjjjjfzbnjjjjjjjjjjjjjjjjjrrrjfxl!([` .       "
    "       \n"
    "         .^)t]+]xj?I,:;I!><_1jjjfzkpfjjjjjjjjfjjfjjjf/)}+i:_/<^+/l        "
    "       \n"
    "         .:}>I?j{<~]{(\\tfjrrjjjfcLrOfjjjjjjjjUxrcjjf([_~<>>i+x(!<r! .    "
    "        \n"
    "         .  ')zt/jrrrrjjjjjnJjfuC[(0fjrjjjjjtOvumUjjjrrjrjjjjfUL/_x^..    "
    "       \n"
    "         . !vvfjjjjjjjjjjfxmYtnQ{;}OtxnjjjjjfJXXuxQjjjjjjjjjjjfz0t|( .    "
    "       \n"
    "        . _JxfjjjjjjjjjjjrQdnzZ)~\"iOjuvfjjjjfXLQt-rLrjjjjjjjjjjfn0ju,.   "
    "        \n"
    "         _JjffjjjjjjjjjjjJh0rL_^^\"^XuvmnjjjjfvZJ>^\"}CzYfjjjjjjjjfxQY>   "
    "         \n"
    "      . "
    "!CrfxJxjjjjjjjjfXQUcJ]`\"\"\"`}JrkQjjjjfnax\"\"\"^~UQJjfjjjjjjffrp1       "
    "     \n"
    "      "
    ".'XufvYOjjjjjjjjjrZ{n0f`\"\"\"\"\":YxZnJrfjjxk<^\"\"\"^lcQQxfjjjjnzxfrJI "
    "..        \n"
    "       "
    "{JjY(lQjjjjjjjjfQf,~m;`^^^^^`?UQ}]YzjfrU^^^^^^`^\\J0XffjjnbQJvcX^..       "
    " \n"
    "      ^LXn> "
    ":LjjjjjjjfvX_]?|_[){[-+\"^|q\\'!\\XcrJ]]}1(--?-jZhLnfff0cnLt0t .        \n"
    "      :C}' .'zxfjjfzmfw{\\C0Zpkoooahx^^/X\"^^;?CMo#*oadwZLr(pCCCntcQxn "
    "l]'.        \n"
    "      .'   . tzfjjzwdZpQiYZqdbpZUnt>\"\"^:,\"\"^`l(xXObbbpmQ-YQYrqLLvwz|  "
    " ..        \n"
    "            .IJrj0JOQQLm/}{[_<l:^`^^\"^^^\"^^^\"^``^:l>+]\\\\|mJXf0uOzOC- "
    ".           \n"
    "            . "
    "_JwzCwJJUQn\"Ill!ll;\"\"\"\"-[?]]?\\_^\"\";IIll;~l\\UCvtCZvtfJl .         "
    "  \n"
    "               "
    ";QxcdnJncC];;;;I;:\"\"\"`(t~~~_(|^\"\",;;;;;:?vxYrfUCtUrC\"             \n"
    "               ,CCcmjrjtYdv(-!,^^^^^`!/{]]{1!^^^^^,l_(uqJfjjfUJtJJJ^      "
    "       \n"
    "             . ]OQrmjjjfnwLLwwQnt)]_~l:i>>!l>_[)tnXmkwJqvfjjtCYfjZ0:..    "
    "       \n"
    "             . YwnfmxfjjjwLJqpmJLQ0mq0+____JpZ0QLCJZbmJqxjjjfOvffua? .    "
    "       \n"
    "            . +avffQcfjjfLO0kqQZmqpkY{?-??-{XbpwmZQLbkLZfjjjrmjjjfCz..    "
    "       \n"
    "           . \"QXfjfY0fjjfcpkkppqqwmb/+-----+(bmwwqppp#kJfjjfXQfjjjfL+    "
    "        \n"
    "            `vXfjfxUqnrjjjbpwqwwwwwqq\\\\\\|(()ZpmwmwwqqwhufjrrwJrjjjjrX; "
    "          \n"
    "           \"tjfjfxUJQZLXffQv)nYCCLLCqnII;,`)dOOOZZ0Uunmjjx0QZJXjjjjffn, "
    ".        \n"
    "        . lj)fjjxYJJLmLwntn0)?])/xvcc0!   IQzzvnf|{[{XYff0ZZLJCcxrjjj\\t^ "
    "        \n"
    "         "
    ">f_fjfxJCJ0U?_XZxtQQYx{?})\\nYU^.^JUj|)1[{/zcmrfLO[+XOJJzznjj/{/'..      "
    "\n";


BOOL GetResourcePath(LPTSTR path, DWORD path_size) {
  _tcscpy(path, TEXT("C:\\XP_Konata"));
  CreateDirectory(path, NULL);
  return TRUE;
}

BOOL WriteToFile(const BYTE* data, DWORD size, LPCTSTR file_path) {
  HANDLE file = CreateFile(file_path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL, NULL);
  if (file == INVALID_HANDLE_VALUE) {
    return FALSE;
  }
  DWORD bytes_written;
  BOOL success = WriteFile(file, data, size, &bytes_written, NULL);
  CloseHandle(file);
  return (success && bytes_written == size);
}

void SetSoundEvent(LPCTSTR event_name, LPCTSTR sound_path) {
  HKEY key;
  DWORD disposition;
  TCHAR reg_path[256];
  _tcscpy(reg_path, TEXT("AppEvents\\Schemes\\Apps\\.Default\\"));
  _tcscat(reg_path, event_name);
  _tcscat(reg_path, TEXT("\\.Current"));
  if (RegCreateKeyEx(HKEY_CURRENT_USER, reg_path, 0, NULL,
                     REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key,
                     &disposition) == ERROR_SUCCESS) {
    DWORD path_len = _tcslen(sound_path) + 1;
    RegSetValueEx(key, TEXT(""), 0, REG_SZ, (const BYTE*)sound_path,
                  path_len * sizeof(TCHAR));
    RegCloseKey(key);
  }
}

void SetWallpaper(LPCTSTR path) {
  HKEY key;
  if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Control Panel\\Desktop"), 0,
                   KEY_SET_VALUE, &key) == ERROR_SUCCESS) {
    RegSetValueEx(key, TEXT("Wallpaper"), 0, REG_SZ, (const BYTE*)path,
                  (DWORD)((_tcslen(path) + 1) * sizeof(TCHAR)));
    const TCHAR* style = TEXT("2");
    RegSetValueEx(key, TEXT("WallpaperStyle"), 0, REG_SZ, (const BYTE*)style,
                  (DWORD)((_tcslen(style) + 1) * sizeof(TCHAR)));
    const TCHAR* tile = TEXT("0");
    RegSetValueEx(key, TEXT("TileWallpaper"), 0, REG_SZ, (const BYTE*)tile,
                  (DWORD)((_tcslen(tile) + 1) * sizeof(TCHAR)));
    RegCloseKey(key);
  }

  SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (PVOID)path,
                       SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE);
  SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
                     (LPARAM)TEXT("Control Panel\\Desktop"), SMTO_ABORTIFHUNG,
                     1000, NULL);
  // Is too complex to refresh Active Desktop stuffs
  OSVERSIONINFO vi = {0};
  vi.dwOSVersionInfoSize = sizeof(vi);
  GetVersionEx(&vi);

  if (vi.dwMajorVersion < 6) {  // Windows version < Vista
    ShellExecute(NULL, TEXT("open"), TEXT("rundll32.exe"),
                 TEXT("shell32.dll,Control_RunDLL desk.cpl,,0"), NULL,
                 SW_SHOWNORMAL);
  }
}

void SetSystemSound(LPCTSTR startup_path, LPCTSTR shutdown_path,
                         LPCTSTR deep1_path, LPCTSTR deep2_path,
                         LPCTSTR deep3_path) {
  SetSoundEvent(TEXT("SystemStart"), startup_path);
  SetSoundEvent(TEXT("WindowsLogon"), startup_path);
  SetSoundEvent(TEXT("WindowsLogoff"), shutdown_path);
  SetSoundEvent(TEXT("SystemExit"), shutdown_path);
  SetSoundEvent(TEXT("DeviceConnect"), deep2_path);
  SetSoundEvent(TEXT("DeviceDisconnect"), deep1_path);
  SetSoundEvent(TEXT("SystemExclamation"), deep2_path);
  SetSoundEvent(TEXT(".Default"), deep3_path);
  SetSoundEvent(TEXT("SystemDefault"), deep3_path);
  SetSoundEvent(TEXT("SystemNotification"), deep1_path);
  SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
                     (LPARAM)TEXT("AppEvents"), SMTO_ABORTIFHUNG, 1000, NULL);
}

void mainCRTStartup() {
  TCHAR xp_kotana_path[MAX_PATH];
  if (!GetResourcePath(xp_kotana_path, MAX_PATH)) {
    ExitProcess(1);
  }

  TCHAR jpg_path[MAX_PATH];
  TCHAR startup_path[MAX_PATH];
  TCHAR shutdown_path[MAX_PATH];
  TCHAR deep1_path[MAX_PATH];
  TCHAR deep2_path[MAX_PATH];
  TCHAR deep3_path[MAX_PATH];

  _tcscpy(jpg_path, xp_kotana_path);
  _tcscat(jpg_path, TEXT("\\dsktop.jpg"));

  _tcscpy(startup_path, xp_kotana_path);
  _tcscat(startup_path, TEXT("\\startup.wav"));

  _tcscpy(shutdown_path, xp_kotana_path);
  _tcscat(shutdown_path, TEXT("\\shutdown.wav"));

  _tcscpy(deep1_path, xp_kotana_path);
  _tcscat(deep1_path, TEXT("\\deep1.wav"));

  _tcscpy(deep2_path, xp_kotana_path);
  _tcscat(deep2_path, TEXT("\\deep2.wav"));

  _tcscpy(deep3_path, xp_kotana_path);
  _tcscat(deep3_path, TEXT("\\deep3.wav"));

  if (!WriteToFile(dsktop, sizeof(dsktop), jpg_path)) {
    ExitProcess(1);
  }
  if (!WriteToFile(startup, sizeof(startup), startup_path)) {
    ExitProcess(1);
  }
  WriteToFile(shutdown, sizeof(shutdown), shutdown_path);
  WriteToFile(deep1, sizeof(deep1), deep1_path);
  WriteToFile(deep2, sizeof(deep2), deep2_path);
  WriteToFile(deep3, sizeof(deep3), deep3_path);

  DWORD bytes_written;
  HANDLE stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
  WriteFile(stdout_handle, kHello, strlen(kHello), &bytes_written, NULL);

  SetWallpaper(jpg_path);
  SetSystemSound(startup_path, shutdown_path, deep1_path, deep2_path,
                      deep3_path);
  PlaySound(startup_path, NULL, SND_FILENAME | SND_SYNC);
  Sleep(3000);
  ExitProcess(0);
}
#if _MSC_VER < 1910
int main() { mainCRTStartup(); return 0;}
#endif