#include "game.h"
#include "gfx.h"

void batcher_init(BatchRenderer *self, u32 maxObjects) {
  *self = (BatchRenderer){0};
  self->maxObjects = maxObjects;
  self->clear_color = (vec4s){{0.1, 0.1, 0.1, 1}};

  DEBUG_LOG("%zu  %zu", sizeof(self->shaders[1]), sizeof(Shader));

  self->shaders[SHADER_PRE] = shader_create("assets/shaders/pre_process.glsl");
  self->shaders[SHADER_POST] =
      shader_create("assets/shaders/post_process.glsl");

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

  framebuffer_init(&self->framebuffer);
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

static void __draw_scene(BatchRenderer *self) {
  shader_bind(&self->shaders[SHADER_PRE]);
  texture_bind(&state.textureList[TEX_GAME_SPRITES], 0);
  texture_bind(&state.textureList[TEX_BUTTONS], 1);
  texture_bind(&state.textureList[TEX_TEXTS], 2);

  shader_setUniform_mat4(&self->shaders[SHADER_PRE], "u_viewMat",
                         state.camera.viewMat);
  shader_setUniform_mat4(&self->shaders[SHADER_PRE], "u_projMat",
                         state.camera.projMat);
  shader_setUniform_mat4(&self->shaders[SHADER_PRE], "u_modelMat",
                         GLMS_MAT4_IDENTITY);

  glBindVertexArray(self->vao);
  glDrawElements(GL_TRIANGLES, self->numElements, GL_UNSIGNED_INT, 0);
}

void batcher_render(BatchRenderer *self) {
  glBindFramebuffer(GL_FRAMEBUFFER, self->framebuffer.fbo);
  glClearColor(self->clear_color.x, self->clear_color.y, self->clear_color.z,
               self->clear_color.w);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  __draw_scene(self);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  if(window.key[GLFW_KEY_1].down){
    self->framebuffer.postEffect = 1;
  }else if(window.key[GLFW_KEY_2].down){
    self->framebuffer.postEffect = 2;
  } else if(window.key[GLFW_KEY_3].down){
    self->framebuffer.postEffect = 3;
  }else if(window.key[GLFW_KEY_4].down){
    self->framebuffer.postEffect = 4;
  }else self->framebuffer.postEffect = 0;



  shader_bind(&self->shaders[SHADER_POST]);
  shader_setUniform1i(&self->shaders[SHADER_POST], "effect",
                      self->framebuffer.postEffect);
  framebuffer_render(&self->framebuffer);
}
void batcher_clear(BatchRenderer *self) {

  glBindVertexArray(self->vao);
  glBindBuffer(GL_ARRAY_BUFFER, self->vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, self->numVertices * sizeof(Vertex), NULL);
  self->numElements = 0;
  self->numVertices = 0;
  self->numObjects = 0;
}
void batcher_delete(BatchRenderer *self) {
  for (u32 i = 0; i < SHADER_COUNT; i++) {
    shader_delete(&self->shaders[i]);
  }
  framebuffer_delete(&self->framebuffer);

  glDeleteVertexArrays(1, &self->vao);
  glDeleteBuffers(1, &self->vbo);
  glDeleteBuffers(1, &self->ebo);
}
