// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_SHA1_H_
#define BASE_SHA1_H_

#include <string>

#include "base/basictypes.h"
#include "base/strings/string_util.h"

namespace base {

class SHA1 {
 public:
  SHA1() { Init(); }
  ~SHA1() {}
  enum {
    kDigestSize = 20,
    kBlockSize = 64
  };

  void Init();
  void Update(const void* data, size_t nbytes);
  void Final();

  int DigestSize() const;
  int BlockSize() const;

  // 20 bytes of message digest.
  const unsigned char* Digest() const {
    return reinterpret_cast<const unsigned char*>(H);
  }

 private:
  void Pad();
  void Process();

  uint32 A, B, C, D, E;

  uint32 H[5];

  union {
    uint32 W[80];
    uint8 M[64];
  };

  uint32 cursor_;
  uint32 l_;
};

void SHA1HashBytes(const unsigned char* data, size_t len, unsigned char* hash);

std::string SHA1HexString(const base::StringPiece& str);

}  // namespace base

#endif  // BASE_SHA1_H_
