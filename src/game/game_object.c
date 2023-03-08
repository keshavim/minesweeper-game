#include "game.h"
#include "gfx.h"

static f32 cell_open_Timer = 2.5f;

GameObject gameobject_create(vec3s pos, vec2s size, enum Sprites sprite_idx,
                             bool *flags) {
  GameObject self = {
      .position = pos, .size = size, .sprite = &state.spriteList[sprite_idx]};
  for (u32 i = 0; i < FLAGS_COUNT; i++) {
    self.flags[i] = flags[i];
  }
  return self;
}
void gameobject_addToBatch(GameObject *self) {
  self->batchOffset = state.renderer.numObjects * 4;
  batcher_addToBatch(&state.renderer, self->position, self->size, self->sprite);
}
void gameObject_update(GameObject *self) {
  batcher_replaceInBatch(&state.renderer, self->batchOffset, self->position,
                         self->size, self->sprite);

  if (window.mouse[GLFW_MOUSE_BUTTON_LEFT].down) {
    vec2s cursor_pos = ADJUSTED_CURSOR_POS;
    if (!gameobject_pointCollisionv(self, cursor_pos))
      return;

    if (self->flags[FLAGS_ISCELL] && state.game_running) {
      u32 numBombs = 0;
      __check_Surrounding_cells(self->cell_idx, &numBombs);

      u32 sprite =
          numBombs > 0 ? SPRITE_CELL_1 + numBombs - 1 : SPRITE_CELL_OPENED;

      __open_cell(self, sprite);
    }
  }

  if (window.mouse[GLFW_MOUSE_BUTTON_RIGHT].down) {
    vec2s cursor_pos = ADJUSTED_CURSOR_POS;
    if (!gameobject_pointCollisionv(self, cursor_pos))
      return;
    __open_cell(self, SPRITE_FLAG);
    return;
  }
}

bool gameobject_pointCollision(GameObject *obj, u32 x, u32 y) {
  vec2s min = glms_vec2(obj->position);
  vec2s max = glms_vec2_add(min, obj->size);
  return (x <= max.x && x >= min.x && y <= max.y && y >= min.y);
}
bool gameobject_pointCollisionv(GameObject *obj, vec2s point) {
  vec2s min = glms_vec2(obj->position);
  vec2s max = glms_vec2_add(min, obj->size);

  return (point.x <= max.x && point.x >= min.x && point.y <= max.y &&
          point.y >= min.y);
}
void gameobject_setSprite(GameObject *self, enum Sprites index) {
  self->sprite = &state.spriteList[index];
}
void gameobject_setFlag(GameObject *self, enum Flags index, bool val) {
  self->flags[index] = val;
}

static ivec4s _get_offset(ivec2s origin) {
  s32 w = state.board_size.x;
  s32 h = state.board_size.y;

  ivec4s offset = {{-1, -1, -1, -1}};

  if (origin.x == 0 && origin.y == 0) { // BL
    offset = (ivec4s){{1, 0, -1, 1}};
  } else if (origin.x == 0 && origin.y == h - 1) { // TL
    offset = (ivec4s){{0, -1, 1, 1}};
  } else if (origin.x == w - 1 && origin.y == h - 1) { // TR
    offset = (ivec4s){{-1, 0, 1, -1}};
  } else if (origin.x == w - 1 && origin.y == 0) { // BR
    offset = (ivec4s){{0, 1, -1, -1}};
    // sizes
  } else if (origin.x == 0) { // L
    offset = (ivec4s){{0, -1, -1, 1}};
  } else if (origin.y == 0) { // B
    offset = (ivec4s){{1, 0, -1, -1}};
  } else if (origin.x == w - 1) { // R
    offset = (ivec4s){{0, 1, 1, -1}};
  } else if (origin.y == h - 1) { // T
    offset = (ivec4s){{-1, 0, 1, 1}};
  }

  return offset;
}
// cell specific functions
void __check_Surrounding_cells(vec2s o, u32 *numBombs) {
  ivec2s origin = {{(int)o.x, (int)o.y}};
  GameObject *origin_obj = gamestate_getCellv(origin);

  ivec4s offset = _get_offset(origin);

  ivec2s soff = {{offset.x, offset.y}};
  ivec2s eoff = {{offset.z, offset.w}};

  ivec2s start_pos = {{origin.x + soff.x, origin.y + soff.y}};
  ivec2s end_pos = {{origin.x + eoff.x, origin.y + eoff.y}};
  ivec2s cur_pos = start_pos;

  ivec2s dir = {0};
  // starting off set
  // not nesacery for the conners
  if (soff.x == -1 && soff.y == 0)
    dir.y = -1;
  else if (soff.x == 1 && soff.y == 0)
    dir.y = 1;
  else if (soff.x == 0 && soff.y == -1)
    dir.x = 1;
  else if (soff.x == 0 && soff.y == 1)
    dir.x = -1;

  // using a do while so it can start at end at the same offsets
  do {
    GameObject *o = gamestate_getCellv(cur_pos);

    if (numBombs != NULL && o->flags[FLAGS_CELL_HASBOMB]) {
      (*numBombs)++;
    } else if (numBombs == NULL) {
      u32 num = 0;
      __check_Surrounding_cells((vec2s){{cur_pos.x, cur_pos.y}}, &num);
      if (num == 0) {
        origin_obj->adjacent_empty_cells[origin_obj->aec_len++] = cur_pos;
      }
    }
    // changes the direction of movement
    if (cur_pos.x - origin.x == -1 && cur_pos.y - origin.y == -1)
      dir = (ivec2s){{1, 0}};
    else if (cur_pos.x - origin.x == 1 && cur_pos.y - origin.y == -1)
      dir = (ivec2s){{0, 1}};
    else if (cur_pos.x - origin.x == 1 && cur_pos.y - origin.y == 1)
      dir = (ivec2s){{-1, 0}};
    else if (cur_pos.x - origin.x == -1 && cur_pos.y - origin.y == 1)
      dir = (ivec2s){{0, -1}};

    cur_pos.x += dir.x;
    cur_pos.y += dir.y;

  } while (!(cur_pos.x == end_pos.x && cur_pos.y == end_pos.y));
}

void __open_cell(GameObject *self, u32 sprite) {
  if (self->flags[FLAGS_CELL_OPENED] || self->flags[FLAGS_CELL_FLAGED])
    return;

  if (self->flags[FLAGS_CELL_HASBOMB]) {
    gameobject_setSprite(self, SPRITE_BOMB_1);
    state.game_running = false;
    return;
  }

  if (sprite == SPRITE_FLAG) {
    gameobject_setSprite(self, sprite);
    gameobject_setFlag(self, FLAGS_CELL_FLAGED, true);
  }

  gameobject_setSprite(self, sprite);
  gameobject_setFlag(self, FLAGS_CELL_OPENED, true);

  if (sprite == SPRITE_CELL_OPENED)
    for (int i = 0; i < self->aec_len; i++) {
      __open_cellv(self->adjacent_empty_cells[i], sprite);
    }
}
void __open_cellv(ivec2s point, u32 sprite) {
  GameObject *obj = gamestate_getCellv(point);
  if (obj->flags[FLAGS_CELL_OPENED] || obj->flags[FLAGS_CELL_FLAGED] ||
      obj->flags[FLAGS_CELL_HASBOMB]) {
    return;
  }

  gameobject_setSprite(obj, sprite);
  gameobject_setFlag(obj, FLAGS_CELL_OPENED, true);

  if (sprite == SPRITE_CELL_OPENED) {
    for (int i = 0; i < obj->aec_len; i++) {
      __open_cellv(obj->adjacent_empty_cells[i], SPRITE_CELL_OPENED);
    }
  }
}
