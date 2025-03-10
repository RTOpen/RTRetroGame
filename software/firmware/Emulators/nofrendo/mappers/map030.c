/*
** Nofrendo (c) 1998-2000 Matthew Conte (matt@conte.com)
**
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of version 2 of the GNU Library General
** Public License as published by the Free Software Foundation.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.  To obtain a
** copy of the GNU Library General Public License, write to the Free
** Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Any permitted reproduction of these routines, in whole or in part,
** must bear this legend.
**
**
** map030.c: UNROM-512 mapper interface
**
*/

#include <nofrendo.h>
#include <mmc.h>

static bool switchable_screen = false;


static void map_write(uint32 address, uint8 value)
{
    if (switchable_screen)
    {
        ppu_setmirroring(PPU_MIRROR_SCR0 + (value >> 7));
    }

    mmc_bankrom(16, 0x8000, value & 0x1F);
    mmc_bankvrom(8, 0x0000, (value >> 5) & 0x03);
}

static void map_init(rom_t *cart)
{
    if (cart->mirroring == PPU_MIRROR_FOUR)
    {
        ppu_setmirroring(PPU_MIRROR_SCR0);
        switchable_screen = true;
    }

    mmc_bankrom(16, 0x8000, 0);
    mmc_bankrom(16, 0xC000, MMC_LASTBANK);
}

mapintf_t map30_intf =
{
    .number     = 30,
    .name       = "UNROM-512",
    .init       = map_init,
    .vblank     = NULL,
    .hblank     = NULL,
    .get_state  = NULL,
    .set_state  = NULL,
    .mem_write  = {
        { 0x8000, 0xFFFF, map_write }
    },
};
