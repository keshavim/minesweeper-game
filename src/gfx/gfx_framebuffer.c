
#include "gfx.h"

void framebuffer_init(FrameBuffer *self) {
  *self =(FrameBuffer){0};
  glCreateFramebuffers(1, &self->fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, self->fbo);

  texture_createv(&self->texBuf, window.size);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         self->texBuf.id, 0);

  glGenRenderbuffers(1, &self->rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, self->rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, window.size.x,
                        window.size.y);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, self->rbo);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    DEBUG_LOG("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glCreateVertexArrays(1, &self->vao);
  glBindVertexArray(self->vao);

  glCreateBuffers(1, &self->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, self->vbo);
  glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), (float[]){
  -1.0f, -1.0f, 0.0f, 0.0f,
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  1.0f,1.0f,1.0f,1.0f,
  -1.0f, 1.0f, 0.0f, 1.0f, 
  -1.0f,-1.0f,0.0f, 0.0f 
  },
               GL_STATIC_DRAW);

  glEnableVertexArrayAttrib(self->vao, 0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)0);

  glBindVertexArray(0);
}
void framebuffer_render(FrameBuffer *self) {
  glBindVertexArray(self->vao);
  glDisable(GL_DEPTH_TEST);
  glBindTexture(GL_TEXTURE_2D, self->texBuf.id);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}
void framebuffer_delete(FrameBuffer *self){
  glDeleteFramebuffers(1, &self->fbo);
  glDeleteRenderbuffers(1, &self->rbo);
  glDeleteTextures(1, &self->texBuf.id);
  glDeleteBuffers(1, &self->vbo);
  glDeleteVertexArrays(1, &self->vao);
}

