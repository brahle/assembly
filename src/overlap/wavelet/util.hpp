#ifndef OVERLAP_WAVELET_UTIL_H_
#define OVERLAP_WAVELET_UTIL_H_

#include <cstdio>
#include <cassert>

#include <unordered_map>
#include <vector>

template<typename T>
void serialize(FILE* out, const T& val) {
  assert(fwrite(&val, sizeof(val), 1, out) > 0);
}

template<typename T>
void deserialize(FILE* in, T& val) {
  assert(fread(&val, sizeof(val), 1, in) > 0);
}

template<typename T>
void serialize_vector(FILE* out, const std::vector<T>& V) {
  unsigned long long int sz = V.size();
  serialize(out, sz);
  if (sz > 0) {
    assert(fwrite(V.data(), sizeof(V[0]), sz, out) > 0);
  }
}

template<typename T>
void deserialize_vector(FILE* in, std::vector<T>& V) {
  unsigned long long int sz;
  deserialize(in, sz);
  V.clear();
  if (sz > 0) {
    V.resize(sz);
    assert(fread(V.data(), sizeof(V[0]), sz, in) > 0);
  }
}

template<typename U, typename V>
void serialize_umap(FILE* out, const std::unordered_map<U, V>& umap) {
  unsigned long long sz = umap.size();
  serialize(out, sz);

  for (auto it = umap.begin(); it != umap.end(); ++it) {
    serialize(out, it->first);
    serialize(out, it->second);
  }
}


template<typename U, typename V>
void deserialize_umap(FILE* in, std::unordered_map<U, V>& umap) {
  unsigned long long sz = umap.size();
  deserialize(in, sz);
  umap.clear();

  while (sz-- > 0) {
    static U u;
    static V v;
    deserialize(in, u);
    deserialize(in, v);
    umap[u] = v;
  }
}


#endif  // OVERLAP_WAVELET_UTIL_H_
