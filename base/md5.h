// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_MD5_H_
#define BASE_MD5_H_

#include <string>

#include "base/basictypes.h"
#include "base/strings/string_util.h"

namespace base {

class MD5 {
 public:
  MD5() { Init(); }
  ~MD5() {}
  enum {
    kDigestSize = 16,
    kBlockSize = 64
  };

  void Init();
  void Update(const void* data, size_t nbytes);
  void Final();

  int DigestSize() const;
  int BlockSize() const;

  // 16 bytes of message digest.
  const unsigned char* Digest() const {
    return reinterpret_cast<const unsigned char*>(buf_);
  }

 private:
  void Transform();

 private:
  uint32 buf_[4];
  uint32 bits_[2];
  unsigned char in_[64];
};

std::string MD5HexString(const base::StringPiece& str);

}  // namespace base

#endif  // BASE_MD5_H_


