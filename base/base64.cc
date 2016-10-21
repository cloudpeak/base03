// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/logging.h"
#include "base/basictypes.h"
#include "base/stl_util.h"
#include "base/base64.h"

namespace base {

namespace {
static const char kPad = '=';

const char kBase64Alphabet[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  "abcdefghijklmnopqrstuvwxyz"
  "0123456789+/";
}

bool Base64Encode(const StringPiece& input, std::string* output) {
  DCHECK(output != NULL);
  std::string temp;
  temp.resize(((input.length() + 2) / 3) * 4);
  const unsigned char* byte_data =
    reinterpret_cast<const unsigned char*>(input.data());

  unsigned char c;
  size_t len = input.length();
  size_t i = 0;
  size_t dest_ix = 0;
  while (i < input.length()) {
    c = (byte_data[i] >> 2) & 0x3f;
    temp[dest_ix++] = kBase64Alphabet[c];

    c = (byte_data[i] << 4) & 0x3f;
    if (++i < len) {
      c |= (byte_data[i] >> 4) & 0x0f;
    }
    temp[dest_ix++] = kBase64Alphabet[c];

    if (i < len) {
      c = (byte_data[i] << 2) & 0x3f;
      if (++i < len) {
        c |= (byte_data[i] >> 6) & 0x03;
      }
      temp[dest_ix++] = kBase64Alphabet[c];
    } else {
      temp[dest_ix++] = kPad;
    }

    if (i < len) {
      c = byte_data[i] & 0x3f;
      temp[dest_ix++] = kBase64Alphabet[c];
      ++i;
    } else {
      temp[dest_ix++] = kPad;
    }
  }
  std::swap(temp, *output);
  return true;
}

bool Base64Decode(const StringPiece& s, std::string* output) {
  size_t n = s.size();
  if ((n % 4) != 0) {
    return false;
  }
  size_t padding = 0;
  if (n >= 1 && s[n - 1] == '=') {
    padding = 1;
    if (n >= 2 && s[n - 2] == '=') {
      padding = 2;
      if (n >= 3 && s[n - 3] == '=') {
        padding = 3;
      }
    }
  }
  // We divide first to avoid overflow. It's OK to do this because we
  // already made sure that n % 4 == 0.
  size_t out_len = (n / 4) * 3 - padding;
  std::string temp;
  temp.resize(out_len);
  uint8* out = reinterpret_cast<uint8*>(string_as_array(&temp));
  size_t j = 0;
  uint32 accum = 0;
  for (size_t i = 0; i < n; ++i) {
    char c = s[i];
    unsigned value;
    if (c >= 'A' && c <= 'Z') {
      value = c - 'A';
    } else if (c >= 'a' && c <= 'z') {
      value = 26 + c - 'a';
    } else if (c >= '0' && c <= '9') {
      value = 52 + c - '0';
    } else if (c == '+') {
      value = 62;
    } else if (c == '/') {
      value = 63;
    } else if (c != '=') {
      return false;
    } else {
      if (i < n - padding) {
        return false;
      }
      value = 0;
    }
    accum = (accum << 6) | value;
    if (((i + 1) % 4) == 0) {
      out[j++] = (accum >> 16);
      if (j < out_len) { out[j++] = (accum >> 8) & 0xff; }
      if (j < out_len) { out[j++] = accum & 0xff; }
      accum = 0;
    }
  }
  std::swap(temp, *output);
  return true;
}

}  // namespace base
