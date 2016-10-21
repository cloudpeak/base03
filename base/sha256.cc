// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

//#include "base/sha1.h"

#include <string.h>
#include <string>

#include "base/sha256.h"

namespace base {

namespace {
static inline void be32enc(void* pp, uint32 x) {
  uint8* p = (uint8*)pp;

  p[3] = x & 0xff;
  p[2] = (x >> 8) & 0xff;
  p[1] = (x >> 16) & 0xff;
  p[0] = (x >> 24) & 0xff;
}

static uint32 be32dec(const void* pp) {
  const uint8* p = (uint8 const*)pp;

  return ((uint32)(p[3]) +
          ((uint32)(p[2]) << 8) +
          ((uint32)(p[1]) << 16) +
          ((uint32)(p[0]) << 24));
}

/*
 * Encode a length len/4 vector of (uint32) into a length len vector of
 * (unsigned char) in big-endian form.  Assumes len is a multiple of 4.
 */
static void
be32enc_vect(unsigned char* dst, const uint32* src, size_t len) {
  size_t i;

  for (i = 0; i < len / 4; i++)
    be32enc(dst + i * 4, src[i]);
}

/*
 * Decode a big-endian length len vector of (unsigned char) into a length
 * len/4 vector of (uint32).  Assumes len is a multiple of 4.
 */
static void be32dec_vect(uint32* dst, const unsigned char* src, size_t len) {
  size_t i;

  for (i = 0; i < len / 4; i++)
    dst[i] = be32dec(src + i * 4);
}

static unsigned char PAD[64] = {
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
} // anonymous namespace

/* Elementary functions used by SHA256 */
#define Ch(x, y, z) ((x & (y ^ z)) ^ z)
#define Maj(x, y, z)  ((x & (y | z)) | (y & z))
#define SHR(x, n) (x >> n)
#define ROTR(x, n)  ((x >> n) | (x << (32 - n)))
#define S0(x)   (ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define S1(x)   (ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define s0(x)   (ROTR(x, 7) ^ ROTR(x, 18) ^ SHR(x, 3))
#define s1(x)   (ROTR(x, 17) ^ ROTR(x, 19) ^ SHR(x, 10))

/* SHA256 round function */
#define RND(a, b, c, d, e, f, g, h, k)      \
  t0 = h + S1(e) + Ch(e, f, g) + k;   \
  t1 = S0(a) + Maj(a, b, c);      \
  d += t0;          \
  h  = t0 + t1;

/* Adjusted round function for rotating state */
#define RNDr(S, W, i, k)      \
  RND(S[(64 - i) % 8], S[(65 - i) % 8], \
      S[(66 - i) % 8], S[(67 - i) % 8], \
      S[(68 - i) % 8], S[(69 - i) % 8], \
      S[(70 - i) % 8], S[(71 - i) % 8], \
      W[i] + k)

/* Add padding and terminating bit-count. */
void SHA256::Pad() {
  unsigned char len[8];
  uint32 r, plen;

  /*
   * Convert length to a vector of bytes -- we do this now rather
   * than later because the length will change after we pad.
   */
  be32enc_vect(len, count_, 8);

  /* Add 1--64 bytes so that the resulting length is 56 mod 64 */
  r = (count_[1] >> 3) & 0x3f;
  plen = (r < 56) ? (56 - r) : (120 - r);
  Update(PAD, (size_t)plen);

  /* Add the terminating bit-count */
  Update(len, 8);
}

void SHA256::Transform(const unsigned char block[64]) {
  uint32 W[64];
  uint32 S[8];
  uint32 t0, t1;
  int i;

  /* 1. Prepare message schedule W. */
  be32dec_vect(W, block, 64);
  for (i = 16; i < 64; i++)
    W[i] = s1(W[i - 2]) + W[i - 7] + s0(W[i - 15]) + W[i - 16];

  /* 2. Initialize working variables. */
  memcpy(S, state_, 32);

  /* 3. Mix. */
  RNDr(S, W, 0, 0x428a2f98);
  RNDr(S, W, 1, 0x71374491);
  RNDr(S, W, 2, 0xb5c0fbcf);
  RNDr(S, W, 3, 0xe9b5dba5);
  RNDr(S, W, 4, 0x3956c25b);
  RNDr(S, W, 5, 0x59f111f1);
  RNDr(S, W, 6, 0x923f82a4);
  RNDr(S, W, 7, 0xab1c5ed5);
  RNDr(S, W, 8, 0xd807aa98);
  RNDr(S, W, 9, 0x12835b01);
  RNDr(S, W, 10, 0x243185be);
  RNDr(S, W, 11, 0x550c7dc3);
  RNDr(S, W, 12, 0x72be5d74);
  RNDr(S, W, 13, 0x80deb1fe);
  RNDr(S, W, 14, 0x9bdc06a7);
  RNDr(S, W, 15, 0xc19bf174);
  RNDr(S, W, 16, 0xe49b69c1);
  RNDr(S, W, 17, 0xefbe4786);
  RNDr(S, W, 18, 0x0fc19dc6);
  RNDr(S, W, 19, 0x240ca1cc);
  RNDr(S, W, 20, 0x2de92c6f);
  RNDr(S, W, 21, 0x4a7484aa);
  RNDr(S, W, 22, 0x5cb0a9dc);
  RNDr(S, W, 23, 0x76f988da);
  RNDr(S, W, 24, 0x983e5152);
  RNDr(S, W, 25, 0xa831c66d);
  RNDr(S, W, 26, 0xb00327c8);
  RNDr(S, W, 27, 0xbf597fc7);
  RNDr(S, W, 28, 0xc6e00bf3);
  RNDr(S, W, 29, 0xd5a79147);
  RNDr(S, W, 30, 0x06ca6351);
  RNDr(S, W, 31, 0x14292967);
  RNDr(S, W, 32, 0x27b70a85);
  RNDr(S, W, 33, 0x2e1b2138);
  RNDr(S, W, 34, 0x4d2c6dfc);
  RNDr(S, W, 35, 0x53380d13);
  RNDr(S, W, 36, 0x650a7354);
  RNDr(S, W, 37, 0x766a0abb);
  RNDr(S, W, 38, 0x81c2c92e);
  RNDr(S, W, 39, 0x92722c85);
  RNDr(S, W, 40, 0xa2bfe8a1);
  RNDr(S, W, 41, 0xa81a664b);
  RNDr(S, W, 42, 0xc24b8b70);
  RNDr(S, W, 43, 0xc76c51a3);
  RNDr(S, W, 44, 0xd192e819);
  RNDr(S, W, 45, 0xd6990624);
  RNDr(S, W, 46, 0xf40e3585);
  RNDr(S, W, 47, 0x106aa070);
  RNDr(S, W, 48, 0x19a4c116);
  RNDr(S, W, 49, 0x1e376c08);
  RNDr(S, W, 50, 0x2748774c);
  RNDr(S, W, 51, 0x34b0bcb5);
  RNDr(S, W, 52, 0x391c0cb3);
  RNDr(S, W, 53, 0x4ed8aa4a);
  RNDr(S, W, 54, 0x5b9cca4f);
  RNDr(S, W, 55, 0x682e6ff3);
  RNDr(S, W, 56, 0x748f82ee);
  RNDr(S, W, 57, 0x78a5636f);
  RNDr(S, W, 58, 0x84c87814);
  RNDr(S, W, 59, 0x8cc70208);
  RNDr(S, W, 60, 0x90befffa);
  RNDr(S, W, 61, 0xa4506ceb);
  RNDr(S, W, 62, 0xbef9a3f7);
  RNDr(S, W, 63, 0xc67178f2);

  /* 4. Mix local working variables into global state */
  for (i = 0; i < 8; i++)
    state_[i] += S[i];

  /* Clean the stack. */
  memset(W, 0, 256);
  memset(S, 0, 32);
  t0 = t1 = 0;
}


void SHA256::Init() {
  /* Zero bits processed so far */
  count_[0] = count_[1] = 0;

  /* Magic initialization constants */
  state_[0] = 0x6A09E667;
  state_[1] = 0xBB67AE85;
  state_[2] = 0x3C6EF372;
  state_[3] = 0xA54FF53A;
  state_[4] = 0x510E527F;
  state_[5] = 0x9B05688C;
  state_[6] = 0x1F83D9AB;
  state_[7] = 0x5BE0CD19;
}

void SHA256::Update(const void* in_local, size_t len) {
  uint32 bitlen[2];
  uint32 r;
  const unsigned char* src = static_cast<const unsigned char*>(in_local);

  /* Number of bytes left in the buffer from previous updates */
  r = (count_[1] >> 3) & 0x3f;

  /* Convert the length into a number of bits */
  bitlen[1] = ((uint32)len) << 3;
  bitlen[0] = (uint32)(len >> 29);

  /* Update number of bits */
  if ((count_[1] += bitlen[1]) < bitlen[1])
    count_[0]++;
  count_[0] += bitlen[0];

  /* Handle the case where we don't need to perform any transforms */
  if (len < 64 - r) {
    memcpy(&buf_[r], src, len);
    return;
  }

  /* Finish the current block */
  memcpy(&buf_[r], src, 64 - r);
  Transform(buf_);
  src += 64 - r;
  len -= 64 - r;

  /* Perform complete blocks */
  while (len >= 64) {
    Transform(src);
    src += 64;
    len -= 64;
  }

  /* Copy left over data into buffer */
  memcpy(buf_, src, len);
}

void SHA256::Final() {
  /* Add padding */
  Pad();
  be32enc_vect(buf_, state_, 32);
}

int SHA256::DigestSize() const {
  return kDigestSize;
}

int SHA256::BlockSize() const {
  return kBlockSize;
}

std::string SHA256HexString(const base::StringPiece& str) {
  SHA256 hasher;
  hasher.Update(str.data(), str.size());
  hasher.Final();
  return base::Base16Encode(hasher.Digest(), SHA256::kDigestSize);
}

}  // namespace base

