#include "zone.h"

namespace raytrace {;

void *MemoryZone::Alloc(int count) {
  if (cur_chunk == -1 || free_ptr + count > chunks[cur_chunk] + chunk_size) {
    ++cur_chunk;
    chunks.push_back(new char[chunk_size]);
    free_ptr = chunks[cur_chunk];
  }

  void *out_ptr = free_ptr;
  free_ptr += count;
  return out_ptr;
}

void MemoryZone::Free() {
  for (int i = 0; i <= cur_chunk; ++i) {
    delete chunks[i];
  }
  cur_chunk = -1;
  chunks.clear();
  free_ptr = bytes = NULL;
}

}