#include "gl/gl_object.h"

namespace GL {
BufferObject::BufferObject(const GLsizeiptr size, const GLbitfield flags,
                           const void *data)
    : size{size} {
  glCreateBuffers(1, &ID);
  glNamedBufferStorage(ID, size, data, flags);
}
BufferObject::~BufferObject() { glDeleteBuffers(1, &ID); }
} // namespace GL