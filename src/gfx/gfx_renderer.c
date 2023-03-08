#include "game.h"
#include "gfx.h"

void batcher_init(BatchRenderer *self, u32 maxObjects) {
  *self = (BatchRenderer){0};
  self->maxObjects = maxObjects;
  self->clear_color = (vec4s){{0.1, 0.1, 0.1, 1}};

  self->shader = shader_create("assets/shaders/def.glsl");

  u32 maxVertices = maxObjects * 4;
  u32 maxIndices = maxObjects * 6;

  glCreateVertexArrays(1, &self->vao);
  glBindVertexArray(self->vao);

  glCreateBuffers(1, &self->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, self->vbo);
  glBufferData(GL_ARRAY_BUFFER, maxVertices * sizeof(Vertex), NULL,
               GL_DYNAMIC_DRAW);

  glEnableVertexArrayAttrib(self->vao, 0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, position));

  glEnableVertexArrayAttrib(self->vao, 1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, color));

  glEnableVertexArrayAttrib(self->vao, 2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, texCoords));

  glEnableVertexArrayAttrib(self->vao, 3);
  glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, texid));

  u32 indices[maxIndices];
  u32 offset = 0;
  for (u32 i = 0; i < maxIndices; i += 6) {
    indices[i + 0] = 0 + offset;
    indices[i + 1] = 1 + offset;
    indices[i + 2] = 2 + offset;
    indices[i + 3] = 2 + offset;
    indices[i + 4] = 3 + offset;
    indices[i + 5] = 0 + offset;

    offset += 4;
  }

  glCreateBuffers(1, &self->ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glBindVertexArray(0);
}

#define createVertex(v, p, sx, sy, tc, tid)                                    \
  v.position = (vec3s){{p.x + sx, p.y + sy, p.z}};                             \
  v.color = (vec4s){{1.0f, 1.0f, 1.0f, 1.0f}};                                 \
  v.texCoords = (vec2s){{tc.x, tc.y}};                                         \
  v.texid = tid;

void batcher_addToBatch(BatchRenderer *self, vec3s position, vec2s size,
                        SubTexture *subTexture) {

  Vertex vertices[4] = {0};
  createVertex(vertices[0], position, 0, 0, subTexture->texCoords[0],
               subTexture->baseTexture->slot);
  createVertex(vertices[1], position, size.x, 0, subTexture->texCoords[1],
               subTexture->baseTexture->slot);
  createVertex(vertices[2], position, size.x, size.y, subTexture->texCoords[2],
               subTexture->baseTexture->slot);
  createVertex(vertices[3], position, 0, size.y, subTexture->texCoords[3],
               subTexture->baseTexture->slot);

  glBindVertexArray(self->vao);
  glBindBuffer(GL_ARRAY_BUFFER, self->vao);
  glBufferSubData(GL_ARRAY_BUFFER, self->numVertices * sizeof(Vertex),
                  4 * sizeof(vertices), vertices);

  self->numElements += 6;
  self->numVertices += 4;
  self->numObjects += 1;
}
void batcher_replaceInBatch(BatchRenderer *self, size_t offset, vec3s position,
                            vec2s size, SubTexture *subTexture) {
  Vertex vertices[4] = {0};
  createVertex(vertices[0], position, 0, 0, subTexture->texCoords[0],
               subTexture->baseTexture->slot);
  createVertex(vertices[1], position, size.x, 0, subTexture->texCoords[1],
               subTexture->baseTexture->slot);
  createVertex(vertices[2], position, size.x, size.y, subTexture->texCoords[2],
               subTexture->baseTexture->slot);
  createVertex(vertices[3], position, 0, size.y, subTexture->texCoords[3],
               subTexture->baseTexture->slot);

  glBindVertexArray(self->vao);
  glBindBuffer(GL_ARRAY_BUFFER, self->vao);
  glBufferSubData(GL_ARRAY_BUFFER, offset * sizeof(Vertex),
                  4 * sizeof(vertices), vertices);
}
void batcher_render(BatchRenderer *self) {
  glClearColor(self->clear_color.x, self->clear_color.y, self->clear_color.z,
               self->clear_color.w);
  glClear(GL_COLOR_BUFFER_BIT);
  shader_bind(&self->shader);
  texture_bind(&state.texture, 0);

  shader_setUniform_mat4(&self->shader, "u_viewMat", state.camera.viewMat);
  shader_setUniform_mat4(&self->shader, "u_projMat", state.camera.projMat);
  shader_setUniform_mat4(&self->shader, "u_modelMat", GLMS_MAT4_IDENTITY);

  glBindVertexArray(self->vao);
  glDrawElements(GL_TRIANGLES, self->numElements, GL_UNSIGNED_INT, 0);
}
void batcher_clear(BatchRenderer *self) {
  self->numElements = 0;
  self->numVertices = 0;
  self->numObjects = 0;
  glBindVertexArray(self->vao);
  glBindBuffer(GL_ARRAY_BUFFER, self->vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, self->numVertices * sizeof(Vertex), NULL);
}
void batcher_delete(BatchRenderer *self) {
  shader_delete(&self->shader);

  glDeleteVertexArrays(1, &self->vao);
  glDeleteBuffers(1, &self->vbo);
  glDeleteBuffers(1, &self->ebo);
}
