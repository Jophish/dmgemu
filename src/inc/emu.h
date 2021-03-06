#ifndef EMU_H
#define EMU_H

#include "mmu.h"
#include "timer.h"
#include "debug.h"
#include "rom.h"
#include "cpu.h"
#include "gpu.h"

typedef struct emu {
  cpu z80;
  rom gb_rom;
  mmu gb_mmu;
  gpu gb_gpu;
  timer gb_timer;
  debugger gb_debugger;
} emu;

// Initializes internal structures of the emulator
void init_emu(emu *emu_p);

#endif /* EMU_H */
