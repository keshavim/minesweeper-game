
#version 330 core
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec4 a_color;
layout (location = 2) in vec2 a_texCoords;
layout (location = 3) in float a_texid;

out vec4 v_color;
out vec2 v_texCoords;
out float v_texid;

uniform mat4 u_modelMat;
uniform mat4 u_viewMat;
uniform mat4 u_projMat;

void main() {
  v_color = a_color;
  v_texCoords = a_texCoords;
  v_texid = a_texid;

  gl_Position = u_projMat * u_viewMat * u_modelMat * vec4(a_position, 1.0); // see how we directly give a vec3 to vec4's constructor
}
//VERTEX_END

#version 330 core
out vec4 FragColor;

in vec4 v_color;
in vec2 v_texCoords;
in float v_texid;

uniform sampler2D u_textures[8];

void main() {
  int id = int(v_texid);
  if (id >= 0) {
    switch (id) {
      case 0:
        FragColor = texture(u_textures[0], v_texCoords);
        break;
      case 1:
        FragColor = texture(u_textures[1], v_texCoords);
        break;
      case 2:
        FragColor = texture(u_textures[2], v_texCoords);
        break;
      case 3:
        FragColor = texture(u_textures[3], v_texCoords);
        break;
      case 4:
        FragColor = texture(u_textures[4], v_texCoords);
        break;
      case 5:
        FragColor = texture(u_textures[5], v_texCoords);
        break;
      case 6:
        FragColor = texture(u_textures[6], v_texCoords);
        break;
      case 7:
        FragColor = texture(u_textures[7], v_texCoords);
        break;
    }
  } else {
    FragColor = v_color;

  }
} 
//FRAGMENT_END
