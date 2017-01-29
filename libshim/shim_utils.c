/*
 * Copyright (C) 2017 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cutils/jstring.h>

#ifdef __aarch64__
  // android::String16::String16(const char16_t * str, size_t len)
  extern void _ZN7android8String16C1EPKDsm(void ** v, const char16_t * str, size_t);
  
  // android::String16::String16(const unsigned short * str, size_t len)
  void _ZN7android8String16C1EPKtm(void ** v, const unsigned short * str, size_t len) {
    return _ZN7android8String16C1EPKDsm(v, (const char16_t *)str, len);
  }
  
  // android::status_t android::String8::setTo(const char16_t * str, size_t len)
  extern void _ZN7android7String85setToEPKDsm(void ** ret, void ** v, const char16_t * str, size_t);
  
  // android::status_t android::String8::setTo(const unsigned short * str, size_t len)
  void _ZN7android7String85setToEPKtm(void ** ret, void ** v, const unsigned short * str, size_t len) {
    return _ZN7android7String85setToEPKDsm(ret, v, (const char16_t *)str, len);
  }
#else
  // android::String16::String16(const char16_t * str, size_t len)
  extern void _ZN7android8String16C1EPKDsj(void ** v, const char16_t * str, size_t); 
  
  // android::String16::String16(const unsigned short * str, size_t len)
  void _ZN7android8String16C1EPKtj(void ** v, const unsigned short * str, size_t len) {
    return _ZN7android8String16C1EPKDsj(v, (const char16_t *)str, len);
  }
  
  // android::status_t android::String8::setTo(const char16_t * str, size_t len)
  extern void _ZN7android7String85setToEPKDsj(void ** ret, void ** v, const char16_t * str, size_t);
  
  // android::status_t android::String8::setTo(const unsigned short * str, size_t len)
  void _ZN7android7String85setToEPKtj(void ** ret, void ** v, const unsigned short * str, size_t len) {
    return _ZN7android7String85setToEPKDsj(ret, v, (const char16_t *)str, len);
  }
#endif
