#ifndef SLASHBITBOT_STORAGE_H__
#define SLASHBITBOT_STORAGE_H__

void initialize_storage();
void store_object(int id, void* object, size_t size);
bool load_object(int id, void* object, size_t size);

template <typename T>
inline void store_object(int id, T const& object) {
  store_object(id, static_cast<void*>(&object), sizeof(T));
}

template <typename T>
inline bool load_object(int id, T& object) {
  return load_object(id, static_cast<void*>(&object), sizeof(T));
}

#endif
