#ifndef GAME_H
#define GAME_H

#include "gfx.h"

#define MAX_OBJECTS 10000

#define ADJUSTED_CURSOR_POS                                                    \
  (vec2s) {                                                                    \
    {                                                                          \
      (int)window.cursor_pos.x / (window.size.x / state.camera.orthoRight),    \
          (int)(window.cursor_pos.y - window.size.y) /                         \
              (window.size.y / state.camera.orthoTop) * -1                     \
    }                                                                          \
  }


enum Textures { TEX_GAME_SPRITES, TEX_BUTTONS, TEX_TEXTS, TEX_COUNT };
// index of the sprite
enum Sprites {
  SPRITE_BOMB_1 = 0,
  SPRITE_BOMB_2,
  SPRITE_FLAG,
  SPRITE_EXPLOSION,
  SPRITE_CELL_1 = 4,
  SPRITE_CELL_2,
  SPRITE_CELL_3,
  SPRITE_CELL_4,
  SPRITE_CELL_5,
  SPRITE_CELL_6,
  SPRITE_CELL_7,
  SPRITE_CELL_8,
  SPRITE_HAPPY_FACE,
  SPRITE_SAD_FACE,
  SPRITE_CELL_CLOSED,
  SPRITE_CELL_OPENED,
  SPRITE_EASY_1,
  SPRITE_EASY_2,
  SPRITE_NORM_1,
  SPRITE_NORM_2,
  SPRITE_HARD_1,
  SPRITE_HARD_2,
  SPRITE_RESET_1,
  SPRITE_RESET_2,
  SPRITE_COUNT = 24
};
enum Flags {
  FLAGS_ISCELL = 0,
  FLAGS_CELL_OPENED,
  FLAGS_CELL_HASBOMB,
  FLAGS_CELL_FLAGED,
  FLAGS_ISBUTTON,
  FLAGS_ISIMAGE,
  FLAGS_COUNT
};

typedef struct GameObject {
  vec3s position;
  vec2s size;
  SubTexture *sprite;
  u32 start_sprite_idx;
  size_t batchOffset;

  //*for cells only
  vec2s cell_idx;
  ivec2s adjacent_empty_cells[8];
  u32 aec_len;

  bool flags[FLAGS_COUNT];
} GameObject;

GameObject gameobject_create(vec3s pos, vec2s size, u32 sprite_idx,
                             bool *flags);
void gameobject_addToBatch(GameObject *self);
void gameObject_update(GameObject *self);

void gameobject_setSprite(GameObject *self, enum Sprites index);
void gameobject_setFlag(GameObject *self, enum Flags index, bool val);
bool gameobject_pointCollision(GameObject *obj, u32 x, u32 y);
bool gameobject_pointCollisionv(GameObject *obj, vec2s point);

void __check_Surrounding_cells(vec2s o, u32 *numBombs);

void __open_cell(GameObject *obj, u32 sprite);
void __open_cellv(ivec2s point, u32 sprite);

// struct the contains all the main parts of the game.
// combines many structs into one state.
typedef struct {
  Window *window;
  BatchRenderer renderer;
  Camera camera;
  Texture textureList[TEX_COUNT];
  SubTexture spriteList[SPRITE_COUNT];

  GameObject objList[MAX_OBJECTS];
  size_t objList_len;
  ivec2s board_size;
  vec2s board_offset;
  u32 numBombs;
  bool game_running;

} GameState;

extern GameState state;

void gameState_init(void);
void gameState_restart(u32 x, u32 y);
void gameState_update(void);
void gameState_render(void);
void gameState_delete(void);

GameObject *gamestate_getCell(u32 x, u32 y);
GameObject *gamestate_getCellv(ivec2s idx);

#endif /* !DEBUG */
