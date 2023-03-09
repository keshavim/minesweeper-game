#include "game.h"


GameState state;

static void __createBoard(size_t width, size_t height) {
  bool flags[FLAGS_COUNT] = {0};
  flags[FLAGS_ISCELL] = true;
  float scalex = (state.camera.orthoRight / 100);
  float scaley = (state.camera.orthoTop / 100);
  state.board_size = (ivec2s){{width, height}};
  state.board_offset = (vec2s){{2 * scalex, 10 *scaley}};

  for (u32 i = 0; i < width; i++) {
    for (u32 j = 0; j < height; j++) {
      flags[FLAGS_CELL_HASBOMB] = random(0, 60) < 10 ? true : false;

      u32 sp = SPRITE_CELL_CLOSED;

      state.numBombs += flags[FLAGS_CELL_HASBOMB];

      state.objList[state.objList_len] =
          gameobject_create((vec3s){{state.board_offset.x + i * 5,
                                     state.board_offset.y + j * 5, 0.0}},
                            (vec2s){{5, 5}}, sp, flags);
      gameobject_addToBatch(&state.objList[state.objList_len]);

      state.objList[state.objList_len].cell_idx = (vec2s){{i, j}};

      state.objList_len++;
    }
  }
  //*gets the adjacent empty cells
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++)
      __check_Surrounding_cells((vec2s){{i, j}}, NULL);
  }
  state.game_running = true;
  
}

static void __createOtherObj(void) {
  bool flags[FLAGS_COUNT] = {0};
  flags[FLAGS_ISBUTTON] = true;

  float scalex = (state.camera.orthoRight / 100);
  float scaley = (state.camera.orthoTop / 100);

  for (u32 i = 0; i < 3; i++) {
    state.objList[state.objList_len] = gameobject_create(
        (vec3s){{(5 *scalex) + (i * (25 * scalex)), 0, 0.0}},
        (vec2s){{20 * scalex, 10 * scaley}}, SPRITE_EASY_1 + (i * 2), flags);
    gameobject_addToBatch(&state.objList[state.objList_len]);
    state.objList_len++;
  }
  flags[FLAGS_ISBUTTON] = false;
  flags[FLAGS_ISIMAGE] = true;
  state.objList[state.objList_len] = gameobject_create(
        (vec3s){{87 * scalex, 50 * scaley, 0.0}},
        (vec2s){{8 * scalex, 8 * scaley}}, SPRITE_HAPPY_FACE, flags);
    gameobject_addToBatch(&state.objList[state.objList_len]);
    state.objList_len++;
}

static void __create_subtextures(void) {
  // creating all subtextures
  u32 subnumBombs = 0;
  for (int j = 3; j >= 0; j--) {
    for (int i = 0; i < 4; i++) {
      state.spriteList[subnumBombs] =
          subTexture_create(&state.textureList[TEX_GAME_SPRITES],
                            (vec2s){{i, j}}, (vec2s){{16, 16}});
      subnumBombs++;
    }
  }

  // creating all subtextures
  for (int j = 3; j >= 0; j--) {
    for (int i = 0; i < 2; i++) {
      state.spriteList[subnumBombs] = subTexture_create(
          &state.textureList[TEX_BUTTONS], (vec2s){{i, j}}, (vec2s){{32, 16}});
      subnumBombs++;
    }
  }
}

void gameState_init(void) {
  state = (GameState){0};

  srand(time(NULL));

  state.window = &window;
  batcher_init(&state.renderer, MAX_OBJECTS);
  camera_init(&state.camera, (vec3s){{0, 0, -1}});
  texture_load(&state.textureList[TEX_GAME_SPRITES],
               "assets/images/minesweeper.png");
  texture_load(&state.textureList[TEX_BUTTONS], "assets/images/buttons.png");
  texture_load(&state.textureList[TEX_TEXTS], "assets/images/font-map.png");

  shader_bind(&state.renderer.shaders[SHADER_PRE]);
  s32 samplers[8] = {0, 1, 2, 3, 4, 5, 6, 7};
  shader_setUniform1iv(&state.renderer.shaders[SHADER_PRE], "u_textures",
                       samplers, 8);
  shader_unbind();

  // creating all subtextures
  __create_subtextures();

  __createBoard(16, 16);
  __createOtherObj();
}
void gameState_restart(u32 x, u32 y) {
  batcher_clear(&state.renderer);
  memset(&state.objList, 0, state.objList_len);
  state.objList_len = 0;
  state.numBombs = 0;

  __createBoard(x, y);
  __createOtherObj();
}

void gameState_update(void) {
  camera_update(&state.camera);

  for (size_t i = 0; i < state.objList_len; i++) {
    gameObject_update(&state.objList[i]);
  }
}
void gameState_render(void) { batcher_render(&state.renderer); }
void gameState_delete(void) { batcher_delete(&state.renderer); }

GameObject *gamestate_getCell(u32 x, u32 y) {
  return &state.objList[y + state.board_size.x * x];
}
GameObject *gamestate_getCellv(ivec2s idx) {
  return &state.objList[((s32)idx.y) + state.board_size.x * ((s32)idx.x)];
}
