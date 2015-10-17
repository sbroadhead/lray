#ifndef _RAYTRACE_UTIL_ZONE_H_
#define _RAYTRACE_UTIL_ZONE_H_

#include <vector>

namespace raytrace {;

// A memory allocator that block-allocates a large chunk of memory and provides
// pointers into it, never freeing anything until the whole zone is freed. This
// is a very naive implementation which will not call the destructors of placed
// objects.
class MemoryZone {
public:
  void *Alloc(int size);
  void Free();

  template <typename T>
  T *Alloc(int count) {
    T *mem = (T *)Alloc(count * sizeof(T));
    for (int i = 0; i < count; ++i) {
      new (&mem[i]) T();
    }

    return mem;
  }

  MemoryZone(int chunk_size = 32767)
      : chunk_size(chunk_size),
        bytes(NULL),
        free_ptr(NULL),
        size(0),
        cur_chunk(-1)
  { }
  ~MemoryZone() {
    Free();
  }
private:
  int chunk_size;
  char *free_ptr;
  char *bytes;
  int size;
  std::vector<char *> chunks;
  int cur_chunk;
};

}

#endif