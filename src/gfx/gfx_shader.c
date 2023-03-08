#include "gfx.h"

static char *_readFile(char *filepath) {
  FILE *file = fopen(filepath, "rb");
  if (!file) {
    perror("gfx_shader");
    exit(1);
  }
  fseek(file, 0, SEEK_END);
  u64 filesize = ftell(file);
  fseek(file, 0, SEEK_SET);

  //*reads the full file
  char *fullstr = calloc(filesize, 1);
  u32 readsize = 0;
  while (readsize != filesize) {
    readsize += fread(fullstr, 1, filesize, file);
  }
  fclose(file);

  return fullstr;
}

static void _compile(Shader *self, char *fullstr) {
  // //*gets the vertex sources
  u32 vsize = strstr(fullstr, "//VERTEX_END") - fullstr;
  char *vertex_source = calloc(vsize, 1);
  memcpy(vertex_source, fullstr, vsize);
  vsize += strlen("//VERTEX_END");

  //*compile and attach vertex source
  u32 vs_handle = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs_handle, 1, (const GLchar *const *)&vertex_source,
                 (const GLint *)&vsize);
  glCompileShader(vs_handle);

  //*gets fragment source
  u32 fsize = strstr(fullstr, "//FRAGMENT_END") - fullstr - vsize;
  char *fragment_source = calloc(fsize, 1);
  memcpy(fragment_source, fullstr + vsize, fsize);

  //*compiles and attaches fragment source
  u32 fs_handle = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs_handle, 1, (const GLchar *const *)&fragment_source,
                 (const GLint *)&fsize);
  glCompileShader(fs_handle);

  glAttachShader(self->program, vs_handle);
  glAttachShader(self->program, fs_handle);

  glBindAttribLocation(self->program, 0, "aPosition");
  glBindAttribLocation(self->program, 1, "aUV");

  free(vertex_source);
  free(fragment_source);

  glLinkProgram(self->program);

  GLint linked;
  char infolog[1000];
  glGetProgramiv(self->program, GL_LINK_STATUS, &linked);
  if (linked == 0) {
    glGetProgramInfoLog(self->program, 512, NULL, infolog);
    printf("%s", infolog);
    exit(EXIT_FAILURE);
  }
}

Shader shader_create(char *filepath) {
  Shader self;

  self.filepath = filepath;

  //*open file
  char *fullstr = _readFile(filepath);

  self.program = glCreateProgram();
  _compile(&self, fullstr);

  free(fullstr);

  return self;
}

void shader_bind(Shader *self) { glUseProgram(self->program); }
void shader_unbind(void) { glUseProgram(0); }
void shader_delete(Shader *self) { glDeleteProgram(self->program); }

void shader_setUniform1i(Shader *self, char *name, s32 val) {
  int loc = glGetUniformLocation(self->program, name);
  glUniform1i(loc, val);
}
void shader_setUniform1iv(Shader *self, char *name, s32 *val, u32 len) {
  int loc = glGetUniformLocation(self->program, name);
  glUniform1iv(loc, len, val);
}

void shader_setUniform1f(Shader *self, char *name, float val) {
  int loc = glGetUniformLocation(self->program, name);
  glUniform1f(loc, val);
}
void shader_setUniform2f(Shader *self, char *name, vec2s val) {
  int loc = glGetUniformLocation(self->program, name);
  glUniform2fv(loc, 1, &val.raw[0]);
}
void shader_setUniform3f(Shader *self, char *name, vec3s val) {
  int loc = glGetUniformLocation(self->program, name);
  glUniform3fv(loc, 1, &val.raw[0]);
}
void shader_setUniform4f(Shader *self, char *name, vec4s val) {
  int loc = glGetUniformLocation(self->program, name);
  glUniform4fv(loc, 1, &val.raw[0]);
}
void shader_setUniform_mat4(Shader *self, char *name, mat4s val) {
  int loc = glGetUniformLocation(self->program, name);
  glUniformMatrix4fv(loc, 1, GL_FALSE, val.raw[0]);
}
