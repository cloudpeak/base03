// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_BIG_ENDIAN_H_
#define BASE_BIG_ENDIAN_H_

#include <stddef.h>

#include "build/build_config.h"
#include "base/strings/string_piece.h"

namespace base {

// current workaround, disable warnings directly.
#if defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshift-count-overflow"
#endif

// Read an integer (signed or unsigned) from |buf| in Big Endian order.
// Note: this loop is unrolled with -O1 and above.
// NOTE(szym): glibc dns-canon.c and SpdyFrameReader use
// ntohs(*(uint16*)ptr) which is potentially unaligned.
// This would cause SIGBUS on ARMv5 or earlier and ARMv6-M.
template<typename T>
inline void ReadBigEndian(const char buf[], T* out) {
  *out = buf[0];
  for (size_t i = 1; i < sizeof(T); ++i) {
    *out <<= 8;
    // Must cast to uint8 to avoid clobbering by sign extension.
    *out |= static_cast<uint8>(buf[i]);
  }
}

// Write an integer (signed or unsigned) |val| to |buf| in Big Endian order.
// Note: this loop is unrolled with -O1 and above.
template<typename T>
inline void WriteBigEndian(char buf[], T val) {
  for (size_t i = 0; i < sizeof(T); ++i) {
    buf[sizeof(T) - i - 1] = static_cast<char>(val & 0xFF);
    val >>= 8;
  }
}

// Specializations to make clang happy about the (dead code) shifts above.
template <>
inline void ReadBigEndian<uint8>(const char buf[], uint8* out) {
  *out = buf[0];
}

template <>
inline void WriteBigEndian<uint8>(char buf[], uint8 val) {
  buf[0] = static_cast<char>(val);
}

#if defined(__clang__)
#pragma GCC diagnostic pop
#endif

// Allows reading integers in network order (big endian) while iterating over
// an underlying buffer. All the reading functions advance the internal pointer.
class BASE_EXPORT BigEndianReader {
 public:
  BigEndianReader(const void* buf, size_t len);

  void reset(const void* buf, size_t len);

  const char* ptr() const { return ptr_; }
  const unsigned char* uptr() const {
    return reinterpret_cast<const unsigned char*>(ptr_);
  }
  int remaining() const { return static_cast<int>(end_ - ptr_); }

  bool Skip(size_t len);
  bool ReadBytes(void* out, size_t len);
  // Creates a StringPiece in |out| that points to the underlying buffer.
  bool ReadPiece(base::StringPiece* out, size_t len);
  bool ReadU8(uint8* value);
  bool ReadU16(uint16* value);
  bool ReadU32(uint32* value);
  bool ReadU64(uint64* value);
  bool ReadI8(int8* value);
  bool ReadI16(int16* value);
  bool ReadI32(int32* value);
  bool ReadI64(int64* value);
  bool ReadDouble(double* value);

  uint8  ReadUInt8();
  uint16 ReadUInt16();
  uint32 ReadUInt32();
  uint64 ReadUInt64();
  int8   ReadInt8();
  int16  ReadInt16();
  int32  ReadInt32();
  int64  ReadInt64();

  bool PeekBytes(void* out, size_t len);
  bool PeekPiece(base::StringPiece* out, size_t len);
  bool PeekU8(uint8* value);
  bool PeekU16(uint16* value);
  bool PeekU32(uint32* value);
  bool PeekU64(uint64* value);
  bool PeekI8(int8* value);
  bool PeekI16(int16* value);
  bool PeekI32(int32* value);
  bool PeekI64(int64* value);
  bool PeekDouble(double* value);

  uint8  PeekUInt8();
  uint16 PeekUInt16();
  uint32 PeekUInt32();
  uint64 PeekUInt64();
  int8   PeekInt8();
  int16  PeekInt16();
  int32  PeekInt32();
  int64  PeekInt64();

 protected:
  // Hidden to promote type safety.
  template<typename T>
  bool Read(T* v);

  template<typename T>
  bool Peek(T* v);
  const char* ptr_;
  const char* end_;
};

// Allows writing integers in network order (big endian) while iterating over
// an underlying buffer. All the writing functions advance the internal pointer.
class BASE_EXPORT BigEndianWriter {
 public:
  BigEndianWriter(void* buf, size_t len);
  BigEndianWriter();
  void reset(void* buf, size_t len);

  char* ptr() const { return ptr_; }
  unsigned char* uptr() const { return reinterpret_cast<unsigned char*>(ptr_); }

  int remaining() const { return static_cast<int>(end_ - ptr_); }

  bool Skip(size_t len);
  bool WriteBytes(const void* buf, size_t len);
  bool WriteU8(uint8 value);
  bool WriteU16(uint16 value);
  bool WriteU32(uint32 value);
  bool WriteU64(uint64 value);
  bool WriteI8(int8 value);
  bool WriteI16(int16 value);
  bool WriteI32(int32 value);
  bool WriteI64(int64 value);
  bool WriteDouble(double value);


 protected:
  // Hidden to promote type safety.
  template<typename T>
  bool Write(T v);

  char* ptr_;
  char* end_;
};

}  // namespace base

#endif  // BASE_BIG_ENDIAN_H_
