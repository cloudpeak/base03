// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_HMAC_H_
#define BASE_HMAC_H_

#include <string>

#include "base/strings/string_piece.h"
#include "base/strings/string_util.h"

#include "base/sha1.h"
#include "base/sha256.h"
#include "base/md5.h"

namespace base {

template<typename HasherType>
class HMAC {
 public:
  HMAC(const void* key, size_t key_len)
    : key_(key)
    , key_len_(key_len) {
    InitInternal();
  }
  HMAC(const base::StringPiece& key)
    : key_(key.data())
    , key_len_(key.length()) {
    InitInternal();
  }

  ~HMAC() {
  }

  void Init(const void* key, size_t key_len) {
    key_ = key;
    key_len_ = key_len;
    ihasher_.Init();
    ohasher_.Init();
    InitInternal();
  }

  void Init(const base::StringPiece& key) {
    key_ = key.data();
    key_len_ = key.length();
    ihasher_.Init();
    ohasher_.Init();
    InitInternal();
  }

  void Init() {
    ihasher_.Init();
    ohasher_.Init();
    InitInternal();
  }

  void Update(const void* data, size_t nbytes) {
    ihasher_.Update(data, nbytes);
  }

  void Update(const base::StringPiece& data) {
    return Update(data.data(), data.size());
  }

  void Final() {
    ihasher_.Final();
    ohasher_.Update(ihasher_.Digest(), HasherType::kDigestSize);
    ohasher_.Final();
  }

  int DigestSize() const {
    return HasherType::kBlockSize;
  }

  const unsigned char* Digest() const {
    return ohasher_.Digest();
  }

  const std::string StringBuffer() const {
    return std::string(reinterpret_cast<const char*>(Digest()),
                       HasherType::kDigestSize);
  }

  bool EqualDigest(const void* digest) const {
    return std::memcmp(Digest(), digest, HasherType::kDigestSize) == 0;
  }

  // base16 encoded digest.
  std::string HexString() const {
    return base::Base16Encode(ohasher_.Digest(), HasherType::kDigestSize);
  }

 private:
  void InitInternal() {
    uint8 hashed_long_key[HasherType::kDigestSize];
    if (key_len_ > HasherType::kBlockSize) {
      ihasher_.Update(key_, key_len_);
      ihasher_.Final();
      std::memcpy(hashed_long_key, ihasher_.Digest(), HasherType::kDigestSize);
      key_ = &hashed_long_key[0];
      key_len_ = HasherType::kDigestSize;
    }

    // initialize key ipad, key opad.
    uint8 key_ipad[HasherType::kBlockSize];
    uint8 key_opad[HasherType::kBlockSize];
    for (size_t i = 0; i < key_len_; ++i) {
      key_ipad[i] = static_cast<const uint8*>(key_)[i] ^ 0x36;
      key_opad[i] = static_cast<const uint8*>(key_)[i] ^ 0x5c;
    }
    size_t pad_left = HasherType::kBlockSize - key_len_;
    if (pad_left > 0) {
      std::memset(&key_ipad[key_len_], 0x36, pad_left);
      std::memset(&key_opad[key_len_], 0x5c, pad_left);
    }

    // compute inner, mix ipad and data.
    ihasher_.Init();
    ihasher_.Update(key_ipad, HasherType::kBlockSize);

    // compute outer, mix opad and inner.
    ohasher_.Init();
    ohasher_.Update(key_opad, HasherType::kBlockSize);
  }

 private:
  const void* key_;
  size_t key_len_;
  HasherType ihasher_;
  HasherType ohasher_;
};

// return digest size.
template<typename HasherType>
void SignHmac(const void* key, size_t key_len,
              const void* data, size_t data_len,
              void* sign_output) {
  HMAC<HasherType> hmac(key, key_len);
  hmac.Update(data, data_len);
  hmac.Final();
  std::memcpy(sign_output, hmac.Digest(), HasherType::kDigestSize);
}

template<typename HasherType>
void SignHmac(const base::StringPiece& key,
              const base::StringPiece& text,
              void* sign_output) {
  return SignHmac<HasherType>(key.data(), key.size(), text.data(), text.size(),
                              sign_output);
}

template<typename HasherType>
std::string SignHmac(const base::StringPiece& key,
                     const base::StringPiece& text) {

  std::string output;
  output.resize(HasherType::kDigestSize);
  SignHmac<HasherType>(key, text, &output[0]);
  return output;
}

template<typename HasherType>
std::string SignHmac(const void* key, size_t key_len,
                     const base::StringPiece& text) {
  base::StringPiece key_str(reinterpret_cast<const char*>(key), key_len);
  return SignHmac<HasherType>(key_str, text);
}

// base16 encoded digest.
template<typename HasherType>
std::string SignHmacHexString(const base::StringPiece& key,
                              const base::StringPiece& text) {
  return base::Base16Encode(SignHmac<HasherType>(key, text));
}

}  // namespace base

#endif  // BASE_HMAC_H_

