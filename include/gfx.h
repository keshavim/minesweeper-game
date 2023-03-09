
#ifndef GFX_H
#define GFX_H

#include "stdafx.h"
#include <GLFW/glfw3.h>

typedef struct {
  bool down, pressed, released;
} Button;

typedef struct {
  GLFWwindow *handle;
  ivec2s size;
  s8 *title;

  Button key[GLFW_KEY_LAST];
  Button mouse[GLFW_MOUSE_BUTTON_LAST];
  vec2s cursor_pos;

  float currentTime, lastTime, deltaTime, fps;
} Window;
// global window
extern Window window;

void window_init(void);
void window_loop(void);

// shader

typedef struct {
  u32 program;
  char *filepath;
} Shader;

Shader shader_create(char *filepath);
void shader_bind(Shader *self);
void shader_unbind(void);
void shader_delete(Shader *self);

// seting uniforms
void shader_setUniform1i(Shader *self, char *name, s32 val);
void shader_setUniform1iv(Shader *self, char *name, s32 *val, u32 len);
void shader_setUniform1f(Shader *self, char *name, float val);
void shader_setUniform2f(Shader *self, char *name, vec2s val);
void shader_setUniform3f(Shader *self, char *name, vec3s val);
void shader_setUniform4f(Shader *self, char *name, vec4s val);
void shader_setUniform_mat4(Shader *self, char *name, mat4s val);

// texture handleing
typedef struct {
  u32 id;
  u32 slot;
  char *filepath;
  u32 format;
  ivec2s size;
} Texture;

void texture_create(Texture *self, u32 width, u32 height);
void texture_createv(Texture *self,ivec2s size);
void texture_load(Texture *self, char *filepath);
void texture_bind(Texture *self, u32 slot);
void texture_unbind(Texture *self, u32 slot);

// subtexture handleing
// create subtexture from a baseTexture
typedef struct {
  Texture *baseTexture;
  vec2s texCoords[4];
} SubTexture;
// coords is the position
// subSize is the size of the subtexture
SubTexture subTexture_create(Texture *texture, vec2s coords, vec2s subSize);

// camera
typedef struct {
  vec3s position;
  float orthoRight;
  float orthoTop;
  mat4s viewMat;
  mat4s projMat;
} Camera;

void camera_init(Camera *self, vec3s position);
void camera_setScale(Camera *self, u32 right, u32 top);

void camera_update(Camera *self);

typedef struct {
  u32 fbo;
  u32 rbo;
  u32 vao;
  u32 vbo;
  u32 textureBuf;
  Texture texBuf;
  u32 postEffect;


} FrameBuffer;

void framebuffer_init(FrameBuffer *self);
void framebuffer_render(FrameBuffer *self);
void framebuffer_delete(FrameBuffer *self);
// vertex attributes
typedef struct {
  vec3s position;
  vec4s color;
  vec2s texCoords;
  float texid;
} Vertex;

enum Shaders{
  SHADER_PRE = 0,
  SHADER_POST,
  SHADER_COUNT
};

// batch renderer
typedef struct {
  u32 vao;
  u32 vbo;
  u32 ebo;

  u32 numElements;
  u32 numVertices;

  u32 numObjects;
  u32 maxObjects;

  Shader shaders[SHADER_COUNT];
  FrameBuffer framebuffer;
  vec4s clear_color;
} BatchRenderer;

void batcher_init(BatchRenderer *self, u32 maxObjects);
void batcher_addToBatch(BatchRenderer *self, vec3s position, vec2s size,
                        SubTexture *subTexture);
void batcher_replaceInBatch(BatchRenderer *self, size_t offset, vec3s position,
                            vec2s size, SubTexture *subTexture);
void batcher_render(BatchRenderer *self);
void batcher_clear(BatchRenderer *self);
void batcher_delete(BatchRenderer *self);


#endif // GFX_H
