#include "gfx.h"

void texture_create(Texture *self, u32 width, u32 height){
  *self = (Texture){.filepath = NULL, .format = GL_RGB, .size = (ivec2s){{width, height}},};

  glCreateTextures(GL_TEXTURE_2D, 1, &self->id);
  glBindTexture(GL_TEXTURE_2D, self->id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
               GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

}
void texture_createv(Texture *self, ivec2s size){
  *self = (Texture){.filepath = NULL, .format = GL_RGB, .size = size,};

  glCreateTextures(GL_TEXTURE_2D, 1, &self->id);
  glBindTexture(GL_TEXTURE_2D, self->id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0,
               GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void texture_load(Texture *self, char *filepath) {
  *self = (Texture){0};

  stbi_set_flip_vertically_on_load(true);

  int width, height, channels;
  unsigned char *data = stbi_load(filepath, &width, &height, &channels, 0);

  if (!data) {
    DEBUG_LOG("failled to load image: %s \n %s", filepath,
              stbi_failure_reason());
    return;
  }

  self->filepath = filepath;
  self->format = channels == 3 ? GL_RGB : GL_RGBA;
  self->size = (ivec2s){{width, height}};
  glGenTextures(1, &self->id);
  glBindTexture(GL_TEXTURE_2D, self->id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D, 0, self->format, self->size.s, self->size.t, 0,
               self->format, GL_UNSIGNED_BYTE, data);

  stbi_image_free(data);
}
void texture_bind(Texture *self, u32 slot) {
  glBindTextureUnit(slot, self->id);
  self->slot = slot;
}
void texture_unbind(Texture *self, u32 slot) {
  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(GL_TEXTURE_2D, 0);
  self->slot = -1;
}
void texture_delete(Texture *self) { glDeleteTextures(1, &self->id); }

static SubTexture _createSubTex(Texture *texture, vec2s min, vec2s max) {
  SubTexture self = {.baseTexture = texture};

  self.texCoords[0] = (vec2s){{min.x, min.y}};
  self.texCoords[1] = (vec2s){{max.x, min.y}};
  self.texCoords[2] = (vec2s){{max.x, max.y}};
  self.texCoords[3] = (vec2s){{min.x, max.y}};
  return self;
}

SubTexture subTexture_create(Texture *texture, vec2s coords, vec2s subSize) {

  vec2s min = {{(coords.x * subSize.x) / texture->size.x,
                (coords.y * subSize.y) / texture->size.y}};
  vec2s max = {{((coords.x + 1) * subSize.x) / texture->size.x,
                ((coords.y + 1) * subSize.y) / texture->size.y}};

  return _createSubTex(texture, min, max);
}
