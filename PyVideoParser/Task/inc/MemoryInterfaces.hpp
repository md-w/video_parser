#pragma once
#ifndef MemoryInterfaces_hpp
#define MemoryInterfaces_hpp
#include "Core.hpp"
#include <cstddef>
#define TRACK_TOKEN_ALLOCATIONS

namespace vp {

enum ColorSpace {
  BT_601 = 0,
  BT_709 = 1,
  UNSPEC = 2,
};

enum ColorRange {
  MPEG = 0, /* Narrow range.*/
  JPEG = 1, /* Full range. */
  UDEF = 2,
};

/* Represents CPU-side memory.
 * May own the memory or be a wrapper around existing ponter;
 */
class Buffer final : public Token {
 public:
  Buffer() = delete;
  Buffer(const Buffer &other) = delete;
  Buffer &operator=(Buffer &other) = delete;

  ~Buffer() final;
  void *GetRawMemPtr();
  const void *GetRawMemPtr() const;
  size_t GetRawMemSize() const;
  void Update(size_t newSize, void *newPtr = nullptr);
  bool CopyFrom(size_t size, void const *ptr);
  template <typename T>
  T *GetDataAs() {
    return (T *)GetRawMemPtr();
  }
  template <typename T>
  T const *GetDataAs() const {
    return (T const *)GetRawMemPtr();
  }

  static Buffer *Make(size_t bufferSize);
  static Buffer *Make(size_t bufferSize, void *pCopyFrom);

  static Buffer *MakeOwnMem(size_t bufferSize);
  static Buffer *MakeOwnMem(size_t bufferSize, const void *pCopyFrom);

 private:
  explicit Buffer(size_t bufferSize, bool ownMemory = true);
  Buffer(size_t bufferSize, void *pCopyFrom, bool ownMemory);
  Buffer(size_t bufferSize, const void *pCopyFrom);
  bool Allocate();
  void Deallocate();

  bool own_memory = true;
  size_t mem_size = 0UL;
  void *pRawData = nullptr;
#ifdef TRACK_TOKEN_ALLOCATIONS
  uint32_t id;
#endif
};
}  // namespace vp
#endif