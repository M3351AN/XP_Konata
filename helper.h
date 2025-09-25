#pragma once
#ifndef HELPER_H_
#define HELPER_H_

extern "C" void _chkstk() {}
// we shall not using CRT functions
#pragma function(memcpy, memset, memmove, memcmp, wcsrchr)

extern "C" void* __cdecl memcpy(void* dst, const void* src, size_t count) {
  auto* d = static_cast<unsigned char*>(dst);
  auto* s = static_cast<const unsigned char*>(src);
  while (count--) *d++ = *s++;
  return dst;
}

extern "C" void* __cdecl memset(void* dst, int val, size_t count) {
  unsigned char* p = static_cast<unsigned char*>(dst);
  while (count--) {
    *p++ = static_cast<unsigned char>(val);
  }
  return dst;
}

extern "C" int __cdecl memcmp(const void* a, const void* b, size_t count) {
  auto* pa = static_cast<const unsigned char*>(a);
  auto* pb = static_cast<const unsigned char*>(b);
  for (; count--; ++pa, ++pb) {
    if (*pa != *pb) return *pa - *pb;
  }
  return 0;
}

extern "C" void* __cdecl memmove(void* dst, const void* src, size_t count) {
  auto* d = static_cast<unsigned char*>(dst);
  auto* s = static_cast<const unsigned char*>(src);
  if (d < s) {
    while (count--) *d++ = *s++;
  } else {
    d += count;
    s += count;
    while (count--) *--d = *--s;
  }
  return dst;
}
extern "C" const TCHAR* __cdecl wcsrchr(const TCHAR* str, TCHAR ch) {
  const TCHAR* last = NULL;
  while (*str) {
    if (*str == ch) last = str;
    str++;
  }
  return last;
}

#endif  // HELPER_H_