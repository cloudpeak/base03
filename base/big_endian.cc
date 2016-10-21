// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/big_endian.h"


namespace base {

BigEndianReader::BigEndianReader(const void* buf, size_t len)
  : ptr_(static_cast<const char*>(buf)), end_(ptr_ + len) {
}

void BigEndianReader::reset(const void* buf, size_t len) {
  ptr_ = static_cast<const char*>(buf);
  end_ = ptr_ + len;
}


bool BigEndianReader::Skip(size_t len) {
  if (ptr_ + len > end_)
    return false;
  ptr_ += len;
  return true;
}

bool BigEndianReader::ReadBytes(void* out, size_t len) {
  if (ptr_ + len > end_)
    return false;
  memcpy(out, ptr_, len);
  ptr_ += len;
  return true;
}

bool BigEndianReader::ReadPiece(base::StringPiece* out, size_t len) {
  if (ptr_ + len > end_)
    return false;
  *out = base::StringPiece(ptr_, len);
  ptr_ += len;
  return true;
}

template<typename T>
bool BigEndianReader::Read(T* value) {
  if (ptr_ + sizeof(T) > end_)
    return false;
  ReadBigEndian<T>(ptr_, value);
  ptr_ += sizeof(T);
  return true;
}

bool BigEndianReader::ReadU8(uint8* value) {
  return Read(value);
}

bool BigEndianReader::ReadU16(uint16* value) {
  return Read(value);
}

bool BigEndianReader::ReadU32(uint32* value) {
  return Read(value);
}

bool BigEndianReader::ReadU64(uint64* value) {
  return Read(value);
}

bool BigEndianReader::ReadI8(int8* value) {
  return Read(value);
}

bool BigEndianReader::ReadI16(int16* value) {
  return Read(value);
}

bool BigEndianReader::ReadI32(int32* value) {
  return Read(value);
}

bool BigEndianReader::ReadI64(int64* value) {
  return Read(value);
}

bool BigEndianReader::ReadDouble(double* value) {
  union { double f; uint64 i; };
  bool ok =  Read(&i);
  *value = f;
  return ok;
}

uint8 BigEndianReader::ReadUInt8() {
  uint8 value;
  return ReadU8(&value) ? value : 0;
}

uint16 BigEndianReader::ReadUInt16() {
  uint16 value;
  return ReadU16(&value) ? value : 0;
}

uint32 BigEndianReader::ReadUInt32() {
  uint32 value;
  return ReadU32(&value) ? value : 0;
}

uint64 BigEndianReader::ReadUInt64() {
  uint64 value;
  return ReadU64(&value) ? value : 0;
}

int8 BigEndianReader::ReadInt8() {
  int8 value;
  return ReadI8(&value) ? value : 0;
}

int16 BigEndianReader::ReadInt16() {
  int16 value;
  return ReadI16(&value) ? value : 0;
}

int32 BigEndianReader::ReadInt32() {
  int32 value;
  return ReadI32(&value) ? value : 0;
}

int64 BigEndianReader::ReadInt64() {
  int64 value;
  return ReadI64(&value) ? value : 0;
}

bool BigEndianReader::PeekBytes(void* out, size_t len) {
  if (ptr_ + len > end_)
    return false;
  memcpy(out, ptr_, len);
  return true;
}

bool BigEndianReader::PeekPiece(base::StringPiece* out, size_t len) {
  if (ptr_ + len > end_)
    return false;
  *out = base::StringPiece(ptr_, len);
  return true;
}

template<typename T>
bool BigEndianReader::Peek(T* value) {
  if (ptr_ + sizeof(T) > end_)
    return false;
  ReadBigEndian<T>(ptr_, value);
  return true;
}

bool BigEndianReader::PeekU8(uint8* value) {
  return Peek(value);
}

bool BigEndianReader::PeekU16(uint16* value) {
  return Peek(value);
}

bool BigEndianReader::PeekU32(uint32* value) {
  return Peek(value);
}

bool BigEndianReader::PeekU64(uint64* value) {
  return Peek(value);
}

bool BigEndianReader::PeekI8(int8* value) {
  return Peek(value);
}

bool BigEndianReader::PeekI16(int16* value) {
  return Peek(value);
}

bool BigEndianReader::PeekI32(int32* value) {
  return Peek(value);
}

bool BigEndianReader::PeekI64(int64* value) {
  return Peek(value);
}

bool BigEndianReader::PeekDouble(double* value) {
  union { double f; uint64 i; };
  bool ok = Peek(&i);
  *value = f;
  return ok;
}

uint8 BigEndianReader::PeekUInt8() {
  uint8 value;
  return PeekU8(&value) ? value : 0;
}

uint16 BigEndianReader::PeekUInt16() {
  uint16 value;
  return PeekU16(&value) ? value : 0;
}

uint32 BigEndianReader::PeekUInt32() {
  uint32 value;
  return PeekU32(&value) ? value : 0;
}

uint64 BigEndianReader::PeekUInt64() {
  uint64 value;
  return PeekU64(&value) ? value : 0;
}

int8 BigEndianReader::PeekInt8() {
  int8 value;
  return PeekI8(&value) ? value : 0;
}

int16 BigEndianReader::PeekInt16() {
  int16 value;
  return PeekI16(&value) ? value : 0;
}

int32 BigEndianReader::PeekInt32() {
  int32 value;
  return PeekI32(&value) ? value : 0;
}

int64 BigEndianReader::PeekInt64() {
  int64 value;
  return PeekI64(&value) ? value : 0;
}

BigEndianWriter::BigEndianWriter(void* buf, size_t len)
  : ptr_(static_cast<char*>(buf)), end_(ptr_ + len) {}

BigEndianWriter::BigEndianWriter()
  : ptr_(NULL)
  , end_(NULL) {
}

void BigEndianWriter::reset(void* buf, size_t len) {
  ptr_ = static_cast<char*>(buf);
  end_ = ptr_ + len;
}

bool BigEndianWriter::Skip(size_t len) {
  if (ptr_ + len > end_)
    return false;
  ptr_ += len;
  return true;
}

bool BigEndianWriter::WriteBytes(const void* buf, size_t len) {
  if (ptr_ + len > end_)
    return false;
  memcpy(ptr_, buf, len);
  ptr_ += len;
  return true;
}

template<typename T>
bool BigEndianWriter::Write(T value) {
  if (ptr_ + sizeof(T) > end_)
    return false;
  WriteBigEndian<T>(ptr_, value);
  ptr_ += sizeof(T);
  return true;
}

bool BigEndianWriter::WriteU8(uint8 value) {
  return Write(value);
}

bool BigEndianWriter::WriteU16(uint16 value) {
  return Write(value);
}

bool BigEndianWriter::WriteU32(uint32 value) {
  return Write(value);
}

bool BigEndianWriter::WriteU64(uint64 value) {
  return Write(value);
}

bool BigEndianWriter::WriteI8(int8 value) {
  return Write(value);
}

bool BigEndianWriter::WriteI16(int16 value) {
  return Write(value);
}

bool BigEndianWriter::WriteI32(int32 value) {
  return Write(value);
}

bool BigEndianWriter::WriteI64(int64 value) {
  return Write(value);
}

bool BigEndianWriter::WriteDouble(double value) {
  union { double f; uint64 i; };
  f = value;
  return Write(i);
}


}  // namespace base
