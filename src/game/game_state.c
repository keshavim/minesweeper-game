#include "game.h"
#include "gfx.h"

GameState state;

static void __createBoard(size_t width, size_t height) {
  bool flags[FLAGS_COUNT] = {0};
  flags[FLAGS_ISCELL] = true;
  state.board_size = (ivec2s){{width, height}};
  state.board_offset = (ivec2s){{10, 10}};

  for (u32 i = 0; i < width; i++) {
    for (u32 j = 0; j < height; j++) {
      flags[FLAGS_CELL_HASBOMB] = random(0, 150) < 10 ? true : false;

      u32 sp = SPRITE_CELL_CLOSED;

      state.numBombs += flags[FLAGS_CELL_HASBOMB];
      

      state.objList[state.objList_len] =
          gameobject_create((vec3s){{state.board_offset.x + i * 5,
                                     state.board_offset.y + j * 5, 0.0}},
                            (vec2s){{5, 5}}, sp, flags);
      gameobject_addToBatch(&state.objList[state.objList_len]);

      state.objList[state.objList_len].cell_idx = (vec2s){{i,j}};
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

static void __createOtherObj(void) {}

void gameState_init(void) {
  state = (GameState){0};

  srand(time(NULL));

  state.window = &window;
  batcher_init(&state.renderer, MAX_OBJECTS);
  camera_init(&state.camera, (vec3s){{0, 0, -1}});
  texture_load(&state.texture, "assets/images/minesweeper.png");

  shader_bind(&state.renderer.shader);
  s32 samplers[8] = {0, 1, 2, 3, 4, 5, 6, 7};
  shader_setUniform1iv(&state.renderer.shader, "u_textures", samplers, 8);

  // creating all subtextures
  u32 subnumBombs = 0;
  for (int j = 3; j >= 0; j--) {
    for (int i = 0; i < 4; i++) {
      state.spriteList[subnumBombs] =
          subTexture_create(&state.texture, (vec2s){{i, j}}, (vec2s){{16, 16}});
      subnumBombs++;
    }
  }
  __createBoard(16, 16);
}
void gameState_restart(u32 x, u32 y) {
  batcher_clear(&state.renderer);
  memset(&state.objList, 0, state.objList_len);
  state.objList_len = 0;
  state.numBombs = 0;

  __createBoard(x, y);
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
GameObject *gamestate_getCellv(ivec2s idx){
  return &state.objList[((s32)idx.y) + state.board_size.x * ((s32)idx.x)];

}

