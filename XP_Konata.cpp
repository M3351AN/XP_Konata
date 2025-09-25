// Copyright (c) 2025 渟雲. All rights reserved.
#include "pch.h"
#include "./resource.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")

const char kAppTitle[] = "XP Konata";
const char kClassName[] = "KonataWnd";
const char kAppDir[] = "C:\\XP_Konata";
const char kBackupIni[] = "C:\\XP_Konata\\backup.ini";

const char kWallpaperKey[] = "Control Panel\\Desktop";
const char kAppEventsKey[] = "AppEvents\\Schemes\\Apps\\.Default\\";

const char kResourcePack[] = "resources.pak";
const char kPackSignature[] = "KONATA_PAK_v1";

#pragma pack(push, 1)
typedef struct {
  char signature[16];
  mz_uint32 file_count;
  mz_uint32 reserved;
} PackHeader;

typedef struct {
  char filename[32];
  mz_uint32 offset;
  mz_uint32 size;
  mz_uint32 compressed_size;
  mz_uint32 crc32;
  mz_uint8 flags;
} FileEntry;
#pragma pack(pop)

static char g_temp_bmp_path[MAX_PATH];
static char g_jpg_path[MAX_PATH];
static char g_bmp_path[MAX_PATH];
static char g_startup_path[MAX_PATH];
static char g_shutdown_path[MAX_PATH];
static char g_deep1_path[MAX_PATH];
static char g_deep2_path[MAX_PATH];
static char g_deep3_path[MAX_PATH];

static DWORD g_windows_version = 0;

static HBITMAP g_bmp = NULL;

static void EnsureWinVer() {
  OSVERSIONINFO vi = {0};
  vi.dwOSVersionInfoSize = sizeof(vi);
  GetVersionEx(&vi);
  g_windows_version = vi.dwMajorVersion;
}

static void EnsureAppDir() { CreateDirectoryA(kAppDir, NULL); }

static bool SaveBytesToFile(const void* data, DWORD size,
                            const char* out_path) {
  HANDLE file = CreateFileA(out_path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL, NULL);
  if (file == INVALID_HANDLE_VALUE) return false;
  DWORD written = 0;
  BOOL ok = WriteFile(file, data, size, &written, NULL);
  CloseHandle(file);
  return (ok && written == size);
}

static bool ExtractFileFromPack(const char* pack_data, DWORD pack_size,
                                const char* filename, const char* output_path) {
  if (!pack_data || pack_size < sizeof(PackHeader)) return false;

  PackHeader* header = (PackHeader*)pack_data;
  if (strncmp(header->signature, kPackSignature, 16) != 0) {
    return false;
  }

  FileEntry* entries = (FileEntry*)(pack_data + sizeof(PackHeader));

  for (mz_uint32 i = 0; i < header->file_count; i++) {
    if (strcmp(entries[i].filename, filename) == 0) {
      if (entries[i].offset + entries[i].compressed_size > pack_size) {
        return false;
      }

      const mz_uint8* compressed_data =
          (const mz_uint8*)(pack_data + entries[i].offset);

      if (entries[i].flags == 1) {
        mz_uint8* uncompressed_data = new mz_uint8[entries[i].size];
        mz_ulong uncompressed_size = entries[i].size;
        int result = mz_uncompress(uncompressed_data, &uncompressed_size,
                                   compressed_data, entries[i].compressed_size);
        if (result == MZ_OK) {
          bool success = SaveBytesToFile(uncompressed_data, uncompressed_size,
                                         output_path);
          delete[] uncompressed_data;
          return success;
        }
        delete[] uncompressed_data;
      } else {
        return SaveBytesToFile(compressed_data, entries[i].compressed_size,
                               output_path);
      }
      break;
    }
  }
  return false;
}

static bool ExtractAllResources(HINSTANCE instance) {
  HRSRC res =
      FindResourceA(instance, MAKEINTRESOURCE(IDR_PAK_RESOURCES), RT_RCDATA);
  if (!res) return false;

  HGLOBAL res_data = LoadResource(instance, res);
  if (!res_data) return false;

  DWORD size = SizeofResource(instance, res);
  const void* raw = LockResource(res_data);
  if (!raw || size == 0) return false;

  if (!SaveBytesToFile(raw, size, kResourcePack)) {
    return false;
  }

  return ExtractFileFromPack((const char*)raw, size, "dsktop.jpg",
                             g_jpg_path) &&
         ExtractFileFromPack((const char*)raw, size, "startup.wav",
                             g_startup_path) &&
         ExtractFileFromPack((const char*)raw, size, "shutdown.wav",
                             g_shutdown_path) &&
         ExtractFileFromPack((const char*)raw, size, "deep1.wav",
                             g_deep1_path) &&
         ExtractFileFromPack((const char*)raw, size, "deep2.wav",
                             g_deep2_path) &&
         ExtractFileFromPack((const char*)raw, size, "deep3.wav",
                             g_deep3_path) &&
         DeleteFileA(kResourcePack);
}

static bool ReadRegSz(HKEY root, const char* subkey, const char* value,
                      char* out, DWORD out_size) {
  HKEY key;
  if (RegOpenKeyExA(root, subkey, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS)
    return false;
  DWORD type = 0;
  DWORD size = out_size;
  LONG r = RegQueryValueExA(key, value, NULL, &type, (LPBYTE)out, &size);
  RegCloseKey(key);
  if (r == ERROR_SUCCESS && (type == REG_SZ || type == REG_EXPAND_SZ)) {
    if (size >= out_size) out[out_size - 1] = '\0';
    return true;
  }
  return false;
}

static bool WriteRegSz(HKEY root, const char* subkey, const char* value,
                       const char* data) {
  HKEY key;
  DWORD disp = 0;
  if (RegCreateKeyExA(root, subkey, 0, NULL, REG_OPTION_NON_VOLATILE,
                      KEY_SET_VALUE, NULL, &key, &disp) != ERROR_SUCCESS)
    return false;
  LONG r = RegSetValueExA(key, value, 0, REG_SZ, (const BYTE*)data,
                          (DWORD)(strlen(data) + 1));
  RegCloseKey(key);
  return (r == ERROR_SUCCESS);
}

static void BroadcastSettings(const char* area) {
  SendMessageTimeoutA(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)area,
                      SMTO_ABORTIFHUNG, 1000, NULL);
}

static void ApplyWallpaper(const char* bmp_path) {
  WriteRegSz(HKEY_CURRENT_USER, kWallpaperKey, "Wallpaper", bmp_path);
  WriteRegSz(HKEY_CURRENT_USER, kWallpaperKey, "WallpaperStyle", "2");
  WriteRegSz(HKEY_CURRENT_USER, kWallpaperKey, "TileWallpaper", "0");
  SystemParametersInfoA(SPI_SETDESKWALLPAPER, 0, (PVOID)bmp_path,
                        SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE);
  BroadcastSettings("Control Panel\\Desktop");
}

static void SetSoundEvent(const char* event_name, const char* wav_path) {
  char reg_path[256];
  lstrcpyA(reg_path, kAppEventsKey);
  lstrcatA(reg_path, event_name);
  lstrcatA(reg_path, "\\.Current");
  WriteRegSz(HKEY_CURRENT_USER, reg_path, "", wav_path);
}

static void ApplySystemSounds(const char* startup, const char* shutdown,
                              const char* deep1, const char* deep2,
                              const char* deep3) {
  if (g_windows_version < 6) {
    SetSoundEvent("SystemStart", startup);
    SetSoundEvent("SystemExit", shutdown);
    SetSoundEvent("WindowsLogon", "");
    SetSoundEvent("WindowsLogoff", "");
  } else if (g_windows_version < 8) {
    SetSoundEvent("SystemStart", "");
    SetSoundEvent("SystemExit", "");
    SetSoundEvent("WindowsLogon", startup);
    SetSoundEvent("WindowsLogoff", shutdown);
    // Disable "startup sound", we using logon sound instead
    const DWORD value = 1;
    HKEY key;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                      L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Authentic"
                      L"ation\\LogonUI\\BootAnimation",
                      0, KEY_SET_VALUE, &key) == ERROR_SUCCESS) {
      RegSetValueExW(key, L"DisableStartupSound", 0, REG_DWORD, (BYTE*)&value,
                     sizeof(value));
      RegCloseKey(key);
    }
  } else {
    SetSoundEvent("SystemStart", "");
    SetSoundEvent("SystemExit", "");
    SetSoundEvent("WindowsLogon", "");
    SetSoundEvent("WindowsLogoff", "");
    // Windows 8 and later do not support logon/logoff sounds
    // TODO: use Task Scheduler or sth else to play sounds at logon/logoff
    // TODO: patch imageres.dll to change default startuo sound
  }

  SetSoundEvent("DeviceConnect", deep2);
  SetSoundEvent("DeviceDisconnect", deep1);
  SetSoundEvent("SystemExclamation", deep2);
  SetSoundEvent(".Default", deep3);
  SetSoundEvent("SystemDefault", deep3);
  SetSoundEvent("SystemNotification", deep1);
  BroadcastSettings("AppEvents");
}

static void SaveBackup() {
  EnsureAppDir();
  char val[512] = {0};

  if (ReadRegSz(HKEY_CURRENT_USER, kWallpaperKey, "Wallpaper", val,
                sizeof(val)))
    WritePrivateProfileStringA("Desktop", "Wallpaper", val, kBackupIni);
  if (ReadRegSz(HKEY_CURRENT_USER, kWallpaperKey, "WallpaperStyle", val,
                sizeof(val)))
    WritePrivateProfileStringA("Desktop", "WallpaperStyle", val, kBackupIni);
  if (ReadRegSz(HKEY_CURRENT_USER, kWallpaperKey, "TileWallpaper", val,
                sizeof(val)))
    WritePrivateProfileStringA("Desktop", "TileWallpaper", val, kBackupIni);

  const char* events[] = {
      "SystemStart",   "WindowsLogon",      "WindowsLogoff",     "SystemExit",
      "DeviceConnect", "DeviceDisconnect",  "SystemExclamation", ".Default",
      "SystemDefault", "SystemNotification"};
  int i;
  for (i = 0; i < (int)(sizeof(events) / sizeof(events[0])); i++) {
    char reg_path[256];
    char cur[512] = {0};
    lstrcpyA(reg_path, kAppEventsKey);
    lstrcatA(reg_path, events[i]);
    lstrcatA(reg_path, "\\.Current");
    if (ReadRegSz(HKEY_CURRENT_USER, reg_path, "", cur, sizeof(cur))) {
      WritePrivateProfileStringA("Sounds", events[i], cur, kBackupIni);
    } else {
      WritePrivateProfileStringA("Sounds", events[i], "", kBackupIni);
    }
  }
}

static void RestoreBackup() {
  char val[512];

  GetPrivateProfileStringA("Desktop", "Wallpaper", "", val, sizeof(val),
                           kBackupIni);
  if (val[0]) WriteRegSz(HKEY_CURRENT_USER, kWallpaperKey, "Wallpaper", val);
  GetPrivateProfileStringA("Desktop", "WallpaperStyle", "", val, sizeof(val),
                           kBackupIni);
  if (val[0])
    WriteRegSz(HKEY_CURRENT_USER, kWallpaperKey, "WallpaperStyle", val);
  GetPrivateProfileStringA("Desktop", "TileWallpaper", "", val, sizeof(val),
                           kBackupIni);
  if (val[0])
    WriteRegSz(HKEY_CURRENT_USER, kWallpaperKey, "TileWallpaper", val);

  if (ReadRegSz(HKEY_CURRENT_USER, kWallpaperKey, "Wallpaper", val,
                sizeof(val))) {
    ApplyWallpaper(val);
  }

  const char* events[] = {
      "SystemStart",   "WindowsLogon",      "WindowsLogoff",     "SystemExit",
      "DeviceConnect", "DeviceDisconnect",  "SystemExclamation", ".Default",
      "SystemDefault", "SystemNotification"};
  int i;
  for (i = 0; i < (int)(sizeof(events) / sizeof(events[0])); i++) {
    char wav[512];
    GetPrivateProfileStringA("Sounds", events[i], "", wav, sizeof(wav),
                             kBackupIni);
    char reg_path[256];
    lstrcpyA(reg_path, kAppEventsKey);
    lstrcatA(reg_path, events[i]);
    lstrcatA(reg_path, "\\.Current");
    WriteRegSz(HKEY_CURRENT_USER, reg_path, "", wav);
  }
  BroadcastSettings("AppEvents");
}

static void SaveWavResource(HINSTANCE instance, UINT resid, const char* path) {
  HRSRC r = FindResourceA(instance, MAKEINTRESOURCE(resid), "WAVE");
  if (!r) return;
  HGLOBAL g = LoadResource(instance, r);
  if (!g) return;
  DWORD sz = SizeofResource(instance, r);
  const void* p = LockResource(g);
  if (p && sz) SaveBytesToFile(p, sz, path);
}

static bool ConvertJpegFileToBmpFile(const char* jpeg_path,
                                     const char* out_bmp_path, int target_w,
                                     int target_h) {
  HANDLE file = CreateFileA(jpeg_path, GENERIC_READ, FILE_SHARE_READ, NULL,
                            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (file == INVALID_HANDLE_VALUE) return false;

  DWORD size = GetFileSize(file, NULL);
  BYTE* data = new BYTE[size];
  DWORD read;
  if (!ReadFile(file, data, size, &read, NULL) || read != size) {
    delete[] data;
    CloseHandle(file);
    return false;
  }
  CloseHandle(file);

  HGLOBAL h_mem = GlobalAlloc(GMEM_MOVEABLE, size);
  if (!h_mem) {
    delete[] data;
    return false;
  }

  void* p_mem = GlobalLock(h_mem);
  memcpy(p_mem, data, size);
  GlobalUnlock(h_mem);
  delete[] data;

  IStream* stream = NULL;
  if (FAILED(CreateStreamOnHGlobal(h_mem, TRUE, &stream))) {
    GlobalFree(h_mem);
    return false;
  }

  OleInitialize(NULL);
  IPicture* picture = NULL;
  HRESULT hr =
      OleLoadPicture(stream, size, FALSE, IID_IPicture, (void**)&picture);
  stream->Release();

  if (FAILED(hr) || !picture) {
    OleUninitialize();
    return false;
  }

  OLE_XSIZE_HIMETRIC hm_width = 0, hm_height = 0;
  picture->get_Width(&hm_width);
  picture->get_Height(&hm_height);

  HDC hdc_screen = GetDC(NULL);
  int dpi_x = GetDeviceCaps(hdc_screen, LOGPIXELSX);
  int dpi_y = GetDeviceCaps(hdc_screen, LOGPIXELSY);

  int px_w = MulDiv(hm_width, dpi_x, 2540);
  int px_h = MulDiv(hm_height, dpi_y, 2540);
  if (target_w > 0 && target_h > 0) {
    px_w = target_w;
    px_h = target_h;
  }

  HDC mem_dc = CreateCompatibleDC(hdc_screen);
  HBITMAP dib = CreateCompatibleBitmap(hdc_screen, px_w, px_h);
  HBITMAP old = (HBITMAP)SelectObject(mem_dc, dib);

  picture->Render(mem_dc, 0, 0, px_w, px_h, 0, hm_height, hm_width, -hm_height,
                  NULL);

  SelectObject(mem_dc, old);

  BITMAPFILEHEADER bfh;
  BITMAPINFOHEADER bih;
  ZeroMemory(&bfh, sizeof(bfh));
  ZeroMemory(&bih, sizeof(bih));
  bih.biSize = sizeof(BITMAPINFOHEADER);
  bih.biWidth = px_w;
  bih.biHeight = px_h;
  bih.biPlanes = 1;
  bih.biBitCount = 24;
  bih.biCompression = BI_RGB;

  DWORD img_size = ((px_w * 3 + 3) & ~3) * px_h;
  bfh.bfType = 0x4D42;  // 'BM'
  bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
  bfh.bfSize = bfh.bfOffBits + img_size;

  HANDLE out_file = CreateFileA(out_bmp_path, GENERIC_WRITE, 0, NULL,
                                CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (out_file == INVALID_HANDLE_VALUE) {
    DeleteObject(dib);
    DeleteDC(mem_dc);
    ReleaseDC(NULL, hdc_screen);
    picture->Release();
    OleUninitialize();
    return false;
  }

  DWORD written;
  WriteFile(out_file, &bfh, sizeof(bfh), &written, NULL);
  WriteFile(out_file, &bih, sizeof(bih), &written, NULL);

  BYTE* bits = new BYTE[img_size];
  GetDIBits(mem_dc, dib, 0, px_h, bits, (BITMAPINFO*)&bih, DIB_RGB_COLORS);
  WriteFile(out_file, bits, img_size, &written, NULL);
  CloseHandle(out_file);

  delete[] bits;
  DeleteObject(dib);
  DeleteDC(mem_dc);
  ReleaseDC(NULL, hdc_screen);
  picture->Release();
  OleUninitialize();

  return true;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wparam,
                            LPARAM lparam) {
  switch (message) {
    case WM_CREATE: {
      g_bmp = (HBITMAP)LoadImageA(NULL, g_temp_bmp_path, IMAGE_BITMAP, 0, 0,
                                  LR_LOADFROMFILE);
      CreateWindowA("BUTTON", "set", WS_CHILD | WS_VISIBLE, 50, 120, 100, 30,
                    hwnd, (HMENU)IDC_BTN_SET, GetModuleHandle(NULL), NULL);
      CreateWindowA("BUTTON", "reset", WS_CHILD | WS_VISIBLE, 250, 120, 100, 30,
                    hwnd, (HMENU)IDC_BTN_RESET, GetModuleHandle(NULL), NULL);
      DeleteFileA(g_temp_bmp_path);
      break;
    }
    case WM_COMMAND: {
      switch (LOWORD(wparam)) {
        case IDC_BTN_SET:
          PlaySoundA(g_deep1_path, NULL, SND_FILENAME | SND_ASYNC);
          SaveBackup();
          ApplyWallpaper(g_bmp_path);
          ApplySystemSounds(g_startup_path, g_shutdown_path, g_deep1_path,
                            g_deep2_path, g_deep3_path);
          break;
        case IDC_BTN_RESET:
          PlaySoundA(g_deep2_path, NULL, SND_FILENAME | SND_ASYNC);
          RestoreBackup();
          break;
      }
      break;
    }
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hwnd, &ps);
      if (g_bmp) {
        HDC mem_dc = CreateCompatibleDC(hdc);
        HBITMAP old_bmp = (HBITMAP)SelectObject(mem_dc, g_bmp);
        BITMAP bm;
        GetObject(g_bmp, sizeof(bm), &bm);
        StretchBlt(hdc, 0, 0, 400, 200, mem_dc, 0, 0, bm.bmWidth, bm.bmHeight,
                   SRCCOPY);
        SelectObject(mem_dc, old_bmp);
        DeleteDC(mem_dc);
      } else {
        RECT rc;
        GetClientRect(hwnd, &rc);
        FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW + 1));
        const char* msg = "No bitmap loaded";
        TextOutA(hdc, 10, 10, msg, lstrlenA(msg));
      }
      EndPaint(hwnd, &ps);
      break;
    }
    case WM_DESTROY:
      PlaySoundA(g_shutdown_path, NULL, SND_FILENAME | SND_SYNC);
      if (g_bmp) {
        DeleteObject(g_bmp);
        g_bmp = NULL;
      }
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProcA(hwnd, message, wparam, lparam);
  }
  return 0;
}

int APIENTRY WinMain(HINSTANCE instance, HINSTANCE prev_instance,
                     LPSTR cmd_line, int cmd_show) {
  EnsureWinVer();
  EnsureAppDir();

  lstrcpyA(g_temp_bmp_path, kAppDir);
  lstrcatA(g_temp_bmp_path, "\\bkground.bmp");
  lstrcpyA(g_jpg_path, kAppDir);
  lstrcatA(g_jpg_path, "\\dsktop.jpg");
  lstrcpyA(g_bmp_path, kAppDir);
  lstrcatA(g_bmp_path, "\\dsktop.bmp");
  lstrcpyA(g_startup_path, kAppDir);
  lstrcatA(g_startup_path, "\\startup.wav");
  lstrcpyA(g_shutdown_path, kAppDir);
  lstrcatA(g_shutdown_path, "\\shutdown.wav");
  lstrcpyA(g_deep1_path, kAppDir);
  lstrcatA(g_deep1_path, "\\deep1.wav");
  lstrcpyA(g_deep2_path, kAppDir);
  lstrcatA(g_deep2_path, "\\deep2.wav");
  lstrcpyA(g_deep3_path, kAppDir);
  lstrcatA(g_deep3_path, "\\deep3.wav");

  if (!ExtractAllResources(instance)) {
    MessageBoxA(NULL, "Failed to extract resources from pack file!", "Error",
                MB_ICONERROR);
    return 1;
  }

  ConvertJpegFileToBmpFile(g_jpg_path, g_temp_bmp_path, 400, 200);
  if (g_windows_version < 6) {
    ConvertJpegFileToBmpFile(g_jpg_path, g_bmp_path, 1920, 1145);
  } else {
    ConvertJpegFileToBmpFile(g_jpg_path, g_bmp_path, 3200, 1908);
  }

  PlaySoundA(g_startup_path, NULL, SND_FILENAME | SND_ASYNC);

  WNDCLASSEX wc;
  ZeroMemory(&wc, sizeof(wc));
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.lpfnWndProc = WindowProc;
  wc.hInstance = instance;
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wc.lpszClassName = kClassName;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hIcon = LoadIcon(instance, MAKEINTRESOURCE(IDI_APP_ICON));
  wc.hIconSm = LoadIcon(instance, MAKEINTRESOURCE(IDI_APP_ICON));

  RegisterClassEx(&wc);

  HWND hwnd = CreateWindowA(
      kClassName, kAppTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
      CW_USEDEFAULT, CW_USEDEFAULT, 400, 200, NULL, NULL, instance, NULL);
  ShowWindow(hwnd, cmd_show);
  UpdateWindow(hwnd);

  MSG msg;
  while (GetMessageA(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessageA(&msg);
  }
  return (int)msg.wParam;
}