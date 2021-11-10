#pragma once
#ifndef zero_copy_queue_h
#define zero_copy_queue_h
template <typename T, uint32_t SIZE> class zero_copy_queue
{
  static_assert(SIZE && !(SIZE & (SIZE - 1)), "SIZE must be a power of 2");

private:
  alignas(64) std::atomic<int64_t> write_idx;
  alignas(64) std::atomic<int64_t> read_idx;
  struct {
    alignas(64) std::atomic<int64_t> stat;
    typename std::aligned_storage<sizeof(T), alignof(T)>::type data;
  } blks[SIZE];

public:
  zero_copy_queue() : write_idx(0), read_idx(0)
  {
    for (uint32_t i = 0; i < SIZE; i++) {
      blks[i].stat.store(i, std::memory_order_relaxed);
      new (&(blks[i].data)) T();
    }
  }

  ~zero_copy_queue()
  {
    for (uint32_t i = 0; i < SIZE; i++) {
      reinterpret_cast<T&>(blks[i].data).~T();
    }
  }

  int64_t size() { return write_idx.load(std::memory_order_relaxed) - read_idx.load(std::memory_order_relaxed); }

  bool empty() { return size() <= 0; }

  bool full() { return size() >= SIZE; }

  int64_t getWriteIdx() { return write_idx.fetch_add(1, std::memory_order_relaxed); }

  T* getWritable(int64_t idx)
  {
    auto& blk = blks[idx % SIZE];
    if (blk.stat.load(std::memory_order_acquire) != idx)
      return nullptr;
    return reinterpret_cast<T*>(&blk.data);
  }

  void commitWrite(int64_t idx)
  {
    auto& blk = blks[idx % SIZE];
    blk.stat.store(~idx, std::memory_order_release);
  }

  int64_t getReadIdx() { return read_idx.fetch_add(1, std::memory_order_relaxed); }

  T* getReadable(int64_t idx)
  {
    auto& blk = blks[idx % SIZE];
    if (blk.stat.load(std::memory_order_acquire) != ~idx)
      return nullptr;
    return reinterpret_cast<T*>(&blk.data);
  }

  void commitRead(int64_t idx)
  {
    auto& blk = blks[idx % SIZE];
    // reinterpret_cast<T&>(blk.data).~T();
    blk.stat.store(idx + SIZE, std::memory_order_release);
  }
};
#endif
