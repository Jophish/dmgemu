#ifndef GPU_H
#define GPU_H

#include <stdint.h>
#include "mmu.h"

// Pixel lengths of screen and background
#define LCD_WIDTH 160
#define LCD_HEIGHT 144
#define BG_WIDTH 256
#define BG_HEIGHT 256

// Number of background blocks per section
#define BG_BLOCKS 1024
#define BG_BLOCKS_PER_ROW 32

// Masks for flag selection
#define BG_CODE_AREA_SELECT_FLAG 0b1000
#define BG_CHAR_DATA_SELECT_FLAG 0b10000

// Tile size
#define TILE_SIZE 0x10

// Pixels per tile
#define PX_PER_TILE 64
#define PX_PER_ROW 8

// Size of OAM sprite attribute entry
#define SPRITE_SIZE 0x4
#define NUM_SPRITES SZ_OAM/SPRITE_SIZE

// 'gpu.h' and 'gpu.c' contain methods and definitions relating to the GPU and LCD/display
// features of the emulator. The framebuffer contained in the gpu struct consists of exactly
// as many pixels as there are in the GB display. Each element in the array denotes the color
// of a single pixel. A '0' corresponds to the lightest shade, and a '3' corresponds to the darkest
// shade. These mappings are fixed, regardless of whatever palette is selected internally, in order
// to decouple the internals and the rendering.

// Forward declaration of emu
typedef struct emu emu;


typedef struct gpu_regs {
  uint8_t reg_lcdc,
    reg_stat,
    reg_scy,
    reg_scx,
    reg_ly,
    reg_lyc,
    reg_wy,
    reg_wx,
    reg_bgp,
    reg_obp0,
    reg_obp1,
    reg_dma;
} gpu_regs;

typedef struct oam_entry {
  uint8_t y_pos,
    x_pos,
    tile_no,
    y_flip,
    x_flip,
    palette_no,
    priority;
} oam_entry;

// OAM struct used while updating scanlines. Each struct
// represents a single pixel of some sprite that intersects
// with the current scanline.
typedef struct oam_update_entry {
  uint8_t line_x, // x coord on scanline where pixel intersects
    obj_no,       // OAM object number, used in calculating priority
    oam_x_coord,  // X coordinate of sprite attribute data, used in calculating priority
    px_col,       // Pixel color to write to scanline, adjusted for x/y reflections and palette
    priority;     // OBJ-to-BG priority
} oam_update_entry;

typedef struct gpu {
  uint8_t mode;
  uint32_t gpu_clock;
  uint16_t line;
  gpu_regs gb_gpu_regs;
  uint8_t framebuffer[LCD_HEIGHT][LCD_WIDTH];
  uint8_t tileset[SZ_CHAR_RAM/TILE_SIZE][PX_PER_ROW][PX_PER_ROW]; // One byte per pixel. Inefficient, but convenient
  oam_entry sprites[NUM_SPRITES];
} gpu;

// Steps the GPU
int step_gpu(emu *gb_emu_p);

// Given a tile number, updates the tileset to reflect the contents of the given tile in char RAM.
// This should be called whenever a write occurs to the char RAM region.
int update_tileset(emu *gb_emu_p, uint16_t addr, uint8_t val);

// Draws a scanline to the framebuffer corresponding to the current contents of the LY register
int draw_scanline(emu *gb_emu_p);

// Given a BGP palette and a pixel, returns the translated pixel
int translate_palette_px(uint8_t bgp_val, uint8_t px);

// Given an x, y coordinate, returns the corresponding pixel in the background
int coord_to_bg_pixel(emu *gb_emu_p, uint16_t x, uint16_t y);

// Initializes the GPU with default values
void init_gpu(gpu *gb_gpu_p);

// Writes val to the GPU register at the given address. If the given address
// doesn't correspond to a valid GPU register, returns ERR_INVALID_ADDRESS.
int write_gpu_reg(emu *gb_emu_p, uint16_t addr, uint8_t val);

// Reads val from the GPU register at the given address. If the given address doesn't
// correspond to a valid GPU register, returns ERR_INVALID_ADDRESS.
int read_gpu_reg(gpu *gb_gpu_p, uint16_t addr);

// Updates the LCD buffer after end of VRAM read mode.
int update_buffer(emu *gb_emu_p);

// Given raw tile data and a pixel number (counting from top left to bottom right),
// returns the parsed value
int tile_data_to_px(uint8_t *tile_data_p, uint8_t px_num);

// Given an address in the OAM region, writes val to the oam buffer of the GPU,
// and updates the sprites buffer in the GPU appropriately
int write_oam(emu *gb_emu_p, uint16_t addr, uint8_t val);

#endif /* GPU_H */
