/*
    System 16 / 18 bootlegs

    Sega's System 16 and System 18 hardware was heavily bootlegged.

    Most of these bootlegs had significant modifications compared to the real hardware.
    They still roughly reflect which system they have been bootlegged from however.


    Bootlegs

    Type 16B:
    --------

    Encrypted / Protected bootlegs
     - Bay Route (set 1)
     - Golden Axe (set 1)

    These share a common encryption, Bay Route is also proteceted, the Golden Axe set has a strange
    unknown rom, maybe it's related to an MCU that isn't present on the GA board?

    ---

    Datsu bootlegs
     - Bay Route (set 2)
     - Flash Point (2 sets)
     - Dynamite Dux
     - Tough Turf

    The tilemap page select writes have been split across 4 8-bit registers on these

    ---

    Other bootegs
     - Tetris
     - E-Swat

    These appear to be a variation no the encrypted / protected bootlegs, but without the encryption
    or protection

    - Golden Axe (set 2)

    Unique bootleg, tilemap paging is split across 8 registers, bits are inverted etc.

    ---

    Type 16A:
    ---------

    Shinobi
    Passing Shot (2 sets)
    Wonderboy 3

    System 18 (more commplex tilemaps)
    ----------------------------------

    Alien Storm
    Shadow Dancer
    Moonwalker



    AMT Games
    ---------

    these aren't strictly bootlegs, but are clearly based on bootleg Tetris hardware with additional protection
    - Beauty Block
    - IQ Pipe



    ToDo
    ----

    Fully fix the tilemap banking and scrolling for all sets
    Fix sprites (and check for bad roms)
    Add support for custom sound HW used by the various bootlegs
    Look at the system18 bootlegs

    Partially Done
    --------------

    Strip out old hacks & obsolete comments (many related to the *original* system16/18 sets which have their own
    driver now)


*/

#include "emu.h"
#include "cpu/z80/z80.h"
#include "includes/system16.h"
#include "cpu/m68000/m68000.h"
#include "sound/msm5205.h"
#include "sound/2151intf.h"
#include "sound/upd7759.h"
#include "sound/2612intf.h"
#include "sound/rf5c68.h"
#include "video/segaic16.h"

#define SHADOW_COLORS_MULTIPLIER 2


static INTERRUPT_GEN( sys16_interrupt )
{
	cpu_set_input_line(device, 4, HOLD_LINE); /* Interrupt vector 4, used by VBlank */
}


/***************************************************************************/

static WRITE16_HANDLER( sound_command_nmi_w )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;

	if (ACCESSING_BITS_0_7)
	{
		soundlatch_w(space, 0, data & 0xff);
		cpu_set_input_line(state->soundcpu, INPUT_LINE_NMI, PULSE_LINE);
	}
}


static ADDRESS_MAP_START( shinobib_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x03ffff) AM_ROM
	AM_RANGE(0x400000, 0x40ffff) AM_RAM // tilemap ram on the original, used as a buffer on the bootlegs
	AM_RANGE(0x410000, 0x410fff) AM_RAM AM_BASE_MEMBER(segas1x_bootleg_state, textram)
	AM_RANGE(0x411000, 0x411fff) AM_RAM AM_BASE_MEMBER(segas1x_bootleg_state, bg0_tileram)
	AM_RANGE(0x412000, 0x412fff) AM_RAM AM_BASE_MEMBER(segas1x_bootleg_state, bg1_tileram)
	AM_RANGE(0x440000, 0x440fff) AM_RAM AM_BASE(&segaic16_spriteram_0)
	AM_RANGE(0x840000, 0x840fff) AM_RAM_WRITE(segaic16_paletteram_w)  AM_BASE(&segaic16_paletteram)
	AM_RANGE(0xc40000, 0xc40001) AM_WRITE(sound_command_nmi_w)
	AM_RANGE(0xc41000, 0xc41001) AM_READ_PORT("SERVICE")
	AM_RANGE(0xc41002, 0xc41003) AM_READ_PORT("P1")
	AM_RANGE(0xc41006, 0xc41007) AM_READ_PORT("P2")
	AM_RANGE(0xc42000, 0xc42001) AM_READ_PORT("DSW1")
	AM_RANGE(0xc42002, 0xc42003) AM_READ_PORT("DSW2")
	AM_RANGE(0xC43000, 0xC43001) AM_WRITENOP
	AM_RANGE(0xC44000, 0xC44001) AM_WRITENOP
	AM_RANGE(0xc46000, 0xc46001) AM_WRITE(s16a_bootleg_bgscrolly_w)
	AM_RANGE(0xc46002, 0xc46003) AM_WRITE(s16a_bootleg_bgscrollx_w)
	AM_RANGE(0xc46004, 0xc46005) AM_WRITE(s16a_bootleg_fgscrolly_w)
	AM_RANGE(0xc46006, 0xc46007) AM_WRITE(s16a_bootleg_fgscrollx_w)
	AM_RANGE(0xc46008, 0xc46009) AM_WRITE(s16a_bootleg_tilemapselect_w)
	AM_RANGE(0xC60000, 0xC60001) AM_READNOP
	AM_RANGE(0xffc000, 0xffffff) AM_RAM // work ram
ADDRESS_MAP_END

/***************************************************************************/

static WRITE16_HANDLER( sound_command_w )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;

	if (ACCESSING_BITS_0_7)
	{
		soundlatch_w(space, 0, data & 0xff);
		cpu_set_input_line(state->soundcpu, 0, HOLD_LINE);
	}
}

static WRITE16_HANDLER( sys16_coinctrl_w )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;

	if (ACCESSING_BITS_0_7)
	{
		state->coinctrl = data & 0xff;
		state->refreshenable = state->coinctrl & 0x20;
		set_led_status(space->machine, 1, state->coinctrl & 0x08);
		set_led_status(space->machine, 0, state->coinctrl & 0x04);
		coin_counter_w(space->machine, 1, state->coinctrl & 0x02);
		coin_counter_w(space->machine, 0, state->coinctrl & 0x01);
		/* bit 6 is also used (1 most of the time; 0 in dduxbl, sdi, wb3;
           tturf has it normally 1 but 0 after coin insertion) */
		/* eswat sets bit 4 */
	}
}

static ADDRESS_MAP_START( passshtb_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x01ffff) AM_ROM

	AM_RANGE(0x400000, 0x407fff) AM_RAM // tilemap ram on original, buffer on bootleg
	AM_RANGE(0x409000, 0x409fff) AM_RAM AM_BASE_MEMBER(segas1x_bootleg_state, bg0_tileram)
	AM_RANGE(0x40a000, 0x40afff) AM_RAM AM_BASE_MEMBER(segas1x_bootleg_state, bg1_tileram)
	AM_RANGE(0x410000, 0x410fff) AM_RAM AM_BASE_MEMBER(segas1x_bootleg_state, textram)

	AM_RANGE(0x440000, 0x440fff) AM_RAM AM_BASE(&segaic16_spriteram_0)
	AM_RANGE(0x840000, 0x840fff) AM_RAM_WRITE(segaic16_paletteram_w) AM_BASE(&segaic16_paletteram)
	AM_RANGE(0xc40000, 0xc40001) AM_WRITE(sys16_coinctrl_w)
	AM_RANGE(0xc41002, 0xc41003) AM_READ_PORT("P1")
	AM_RANGE(0xc41004, 0xc41005) AM_READ_PORT("P2")
	AM_RANGE(0xc41000, 0xc41001) AM_READ_PORT("SERVICE")
	AM_RANGE(0xc42002, 0xc42003) AM_READ_PORT("DSW1")
	AM_RANGE(0xc42000, 0xc42001) AM_READ_PORT("DSW2")
	AM_RANGE(0xc42006, 0xc42007) AM_WRITE(sound_command_w)
	AM_RANGE(0xc46000, 0xc46001) AM_WRITE(s16a_bootleg_bgscrolly_w)
	AM_RANGE(0xc46002, 0xc46003) AM_WRITE(s16a_bootleg_bgscrollx_w)
	AM_RANGE(0xc46004, 0xc46005) AM_WRITE(s16a_bootleg_fgscrolly_w)
	AM_RANGE(0xc46006, 0xc46007) AM_WRITE(s16a_bootleg_fgscrollx_w)
	AM_RANGE(0xc46008, 0xc46009) AM_WRITE(s16a_bootleg_tilemapselect_w)

	AM_RANGE(0xffc000, 0xffffff) AM_RAM // work ram
ADDRESS_MAP_END

/***************************************************************************/

static READ16_HANDLER( passht4b_service_r )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;
	UINT16 val = input_port_read(space->machine, "SERVICE");

	if(!(input_port_read(space->machine, "P1") & 0x40)) val &= 0xef;
	if(!(input_port_read(space->machine, "P2") & 0x40)) val &= 0xdf;
	if(!(input_port_read(space->machine, "P3") & 0x40)) val &= 0xbf;
	if(!(input_port_read(space->machine, "P4") & 0x40)) val &= 0x7f;

	state->passht4b_io3_val = (input_port_read(space->machine, "P1") << 4) | (input_port_read(space->machine, "P3") & 0xf);
	state->passht4b_io2_val = (input_port_read(space->machine, "P2") << 4) | (input_port_read(space->machine, "P4") & 0xf);

	state->passht4b_io1_val = 0xff;

	// player 1 buttons
	if(!(input_port_read(space->machine, "P1") & 0x10)) state->passht4b_io1_val &= 0xfe;
	if(!(input_port_read(space->machine, "P1") & 0x20)) state->passht4b_io1_val &= 0xfd;
	if(!(input_port_read(space->machine, "P1") & 0x80)) state->passht4b_io1_val &= 0xfc;

	// player 2 buttons
	if(!(input_port_read(space->machine, "P2") & 0x10)) state->passht4b_io1_val &= 0xfb;
	if(!(input_port_read(space->machine, "P2") & 0x20)) state->passht4b_io1_val &= 0xf7;
	if(!(input_port_read(space->machine, "P2") & 0x80)) state->passht4b_io1_val &= 0xf3;

	// player 3 buttons
	if(!(input_port_read(space->machine, "P3") & 0x10)) state->passht4b_io1_val &= 0xef;
	if(!(input_port_read(space->machine, "P3") & 0x20)) state->passht4b_io1_val &= 0xdf;
	if(!(input_port_read(space->machine, "P3") & 0x80)) state->passht4b_io1_val &= 0xcf;

	// player 4 buttons
	if(!(input_port_read(space->machine, "P4") & 0x10)) state->passht4b_io1_val &= 0xbf;
	if(!(input_port_read(space->machine, "P4") & 0x20)) state->passht4b_io1_val &= 0x7f;
	if(!(input_port_read(space->machine, "P4") & 0x80)) state->passht4b_io1_val &= 0x3f;

	return val;
}

static READ16_HANDLER( passht4b_io1_r )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;
	return state->passht4b_io1_val;
}

static READ16_HANDLER( passht4b_io2_r )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;
	return state->passht4b_io2_val;
}

static READ16_HANDLER( passht4b_io3_r )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;
	return state->passht4b_io3_val;
}

static ADDRESS_MAP_START( passht4b_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x01ffff) AM_ROM
	AM_RANGE(0x400000, 0x407fff) AM_RAM // tilemap ram on original, buffer on bootleg
	AM_RANGE(0x409000, 0x40afff) AM_RAM AM_BASE_MEMBER(segas1x_bootleg_state, bg0_tileram)
	AM_RANGE(0x40a000, 0x40bfff) AM_RAM AM_BASE_MEMBER(segas1x_bootleg_state, bg1_tileram)
	AM_RANGE(0x410000, 0x410fff) AM_RAM AM_BASE_MEMBER(segas1x_bootleg_state, textram)
	AM_RANGE(0x440000, 0x440fff) AM_RAM AM_BASE(&segaic16_spriteram_0)
	AM_RANGE(0x840000, 0x840fff) AM_RAM_WRITE(segaic16_paletteram_w)  AM_BASE(&segaic16_paletteram)
	AM_RANGE(0xc41000, 0xc41001) AM_READ(passht4b_service_r)
	AM_RANGE(0xc41002, 0xc41003) AM_READ(passht4b_io1_r)
	AM_RANGE(0xc41004, 0xc41005) AM_READ(passht4b_io2_r)
	AM_RANGE(0xc41006, 0xc41007) AM_READ(passht4b_io3_r)
	AM_RANGE(0xc42000, 0xc42001) AM_READ_PORT("DSW2")
	AM_RANGE(0xc42002, 0xc42003) AM_READ_PORT("DSW1")
	AM_RANGE(0xc42006, 0xc42007) AM_WRITE(sound_command_w)
	AM_RANGE(0xc43000, 0xc43001) AM_READ_PORT("P1")		// test mode only
	AM_RANGE(0xc43002, 0xc43003) AM_READ_PORT("P2")
	AM_RANGE(0xc43004, 0xc43005) AM_READ_PORT("P3")
	AM_RANGE(0xc43006, 0xc43007) AM_READ_PORT("P4")
	AM_RANGE(0xc4600a, 0xc4600b) AM_WRITE(sys16_coinctrl_w)	/* coin counter doesn't work */
	AM_RANGE(0xc46000, 0xc46001) AM_WRITE(s16a_bootleg_bgscrolly_w)
	AM_RANGE(0xc46002, 0xc46003) AM_WRITE(s16a_bootleg_bgscrollx_w)
	AM_RANGE(0xc46004, 0xc46005) AM_WRITE(s16a_bootleg_fgscrolly_w)
	AM_RANGE(0xc46006, 0xc46007) AM_WRITE(s16a_bootleg_fgscrollx_w)
	AM_RANGE(0xc46008, 0xc46009) AM_WRITE(s16a_bootleg_tilemapselect_w)

	AM_RANGE(0xffc000, 0xffffff) AM_RAM // work ram
ADDRESS_MAP_END

/***************************************************************************/

static WRITE16_HANDLER( sys16_tilebank_w )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;

	if (ACCESSING_BITS_0_7)
	{
		switch (offset & 1)
		{
			case 0:
				state->tile_bank0 = data & 0x0f;
				break;
			case 1:
				state->tile_bank1 = data & 0x0f;
				break;
		}
	}
}

static ADDRESS_MAP_START( wb3bbl_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x03ffff) AM_ROM
	AM_RANGE(0x3f0000, 0x3fffff) AM_WRITE(sys16_tilebank_w)
	AM_RANGE(0x400000, 0x407fff) AM_RAM // tilemap ram on the original, used as a buffer on the bootlegs
	AM_RANGE(0x409000, 0x40afff) AM_RAM AM_BASE_MEMBER(segas1x_bootleg_state, bg0_tileram)
	AM_RANGE(0x40a000, 0x40bfff) AM_RAM AM_BASE_MEMBER(segas1x_bootleg_state, bg1_tileram)
	AM_RANGE(0x410000, 0x410fff) AM_RAM AM_BASE_MEMBER(segas1x_bootleg_state, textram)
	AM_RANGE(0x440000, 0x440fff) AM_RAM AM_BASE(&segaic16_spriteram_0)
	AM_RANGE(0x840000, 0x840fff) AM_RAM_WRITE(segaic16_paletteram_w) AM_BASE(&segaic16_paletteram)
	AM_RANGE(0xc40000, 0xc40001) AM_WRITE(sys16_coinctrl_w)
	AM_RANGE(0xc41000, 0xc41001) AM_READ_PORT("SERVICE")
	AM_RANGE(0xc41002, 0xc41003) AM_READ_PORT("P1")
	AM_RANGE(0xc41004, 0xc41005) AM_READ_PORT("P2")
	AM_RANGE(0xc42000, 0xc42001) AM_READ_PORT("DSW2")
	AM_RANGE(0xc42002, 0xc42003) AM_READ_PORT("DSW1")
	AM_RANGE(0xc42006, 0xc42007) AM_WRITE(sound_command_w)
	AM_RANGE(0xC44000, 0xC44001) AM_WRITENOP
	AM_RANGE(0xc46000, 0xc46001) AM_WRITE(s16a_bootleg_bgscrolly_w)
	AM_RANGE(0xc46002, 0xc46003) AM_WRITE(s16a_bootleg_bgscrollx_w)
	AM_RANGE(0xc46004, 0xc46005) AM_WRITE(s16a_bootleg_fgscrolly_w)
	AM_RANGE(0xc46006, 0xc46007) AM_WRITE(s16a_bootleg_fgscrollx_w)
	AM_RANGE(0xc46008, 0xc46009) AM_WRITE(s16a_bootleg_tilemapselect_w)
	AM_RANGE(0xff0000, 0xffffff) AM_RAM // work ram
ADDRESS_MAP_END

/***************************************************************************

    Tough Turf (Datsu bootleg) sound emulation

    Memory map

    0000-7fff : ROM (fixed, tt014d68 0000-7fff)
    8000-bfff : ROM (banked)
    e000      : Bank control
    e800      : Sound command latch
    f000      : MSM5205 sample data buffer
    f800-ffff : Work RAM

    Interrupts

    IRQ = Read sound command from $E800
    NMI = Copy data from fixed/banked ROM to $F000

    Bank control values

    00 = tt014d68 8000-bfff
    01 = tt014d68 c000-ffff
    02 = tt0246ff 0000-3fff
    03 = tt0246ff 4000-7fff
    04 = tt0246ff 8000-bfff

    The sample sound codes in the sound test are OK, but in-game sample playback is bad.
    There seems to be more data in the high bits of the ROM bank control word which may be related.
***************************************************************************/

static WRITE8_HANDLER( tturfbl_msm5205_data_w )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;
	state->sample_buffer = data;
}

static void tturfbl_msm5205_callback( running_device *device )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)device->machine->driver_data;
	msm5205_data_w(device, (state->sample_buffer >> 4) & 0x0f);

	state->sample_buffer <<=  4;
	state->sample_select ^=  1;
	if(state->sample_select == 0)
		cpu_set_input_line(state->soundcpu, INPUT_LINE_NMI, PULSE_LINE);
}

static const msm5205_interface tturfbl_msm5205_interface  =
{
	tturfbl_msm5205_callback,
	MSM5205_S48_4B
};


static READ8_HANDLER( tturfbl_soundbank_r )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;

	if (state->soundbank_ptr)
		return state->soundbank_ptr[offset & 0x3fff];
	return 0x80;
}

static WRITE8_HANDLER( tturfbl_soundbank_w )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;
	UINT8 *mem = memory_region(space->machine, "soundcpu");

	switch(data)
	{
		case 0:
			state->soundbank_ptr = &mem[0x18000]; /* tt014d68 8000-bfff */
			break;
		case 1:
			state->soundbank_ptr = &mem[0x1c000]; /* tt014d68 c000-ffff */
			break;
		case 2:
			state->soundbank_ptr = &mem[0x20000]; /* tt0246ff 0000-3fff */
			break;
		case 3:
			state->soundbank_ptr = &mem[0x24000]; /* tt0246ff 4000-7fff */
			break;
		case 4:
			state->soundbank_ptr = &mem[0x28000]; /* tt0246ff 8000-bfff */
			break;
		case 8:
			state->soundbank_ptr = mem;
			break;
		default:
			state->soundbank_ptr = NULL;
			logerror("Invalid bank setting %02X (%04X)\n", data, cpu_get_pc(space->cpu));
			break;
	}
}

static ADDRESS_MAP_START(tturfbl_sound_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0x8000, 0xbfff) AM_READ(tturfbl_soundbank_r)
	AM_RANGE(0xe000, 0xe000) AM_WRITE(tturfbl_soundbank_w)
	AM_RANGE(0xe800, 0xe800) AM_READ(soundlatch_r)
	AM_RANGE(0xf000, 0xf000) AM_WRITE(tturfbl_msm5205_data_w)
	AM_RANGE(0xf800, 0xffff) AM_RAM
ADDRESS_MAP_END

static ADDRESS_MAP_START( tturfbl_sound_io_map, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x00, 0x01) AM_DEVREADWRITE("ymsnd", ym2151_r, ym2151_w)
	AM_RANGE(0x40, 0x40) AM_WRITENOP
	AM_RANGE(0x80, 0x80) AM_NOP
ADDRESS_MAP_END

/*******************************************************************************/

static ADDRESS_MAP_START( sound_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0xe800, 0xe800) AM_READ(soundlatch_r)
	AM_RANGE(0xf800, 0xffff) AM_RAM
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_io_map, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x00, 0x01) AM_DEVREADWRITE("ymsnd", ym2151_r, ym2151_w)
	AM_RANGE(0xc0, 0xc0) AM_READ(soundlatch_r)
ADDRESS_MAP_END


// 7759
static ADDRESS_MAP_START( sound_7759_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0x8000, 0xdfff) AM_ROMBANK("bank1")
	AM_RANGE(0xe800, 0xe800) AM_READ(soundlatch_r)
	AM_RANGE(0xf800, 0xffff) AM_RAM
ADDRESS_MAP_END


static WRITE8_DEVICE_HANDLER( upd7759_bank_w ) //*
{
	int offs, size = memory_region_length(device->machine, "soundcpu") - 0x10000;

	upd7759_reset_w(device, data & 0x40);
	offs = 0x10000 + (data * 0x4000) % size;
	memory_set_bankptr(device->machine, "bank1", memory_region(device->machine, "soundcpu") + offs);
}


static ADDRESS_MAP_START( sound_7759_io_map, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x00, 0x01) AM_DEVREADWRITE("ymsnd", ym2151_r, ym2151_w)
	AM_RANGE(0x40, 0x40) AM_DEVWRITE("7759", upd7759_bank_w)
	AM_RANGE(0x80, 0x80) AM_DEVWRITE("7759", upd7759_port_w)
	AM_RANGE(0xc0, 0xc0) AM_READ(soundlatch_r)
ADDRESS_MAP_END


/***************************************************************************/

static void set_tile_bank( running_machine *machine, int data )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)machine->driver_data;
	state->tile_bank0 = (data >> 4) & 0x0f;
	state->tile_bank1 = data & 0x0f;
}

static void set_fg_page( running_machine *machine, int data )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)machine->driver_data;
	state->fg_page[0] = data >> 12;
	state->fg_page[1] = (data >> 8) & 0x0f;
	state->fg_page[2] = (data >> 4) & 0x0f;
	state->fg_page[3] = data & 0x0f;
}

static void set_bg_page( running_machine *machine, int data )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)machine->driver_data;
	state->bg_page[0] = data >> 12;
	state->bg_page[1] = (data >> 8) & 0x0f;
	state->bg_page[2] = (data >> 4) & 0x0f;
	state->bg_page[3] = data & 0x0f;
}


static ADDRESS_MAP_START( bayroute_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x0bffff) AM_ROM
	AM_RANGE(0x100000, 0x100003) AM_WRITENOP // tilebank control?
	AM_RANGE(0x500000, 0x503fff) AM_RAM // work ram
	AM_RANGE(0x600000, 0x600fff) AM_RAM AM_BASE(&segaic16_spriteram_0)
	AM_RANGE(0x700000, 0x70ffff) AM_RAM_WRITE(sys16_tileram_w) AM_BASE_MEMBER(segas1x_bootleg_state, tileram)
	AM_RANGE(0x710000, 0x710fff) AM_RAM_WRITE(sys16_textram_w) AM_BASE_MEMBER(segas1x_bootleg_state, textram)
	AM_RANGE(0x800000, 0x800fff) AM_RAM_WRITE(segaic16_paletteram_w)  AM_BASE(&segaic16_paletteram)
	AM_RANGE(0x900000, 0x900001) AM_WRITE(sys16_coinctrl_w)
	AM_RANGE(0x901002, 0x901003) AM_READ_PORT("P1")
	AM_RANGE(0x901006, 0x901007) AM_READ_PORT("P2")
	AM_RANGE(0x901000, 0x901001) AM_READ_PORT("SERVICE")
	AM_RANGE(0x902002, 0x902003) AM_READ_PORT("DSW1")
	AM_RANGE(0x902000, 0x902001) AM_READ_PORT("DSW2")
	AM_RANGE(0xff0006, 0xff0007) AM_WRITE(sound_command_w)
	AM_RANGE(0xff0020, 0xff003f) AM_WRITENOP // config regs
ADDRESS_MAP_END

/***************************************************************************/

/*
    Flash Point (Datsu bootlegs = fpointbl, fpointbj)
    Has sound latch at $E000 instead of I/O ports $C0-FF
*/
static ADDRESS_MAP_START( fpointbl_sound_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0xe000, 0xe000) AM_READ(soundlatch_r)
	AM_RANGE(0xf800, 0xffff) AM_RAM
ADDRESS_MAP_END

/***************************************************************************/

static WRITE16_HANDLER( s16bl_bgpage_w )
{
	set_bg_page(space->machine, data);
}

static WRITE16_HANDLER( s16bl_fgpage_w )
{
	set_fg_page(space->machine, data);
}

static WRITE16_HANDLER( s16bl_fgscrollx_bank_w )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;
	int scroll = data & 0x1ff;
	int bank = (data & 0xc000) >> 14;

	scroll += 0x200;
	set_tile_bank(space->machine, bank);

	scroll += 3; // so that the character portraits in attract mode are properly aligned (alighnment on character select no longer matches original tho?)
	state->fg_scrollx = -scroll;
}

static WRITE16_HANDLER( s16bl_fgscrollx_w )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;
	int scroll = data & 0x1ff;

	scroll += 0x200;
	scroll += 3;

	state->fg_scrollx = -scroll;
}


static WRITE16_HANDLER( s16bl_fgscrolly_w )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;
	int scroll = data & 0xff;

	state->fg_scrolly = scroll;
}

static WRITE16_HANDLER( s16bl_bgscrollx_w )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;
	int scroll = data & 0x1ff;

	scroll+= 0x200;
	scroll+= 1; // so that the background fo the select screen is properly aligned
	state->bg_scrollx = -scroll;
}

static WRITE16_HANDLER( s16bl_bgscrolly_w )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;
	int scroll = data & 0xff;

	state->bg_scrolly = scroll;
}


static ADDRESS_MAP_START( goldnaxeb1_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x0bffff) AM_ROM
	AM_RANGE(0x100000, 0x10ffff) AM_RAM_WRITE(sys16_tileram_w) AM_BASE_MEMBER(segas1x_bootleg_state, tileram)
	AM_RANGE(0x110000, 0x110fff) AM_RAM_WRITE(sys16_textram_w) AM_BASE_MEMBER(segas1x_bootleg_state, textram)
	AM_RANGE(0x118000, 0x118001) AM_WRITE(s16bl_fgscrolly_w)
	AM_RANGE(0x118008, 0x118009) AM_WRITE(s16bl_fgscrollx_bank_w) // and tile bank
	AM_RANGE(0x118010, 0x118011) AM_WRITE(s16bl_bgscrolly_w)
	AM_RANGE(0x118018, 0x118019) AM_WRITE(s16bl_bgscrollx_w)
	AM_RANGE(0x118020, 0x118021) AM_WRITE(s16bl_fgpage_w)
	AM_RANGE(0x118028, 0x118029) AM_WRITE(s16bl_bgpage_w)
	AM_RANGE(0x140000, 0x143fff) AM_RAM_WRITE(segaic16_paletteram_w)  AM_BASE(&segaic16_paletteram)
	AM_RANGE(0x200000, 0x200fff) AM_RAM AM_BASE(&segaic16_spriteram_0)
	AM_RANGE(0xc40000, 0xc40001) AM_WRITE(sys16_coinctrl_w)
	AM_RANGE(0xc41002, 0xc41003) AM_READ_PORT("P1")
	AM_RANGE(0xc41006, 0xc41007) AM_READ_PORT("P2")
	AM_RANGE(0xc41000, 0xc41001) AM_READ_PORT("SERVICE")
	AM_RANGE(0xc42002, 0xc42003) AM_READ_PORT("DSW1")
	AM_RANGE(0xc42000, 0xc42001) AM_READ_PORT("DSW2")
	AM_RANGE(0xc42006, 0xc42007) AM_WRITENOP // sound related?
	AM_RANGE(0xc43000, 0xc43001) AM_WRITENOP
	AM_RANGE(0xc43034, 0xc43035) AM_WRITENOP
	AM_RANGE(0xc80000, 0xc80001) AM_WRITENOP
	AM_RANGE(0xffc000, 0xffffff) AM_RAM // work ram
ADDRESS_MAP_END

static ADDRESS_MAP_START( bayrouteb1_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x0bffff) AM_ROM
	AM_RANGE(0x500000, 0x503fff) AM_RAM // work ram
	AM_RANGE(0x600000, 0x600fff) AM_RAM AM_BASE(&segaic16_spriteram_0)
	AM_RANGE(0x700000, 0x70ffff) AM_RAM_WRITE(sys16_tileram_w) AM_BASE_MEMBER(segas1x_bootleg_state, tileram)
	AM_RANGE(0x710000, 0x710fff) AM_RAM_WRITE(sys16_textram_w) AM_BASE_MEMBER(segas1x_bootleg_state, textram)
	AM_RANGE(0x718000, 0x718001) AM_WRITE(s16bl_fgscrolly_w)
	AM_RANGE(0x718008, 0x718009) AM_WRITE(s16bl_fgscrollx_bank_w) // and tile bank
	AM_RANGE(0x718010, 0x718011) AM_WRITE(s16bl_bgscrolly_w)
	AM_RANGE(0x718018, 0x718019) AM_WRITE(s16bl_bgscrollx_w)
	AM_RANGE(0x718020, 0x718021) AM_WRITE(s16bl_fgpage_w)
	AM_RANGE(0x718028, 0x718029) AM_WRITE(s16bl_bgpage_w)
	AM_RANGE(0x800000, 0x800fff) AM_RAM_WRITE(segaic16_paletteram_w)  AM_BASE(&segaic16_paletteram)
	AM_RANGE(0x901000, 0x901001) AM_READ_PORT("SERVICE") AM_WRITE(sys16_coinctrl_w)
	AM_RANGE(0x901002, 0x901003) AM_READ_PORT("P1")
	AM_RANGE(0x901006, 0x901007) AM_READ_PORT("P2")
	AM_RANGE(0x902000, 0x902001) AM_READ_PORT("DSW2")
	AM_RANGE(0x902002, 0x902003) AM_READ_PORT("DSW1")
ADDRESS_MAP_END

static void datsu_set_pages( running_machine *machine )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)machine->driver_data;
	UINT16 page;

	page = ((state->datsu_page[0] & 0x00f0) >>0) |
		   ((state->datsu_page[1] & 0x00f0) >>4) |
		   ((state->datsu_page[2] & 0x00f0) <<8) |
		   ((state->datsu_page[3] & 0x00f0) <<4);


	set_fg_page(machine, page);

	page = ((state->datsu_page[0] & 0x000f) <<4) |
		   ((state->datsu_page[1] & 0x000f) <<0) |
		   ((state->datsu_page[2] & 0x000f) <<12) |
		   ((state->datsu_page[3] & 0x000f) <<8);

	set_bg_page(machine, page);
}

static WRITE16_HANDLER( datsu_page0_w )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;

	COMBINE_DATA(&state->datsu_page[0]);
	datsu_set_pages(space->machine);
}

static WRITE16_HANDLER( datsu_page1_w )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;

	COMBINE_DATA(&state->datsu_page[1]);
	datsu_set_pages(space->machine);
}

static WRITE16_HANDLER( datsu_page2_w )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;

	COMBINE_DATA(&state->datsu_page[2]);
	datsu_set_pages(space->machine);
}

static WRITE16_HANDLER( datsu_page3_w )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;

	COMBINE_DATA(&state->datsu_page[3]);
	datsu_set_pages(space->machine);
}

static ADDRESS_MAP_START( bayrouteb2_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x0bffff) AM_ROM
	AM_RANGE(0x500000, 0x503fff) AM_RAM // work ram
	AM_RANGE(0x600000, 0x600fff) AM_RAM AM_BASE(&segaic16_spriteram_0)
	AM_RANGE(0x700000, 0x70ffff) AM_RAM_WRITE(sys16_tileram_w) AM_BASE_MEMBER(segas1x_bootleg_state, tileram)
	AM_RANGE(0x710000, 0x710fff) AM_RAM_WRITE(sys16_textram_w) AM_BASE_MEMBER(segas1x_bootleg_state, textram)
	AM_RANGE(0x718000, 0x718001) AM_WRITE(s16bl_fgscrolly_w)
	AM_RANGE(0x718008, 0x718009) AM_WRITE(s16bl_fgscrollx_bank_w) // and tile bank
	AM_RANGE(0x718010, 0x718011) AM_WRITE(s16bl_bgscrolly_w)
	AM_RANGE(0x718018, 0x718019) AM_WRITE(s16bl_bgscrollx_w)
	AM_RANGE(0x718020, 0x718021) AM_WRITE(datsu_page0_w)
	AM_RANGE(0x718022, 0x718023) AM_WRITE(datsu_page1_w)
	AM_RANGE(0x718024, 0x718025) AM_WRITE(datsu_page2_w)
	AM_RANGE(0x718026, 0x718027) AM_WRITE(datsu_page3_w)

	AM_RANGE(0x800000, 0x800fff) AM_RAM_WRITE(segaic16_paletteram_w)  AM_BASE(&segaic16_paletteram)
	AM_RANGE(0x900000, 0x900001) AM_READ_PORT("DSW1")
	AM_RANGE(0x900002, 0x900003) AM_READ_PORT("DSW2")
	AM_RANGE(0x900006, 0x900007) AM_WRITE(sound_command_w)
	AM_RANGE(0x901000, 0x901001) AM_READ_PORT("SERVICE") AM_WRITE(sys16_coinctrl_w)
	AM_RANGE(0x901002, 0x901003) AM_READ_PORT("P1")
	AM_RANGE(0x901006, 0x901007) AM_READ_PORT("P2")
ADDRESS_MAP_END

static ADDRESS_MAP_START( dduxbl_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x0bffff) AM_ROM
	AM_RANGE(0x3f0000, 0x3fffff) AM_WRITE(sys16_tilebank_w)
	AM_RANGE(0x400000, 0x40ffff) AM_RAM_WRITE(sys16_tileram_w) AM_BASE_MEMBER(segas1x_bootleg_state, tileram)
	AM_RANGE(0x410000, 0x410fff) AM_RAM_WRITE(sys16_textram_w) AM_BASE_MEMBER(segas1x_bootleg_state, textram)
	AM_RANGE(0x440000, 0x440fff) AM_RAM AM_BASE(&segaic16_spriteram_0)
	AM_RANGE(0x840000, 0x840fff) AM_RAM_WRITE(segaic16_paletteram_w)  AM_BASE(&segaic16_paletteram)
	AM_RANGE(0xc40000, 0xc40001) AM_WRITE(sys16_coinctrl_w)
	AM_RANGE(0xc40006, 0xc40007) AM_WRITE(sound_command_w)
	AM_RANGE(0xc41002, 0xc41003) AM_READ_PORT("P1")
	AM_RANGE(0xc41004, 0xc41005) AM_READ_PORT("P2")
	AM_RANGE(0xc41000, 0xc41001) AM_READ_PORT("SERVICE")
	AM_RANGE(0xc42002, 0xc42003) AM_READ_PORT("DSW1")
	AM_RANGE(0xc42000, 0xc42001) AM_READ_PORT("DSW2")

	AM_RANGE(0xC46000, 0xC46001) AM_WRITE(s16bl_fgscrolly_w)
	AM_RANGE(0xC46008, 0xC46009) AM_WRITE(s16bl_fgscrollx_w)
	AM_RANGE(0xC46010, 0xC46011) AM_WRITE(s16bl_bgscrolly_w)
	AM_RANGE(0xC46018, 0xC46019) AM_WRITE(s16bl_bgscrollx_w)
	AM_RANGE(0xC46020, 0xC46021) AM_WRITE(datsu_page0_w)
	AM_RANGE(0xC46022, 0xC46023) AM_WRITE(datsu_page1_w)
	AM_RANGE(0xC46024, 0xC46025) AM_WRITE(datsu_page2_w)
	AM_RANGE(0xC46026, 0xC46027) AM_WRITE(datsu_page3_w)

	AM_RANGE(0xffc000, 0xffffff) AM_RAM // work ram
ADDRESS_MAP_END

static WRITE16_HANDLER( goldnaxeb2_fgscrollx_w )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;
	int scroll = data & 0x1ff;
	int bank = (data & 0xc000) >> 14;

	set_tile_bank(space->machine,  bank);
	scroll += 0x1f6;
	scroll &= 0x3ff;
	state->fg_scrollx = -scroll;
}

static WRITE16_HANDLER( goldnaxeb2_bgscrollx_w )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;
	int scroll = data & 0x1ff;

	scroll += 0x1f4;
	scroll &= 0x3ff;
	state->bg_scrollx = -scroll;
}


static WRITE16_HANDLER( goldnaxeb2_fgscrolly_w )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;
	int scroll = data & 0xff;

	scroll += 0x1;
	state->fg_scrolly = scroll;
}

static WRITE16_HANDLER( goldnaxeb2_bgscrolly_w )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;
	int scroll = data & 0xff;

	scroll += 0x1;
	state->bg_scrolly = scroll;
}

static WRITE16_HANDLER( goldnaxeb2_fgpage_w )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;
	UINT16 page;

	COMBINE_DATA(&state->goldnaxeb2_fgpage[offset]);

	page = ((state->goldnaxeb2_fgpage[1] & 0xf) << 0) |
		   ((state->goldnaxeb2_fgpage[0] & 0xf) << 4) |
		   ((state->goldnaxeb2_fgpage[3] & 0xf) << 8) |
		   ((state->goldnaxeb2_fgpage[2] & 0xf) << 12);

	set_fg_page(space->machine, page ^ 0xffff);

}

static WRITE16_HANDLER( goldnaxeb2_bgpage_w )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;
	UINT16 page;

	COMBINE_DATA(&state->goldnaxeb2_bgpage[offset]);

	page = ((state->goldnaxeb2_bgpage[1] & 0xf) << 0) |
		   ((state->goldnaxeb2_bgpage[0] & 0xf) << 4) |
		   ((state->goldnaxeb2_bgpage[3] & 0xf) << 8) |
		   ((state->goldnaxeb2_bgpage[2] & 0xf) << 12);

	set_bg_page(space->machine, page ^ 0xffff);
}

static ADDRESS_MAP_START( goldnaxeb2_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x0bffff) AM_ROM
	AM_RANGE(0x100000, 0x10ffff) AM_RAM_WRITE(sys16_tileram_w) AM_BASE_MEMBER(segas1x_bootleg_state, tileram)
	AM_RANGE(0x110000, 0x110fff) AM_RAM_WRITE(sys16_textram_w) AM_BASE_MEMBER(segas1x_bootleg_state, textram)
	AM_RANGE(0x140000, 0x143fff) AM_RAM_WRITE(segaic16_paletteram_w) AM_BASE(&segaic16_paletteram)
	AM_RANGE(0x200000, 0x200fff) AM_RAM AM_BASE(&segaic16_spriteram_0)
	AM_RANGE(0xc40000, 0xc40001) AM_READ_PORT("DSW2") AM_WRITENOP
	AM_RANGE(0xc40002, 0xc40003) AM_READ_PORT("DSW1")
	AM_RANGE(0xc41000, 0xc41001) AM_READ_PORT("SERVICE")
	AM_RANGE(0xc41002, 0xc41003) AM_READ_PORT("P1")
	AM_RANGE(0xc41004, 0xc41005) AM_READ_PORT("P2")
	AM_RANGE(0xc43000, 0xc43001) AM_WRITENOP
	AM_RANGE(0xc44000, 0xc44001) AM_WRITE(goldnaxeb2_fgscrolly_w)
	AM_RANGE(0xc44008, 0xc44009) AM_WRITE(goldnaxeb2_fgscrollx_w) // and tile bank
	AM_RANGE(0xc44010, 0xc44011) AM_WRITE(goldnaxeb2_bgscrolly_w)
	AM_RANGE(0xc44018, 0xc44019) AM_WRITE(goldnaxeb2_bgscrollx_w)
	AM_RANGE(0xc44020, 0xc44027) AM_WRITE(goldnaxeb2_bgpage_w) AM_BASE_MEMBER(segas1x_bootleg_state, goldnaxeb2_bgpage)
	AM_RANGE(0xc44060, 0xc44067) AM_WRITE(goldnaxeb2_fgpage_w) AM_BASE_MEMBER(segas1x_bootleg_state, goldnaxeb2_fgpage)
	AM_RANGE(0xc46000, 0xc46001) AM_WRITENOP
	AM_RANGE(0xc43034, 0xc43035) AM_WRITENOP
	AM_RANGE(0xfe0006, 0xfe0007) AM_WRITENOP
	AM_RANGE(0xffc000, 0xffffff) AM_RAM // work ram
ADDRESS_MAP_END


/***************************************************************************/


static ADDRESS_MAP_START( fpointbl_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x0bffff) AM_ROM

	AM_RANGE(0x400000, 0x40ffff) AM_RAM_WRITE(sys16_tileram_w) AM_BASE_MEMBER(segas1x_bootleg_state, tileram)
	AM_RANGE(0x410000, 0x410fff) AM_RAM_WRITE(sys16_textram_w) AM_BASE_MEMBER(segas1x_bootleg_state, textram)
	AM_RANGE(0x440000, 0x440fff) AM_RAM AM_BASE(&segaic16_spriteram_0)

	AM_RANGE(0x600006, 0x600007) AM_WRITE(sound_command_w)
	AM_RANGE(0x601000, 0x601001) AM_READ_PORT("SERVICE")
	AM_RANGE(0x601002, 0x601003) AM_READ_PORT("P1")
	AM_RANGE(0x601004, 0x601005) AM_READ_PORT("P2")
	AM_RANGE(0x600000, 0x600001) AM_READ_PORT("DSW2")
	AM_RANGE(0x600002, 0x600003) AM_READ_PORT("DSW1")

	AM_RANGE(0x840000, 0x840fff) AM_RAM_WRITE(segaic16_paletteram_w)  AM_BASE(&segaic16_paletteram)
	AM_RANGE(0x843000, 0x843001) AM_WRITENOP

	AM_RANGE(0xC46000, 0xC46001) AM_WRITE(s16bl_fgscrolly_w)
	AM_RANGE(0xC46008, 0xC46009) AM_WRITE(s16bl_fgscrollx_w)
	AM_RANGE(0xC46010, 0xC46011) AM_WRITE(s16bl_bgscrolly_w)
	AM_RANGE(0xC46018, 0xC46019) AM_WRITE(s16bl_bgscrollx_w)

	AM_RANGE(0xC46020, 0xC46021) AM_WRITE(datsu_page0_w)
	AM_RANGE(0xc46022, 0xc46023) AM_WRITE(datsu_page1_w)
	AM_RANGE(0xC46024, 0xC46025) AM_WRITE(datsu_page2_w)
	AM_RANGE(0xC46026, 0xC46027) AM_WRITE(datsu_page3_w)

	AM_RANGE(0xffc000, 0xffffff) AM_RAM // work ram
ADDRESS_MAP_END


static WRITE16_HANDLER( eswat_tilebank0_w )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;

	if (ACCESSING_BITS_0_7)
	{
		state->eswat_tilebank0 = data & 0xff;
	}
}

static ADDRESS_MAP_START( eswatbl_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x07ffff) AM_ROM

	AM_RANGE(0x3e2000, 0x3e2001) AM_WRITE(eswat_tilebank0_w) // external tile bank ( > 0x4000 tiles )

	AM_RANGE(0x400000, 0x40ffff) AM_RAM_WRITE(sys16_tileram_w) AM_BASE_MEMBER(segas1x_bootleg_state, tileram)
	AM_RANGE(0x410000, 0x410fff) AM_RAM_WRITE(sys16_textram_w) AM_BASE_MEMBER(segas1x_bootleg_state, textram)
	AM_RANGE(0x418000, 0x418001) AM_WRITE(s16bl_bgscrolly_w)
	AM_RANGE(0x418008, 0x418009) AM_WRITE(s16bl_bgscrollx_w) // and tile bank
	AM_RANGE(0x418010, 0x418011) AM_WRITE(s16bl_fgscrolly_w)
	AM_RANGE(0x418018, 0x418019) AM_WRITE(s16bl_fgscrollx_bank_w)
	AM_RANGE(0x418020, 0x418021) AM_WRITE(s16bl_bgpage_w)
	AM_RANGE(0x418028, 0x418029) AM_WRITE(s16bl_fgpage_w)

	AM_RANGE(0x440000, 0x440fff) AM_RAM AM_BASE(&segaic16_spriteram_0)
	AM_RANGE(0x840000, 0x840fff) AM_RAM_WRITE(segaic16_paletteram_w)  AM_BASE(&segaic16_paletteram)
	AM_RANGE(0xc40000, 0xc40001) AM_WRITE(sys16_coinctrl_w)
	AM_RANGE(0xc41002, 0xc41003) AM_READ_PORT("P1")
	AM_RANGE(0xc41006, 0xc41007) AM_READ_PORT("P2")
	AM_RANGE(0xc41000, 0xc41001) AM_READ_PORT("SERVICE")
	AM_RANGE(0xc42002, 0xc42003) AM_READ_PORT("DSW1")
	AM_RANGE(0xc42000, 0xc42001) AM_READ_PORT("DSW2")
	AM_RANGE(0xc42006, 0xc42007) AM_WRITE(sound_command_w)
	AM_RANGE(0xc80000, 0xc80001) AM_WRITENOP
	AM_RANGE(0xffc000, 0xffffff) AM_RAM // work ram
ADDRESS_MAP_END

/***************************************************************************/

static ADDRESS_MAP_START( tetrisbl_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x03ffff) AM_ROM
	AM_RANGE(0x400000, 0x40ffff) AM_RAM_WRITE(sys16_tileram_w) AM_BASE_MEMBER(segas1x_bootleg_state, tileram)
	AM_RANGE(0x410000, 0x410fff) AM_RAM_WRITE(sys16_textram_w) AM_BASE_MEMBER(segas1x_bootleg_state, textram)

	AM_RANGE(0x418000, 0x418001) AM_WRITE(s16bl_fgscrolly_w)
	AM_RANGE(0x418008, 0x418009) AM_WRITE(s16bl_fgscrollx_w)
	AM_RANGE(0x418010, 0x418011) AM_WRITE(s16bl_bgscrolly_w)
	AM_RANGE(0x418018, 0x418019) AM_WRITE(s16bl_bgscrollx_w)
	AM_RANGE(0x418020, 0x418021) AM_WRITE(s16bl_fgpage_w)
	AM_RANGE(0x418028, 0x418029) AM_WRITE(s16bl_bgpage_w)

	AM_RANGE(0x440000, 0x440fff) AM_RAM AM_BASE(&segaic16_spriteram_0)
	AM_RANGE(0x840000, 0x840fff) AM_RAM_WRITE(segaic16_paletteram_w)  AM_BASE(&segaic16_paletteram)
	AM_RANGE(0xc40000, 0xc40001) AM_WRITE(sys16_coinctrl_w)
	AM_RANGE(0xc41000, 0xc41001) AM_READ_PORT("SERVICE")
	AM_RANGE(0xc41002, 0xc41003) AM_READ_PORT("P1")
	AM_RANGE(0xc41006, 0xc41007) AM_READ_PORT("P2")
	AM_RANGE(0xc42000, 0xc42001) AM_READ_PORT("DSW2")
	AM_RANGE(0xc42002, 0xc42003) AM_READ_PORT("DSW1")
	AM_RANGE(0xc42006, 0xc42007) AM_WRITE(sound_command_w)
	AM_RANGE(0xc43034, 0xc43035) AM_WRITENOP
	AM_RANGE(0xc80000, 0xc80001) AM_NOP
	AM_RANGE(0xffc000, 0xffffff) AM_RAM // work ram
ADDRESS_MAP_END


static READ16_HANDLER( beautyb_unkx_r )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;

	state->beautyb_unkx++;
	state->beautyb_unkx &= 0x7f;
	return state->beautyb_unkx;
}

static ADDRESS_MAP_START( beautyb_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x00ffff) AM_ROM AM_WRITENOP
	AM_RANGE(0x010000, 0x03ffff) AM_WRITENOP

	AM_RANGE(0x0280D6, 0x0280D7) AM_READ(beautyb_unkx_r)
	AM_RANGE(0x0280D8, 0x0280D9) AM_READ(beautyb_unkx_r)

	AM_RANGE(0x400000, 0x40ffff) AM_RAM_WRITE(sys16_tileram_w) AM_BASE_MEMBER(segas1x_bootleg_state, tileram)
	AM_RANGE(0x410000, 0x413fff) AM_RAM_WRITE(sys16_textram_w) AM_BASE_MEMBER(segas1x_bootleg_state, textram)

	AM_RANGE(0x418000, 0x418001) AM_WRITE(s16bl_bgscrolly_w)
	AM_RANGE(0x418008, 0x418009) AM_WRITE(s16bl_bgscrollx_w)
	AM_RANGE(0x418010, 0x418011) AM_WRITE(s16bl_fgscrolly_w)
	AM_RANGE(0x418018, 0x418019) AM_WRITE(s16bl_fgscrollx_w)
	AM_RANGE(0x418020, 0x418021) AM_WRITE(s16bl_bgpage_w)
	AM_RANGE(0x418028, 0x418029) AM_WRITE(s16bl_fgpage_w)

	AM_RANGE(0x440000, 0x440fff) AM_RAM AM_BASE(&segaic16_spriteram_0)
	AM_RANGE(0x840000, 0x840fff) AM_RAM_WRITE(segaic16_paletteram_w)  AM_BASE(&segaic16_paletteram)

	AM_RANGE(0xC41000, 0xC41001) AM_READ(beautyb_unkx_r )
	AM_RANGE(0xC41002, 0xC41003) AM_READ(beautyb_unkx_r )

	AM_RANGE(0xc40000, 0xc40001) AM_WRITENOP
	AM_RANGE(0xc80000, 0xc80001) AM_WRITENOP

	AM_RANGE(0xffc000, 0xffffff) AM_RAM // work ram
ADDRESS_MAP_END

/***************************************************************************/

static ADDRESS_MAP_START( tturfbl_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x03ffff) AM_ROM
	AM_RANGE(0x200000, 0x203fff) AM_RAM // work ram
	AM_RANGE(0x300000, 0x300fff) AM_RAM AM_BASE(&segaic16_spriteram_0)
	AM_RANGE(0x400000, 0x40ffff) AM_RAM_WRITE(sys16_tileram_w) AM_BASE_MEMBER(segas1x_bootleg_state, tileram)
	AM_RANGE(0x410000, 0x410fff) AM_RAM_WRITE(sys16_textram_w) AM_BASE_MEMBER(segas1x_bootleg_state, textram)
	AM_RANGE(0x500000, 0x500fff) AM_RAM_WRITE(segaic16_paletteram_w) AM_BASE(&segaic16_paletteram)
	AM_RANGE(0x600000, 0x600001) AM_WRITE(sys16_coinctrl_w)
	AM_RANGE(0x600000, 0x600001) AM_READ_PORT("DSW2")
	AM_RANGE(0x600002, 0x600003) AM_READ_PORT("DSW1")
	AM_RANGE(0x600006, 0x600007) AM_WRITE(sound_command_w)
	AM_RANGE(0x601000, 0x601001) AM_READ_PORT("SERVICE")
	AM_RANGE(0x601002, 0x601003) AM_READ_PORT("P1")
	AM_RANGE(0x601004, 0x601005) AM_READ_PORT("P2")
	AM_RANGE(0x602000, 0x602001) AM_READ_PORT("DSW2")
	AM_RANGE(0x602002, 0x602003) AM_READ_PORT("DSW1")
	AM_RANGE(0xc44000, 0xc44001) AM_WRITENOP

	AM_RANGE(0xC46000, 0xC46001) AM_WRITE(s16bl_fgscrolly_w)
	AM_RANGE(0xC46008, 0xC46009) AM_WRITE(s16bl_fgscrollx_w)
	AM_RANGE(0xC46010, 0xC46011) AM_WRITE(s16bl_bgscrolly_w)
	AM_RANGE(0xC46018, 0xC46019) AM_WRITE(s16bl_bgscrollx_w)
	AM_RANGE(0xC46020, 0xC46021) AM_WRITE(datsu_page0_w)
	AM_RANGE(0xc46022, 0xc46023) AM_WRITE(datsu_page1_w)
	AM_RANGE(0xC46024, 0xC46025) AM_WRITE(datsu_page2_w)
	AM_RANGE(0xC46026, 0xC46027) AM_WRITE(datsu_page3_w)
ADDRESS_MAP_END

/***************************************************************************/

static WRITE16_HANDLER( sys18_refreshenable_w )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;

	if (ACCESSING_BITS_0_7)
	{
		state->refreshenable = data & 0x02;
	}
}

static WRITE16_HANDLER( sys18_tilebank_w )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;

	if (ACCESSING_BITS_0_7)
	{
		state->tile_bank0 = (data >> 0) & 0x0f;
		state->tile_bank1 = (data >> 4) & 0x0f;
	}
}

static READ8_HANDLER( system18_bank_r )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;

	if (state->sound_bank != NULL)
		return state->sound_bank[offset];

	return 0xff;
}

static ADDRESS_MAP_START( sound_18_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x9fff) AM_ROM
	AM_RANGE(0xa000, 0xbfff) AM_READ(system18_bank_r)
	/**** D/A register ****/
	AM_RANGE(0xc000, 0xc008) AM_DEVWRITE("5c68", rf5c68_w)
	AM_RANGE(0xd000, 0xdfff) AM_DEVREADWRITE("5c68", rf5c68_mem_r, rf5c68_mem_w)
	AM_RANGE(0xe000, 0xffff) AM_RAM	//??
ADDRESS_MAP_END


static WRITE8_HANDLER( sys18_soundbank_w )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;
	UINT8 *mem = memory_region(space->machine, "soundcpu");
	int rom = (data >> 6) & 3;
	int bank = (data & 0x3f);
	int mask = state->sound_info[rom * 2 + 0];
	int offs = state->sound_info[rom * 2 + 1];

	if (mask)
		state->sound_bank = &mem[0x10000 + offs + ((bank & mask) << 13)];
	else
		state->sound_bank = NULL;
}

static ADDRESS_MAP_START( sound_18_io_map, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x80, 0x83) AM_DEVREADWRITE("3438.0", ym3438_r, ym3438_w)
	AM_RANGE(0x90, 0x93) AM_DEVREADWRITE("3438.1", ym3438_r, ym3438_w)
	AM_RANGE(0xa0, 0xa0) AM_WRITE(sys18_soundbank_w)
	AM_RANGE(0xc0, 0xc0) AM_READ(soundlatch_r)
ADDRESS_MAP_END


/***************************************************************************

    Shadow Dancer (Bootleg)

    This seems to be a modified version of shdancer. It has no warning screen, displays English text during the
    attract sequence, and has a 2P input test. The 'Sega' copyright text was changed to 'Datsu', and their
    logo is missing.

    Access to the configuration registers, I/O chip, and VDP are done even though it's likely none of this hardware
    exists in the bootleg. For example:

    - Most I/O port access has been redirected to new addresses.
    - Z80 sound command has been redirected to a new address.
    - The tilebank routine which saves the bank value in VDP VRAM has a form of protection has been modified to store
      the tilebank value directly to $E4001F.
    - Implementing screen blanking control via $E4001D leaves the screen blanked at the wrong times (after coin-up).

    This is probably due to unmodified parts of the original code accessing these components, which would be ignored
    on the bootleg hardware. Both the I/O chip and VDP are supported in this driver, just as I don't know for certain
    how much of either are present on the real board.

    Bootleg specific addresses:

    C40001 = DIP switch #1
    C40003 = DIP switch #2
    C40007 = Z80 sound command
    C41001 = Service input
    C41003 = Player 1 input
    C41005 = Player 2 input
    C44000 = Has 'clr.w' done after setting tile bank in $E4000F.
    C460xx = Extra video hardware controls

    Here are the I/O chip addresses accessed:

    E40001 = Player 1
    E40007 = Miscellaneous outputs (coin control, etc.)
    E4000F = Tile bank
    E4001D = CNT2-0 pin output state
    E4001F = I/O chip port direction

***************************************************************************/

static WRITE16_HANDLER( sound_command_irq_w )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;

	if (ACCESSING_BITS_0_7)
	{
		soundlatch_w(space, 0, data & 0xff);
		cpu_set_input_line(state->soundcpu, 0, HOLD_LINE);
	}
}

static ADDRESS_MAP_START( shdancbl_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x07ffff) AM_ROM
	AM_RANGE(0x400000, 0x40ffff) AM_RAM_WRITE(sys16_tileram_w) AM_BASE_MEMBER(segas1x_bootleg_state, tileram)
	AM_RANGE(0x410000, 0x410fff) AM_RAM_WRITE(sys16_textram_w) AM_BASE_MEMBER(segas1x_bootleg_state, textram)
	AM_RANGE(0x440000, 0x440fff) AM_RAM AM_BASE(&segaic16_spriteram_0)
	AM_RANGE(0x840000, 0x840fff) AM_RAM_WRITE(segaic16_paletteram_w) AM_BASE(&segaic16_paletteram)
	AM_RANGE(0xc00000, 0xc0ffff) AM_NOP
	AM_RANGE(0xc40000, 0xc40001) AM_READ_PORT("COINAGE")
	AM_RANGE(0xc40002, 0xc40003) AM_READ_PORT("DSW1")
	AM_RANGE(0xc40006, 0xc40007) AM_WRITE(sound_command_irq_w)
	AM_RANGE(0xc41000, 0xc41001) AM_READ_PORT("SERVICE")
	AM_RANGE(0xc41002, 0xc41003) AM_READ_PORT("P1")
	AM_RANGE(0xc41004, 0xc41005) AM_READ_PORT("P2")
	AM_RANGE(0xc44000, 0xc44001) AM_WRITENOP // only used via clr.w after tilebank set


	AM_RANGE(0xe4001c, 0xe4001d) AM_WRITENOP // to prevent access to screen blanking control below
	AM_RANGE(0xe40000, 0xe4ffff) AM_NOP
	AM_RANGE(0xfe0020, 0xfe003f) AM_WRITENOP // config regs
	AM_RANGE(0xffc000, 0xffffff) AM_RAM
ADDRESS_MAP_END


/***************************************************************************

    Sound hardware for Shadow Dancer (Datsu bootleg)

    Z80 memory map
    0000-7FFF : ROM (fixed)
    8000-BFFF : ROM (banked)
    C000-C007 : ?
    C400      : Sound command (r/o)
    C800      : MSM5205 sample data output (w/o)
    CC00-CC03 : YM3438 #1
    D000-D003 : YM3438 #2
    D400      : ROM bank control (w/o)
    DF00-DFFF : ?
    E000-FFFF : Work RAM

    The unused memory locations and I/O port access seem to be remnants of the original code that were not patched out:

    - Program accesses RF5C68A channel registes at $C000-$C007
    - Program clears RF5C68A wave memory at $DF00-$DFFF
    - Program writes to port $A0 to access sound ROM banking control latch
    - Program reads port $C0 to access sound command

    Interrupts

    IRQ = Triggered when 68000 writes sound command. Z80 reads from $C400.
    NMI = Triggered when second nibble of sample data has been output to the MSM5205.
          Program copies sample data from ROM bank to the MSM5205 sample data buffer at $C800.

    ROM banking seems correct.
    It doesn't look like there's a way to reset the MSM5205, unless that's related to bit 7 of the
    ROM bank control register.
    MSM5205 clock speed hasn't been confirmed.

***************************************************************************/

static WRITE8_HANDLER( shdancbl_msm5205_data_w )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;
	state->sample_buffer = data;
}

static void shdancbl_msm5205_callback(running_device *device)
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)device->machine->driver_data;
	msm5205_data_w(device, state->sample_buffer & 0x0f);

	state->sample_buffer >>=  4;
	state->sample_select ^=  1;
	if (state->sample_select == 0)
		cpu_set_input_line(state->soundcpu, INPUT_LINE_NMI, PULSE_LINE);
}

static const msm5205_interface shdancbl_msm5205_interface  =
{
	shdancbl_msm5205_callback,
	MSM5205_S48_4B
};

static READ8_HANDLER( shdancbl_soundbank_r )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;

	if (state->soundbank_ptr)
		return state->soundbank_ptr[offset & 0x3fff];
	return 0xff;
}

static WRITE8_HANDLER( shdancbl_bankctrl_w )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)space->machine->driver_data;
	UINT8 *mem = memory_region(space->machine, "soundcpu");

	switch (data)
	{
		case 0:
			state->soundbank_ptr = &mem[0x18000]; /* IC45 8000-BFFF */
			break;
		case 1:
			state->soundbank_ptr = &mem[0x1C000]; /* IC45 C000-FFFF */
			break;
		case 2:
			state->soundbank_ptr = &mem[0x20000]; /* IC46 0000-3FFF */
			break;
		case 3:
			state->soundbank_ptr = &mem[0x24000]; /* IC46 4000-7FFF */
			break;
		default:
			state->soundbank_ptr = NULL;
			logerror("Invalid bank setting %02X (%04X)\n", data, cpu_get_pc(space->cpu));
			break;
	}
}

static ADDRESS_MAP_START(shdancbl_sound_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0x8000, 0xbfff) AM_READ(shdancbl_soundbank_r)
	AM_RANGE(0xc000, 0xc00f) AM_WRITENOP
	AM_RANGE(0xc400, 0xc400) AM_READ(soundlatch_r)
	AM_RANGE(0xc800, 0xc800) AM_WRITE(shdancbl_msm5205_data_w)
	AM_RANGE(0xcc00, 0xcc03) AM_DEVREADWRITE("3438.0", ym3438_r, ym3438_w)
	AM_RANGE(0xd000, 0xd003) AM_DEVREADWRITE("3438.1", ym3438_r, ym3438_w)
	AM_RANGE(0xd400, 0xd400) AM_WRITE(shdancbl_bankctrl_w)
	AM_RANGE(0xdf00, 0xdfff) AM_NOP
	AM_RANGE(0xe000, 0xffff) AM_RAM
ADDRESS_MAP_END


static ADDRESS_MAP_START( shdancbl_sound_io_map, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0xa0, 0xbf) AM_WRITENOP
	AM_RANGE(0xc0, 0xdf) AM_READNOP
ADDRESS_MAP_END


/***************************************************************************

    Moonwalker (Bootleg)

***************************************************************************/

static ADDRESS_MAP_START( mwalkbl_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x07ffff) AM_ROM
	AM_RANGE(0x400000, 0x40ffff) AM_RAM_WRITE(sys16_tileram_w) AM_BASE_MEMBER(segas1x_bootleg_state, tileram)
	AM_RANGE(0x410000, 0x410fff) AM_RAM_WRITE(sys16_textram_w) AM_BASE_MEMBER(segas1x_bootleg_state, textram)
	AM_RANGE(0x440000, 0x440fff) AM_RAM AM_BASE(&segaic16_spriteram_0)
	AM_RANGE(0x840000, 0x840fff) AM_RAM_WRITE(segaic16_paletteram_w) AM_BASE(&segaic16_paletteram)

	/* bootleg video regs */
	/*AM_RANGE(0xc00000, 0xc00001) AM_NOP
    AM_RANGE(0xc00002, 0xc00003) AM_NOP
    AM_RANGE(0xc00004, 0xc00005) AM_NOP // tile bank?
    AM_RANGE(0xc00006, 0xc00007) AM_NOP
    AM_RANGE(0xc44000, 0xc44001) AM_NOP
    AM_RANGE(0xc46000, 0xc46001) AM_NOP
    AM_RANGE(0xc46200, 0xc46201) AM_NOP
    AM_RANGE(0xc46400, 0xc464ff) AM_NOP // scroll?
    AM_RANGE(0xc46500, 0xc465ff) AM_NOP // scroll?
    */

	AM_RANGE(0xc40000, 0xc40001) AM_READ_PORT("COINAGE")
	AM_RANGE(0xc40002, 0xc40003) AM_READ_PORT("DSW1")
	AM_RANGE(0xc40006, 0xc40007) AM_WRITE(sound_command_nmi_w)
	AM_RANGE(0xc41000, 0xc41001) AM_READ_PORT("SERVICE")
	AM_RANGE(0xc41002, 0xc41003) AM_READ_PORT("P1")
	AM_RANGE(0xc41004, 0xc41005) AM_READ_PORT("P2")
	AM_RANGE(0xc41006, 0xc41007) AM_READ_PORT("P3")
	AM_RANGE(0xc41008, 0xc41009) AM_READNOP // figure this out, extra input for 3p?
	AM_RANGE(0xc46600, 0xc46601) AM_WRITE(sys18_refreshenable_w)
	AM_RANGE(0xc46800, 0xc46801) AM_WRITE(sys18_tilebank_w)

	AM_RANGE(0xfe0020, 0xfe003f) AM_WRITENOP // config regs
	AM_RANGE(0xffc000, 0xffffff) AM_RAM
ADDRESS_MAP_END


/***************************************************************************

    Alien Storm (Bootleg)

***************************************************************************/

/* bootleg doesn't have real vdp or i/o */
static ADDRESS_MAP_START( astormbl_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x07ffff) AM_ROM
	AM_RANGE(0x100000, 0x10ffff) AM_RAM_WRITE(sys16_tileram_w) AM_BASE_MEMBER(segas1x_bootleg_state, tileram)
	AM_RANGE(0x110000, 0x110fff) AM_RAM_WRITE(sys16_textram_w) AM_BASE_MEMBER(segas1x_bootleg_state, textram)
	AM_RANGE(0x140000, 0x140fff) AM_RAM_WRITE(segaic16_paletteram_w) AM_BASE(&segaic16_paletteram)
	AM_RANGE(0x200000, 0x200fff) AM_RAM AM_BASE(&segaic16_spriteram_0)
	AM_RANGE(0xa00000, 0xa00001) AM_READ_PORT("COINAGE")
	AM_RANGE(0xa00002, 0xa00003) AM_READ_PORT("DSW1")
	AM_RANGE(0xa00006, 0xa00007) AM_WRITE(sound_command_nmi_w)
	AM_RANGE(0xa0000e, 0xa0000f) AM_WRITE(sys18_tilebank_w)
	AM_RANGE(0xa01000, 0xa01001) AM_READ_PORT("SERVICE")
	AM_RANGE(0xa01002, 0xa01003) AM_READ_PORT("P1")
	AM_RANGE(0xa01004, 0xa01005) AM_READ_PORT("P2")
	AM_RANGE(0xa01006, 0xa01007) AM_READ_PORT("P3")
	AM_RANGE(0xa02100, 0xa02101) AM_NOP
	AM_RANGE(0xa03000, 0xa03001) AM_NOP
	AM_RANGE(0xa03034, 0xa03035) AM_NOP

	/* bootleg video regs */
	AM_RANGE(0xc00000, 0xc00001) AM_NOP
	AM_RANGE(0xc00002, 0xc00003) AM_NOP
	AM_RANGE(0xc00004, 0xc00005) AM_NOP // tile bank?
	AM_RANGE(0xc00006, 0xc00007) AM_NOP
	AM_RANGE(0xc44000, 0xc44001) AM_NOP
	AM_RANGE(0xc46000, 0xc46001) AM_NOP
	AM_RANGE(0xc46200, 0xc46201) AM_NOP
	AM_RANGE(0xc46400, 0xc464ff) AM_NOP // scroll?
	AM_RANGE(0xc46500, 0xc465ff) AM_NOP // scroll?

	AM_RANGE(0xc46600, 0xc46601) AM_WRITE(sys18_refreshenable_w)
	AM_RANGE(0xfe0020, 0xfe003f) AM_WRITENOP
	AM_RANGE(0xffc000, 0xffffff) AM_RAM
ADDRESS_MAP_END

/*************************************
 *
 *  Input ports
 *
 *************************************/

/* System 16A/B Bootlegs */
static INPUT_PORTS_START( sys16_common )
	PORT_START("P1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON3 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY

	PORT_START("P2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_COCKTAIL
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL

	PORT_START("SERVICE")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_SERVICE_NO_TOGGLE(0x04, IP_ACTIVE_LOW)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("DSW1")
	PORT_DIPNAME( 0x0f, 0x0f, DEF_STR( Coin_A ) ) PORT_DIPLOCATION("SW1:1,2,3,4")
	PORT_DIPSETTING(    0x07, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x09, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x05, "2 Coins/1 Credit 5/3 6/4" )
	PORT_DIPSETTING(    0x04, "2 Coins/1 Credit 4/3" )
	PORT_DIPSETTING(    0x0f, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x01, "1 Coin/1 Credit 2/3" )
	PORT_DIPSETTING(    0x02, "1 Coin/1 Credit 4/5" )
	PORT_DIPSETTING(    0x03, "1 Coin/1 Credit 5/6" )
	PORT_DIPSETTING(    0x06, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x0e, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x0d, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x0b, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x0a, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x00, "Free Play (if Coin B too) or 1/1" )
	PORT_DIPNAME( 0xf0, 0xf0, DEF_STR( Coin_B ) ) PORT_DIPLOCATION("SW1:5,6,7,8")
	PORT_DIPSETTING(    0x70, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x90, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x50, "2 Coins/1 Credit 5/3 6/4" )
	PORT_DIPSETTING(    0x40, "2 Coins/1 Credit 4/3" )
	PORT_DIPSETTING(    0xf0, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x10, "1 Coin/1 Credit 2/3" )
	PORT_DIPSETTING(    0x20, "1 Coin/1 Credit 4/5" )
	PORT_DIPSETTING(    0x30, "1 Coin/1 Credit 5/6" )
	PORT_DIPSETTING(    0x60, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0xe0, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0xd0, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0xb0, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0xa0, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x00, "Free Play (if Coin A too) or 1/1" )
INPUT_PORTS_END


static INPUT_PORTS_START( shinobi )
	PORT_INCLUDE( sys16_common )

	PORT_START("DSW2")
	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Cabinet ) ) PORT_DIPLOCATION("SW2:1")
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SW2:2")
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Lives ) ) PORT_DIPLOCATION("SW2:3,4")
	PORT_DIPSETTING(    0x08, "2" )
	PORT_DIPSETTING(    0x0c, "3" )
	PORT_DIPSETTING(    0x04, "5" )
	PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SW2:5,6")
	PORT_DIPSETTING(    0x20, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x40, 0x40, "Enemy's Bullet Speed" ) PORT_DIPLOCATION("SW2:7")
	PORT_DIPSETTING(    0x40, "Slow" )
	PORT_DIPSETTING(    0x00, "Fast" )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Language ) ) PORT_DIPLOCATION("SW2:8")
	PORT_DIPSETTING(    0x80, DEF_STR( Japanese ) )
	PORT_DIPSETTING(    0x00, DEF_STR( English ) )
INPUT_PORTS_END

static INPUT_PORTS_START( passsht )
	PORT_INCLUDE( sys16_common )

	PORT_MODIFY("P1")
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON4 )

	PORT_MODIFY("P2")
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_COCKTAIL

	PORT_START("DSW2")
	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SW2:1")
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x0e, 0x0e, "Initial Point" ) PORT_DIPLOCATION("SW2:2,3,4")
	PORT_DIPSETTING(    0x06, "2000" )
	PORT_DIPSETTING(    0x0a, "3000" )
	PORT_DIPSETTING(    0x0c, "4000" )
	PORT_DIPSETTING(    0x0e, "5000" )
	PORT_DIPSETTING(    0x08, "6000" )
	PORT_DIPSETTING(    0x04, "7000" )
	PORT_DIPSETTING(    0x02, "8000" )
	PORT_DIPSETTING(    0x00, "9000" )
	PORT_DIPNAME( 0x30, 0x30, "Point Table" ) PORT_DIPLOCATION("SW2:5,6")
	PORT_DIPSETTING(    0x20, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SW2:7,8")
	PORT_DIPSETTING(    0x80, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
INPUT_PORTS_END

static INPUT_PORTS_START( passht4b )
	PORT_INCLUDE( passsht )

	PORT_MODIFY("P1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON4 )

	PORT_MODIFY("P2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_COCKTAIL
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_COCKTAIL
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_COCKTAIL

	PORT_MODIFY("SERVICE")
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START3 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START4 )

	PORT_START("P3")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(3)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(3)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(3)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(3)

	PORT_START("P4")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(4)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(4)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(4)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(4)
INPUT_PORTS_END

static INPUT_PORTS_START( wb3b )
	PORT_INCLUDE( sys16_common )

	PORT_START("DSW2")
	PORT_DIPUNUSED_DIPLOC( 0x01, 0x01, "SW2:1" ) /* Listed as "Unused" */
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SW2:2")
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Lives ) ) PORT_DIPLOCATION("SW2:3,4")
	PORT_DIPSETTING(    0x00, "2" )
	PORT_DIPSETTING(    0x0c, "3" )
	PORT_DIPSETTING(    0x08, "4" )
	PORT_DIPSETTING(    0x04, "5" )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Bonus_Life ) ) PORT_DIPLOCATION("SW2:5")
	PORT_DIPSETTING(    0x10, "50k/100k/180k/300k" )
	PORT_DIPSETTING(    0x00, "50k/150k/300k" )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SW2:6")
	PORT_DIPSETTING(    0x20, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hard ) )
	PORT_DIPNAME( 0x40, 0x40, "Test Mode" ) PORT_DIPLOCATION("SW2:7")
	PORT_DIPSETTING(    0x40, DEF_STR( No ) )	/* Normal game */
	PORT_DIPSETTING(    0x00, DEF_STR( Yes ) )	/* Levels are selectable / Player is Invincible */
	PORT_DIPUNUSED_DIPLOC( 0x80, 0x80, "SW2:8" ) /* Listed as "Unused" */
INPUT_PORTS_END

static INPUT_PORTS_START( bayroute )
	PORT_INCLUDE( sys16_common )

	PORT_START("DSW2")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Allow_Continue ) ) PORT_DIPLOCATION("SW2:1")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SW2:2")
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Lives ) ) PORT_DIPLOCATION("SW2:3,4")
	PORT_DIPSETTING(    0x04, "1" )
	PORT_DIPSETTING(    0x0c, "3" )
	PORT_DIPSETTING(    0x08, "5" )
	PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Bonus_Life ) ) PORT_DIPLOCATION("SW2:5,6")
	PORT_DIPSETTING(    0x30, "100k" )
	PORT_DIPSETTING(    0x20, "150k" )
	PORT_DIPSETTING(    0x10, "200k" )
	PORT_DIPSETTING(    0x00, DEF_STR( None ) )
	PORT_DIPNAME( 0xc0, 0x80, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SW2:7,8")
	PORT_DIPSETTING(    0xc0, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
INPUT_PORTS_END

static INPUT_PORTS_START( goldnaxe )
	PORT_INCLUDE( sys16_common )

	PORT_START("DSW2")
	PORT_DIPNAME( 0x01, 0x01, "Credits Needed" ) PORT_DIPLOCATION("SW2:1")
	PORT_DIPSETTING(    0x01, "1 Credit To Start" )
	PORT_DIPSETTING(    0x00, "2 Credits To Start" )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SW2:2")
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x3c, 0x3c, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SW2:3,4,5,6") /* Definition according to manual */
	PORT_DIPSETTING(    0x00, "Special" )
	PORT_DIPSETTING(    0x14, DEF_STR( Easiest ) )
	PORT_DIPSETTING(    0x1c, DEF_STR( Easier ) )
	PORT_DIPSETTING(    0x34, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x3c, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x38, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x2c, DEF_STR( Harder ) )
	PORT_DIPSETTING(    0x28, DEF_STR( Hardest ) )
	PORT_DIPUNUSED_DIPLOC( 0x40, 0x40, "SW2:7" ) /* Listed as "Unused" */
	PORT_DIPUNUSED_DIPLOC( 0x80, 0x80, "SW2:8" ) /* Listed as "Unused" */

/*  PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Lives ) ) PORT_DIPLOCATION("SW2:3,4")
    PORT_DIPSETTING(    0x08, "1" )
    PORT_DIPSETTING(    0x0c, "2" )
    PORT_DIPSETTING(    0x04, "3" )
    PORT_DIPSETTING(    0x00, "5" )
    PORT_DIPNAME( 0x30, 0x30, "Energy Meter" ) PORT_DIPLOCATION("SW2:5,6")
    PORT_DIPSETTING(    0x20, "2" )
    PORT_DIPSETTING(    0x30, "3" )
    PORT_DIPSETTING(    0x10, "4" )
    PORT_DIPSETTING(    0x00, "5" )
*/
INPUT_PORTS_END

static INPUT_PORTS_START( tturf )
	PORT_INCLUDE( sys16_common )

	PORT_START("DSW2")
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Continues ) ) PORT_DIPLOCATION("SW2:1,2")
	PORT_DIPSETTING(    0x00, DEF_STR( None ) )
	PORT_DIPSETTING(    0x01, "3" )
	PORT_DIPSETTING(    0x02, "Unlimited" )
	PORT_DIPSETTING(    0x03, "Unlimited" )
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SW2:3,4")
	PORT_DIPSETTING(    0x08, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x30, 0x20, "Starting Energy" ) PORT_DIPLOCATION("SW2:5,6")
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x10, "4" )
	PORT_DIPSETTING(    0x20, "6" )
	PORT_DIPSETTING(    0x30, "8" )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SW2:7")
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x00, "Bonus Energy" ) PORT_DIPLOCATION("SW2:8")
	PORT_DIPSETTING(    0x80, "1" )
	PORT_DIPSETTING(    0x00, "2" )
INPUT_PORTS_END

static INPUT_PORTS_START( ddux )
	PORT_INCLUDE( sys16_common )

	PORT_START("DSW2")
	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SW2:1")
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x06, 0x06, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SW2:2,3")
	PORT_DIPSETTING(    0x04, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x06, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x18, 0x18, DEF_STR( Lives ) ) PORT_DIPLOCATION("SW2:4,5")
	PORT_DIPSETTING(    0x10, "2" )
	PORT_DIPSETTING(    0x18, "3" )
	PORT_DIPSETTING(    0x08, "4" )
	PORT_DIPSETTING(    0x00, "5" )
	PORT_DIPNAME( 0x60, 0x60, DEF_STR( Bonus_Life ) ) PORT_DIPLOCATION("SW2:6,7")
	PORT_DIPSETTING(    0x40, "150k" )
	PORT_DIPSETTING(    0x60, "200k" )
	PORT_DIPSETTING(    0x20, "300k" )
	PORT_DIPSETTING(    0x00, "400k" )
	PORT_DIPUNUSED_DIPLOC( 0x80, 0x80, "SW2:8" ) /* Listed as "Unused" */
INPUT_PORTS_END

static INPUT_PORTS_START( eswat )
	PORT_INCLUDE( sys16_common )

	PORT_START("DSW2")
	PORT_DIPNAME( 0x01, 0x01, "Credits To Start" ) PORT_DIPLOCATION("SW2:1")
	PORT_DIPSETTING(    0x01, "1 Credit" )
	PORT_DIPSETTING(    0x00, "2 Credits" )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SW2:2")
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Flip_Screen ) ) PORT_DIPLOCATION("SW2:3")
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, "Timer" ) PORT_DIPLOCATION("SW2:4")
	PORT_DIPSETTING(    0x08, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hard ) )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SW2:5,6")
	PORT_DIPSETTING(    0x20, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Lives ) ) PORT_DIPLOCATION("SW2:7,8")
	PORT_DIPSETTING(    0x00, "1" )
	PORT_DIPSETTING(    0x40, "2" )
	PORT_DIPSETTING(    0xc0, "3" )
	PORT_DIPSETTING(    0x80, "4" )
INPUT_PORTS_END

static INPUT_PORTS_START( fpointbl )
	PORT_START("SERVICE")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_SERVICE_NO_TOGGLE(0x04, IP_ACTIVE_LOW)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("P1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON3 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY

	PORT_START("P2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_COCKTAIL
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL

	PORT_START("DSW2")
	PORT_DIPUNUSED_DIPLOC( 0x01, 0x01, "SW2:1" ) /* Listed as "Unused" */
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SW2:2")
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPUNUSED_DIPLOC( 0x04, 0x04, "SW2:3" ) /* Listed as "Unused" */
	PORT_DIPUNUSED_DIPLOC( 0x08, 0x08, "SW2:4" ) /* Listed as "Unused" */
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SW2:5,6")
	PORT_DIPSETTING(    0x20, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x40, 0x40, "Clear Round Allowed" ) PORT_DIPLOCATION("SW2:7") /* Use button 3 */
	PORT_DIPSETTING(    0x00, "1" )
	PORT_DIPSETTING(    0x40, "2" )
	/* SW2:8 The mode in which a block falls at twice [ usual ] speed as this when playing 25 minutes or more on one coin. */
	PORT_DIPNAME( 0x80, 0x80, "2 Cell Move" ) PORT_DIPLOCATION("SW2:8")
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Yes ) )

	PORT_START("DSW1")
	PORT_DIPNAME( 0x0f, 0x0f, DEF_STR( Coin_A ) ) PORT_DIPLOCATION("SW1:1,2,3,4")
	PORT_DIPSETTING(    0x07, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x09, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x05, "2 Coins/1 Credit 5/3 6/4" )
	PORT_DIPSETTING(    0x04, "2 Coins/1 Credit 4/3" )
	PORT_DIPSETTING(    0x0f, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x01, "1 Coin/1 Credit 2/3" )
	PORT_DIPSETTING(    0x02, "1 Coin/1 Credit 4/5" )
	PORT_DIPSETTING(    0x03, "1 Coin/1 Credit 5/6" )
	PORT_DIPSETTING(    0x06, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x0e, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x0d, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x0b, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x0a, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x00, "Free Play (if Coin B too) or 1/1" )
	PORT_DIPNAME( 0xf0, 0xf0, DEF_STR( Coin_B ) ) PORT_DIPLOCATION("SW1:5,6,7,8")
	PORT_DIPSETTING(    0x70, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x90, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x50, "2 Coins/1 Credit 5/3 6/4" )
	PORT_DIPSETTING(    0x40, "2 Coins/1 Credit 4/3" )
	PORT_DIPSETTING(    0xf0, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x10, "1 Coin/1 Credit 2/3" )
	PORT_DIPSETTING(    0x20, "1 Coin/1 Credit 4/5" )
	PORT_DIPSETTING(    0x30, "1 Coin/1 Credit 5/6" )
	PORT_DIPSETTING(    0x60, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0xe0, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0xd0, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0xb0, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0xa0, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x00, "Free Play (if Coin A too) or 1/1" )
INPUT_PORTS_END

static INPUT_PORTS_START( tetris )
	PORT_INCLUDE( sys16_common )	/* unconfirmed coinage dip */

	PORT_START("DSW2")
	PORT_DIPUNUSED_DIPLOC( 0x01, 0x01, "SW2:1" ) /* Listed as "Unused" */
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SW2:2")
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	/* From the code SW2:3,4 looks like some kind of difficulty level,
    but all 4 levels points to the same place so it doesn't actually change anything!! */
	PORT_DIPUNUSED_DIPLOC( 0x04, 0x04, "SW2:3" ) /* Listed as "Unused" */
	PORT_DIPUNUSED_DIPLOC( 0x08, 0x08, "SW2:4" ) /* Listed as "Unused" */
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SW2:5,6")
	PORT_DIPSETTING(    0x20, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPUNUSED_DIPLOC( 0x40, 0x40, "SW2:7" ) /* Listed as "Unused" */
	PORT_DIPUNUSED_DIPLOC( 0x80, 0x80, "SW2:8" ) /* Listed as "Unused" */
INPUT_PORTS_END

/* System 18 Bootlegs */
static INPUT_PORTS_START( astormbl )
	PORT_START("P1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON3 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY

	PORT_START("P2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)

	PORT_START("P3")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(3)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(3)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(3)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(3)

	PORT_START("SERVICE")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_SERVICE_NO_TOGGLE(0x04, IP_ACTIVE_LOW)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START3 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN1 )

	PORT_START("COINAGE")
	PORT_DIPNAME( 0x0f, 0x0f, DEF_STR( Coin_A ) ) PORT_DIPLOCATION("SW1:1,2,3,4")
	PORT_DIPSETTING(    0x07, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x09, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x05, "2 Coins/1 Credit 5/3 6/4" )
	PORT_DIPSETTING(    0x04, "2 Coins/1 Credit 4/3" )
	PORT_DIPSETTING(    0x0f, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x01, "1 Coin/1 Credit 2/3" )
	PORT_DIPSETTING(    0x02, "1 Coin/1 Credit 4/5" )
	PORT_DIPSETTING(    0x03, "1 Coin/1 Credit 5/6" )
	PORT_DIPSETTING(    0x06, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x0e, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x0d, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x0b, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x0a, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x00, "Free Play (if Coin B too) or 1/1" )
	PORT_DIPNAME( 0xf0, 0xf0, DEF_STR( Coin_B ) ) PORT_DIPLOCATION("SW1:5,6,7,8")
	PORT_DIPSETTING(    0x70, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x90, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x50, "2 Coins/1 Credit 5/3 6/4" )
	PORT_DIPSETTING(    0x40, "2 Coins/1 Credit 4/3" )
	PORT_DIPSETTING(    0xf0, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x10, "1 Coin/1 Credit 2/3" )
	PORT_DIPSETTING(    0x20, "1 Coin/1 Credit 4/5" )
	PORT_DIPSETTING(    0x30, "1 Coin/1 Credit 5/6" )
	PORT_DIPSETTING(    0x60, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0xe0, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0xd0, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0xb0, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0xa0, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x00, "Free Play (if Coin A too) or 1/1" )

	PORT_START("DSW1")
	PORT_DIPNAME( 0x01, 0x01, "2 Credits to Start" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x1c, 0x1c, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Easiest ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Easier ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x1c, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x14, DEF_STR( Harder ) )
	PORT_DIPSETTING(    0x18, DEF_STR( Hardest ) )
	PORT_DIPSETTING(    0x00, "Special" )
	PORT_DIPNAME( 0x20, 0x20, "Coin Chutes" )
	PORT_DIPSETTING(    0x20, "3" )
	PORT_DIPSETTING(    0x00, "1" )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END

static INPUT_PORTS_START( mwalkbl )
	PORT_INCLUDE( astormbl )

	PORT_MODIFY("P3")
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START3 )

	PORT_MODIFY("SERVICE")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_MODIFY("DSW1")
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x04, "2" )
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPNAME( 0x08, 0x08, "Player Vitality" )
	PORT_DIPSETTING(    0x08, DEF_STR( Low ) )
	PORT_DIPSETTING(    0x00, DEF_STR( High ) )
	PORT_DIPNAME( 0x10, 0x00, "Play Mode" )
	PORT_DIPSETTING(    0x10, "2 Players" )
	PORT_DIPSETTING(    0x00, "3 Players" )
	PORT_DIPNAME( 0x20, 0x20, "Coin Mode" )
	PORT_DIPSETTING(    0x20, "Common" )
	PORT_DIPSETTING(    0x00, "Individual" )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
INPUT_PORTS_END


/*************************************
 *
 *  Graphics definitions
 *
 *************************************/

static const gfx_layout charlayout  =
{
	8,8,
	RGN_FRAC(1,3),
	3,
	{ RGN_FRAC(2,3), RGN_FRAC(1,3), RGN_FRAC(0,3) },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};

GFXDECODE_START( sys16 )
	GFXDECODE_ENTRY( "gfx1", 0, charlayout,	0, 1024 )
GFXDECODE_END


/*************************************
 *
 *  Machine driver
 *
 *************************************/

/* System 16A/B Bootlegs */
static MACHINE_DRIVER_START( system16 )

	/* driver data */
	MDRV_DRIVER_DATA(segas1x_bootleg_state)

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", M68000, 10000000)
	MDRV_CPU_VBLANK_INT("screen", sys16_interrupt)

	MDRV_CPU_ADD("soundcpu", Z80, 4000000)
	MDRV_CPU_PROGRAM_MAP(sound_map)
	MDRV_CPU_IO_MAP(sound_io_map)

	/* video hardware */
	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(40*8, 28*8)
	MDRV_SCREEN_VISIBLE_AREA(0*8, 40*8-1, 0*8, 28*8-1)

	MDRV_GFXDECODE(sys16)
	MDRV_PALETTE_LENGTH(2048*SHADOW_COLORS_MULTIPLIER)

	MDRV_VIDEO_START(system16)
	MDRV_VIDEO_UPDATE(system16)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")

	MDRV_SOUND_ADD("ymsnd", YM2151, 4000000)
	MDRV_SOUND_ROUTE(0, "lspeaker", 0.32)
	MDRV_SOUND_ROUTE(1, "rspeaker", 0.32)
MACHINE_DRIVER_END


static void sound_cause_nmi( running_device *device, int chip )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)device->machine->driver_data;

	/* upd7759 callback */
	cpu_set_input_line(state->soundcpu, INPUT_LINE_NMI, PULSE_LINE);
}


const upd7759_interface sys16_upd7759_interface  =
{
	sound_cause_nmi
};


static MACHINE_DRIVER_START( system16_7759 )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(system16)

	MDRV_CPU_MODIFY("soundcpu")
	MDRV_CPU_PROGRAM_MAP(sound_7759_map)
	MDRV_CPU_IO_MAP(sound_7759_io_map)

	/* sound hardware */
	MDRV_SOUND_ADD("7759", UPD7759, UPD7759_STANDARD_CLOCK)
	MDRV_SOUND_CONFIG(sys16_upd7759_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "lspeaker", 0.48)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "rspeaker", 0.48)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( system16_datsu_sound )
	/* TODO:
    - other games might use this sound configuration
    - speaker is likely to be mono for the bootlegs, not stereo.
    - check msm5205 frequency.
    */
	MDRV_CPU_ADD("soundcpu",Z80, 4000000)
	MDRV_CPU_PROGRAM_MAP(tturfbl_sound_map)
	MDRV_CPU_IO_MAP(tturfbl_sound_io_map)

	MDRV_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")

	MDRV_SOUND_ADD("ymsnd", YM2151, 4000000)
	MDRV_SOUND_ROUTE(0, "lspeaker", 0.32)
	MDRV_SOUND_ROUTE(1, "rspeaker", 0.32)

	MDRV_SOUND_ADD("5205", MSM5205, 220000)
	MDRV_SOUND_CONFIG(tturfbl_msm5205_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "lspeaker", 0.80)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "rspeaker", 0.80)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( shinobib )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(system16)
	MDRV_CPU_MODIFY("maincpu")
	MDRV_CPU_PROGRAM_MAP(shinobib_map)

	MDRV_SEGA16SP_ADD_SHINOBI_BOOTLEG("segaspr1")

	MDRV_VIDEO_START( s16a_bootleg_shinobi )
	MDRV_VIDEO_UPDATE( s16a_bootleg )
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( passshtb )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(system16_7759)
	MDRV_CPU_MODIFY("maincpu")
	MDRV_CPU_PROGRAM_MAP(passshtb_map)

	MDRV_SEGA16SP_ADD_PASSINGSHOT_BOOTLEG("segaspr1")

	MDRV_VIDEO_START( s16a_bootleg_passsht )
	MDRV_VIDEO_UPDATE( s16a_bootleg )
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( passsht4b )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(system16_7759)
	MDRV_CPU_MODIFY("maincpu")
	MDRV_CPU_PROGRAM_MAP(passht4b_map)

	// wrong
	MDRV_SEGA16SP_ADD_PASSINGSHOT_BOOTLEG("segaspr1")

	MDRV_VIDEO_START( s16a_bootleg_passsht )
	MDRV_VIDEO_UPDATE( s16a_bootleg_passht4b )
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( wb3bb )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(system16)
	MDRV_CPU_MODIFY("maincpu")
	MDRV_CPU_PROGRAM_MAP(wb3bbl_map)

	MDRV_SEGA16SP_ADD_WONDERBOY3_BOOTLEG("segaspr1")

	MDRV_VIDEO_START( s16a_bootleg_wb3bl )
	MDRV_VIDEO_UPDATE( s16a_bootleg )
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( goldnaxeb1 )

	/* driver data */
	MDRV_DRIVER_DATA(segas1x_bootleg_state)

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", M68000, 10000000)
	MDRV_CPU_PROGRAM_MAP(goldnaxeb1_map)
	MDRV_CPU_VBLANK_INT("screen", sys16_interrupt)


	/* video hardware */
	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(40*8, 28*8)
	MDRV_SCREEN_VISIBLE_AREA(0*8, 40*8-1, 0*8, 28*8-1)

	MDRV_GFXDECODE(sys16)
	MDRV_PALETTE_LENGTH(2048*SHADOW_COLORS_MULTIPLIER)

	MDRV_SEGA16SP_ADD_16B("segaspr1")

	MDRV_PALETTE_INIT( all_black )
	MDRV_VIDEO_START(system16)
	MDRV_VIDEO_UPDATE(system16)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( goldnaxeb2 )
	/* basic machine hardware */
	MDRV_IMPORT_FROM(goldnaxeb1)
	MDRV_CPU_MODIFY("maincpu")
	MDRV_CPU_PROGRAM_MAP(goldnaxeb2_map)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( bayrouteb1 )
	/* basic machine hardware */
	MDRV_IMPORT_FROM(goldnaxeb1)
	MDRV_CPU_MODIFY("maincpu")
	MDRV_CPU_PROGRAM_MAP(bayrouteb1_map)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( bayrouteb2 )
	/* basic machine hardware */
	MDRV_IMPORT_FROM(goldnaxeb1)
	MDRV_CPU_MODIFY("maincpu")
	MDRV_CPU_PROGRAM_MAP(bayrouteb2_map)

	MDRV_IMPORT_FROM(system16_datsu_sound)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( tturfbl )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(system16_7759)
	MDRV_CPU_MODIFY("maincpu")
	MDRV_CPU_PROGRAM_MAP(tturfbl_map)

	MDRV_CPU_MODIFY("soundcpu")
	MDRV_CPU_PROGRAM_MAP(tturfbl_sound_map)
	MDRV_CPU_IO_MAP(tturfbl_sound_io_map)

	MDRV_DEVICE_REMOVE("7759")
	MDRV_SOUND_ADD("5205", MSM5205, 220000)
	MDRV_SOUND_CONFIG(tturfbl_msm5205_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "lspeaker", 0.80)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "rspeaker", 0.80)

	MDRV_SEGA16SP_ADD_16B("segaspr1")
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( dduxbl )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(system16)
	MDRV_CPU_MODIFY("maincpu")
	MDRV_CPU_PROGRAM_MAP(dduxbl_map)

	MDRV_SEGA16SP_ADD_16B("segaspr1")
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( eswatbl )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(system16_7759)
	MDRV_CPU_MODIFY("maincpu")
	MDRV_CPU_PROGRAM_MAP(eswatbl_map)

	MDRV_SEGA16SP_ADD_16B("segaspr1")
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( fpointbl )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(system16)
	MDRV_CPU_MODIFY("maincpu")
	MDRV_CPU_PROGRAM_MAP(fpointbl_map)

	MDRV_CPU_MODIFY("soundcpu")
	MDRV_CPU_PROGRAM_MAP(fpointbl_sound_map)

	MDRV_SEGA16SP_ADD_16B("segaspr1")
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( tetrisbl )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(system16)
	MDRV_CPU_MODIFY("maincpu")
	MDRV_CPU_PROGRAM_MAP(tetrisbl_map)

	MDRV_SEGA16SP_ADD_16B("segaspr1")
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( beautyb )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(system16)
	MDRV_CPU_MODIFY("maincpu")
	MDRV_CPU_PROGRAM_MAP(beautyb_map)

	MDRV_SEGA16SP_ADD_16B("segaspr1")
MACHINE_DRIVER_END


/* System 18 Bootlegs */
static MACHINE_DRIVER_START( system18 )

	/* driver data */
	MDRV_DRIVER_DATA(segas1x_bootleg_state)

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", M68000, 10000000)
	MDRV_CPU_VBLANK_INT("screen", irq4_line_hold)

	MDRV_CPU_ADD("soundcpu", Z80, 8000000)
	MDRV_CPU_PROGRAM_MAP(sound_18_map)
	MDRV_CPU_IO_MAP(sound_18_io_map)

	/* video hardware */
	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(40*8, 28*8)
	MDRV_SCREEN_VISIBLE_AREA(0*8, 40*8-1, 0*8, 28*8-1)

	MDRV_GFXDECODE(sys16)
	MDRV_PALETTE_LENGTH((2048+2048)*SHADOW_COLORS_MULTIPLIER) // 64 extra colours for vdp (but we use 2048 so shadow mask works)

	MDRV_VIDEO_START(system18old)
	MDRV_VIDEO_UPDATE(system18old)

	MDRV_SEGA16SP_ADD_16B("segaspr1")

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")

	MDRV_SOUND_ADD("3438.0", YM3438, 8000000)
	MDRV_SOUND_ROUTE(0, "lspeaker", 0.40)
	MDRV_SOUND_ROUTE(1, "rspeaker", 0.40)
	MDRV_SOUND_ROUTE(2, "lspeaker", 0.40)
	MDRV_SOUND_ROUTE(3, "rspeaker", 0.40)

	MDRV_SOUND_ADD("3438.1", YM3438, 8000000)
	MDRV_SOUND_ROUTE(0, "lspeaker", 0.40)
	MDRV_SOUND_ROUTE(1, "rspeaker", 0.40)
	MDRV_SOUND_ROUTE(2, "lspeaker", 0.40)
	MDRV_SOUND_ROUTE(3, "rspeaker", 0.40)

	MDRV_SOUND_ADD("5c68", RF5C68, 8000000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "lspeaker", 1.0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "rspeaker", 1.0)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( astormbl )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(system18)
	MDRV_CPU_MODIFY("maincpu")
	MDRV_CPU_PROGRAM_MAP(astormbl_map)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( mwalkbl )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(system18)
	MDRV_CPU_MODIFY("maincpu")
	MDRV_CPU_PROGRAM_MAP(mwalkbl_map)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( shdancbl )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(system18)
	MDRV_CPU_MODIFY("maincpu")
	MDRV_CPU_PROGRAM_MAP(shdancbl_map)

	MDRV_CPU_MODIFY("soundcpu")
	MDRV_CPU_PROGRAM_MAP(shdancbl_sound_map)
	MDRV_CPU_IO_MAP(shdancbl_sound_io_map)
	MDRV_DEVICE_REMOVE("5c68")

	MDRV_SOUND_ADD("5205", MSM5205, 200000)
	MDRV_SOUND_CONFIG(shdancbl_msm5205_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "lspeaker", 0.80)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "rspeaker", 0.80)
MACHINE_DRIVER_END


/*************************************
 *
 *  ROM definition(s)
 *
 *************************************/

/******************************
    System 16A Bootlegs
******************************/

/* Shinobi bootleg by 'Datsu' - Sound hardware is different */
ROM_START( shinobld )
	ROM_REGION( 0x040000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "12.bin", 0x000001, 0x10000, CRC(757a0c71) SHA1(8f476b0fd5f5dd480489af09b99585c58a4801fc) )
	ROM_LOAD16_BYTE( "14.bin", 0x000000, 0x10000, CRC(a65870b2) SHA1(3e9b4aa694bf86ef9db4756ebaa3d8d87d7f269a) )
	ROM_LOAD16_BYTE( "13.bin", 0x020001, 0x10000, CRC(c4334bcd) SHA1(ea1dd23ca6fbf632d8e10bbb9ced6515a69bd14a) )
	ROM_LOAD16_BYTE( "15.bin", 0x020000, 0x10000, CRC(b70a6ec1) SHA1(79db41c36d6a053bcdc355b46b19ae938a7755a9) )

	ROM_REGION( 0x30000, "gfx1", ROMREGION_INVERT ) /* tiles */
	ROM_LOAD( "9.bin",  0x00000, 0x10000, CRC(565e11c6) SHA1(e063400b3d0470b932d75da0be9cd4b446189dea) )
	ROM_LOAD( "10.bin", 0x10000, 0x10000, CRC(7cc40b6c) SHA1(ffad7eef7ab2ff9a2e49a8d71b5785a61fa3c675) )
	ROM_LOAD( "11.bin", 0x20000, 0x10000, CRC(0f6c7b1c) SHA1(defc76592c285b3396e89a3cff7a73f3a948117f) )

	ROM_REGION16_BE( 0x080000, "gfx2", ROMREGION_ERASEFF ) /* sprites */
	ROM_LOAD16_BYTE( "5.bin", 0x00001, 0x10000, CRC(611f413a) SHA1(180f83216e2dfbfd77b0fb3be83c3042954d12df) )
	ROM_LOAD16_BYTE( "3.bin", 0x00000, 0x10000, CRC(5eb00fc1) SHA1(97e02eee74f61fabcad2a9e24f1868cafaac1d51) )
	ROM_LOAD16_BYTE( "8.bin", 0x20001, 0x10000, CRC(3c0797c0) SHA1(df18c7987281bd9379026c6cf7f96f6ae49fd7f9) )
	ROM_LOAD16_BYTE( "2.bin", 0x20000, 0x10000, CRC(25307ef8) SHA1(91ffbe436f80d583524ee113a8b7c0cf5d8ab286) )
	ROM_LOAD16_BYTE( "6.bin", 0x40001, 0x10000, CRC(c29ac34e) SHA1(b5e9b8c3233a7d6797f91531a0d9123febcf1660) )
	ROM_LOAD16_BYTE( "4.bin", 0x40000, 0x10000, CRC(04a437f8) SHA1(ea5fed64443236e3404fab243761e60e2e48c84c) )
	ROM_LOAD16_BYTE( "7.bin", 0x60001, 0x10000, CRC(41f41063) SHA1(5cc461e9738dddf9eea06831fce3702d94674163) )
	ROM_LOAD16_BYTE( "1.bin", 0x60000, 0x10000, CRC(b6e1fd72) SHA1(eb86e4bf880bd1a1d9bcab3f2f2e917bcaa06172) )

	ROM_REGION( 0x10000, "soundcpu", 0 ) /* sound CPU + data */
	ROM_LOAD( "16.bin", 0x0000, 0x10000, CRC(52c8364e) SHA1(01d30b82f92498d155d2e31d43d58dff0285cce3) )
ROM_END

/* Passing Shot Bootleg is a decrypted version of Passing Shot Japanese (passshtj). It has been heavily modified */
ROM_START( passshtb )
	ROM_REGION( 0x020000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "pass3_2p.bin", 0x000000, 0x10000, CRC(26bb9299) SHA1(11bacf86dfdd8bcfbfb61f0ebc59890325c48adc) )
	ROM_LOAD16_BYTE( "pass4_2p.bin", 0x000001, 0x10000, CRC(06ac6d5d) SHA1(2dd71a8a956404326797de8beed7bca016c9919e) )

	ROM_REGION( 0x30000, "gfx1", 0 ) /* tiles */
	ROM_LOAD( "opr11854.b9",  0x00000, 0x10000, CRC(d31c0b6c) SHA1(610d04988da70c30300cc5614817eda9d2204f39) )
	ROM_LOAD( "opr11855.b10", 0x10000, 0x10000, CRC(b78762b4) SHA1(d594ef846bd7fed8da91a89906b39c1a2867a1fe) )
	ROM_LOAD( "opr11856.b11", 0x20000, 0x10000, CRC(ea49f666) SHA1(36ccd32cdcbb7fcc300628bb59c220ec3c324d82) )

	ROM_REGION16_BE( 0x80000, "gfx2", ROMREGION_ERASEFF ) /* sprites */
	ROM_LOAD16_BYTE( "opr11862.b1",  0x00001, 0x10000, CRC(b6e94727) SHA1(0838e034f1f10d9cd1312c8c94b5c57387c0c271) )
	ROM_LOAD16_BYTE( "opr11865.b5",  0x00000, 0x10000, CRC(17e8d5d5) SHA1(ac1074b0a705be13c6e3391441e6cfec1d2b3f8a) )
	ROM_LOAD16_BYTE( "opr11863.b2",  0x20001, 0x10000, CRC(3e670098) SHA1(2cfc83f4294be30cd868738886ac546bd8489962) )
	ROM_LOAD16_BYTE( "opr11866.b6",  0x20000, 0x10000, CRC(50eb71cc) SHA1(463b4917ca19c7f4ad2c2845caa104d5e4a2dda3) )
	ROM_LOAD16_BYTE( "opr11864.b3",  0x40001, 0x10000, CRC(05733ca8) SHA1(1dbc7c99450ebe6a9fd8c0244fd3cb38b74984ef) )
	ROM_LOAD16_BYTE( "opr11867.b7",  0x40000, 0x10000, CRC(81e49697) SHA1(a70fa409e3555ad6c8f28930a7026fdf2deb8c65) )

	ROM_REGION( 0x30000, "soundcpu", 0 ) /* sound CPU */
	ROM_LOAD( "epr11857.a7",  0x00000, 0x08000, CRC(789edc06) SHA1(8c89c94e503513c287807d187de78a7fbd75a7cf) )
	ROM_LOAD( "epr11858.a8",  0x10000, 0x08000, CRC(08ab0018) SHA1(0685f80a7d403208c9cfffea3f2035324f3924fe) )
	ROM_LOAD( "epr11859.a9",  0x18000, 0x08000, CRC(8673e01b) SHA1(e79183ab30e683fdf61ced2e9dbe010567c324cb) )
	ROM_LOAD( "epr11860.a10", 0x20000, 0x08000, CRC(10263746) SHA1(1f981fb185c6a9795208ecdcfba36cf892a99ed5) )
	ROM_LOAD( "epr11861.a11", 0x28000, 0x08000, CRC(38b54a71) SHA1(68ec4ef5b115844214ff2213be1ce6678904fbd2) )
ROM_END

ROM_START( passht4b )
	ROM_REGION( 0x20000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "pas4p.3", 0x000000, 0x10000, CRC(2d8bc946) SHA1(35d3e529d4815543d9876fd0545c3d686467abaa) )
	ROM_LOAD16_BYTE( "pas4p.4", 0x000001, 0x10000, CRC(e759e831) SHA1(dd5727dc28010cb988e4951723171171eb645ce8) )

	ROM_REGION( 0x30000, "gfx1", ROMREGION_INVERT ) /* tiles */
	ROM_LOAD( "pas4p.11", 0x00000, 0x10000, CRC(da20fbc9) SHA1(21dc8143f4d1cebae4f86e83495fa84e5293ba48) )
	ROM_LOAD( "pas4p.12", 0x10000, 0x10000, CRC(bebb9211) SHA1(4f56048f6f70b63f74a4c0d64456213d36ce5b26) )
	ROM_LOAD( "pas4p.13", 0x20000, 0x10000, CRC(e37506c3) SHA1(e6fbf15d58f321a3d052fefbe5a1901e4a1734ae) )

	ROM_REGION16_BE( 0x60000, "gfx2", 0 ) /* sprites */
	ROM_LOAD16_BYTE( "opr11862.b1",  0x00001, 0x10000, CRC(b6e94727) SHA1(0838e034f1f10d9cd1312c8c94b5c57387c0c271) )
	ROM_LOAD16_BYTE( "opr11865.b5",  0x00000, 0x10000, CRC(17e8d5d5) SHA1(ac1074b0a705be13c6e3391441e6cfec1d2b3f8a) )
	ROM_LOAD16_BYTE( "opr11863.b2",  0x20001, 0x10000, CRC(3e670098) SHA1(2cfc83f4294be30cd868738886ac546bd8489962) )
	ROM_LOAD16_BYTE( "opr11866.b6",  0x20000, 0x10000, CRC(50eb71cc) SHA1(463b4917ca19c7f4ad2c2845caa104d5e4a2dda3) )
	ROM_LOAD16_BYTE( "opr11864.b3",  0x40001, 0x10000, CRC(05733ca8) SHA1(1dbc7c99450ebe6a9fd8c0244fd3cb38b74984ef) )
	ROM_LOAD16_BYTE( "opr11867.b7",  0x40000, 0x10000, CRC(81e49697) SHA1(a70fa409e3555ad6c8f28930a7026fdf2deb8c65) )

	ROM_REGION( 0x20000, "soundcpu", 0 ) /* sound CPU */
	ROM_LOAD( "pas4p.1",  0x00000, 0x08000, CRC(e60fb017) SHA1(21298036eab55c74427f1c2e3a9623d41bca4849) )
	ROM_LOAD( "pas4p.2",  0x10000, 0x10000, CRC(092e016e) SHA1(713638749efa9dce19c547b84308236110bc85fe) )
ROM_END

ROM_START( wb3bbl )
	ROM_REGION( 0x040000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "wb3_03", 0x000000, 0x10000, CRC(0019ab3b) SHA1(89d49a437690fa6e0c35bb9f1450042f89504714) )
	ROM_LOAD16_BYTE( "wb3_05", 0x000001, 0x10000, CRC(196e17ee) SHA1(71e4345b2c3d1612a3d424c9310fad1e23c8a9f7) )
	ROM_LOAD16_BYTE( "wb3_02", 0x020000, 0x10000, CRC(c87350cb) SHA1(55a8cb68d70b6060dd9a55e281e216ce3917ea5b) )
	ROM_LOAD16_BYTE( "wb3_04", 0x020001, 0x10000, CRC(565d5035) SHA1(e28a132f1a4ce9466945e231c54502178748af98) )

	ROM_REGION( 0x30000, "gfx1", ROMREGION_INVERT ) /* tiles */
	ROM_LOAD( "wb3_14", 0x00000, 0x10000, CRC(d3f20bca) SHA1(0a87f709f8e2a913473512ede408e2cbc535443f) )
	ROM_LOAD( "wb3_15", 0x10000, 0x10000, CRC(96ff9d52) SHA1(791a9da4860e0d42fba98f80a3c6725ad8c73e33) )
	ROM_LOAD( "wb3_16", 0x20000, 0x10000, CRC(afaf0d31) SHA1(d4309329a0a543250788146b63b27ff058c02fc3) )

	ROM_REGION16_BE( 0x100000, "gfx2", ROMREGION_ERASEFF ) /* sprites */
	ROM_LOAD16_BYTE( "epr12090.b1", 0x00001, 0x010000, CRC(aeeecfca) SHA1(496124b170a725ad863c741d4e021ab947511e4c) )
	ROM_LOAD16_BYTE( "epr12094.b5", 0x00000, 0x010000, CRC(615e4927) SHA1(d23f164973afa770714e284a77ddf10f18cc596b) )
	ROM_LOAD16_BYTE( "epr12091.b2", 0x20001, 0x010000, CRC(8409a243) SHA1(bcbb9510a6499d8147543d6befa5a49f4ac055d9) )
	ROM_LOAD16_BYTE( "epr12095.b6", 0x20000, 0x010000, CRC(e774ec2c) SHA1(a4aa15ec7be5539a740ad02ff720458018dbc536) )
	ROM_LOAD16_BYTE( "epr12092.b3", 0x40001, 0x010000, CRC(5c2f0d90) SHA1(e0fbc0f841e4607ad232931368b16e81440a75c4) )
	ROM_LOAD16_BYTE( "epr12096.b7", 0x40000, 0x010000, CRC(0cd59d6e) SHA1(caf754a461feffafcfe7bfc6e89da76c4db257c5) )
	ROM_LOAD16_BYTE( "epr12093.b4", 0x60001, 0x010000, CRC(4891e7bb) SHA1(1be04fcabe9bfa8cf746263a5bcca67902a021a0) )
	ROM_LOAD16_BYTE( "epr12097.b8", 0x60000, 0x010000, CRC(e645902c) SHA1(497cfcf6c25cc2e042e16dbcb1963d2223def15a) )


	ROM_REGION( 0x10000, "soundcpu", 0 ) /* sound CPU */
	ROM_LOAD( "epr12127.a10", 0x0000, 0x8000, CRC(0bb901bb) SHA1(c81b198df8e3b0ec568032c76addf0d1a1711194) )
ROM_END


/******************************
    System 16B Bootlegs
******************************/

// protected + encrypted bootleg
ROM_START( bayrouteb1 )
	ROM_REGION( 0xc0000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "b4.bin", 0x000000, 0x10000, CRC(eb6646ae) SHA1(073bc0a3868e70785f44e497a949cd9e3b591a33) )
	ROM_LOAD16_BYTE( "b2.bin", 0x000001, 0x10000, CRC(ecd9cd0e) SHA1(177c38ca02c4e87d6adcae77ce4e9237938d23a9) )
	/* empty 0x20000-0x80000*/
	ROM_LOAD16_BYTE( "br.5a",  0x080000, 0x10000, CRC(9d6fd183) SHA1(5ae78d33c0e929886d84a25c0fbd62ab45dcbff4) )
	ROM_LOAD16_BYTE( "br.2a",  0x080001, 0x10000, CRC(5ca1e3d2) SHA1(51ce67ed0a0054f9c9c4ac56c5775716c44d74b1) )
	ROM_LOAD16_BYTE( "b8.bin", 0x0a0000, 0x10000, CRC(e7ca0331) SHA1(b255939576a84f4d266f31a7fde818e04ff35b24) )
	ROM_LOAD16_BYTE( "b6.bin", 0x0a0001, 0x10000, CRC(2bc748a6) SHA1(9ab760377fde24cecb703726ee3e59ee23d60a3a) )

	// interrupt code, taken from the other bootleg set(!)
	// might be wrong for this, hence the broken sprites
	ROM_LOAD( "protdata", 0x0bf000, 0x01000, BAD_DUMP CRC(5474fd95) SHA1(1cbd47aa8f8b9641ba81942bcaae0bc768fd33fd) )

	// there clearly should be some kind of MCU on this bootleg to put the interrupt code in RAM
	// as the game code waits for this to happen, and the interrupt points there
	ROM_REGION( 0xc0000, "mcu", 0 ) /* MCU code */
	ROM_LOAD( "unknown.mcu", 0x0000, 0x1000, NO_DUMP )


	ROM_REGION( 0x30000, "gfx1", ROMREGION_INVERT) /* tiles */
	ROM_LOAD( "bs16.bin", 0x00000, 0x10000, CRC(a8a5b310) SHA1(8883e1ed48a3e0f7b4c36d83579f93e84e28568c) )
	ROM_LOAD( "bs14.bin", 0x10000, 0x10000, CRC(6bc4d0a8) SHA1(90b9a61c7a140291d72554857ce26d54ebf03fc2) )
	ROM_LOAD( "bs12.bin", 0x20000, 0x10000, CRC(c1f967a6) SHA1(8eb6bbd9e17dc531830bc798b8485c8ea999e56e) )

	ROM_REGION16_BE( 0x80000, "gfx2", 0 ) /* sprites */
	ROM_LOAD16_BYTE( "br_obj0o.1b", 0x00001, 0x10000, CRC(098a5e82) SHA1(c5922f418773bc3629071e584457839d67a370e9) )
	ROM_LOAD16_BYTE( "br_obj0e.5b", 0x00000, 0x10000, CRC(85238af9) SHA1(39989a8d9b60c6d55272b5e2c213341a563dd993) )
	ROM_LOAD16_BYTE( "br_obj1o.2b", 0x20001, 0x10000, CRC(cc641da1) SHA1(28f8a6502702cb9e2cc7f3e98f6c5d201f462fa3) )
	ROM_LOAD16_BYTE( "br_obj1e.6b", 0x20000, 0x10000, CRC(d3123315) SHA1(16a87caed1cabb080d4f35935910b38797344ca5) )
	ROM_LOAD16_BYTE( "br_obj2o.3b", 0x40001, 0x10000, CRC(84efac1f) SHA1(41c43d70dc7ae7e361d6fa12c5790ea7ebf13ca8) )
	ROM_LOAD16_BYTE( "br_obj2e.7b", 0x40000, 0x10000, CRC(b73b12cb) SHA1(e8265ae90aabf1ee0522dbc6541a0f82fec97c7a) )
	ROM_LOAD16_BYTE( "br_obj3o.4b", 0x60001, 0x10000, CRC(a2e238ac) SHA1(c854774c0ffd1ccf6e46591a8fa3c80a4630e007) )
	ROM_LOAD16_BYTE( "bs7.bin",     0x60000, 0x10000, CRC(0c91abcc) SHA1(d25608f3cbacd1bd169f1a2247f007ac8bc8dda0) )

	ROM_REGION( 0x50000, "soundcpu", 0 ) /* sound CPU */
	ROM_LOAD( "epr12459.a10", 0x00000, 0x08000, CRC(3e1d29d0) SHA1(fe3d985983e5132e8a26a02a3f2d8d420cbf1a49) )
	ROM_LOAD( "mpr12460.a11", 0x10000, 0x20000, CRC(0bae570d) SHA1(05fa4a3405666342ab66e696a7344cca97569f19) )
	ROM_LOAD( "mpr12461.a12", 0x30000, 0x20000, CRC(b03b8b46) SHA1(b0283ac377d464f3d9374a992192ec6c515a3c2f) )

	// this is taken from the encrypted golden axe bootleg, the encryption is identical, so it was probably just missing from this dump
	ROM_REGION( 0x30000, "decryption", 0 ) // a 0x800 byte repeating rom containing part of an MS-DOS executable, used as the decryption key
	ROM_LOAD( "12.12",     0x00000, 0x08000, CRC(81ce4576) SHA1(84eea0ab8d8ea8869c12f0b603b91b1188d0e288) )
ROM_END

// unprotected bootleg
ROM_START( bayrouteb2 )
	ROM_REGION( 0xc0000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "br_04", 0x000000, 0x10000, CRC(2e33ebfc) SHA1(f6b5a4bd28d302abd6b1e5a9ec6f2a8b57ff213e) )
	ROM_LOAD16_BYTE( "br_06", 0x000001, 0x10000, CRC(3db42313) SHA1(e1c874ebf83e1a458cefaa038fbe89a9804ca30d) )
	/* empty 0x20000-0x80000*/
	ROM_LOAD16_BYTE( "br_03", 0x080000, 0x20000, CRC(285d256b) SHA1(73eac0131d14f0d7fe2a06cb2e0e57dcf4779cf9) )
	ROM_LOAD16_BYTE( "br_05", 0x080001, 0x20000, CRC(552e6384) SHA1(2770b0c9d961671576e09ada2ebd7bb486f24547) )

	ROM_REGION( 0x30000, "gfx1", ROMREGION_INVERT ) /* tiles */
	/* roms in this set were bad dumps, except for bs12, replaced with roms from above set which should be good versions */
	ROM_LOAD( "bs16.bin", 0x00000, 0x10000, CRC(a8a5b310) SHA1(8883e1ed48a3e0f7b4c36d83579f93e84e28568c) )
	ROM_LOAD( "bs14.bin", 0x10000, 0x10000, CRC(6bc4d0a8) SHA1(90b9a61c7a140291d72554857ce26d54ebf03fc2) )
	ROM_LOAD( "bs12.bin", 0x20000, 0x10000, CRC(c1f967a6) SHA1(8eb6bbd9e17dc531830bc798b8485c8ea999e56e) )

	#if 0 // these look bad
	ROM_REGION16_BE( 0x080000, "gfx2", ROMREGION_ERASEFF ) /* sprites */
	ROM_LOAD16_BYTE( "br_11",       0x00001, 0x10000, CRC(65232905) SHA1(cb195a0ce8bff9d1d3e31678060b3aaccfefcd2d) )
	ROM_LOAD16_BYTE( "br_obj0e.5b", 0x00000, 0x10000, CRC(85238af9) SHA1(39989a8d9b60c6d55272b5e2c213341a563dd993) )
	ROM_LOAD16_BYTE( "br_obj1o.2b", 0x20001, 0x10000, CRC(cc641da1) SHA1(28f8a6502702cb9e2cc7f3e98f6c5d201f462fa3) )
	ROM_LOAD16_BYTE( "br_obj1e.6b", 0x20000, 0x10000, CRC(d3123315) SHA1(16a87caed1cabb080d4f35935910b38797344ca5) )
	ROM_LOAD16_BYTE( "br_obj2o.3b", 0x40001, 0x10000, CRC(84efac1f) SHA1(41c43d70dc7ae7e361d6fa12c5790ea7ebf13ca8) )
	ROM_LOAD16_BYTE( "br_09",       0x40000, 0x10000, CRC(05e9b840) SHA1(7cc1c9ac7b85f1e1bdb68215b5e83eae3ee5ba2a) )
	ROM_LOAD16_BYTE( "br_14",       0x60001, 0x10000, CRC(4c4a177b) SHA1(a9dfd7e56b0a21a0f7750d8ec4631901ad182609) )
	ROM_LOAD16_BYTE( "bs7.bin",     0x60000, 0x10000, CRC(0c91abcc) SHA1(d25608f3cbacd1bd169f1a2247f007ac8bc8dda0) )
	#endif

	// use the roms from the first bootleg set
	ROM_REGION16_BE( 0x080000, "gfx2", ROMREGION_ERASEFF ) /* sprites */
	ROM_LOAD16_BYTE( "br_obj0o.1b", 0x00001, 0x10000, CRC(098a5e82) SHA1(c5922f418773bc3629071e584457839d67a370e9) )
	ROM_LOAD16_BYTE( "br_obj0e.5b", 0x00000, 0x10000, CRC(85238af9) SHA1(39989a8d9b60c6d55272b5e2c213341a563dd993) )
	ROM_LOAD16_BYTE( "br_obj1o.2b", 0x20001, 0x10000, CRC(cc641da1) SHA1(28f8a6502702cb9e2cc7f3e98f6c5d201f462fa3) )
	ROM_LOAD16_BYTE( "br_obj1e.6b", 0x20000, 0x10000, CRC(d3123315) SHA1(16a87caed1cabb080d4f35935910b38797344ca5) )
	ROM_LOAD16_BYTE( "br_obj2o.3b", 0x40001, 0x10000, CRC(84efac1f) SHA1(41c43d70dc7ae7e361d6fa12c5790ea7ebf13ca8) )
	ROM_LOAD16_BYTE( "br_obj2e.7b", 0x40000, 0x10000, CRC(b73b12cb) SHA1(e8265ae90aabf1ee0522dbc6541a0f82fec97c7a) )
	ROM_LOAD16_BYTE( "br_obj3o.4b", 0x60001, 0x10000, CRC(a2e238ac) SHA1(c854774c0ffd1ccf6e46591a8fa3c80a4630e007) )
	ROM_LOAD16_BYTE( "bs7.bin",     0x60000, 0x10000, CRC(0c91abcc) SHA1(d25608f3cbacd1bd169f1a2247f007ac8bc8dda0) )

	ROM_REGION( 0x50000, "soundcpu", 0 ) /* sound CPU */
	ROM_LOAD( "br_01", 0x10000, 0x10000, CRC(b87156ec) SHA1(bdfef2ab5a4d3cac4077c92ce1ef4604b4c11cf8) )
	ROM_LOAD( "br_02", 0x20000, 0x10000, CRC(ef63991b) SHA1(4221741780f88c80b3213ddca949bee7d4c1469a) )
ROM_END

/*

    this is a more complete dump of the old encrypted bootleg set

    Golden Axe (different HW bottleg)

    Anno    1989
    Produttore
    N.revisione

    CPU:
    main PCB:
        1x 68000 (main)(missing)
        1x LH0080B-Z80B-CPU (sound)
        1x D7759C (sound)
        1x YM2151 (sound)
        1x YM3012 (sound)
        1x UPC1241H (sound)
        1x oscillator 20.000 (close to main)
        1x oscillator 24.000MHz (close to sound)
        1x blue resonator 655K (close to sound)

    ROMs:
    main PCB:
        2x NMC27C512 (1,3)
        2x NMC27C256 (2,12)
        8x TMS27PC512 (4,5,6,7,8,9,10,11)

    roms PCB:
        29x Am27C512 (13-32,34-42)
        1x NMC27C512 (33)
        1x PROM N82S129N


    RAMs:
    main PCB:
        1x GM76C28
        2x GM76C88
        2x HM6116K

    roms PCB:
        8x LC3517BS
        2x 256K S BGD-A


    PLDs:
    main PCB:
        3x PAL16L8ACN (not dumped)

    roms PCB:
        1x PEEL18CV8P (not dumped)

    Note
    main PCB:
        1x JAMMA edge connector
        2x 50 pins flat cable connector to roms PCB
        1x trimmer (volume)
        2x 8x2 switches dip

    roms PCB:
        2x 50 pins flat cable connector to roms PCB
*/

ROM_START( goldnaxeb1 )
	ROM_REGION( 0x0c0000, "maincpu", 0 ) /* 68000 code */
	// encrypted code
	ROM_LOAD16_BYTE( "6.6",   0x00000, 0x10000, CRC(f95b459f) SHA1(dadf66d63454ed62fefa521d4fed249d28c63778) )
	ROM_LOAD16_BYTE( "4.4",   0x00001, 0x10000, CRC(83eabdf5) SHA1(1effef966f513fbdec2026d535658e17ef7dea51) )
	ROM_LOAD16_BYTE( "11.11", 0x20000, 0x10000, CRC(f4ef9349) SHA1(3ffa335e74ffbc10f80387268da659643c566897) )
	ROM_LOAD16_BYTE( "8.8",   0x20001, 0x10000, CRC(37a65839) SHA1(6e8055d91b840afd8526041d3752c0a55eaebe0c) )
	/* emtpy 0x40000 - 0x80000 */
	ROM_LOAD16_BYTE( "7.7",   0x80000, 0x10000, CRC(056879fd) SHA1(fc0a910c1dc4cf535ad589f482f57379798b5524) )
	ROM_LOAD16_BYTE( "5.5",   0x80001, 0x10000, CRC(8c77d958) SHA1(f7c5abfec2a9d1724c83b1de2cd994bb4c42857c) )
	ROM_LOAD16_BYTE( "10.10", 0xa0000, 0x10000, CRC(b69ab892) SHA1(9b426058a80abb8dd3d6c0c55574fdc841889a72) )
	ROM_LOAD16_BYTE( "9.9",   0xa0001, 0x10000, CRC(3cf2f725) SHA1(1f620fcebe8533cba50736ae1d97c095abf1bc25) )


	ROM_REGION( 0x60000, "gfx1", ROMREGION_INVERT ) /* tiles */
	ROM_LOAD( "32.16",  0x00000, 0x10000, CRC(84587263) SHA1(3a88c8578a477a487a0a214a367042b9739f39eb) )
	ROM_LOAD( "31.15", 0x10000, 0x10000, CRC(63d72388) SHA1(ba0a582b1daf3a1e316237efbad17fcc0381643f) )
	ROM_LOAD( "30.14",  0x20000, 0x10000, CRC(f8b6ae4f) SHA1(55132c98955107e4b247992f7917a6ce588460a7) )
	ROM_LOAD( "29.13", 0x30000, 0x10000, CRC(e29baf4f) SHA1(3761cb2217599fe3f2f860f9395930b96ec52f47) )
	ROM_LOAD( "28.12",  0x40000, 0x10000, CRC(22f0667e) SHA1(2d11b2ce105a3db9c914942cace85aff17deded9) )
	ROM_LOAD( "27.11", 0x50000, 0x10000, CRC(afb1a7e4) SHA1(726fded9db72a881128b43f449d2baf450131f63) )

	ROM_REGION16_BE( 0x1c0000, "gfx2", 0 ) /* sprites */
	ROM_LOAD16_BYTE( "33.17",		0x000001, 0x10000, CRC(28ba70c8) SHA1(a6f33e1404928b6d1006943494646d6cfbd60a4b) )
	ROM_LOAD16_BYTE( "34.18",		0x020001, 0x10000, CRC(2ed96a26) SHA1(edcf915243e6f92d31cdfc53965438f6b6bff51d) )
	ROM_LOAD16_BYTE( "37.bin",		0x100001, 0x10000, CRC(84dccc5b) SHA1(10263d98d663f1170c3203066f391075a1d64ff5) )
	ROM_LOAD16_BYTE( "13.bin",  	0x120001, 0x10000, CRC(3d41b995) SHA1(913d2a0c9a2ac6d36589966d7eb5537120ea6ff0) )
	//ROM_LOAD16_BYTE( "ga18.a9",     0x120001, 0x10000, CRC(de346006) SHA1(65aa489373b6d2cccbb024f13fc190a7cae86274) ) // the old set had this rom, with most of the data blanked out.. why? logo removed? bad?

	ROM_LOAD16_BYTE( "21.7",		0x000000, 0x10000, CRC(ede51fe0) SHA1(c05a2b51095a322bac5a67ee8886aecc186cbdfe) )
	ROM_LOAD16_BYTE( "22.8",		0x020000, 0x10000, CRC(30989141) SHA1(f6e7ae258deedec11b1f80c19575c884aac26c56) )
	ROM_LOAD16_BYTE( "38.bin",		0x100000, 0x10000, CRC(38e4c92a) SHA1(8b8a596da0726bc02c412a68e5163770fe3e62e4) )
	ROM_LOAD16_BYTE( "14.bin",  	0x120000, 0x10000, CRC(9fb1f9b6) SHA1(a193ff48f2bfbb4afbc322ae33cdf360afcb681e) )

	ROM_LOAD16_BYTE( "35.19",		0x040001, 0x10000, CRC(101d2fff) SHA1(1de1390c5f55f192491053c8aac31be3389aab2b) )
	ROM_LOAD16_BYTE( "36.20",		0x060001, 0x10000, CRC(677e64a6) SHA1(e3d0d31097017c6cb1a7f41292783f18ce13b41c) )
	ROM_LOAD16_BYTE( "15.bin",		0x140001, 0x10000, CRC(11794d05) SHA1(eef52d7a644dbcc5f983222f163445a725286a32) )
	ROM_LOAD16_BYTE( "17.bin",		0x160001, 0x10000, CRC(ad1c1c90) SHA1(155f17593cfab1a117bb755b1edd0c473d455f91) )

	ROM_LOAD16_BYTE( "23.9",		0x040000, 0x10000, CRC(5853000d) SHA1(db7adf1de74c66f667ea7ccc41702576de081ff5) )
	ROM_LOAD16_BYTE( "24.10",		0x060000, 0x10000, CRC(697b3276) SHA1(a8aeb2cfaca9368d5bfa14a67de36dbadd4a0585) )
	ROM_LOAD16_BYTE( "16.bin",		0x140000, 0x10000, CRC(753b01e8) SHA1(0180cf893d63e60e14e2fb0f5836b302f08f0228) )
	ROM_LOAD16_BYTE( "18.bin",		0x160000, 0x10000, CRC(5dd6d8db) SHA1(270886aebf4549c5cf456b1e90927b759a53e2e1) )

	ROM_LOAD16_BYTE( "40.23",		0x080001, 0x10000, CRC(a9519afe) SHA1(0550f596ef080db25241d88242d57edb3c7fb685) )
	ROM_LOAD16_BYTE( "39.22",		0x0a0001, 0x10000, CRC(74df9232) SHA1(61286c83eb6f31983c0ed24ca2151c58a95238f1) )
	ROM_LOAD16_BYTE( "19.bin",		0x180001, 0x10000, CRC(a8c8c784) SHA1(8e998019b4dbf509179d41eb2b446647db3d00a6) )
	ROM_LOAD16_BYTE( "25.bin",		0x1a0001, 0x10000, CRC(cc3a922c) SHA1(3c96459d95961d463909b6b0e8c77916c3a018e3) )

	ROM_LOAD16_BYTE( "42.26",		0x080000, 0x10000, CRC(ac6ad195) SHA1(3c3fe38047698e28cae9193438fe0b85941476c5) )
	ROM_LOAD16_BYTE( "41.25",		0x0a0000, 0x10000, CRC(03a905c4) SHA1(c526a744021a392c860ded37afd54a78e9f47778) )
	ROM_LOAD16_BYTE( "20.bin",		0x180000, 0x10000, CRC(cba013c7) SHA1(a0658aaf7893bc9fb8f0435cab9f77ceb1fb4e1d) )
	ROM_LOAD16_BYTE( "26.bin",		0x1a0000, 0x10000, CRC(bea4d237) SHA1(46e51e89b4ee1e2701da1004758d7da547a2e4c2) )


	ROM_REGION( 0x30000, "soundcpu", 0 ) /* sound CPU */
	ROM_LOAD( "2.3",     0x00000, 0x08000, CRC(399fc5f5) SHA1(6f290b36dc71ff4759598e2a9c185a8945a3c9e7) )
	ROM_LOAD( "3.1",     0x10000, 0x10000, CRC(50eb5a56) SHA1(d59ba04254000de5577e8a58d0b51c73112a4c80) )
	ROM_LOAD( "1.2",     0x20000, 0x10000, CRC(b372eb94) SHA1(8f82530633589de003a5462b227335526a6a61a6) )

	ROM_REGION( 0x30000, "decryption", 0 ) // a 0x800 byte repeating rom containing part of an MS-DOS executable, used as the decryption key
	ROM_LOAD( "12.12",     0x00000, 0x08000, CRC(81ce4576) SHA1(84eea0ab8d8ea8869c12f0b603b91b1188d0e288) )

	ROM_REGION( 0x100, "prom", 0 )
	ROM_LOAD( "82s129.bin",     0x000, 0x100,  CRC(88962e80) SHA1(ebf3d57d53fcba727cf20e4bb26f12934f7d1bc7) )
ROM_END

/*
    Golden Axe (bootleg) made in Italy?
    PCB: GENSYS-1/I

    CPUs
        1x SCN68000CAN64-KGQ7551-8931KE (main - upper board)(IC40)
        1x oscillator 20.000MHz (upper board - near main CPU)(XL2)
        1x STZ8400BB1-Z80BCPU-28911 (sound - upper board)(IC44)
        2x YAMAHA YM2203C (sound - upper board)IC27, IC28)
        1x OKI M5205 (sound - upper board)(IC16)
        2x YAMAHA Y3014B (DAC - sound - upper board)(IC19, IC20)
        1x crystal resonator CSB398P (upper board - near sound CPUs)(XL1)
        1x oscillator 24MHz (lower board near connectors to upper board)(XL1)

    ROMs
        8x TMS27C512-20JL (from 1 to 8 - upper board) (CPU)
        26x ST M27512FI (from 9 to 34 - upper board) (Sound + Tilemap)
        6x ST M27512FI (from 35 to 40 - lower board) (Sprites)

*/

ROM_START( goldnaxeb2 )
	ROM_REGION( 0x0c0000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "ic39.1", 0x00000, 0x10000, CRC(71489c9a) SHA1(ec25463a2c63027d2635f0e27f5a971d68938fe3) )
	ROM_LOAD16_BYTE( "ic53.5", 0x00001, 0x10000, CRC(7a98512d) SHA1(f022a047d23051211e2de761b6c2289438b8f3cc) )
	ROM_LOAD16_BYTE( "ic38.2", 0x20000, 0x10000, CRC(8c8c95b1) SHA1(2b3897b8af587e6397dca0f3409a803432be07aa) )
	ROM_LOAD16_BYTE( "ic52.6", 0x20001, 0x10000, CRC(4f58f15b) SHA1(4dcd7e8cfd8fe5fe9f9259abf53fd0d4c64b5abd) )
	/* emtpy 0x40000 - 0x80000 */
	ROM_LOAD16_BYTE( "ic37.3", 0x80000, 0x10000, CRC(056879fd) SHA1(fc0a910c1dc4cf535ad589f482f57379798b5524) )
	ROM_LOAD16_BYTE( "ic51.7", 0x80001, 0x10000, CRC(8c77d958) SHA1(f7c5abfec2a9d1724c83b1de2cd994bb4c42857c) )
	ROM_LOAD16_BYTE( "ic36.4", 0xa0000, 0x10000, CRC(b69ab892) SHA1(9b426058a80abb8dd3d6c0c55574fdc841889a72) )
	ROM_LOAD16_BYTE( "ic50.8", 0xa0001, 0x10000, CRC(3cf2f725) SHA1(1f620fcebe8533cba50736ae1d97c095abf1bc25) )

	ROM_REGION( 0x60000, "gfx1", ROMREGION_INVERT ) /* tiles */
	ROM_LOAD( "ic4.35",  0x00000, 0x10000, CRC(84587263) SHA1(3a88c8578a477a487a0a214a367042b9739f39eb) )
	ROM_LOAD( "ic18.38", 0x10000, 0x10000, CRC(63d72388) SHA1(ba0a582b1daf3a1e316237efbad17fcc0381643f) )
	ROM_LOAD( "ic3.36",  0x20000, 0x10000, CRC(f8b6ae4f) SHA1(55132c98955107e4b247992f7917a6ce588460a7) )
	ROM_LOAD( "ic17.39", 0x30000, 0x10000, CRC(e29baf4f) SHA1(3761cb2217599fe3f2f860f9395930b96ec52f47) )
	ROM_LOAD( "ic2.37",  0x40000, 0x10000, CRC(22f0667e) SHA1(2d11b2ce105a3db9c914942cace85aff17deded9) )
	ROM_LOAD( "ic16.40", 0x50000, 0x10000, CRC(afb1a7e4) SHA1(726fded9db72a881128b43f449d2baf450131f63) )

	ROM_REGION16_BE( 0x1c0000, "gfx2", 0 ) /* sprites */
	ROM_LOAD16_BYTE( "ic73.34", 		0x000001, 0x10000, CRC(28ba70c8) SHA1(a6f33e1404928b6d1006943494646d6cfbd60a4b) ) // mpr12378.b1  [1/4]      IDENTICAL
	ROM_LOAD16_BYTE( "ic74.33", 		0x020001, 0x10000, CRC(2ed96a26) SHA1(edcf915243e6f92d31cdfc53965438f6b6bff51d) ) // mpr12378.b1  [2/4]      IDENTICAL
	ROM_LOAD16_BYTE( "ic79.28", 		0x100001, 0x10000, CRC(84dccc5b) SHA1(10263d98d663f1170c3203066f391075a1d64ff5) ) // mpr12378.b1  [3/4]      IDENTICAL
	ROM_LOAD16_BYTE( "ic80.27", 		0x120001, 0x10000, CRC(3d41b995) SHA1(913d2a0c9a2ac6d36589966d7eb5537120ea6ff0) ) // mpr12378.b1  [4/4]      IDENTICAL
	ROM_LOAD16_BYTE( "ic58.22", 		0x000000, 0x10000, CRC(ede51fe0) SHA1(c05a2b51095a322bac5a67ee8886aecc186cbdfe) ) // mpr12379.b4  [1/4]      IDENTICAL
	ROM_LOAD16_BYTE( "ic59.21", 		0x020000, 0x10000, CRC(30989141) SHA1(f6e7ae258deedec11b1f80c19575c884aac26c56) ) // mpr12379.b4  [2/4]      IDENTICAL
	ROM_LOAD16_BYTE( "ic64.16", 		0x100000, 0x10000, CRC(38e4c92a) SHA1(8b8a596da0726bc02c412a68e5163770fe3e62e4) ) // mpr12379.b4  [3/4]      IDENTICAL
	ROM_LOAD16_BYTE( "ic65.15", 		0x120000, 0x10000, CRC(9fb1f9b6) SHA1(a193ff48f2bfbb4afbc322ae33cdf360afcb681e) ) // mpr12379.b4  [4/4]      IDENTICAL
	ROM_LOAD16_BYTE( "ic75.32", 		0x040001, 0x10000, CRC(101d2fff) SHA1(1de1390c5f55f192491053c8aac31be3389aab2b) ) // mpr12380.b2  [1/4]      IDENTICAL
	ROM_LOAD16_BYTE( "ic76.31", 		0x060001, 0x10000, CRC(677e64a6) SHA1(e3d0d31097017c6cb1a7f41292783f18ce13b41c) ) // mpr12380.b2  [2/4]      IDENTICAL
	ROM_LOAD16_BYTE( "ic81.26", 		0x140001, 0x10000, CRC(a34e84d1) SHA1(5aa8836ef1bdf97e399899c911967840e9873bfb) ) // mpr12380.b2  [3/4]      99.992371%
	ROM_LOAD16_BYTE( "ic82.25", 		0x160001, 0x10000, CRC(ad1c1c90) SHA1(155f17593cfab1a117bb755b1edd0c473d455f91) ) // mpr12380.b2  [4/4]      IDENTICAL
	ROM_LOAD16_BYTE( "ic60.20", 		0x040000, 0x10000, CRC(5853000d) SHA1(db7adf1de74c66f667ea7ccc41702576de081ff5) ) // mpr12381.b5  [1/4]      IDENTICAL
	ROM_LOAD16_BYTE( "ic61.19", 		0x060000, 0x10000, CRC(697b3276) SHA1(a8aeb2cfaca9368d5bfa14a67de36dbadd4a0585) ) // mpr12381.b5  [2/4]      IDENTICAL
	ROM_LOAD16_BYTE( "ic66.14", 		0x140000, 0x10000, CRC(753b01e8) SHA1(0180cf893d63e60e14e2fb0f5836b302f08f0228) ) // mpr12381.b5  [3/4]      IDENTICAL
	ROM_LOAD16_BYTE( "ic67.13", 		0x160000, 0x10000, CRC(5dd6d8db) SHA1(270886aebf4549c5cf456b1e90927b759a53e2e1) ) // mpr12381.b5  [4/4]      IDENTICAL
	ROM_LOAD16_BYTE( "ic77.30", 		0x080001, 0x10000, CRC(a9519afe) SHA1(0550f596ef080db25241d88242d57edb3c7fb685) ) // mpr12382.b3  [1/4]      IDENTICAL
	ROM_LOAD16_BYTE( "ic78.29", 		0x0a0001, 0x10000, CRC(74df9232) SHA1(61286c83eb6f31983c0ed24ca2151c58a95238f1) ) // mpr12382.b3  [2/4]      IDENTICAL
	ROM_LOAD16_BYTE( "ic83.24", 		0x180001, 0x10000, CRC(a8c8c784) SHA1(8e998019b4dbf509179d41eb2b446647db3d00a6) ) // mpr12382.b3  [3/4]      IDENTICAL
	ROM_LOAD16_BYTE( "ic84.23", 		0x1a0001, 0x10000, CRC(cc3a922c) SHA1(3c96459d95961d463909b6b0e8c77916c3a018e3) ) // mpr12382.b3  [4/4]      IDENTICAL
	ROM_LOAD16_BYTE( "ic62.18", 		0x080000, 0x10000, CRC(ac6ad195) SHA1(3c3fe38047698e28cae9193438fe0b85941476c5) ) // mpr12383.b6  [1/4]      IDENTICAL
	ROM_LOAD16_BYTE( "ic63.17", 		0x0a0000, 0x10000, CRC(03a905c4) SHA1(c526a744021a392c860ded37afd54a78e9f47778) ) // mpr12383.b6  [2/4]      IDENTICAL
	ROM_LOAD16_BYTE( "ic68.12", 		0x180000, 0x10000, CRC(cba013c7) SHA1(a0658aaf7893bc9fb8f0435cab9f77ceb1fb4e1d) ) // mpr12383.b6  [3/4]      IDENTICAL
	ROM_LOAD16_BYTE( "ic69.11", 		0x1a0000, 0x10000, CRC(bea4d237) SHA1(46e51e89b4ee1e2701da1004758d7da547a2e4c2) ) // mpr12383.b6  [4/4]      IDENTICAL

	ROM_REGION( 0x20000, "soundcpu", 0 ) /* sound CPU + samples */
	ROM_LOAD( "ic45.10", 0x00000, 0x10000, CRC(804dfba8) SHA1(650ca61f78eb4d0aa256b554fb1570330166cc24) )
	ROM_LOAD( "ic46.9",  0x10000, 0x10000, CRC(634dd54e) SHA1(ac4ab0ad9c1ac634ff4dfb83232b0fa5cfb26fdd) )
ROM_END

ROM_START( tturfbl )
	ROM_REGION( 0x40000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "tt042197.rom", 0x00000, 0x10000, CRC(deee5af1) SHA1(0caba775021dc7e28ac6b7af8eac4f49d3102c83) )
	ROM_LOAD16_BYTE( "tt06c794.rom", 0x00001, 0x10000, CRC(90e6a95a) SHA1(014a0ae5cebcba9cc99e6ccde4ad5d938fab915c) )
	ROM_LOAD16_BYTE( "tt030be3.rom", 0x20000, 0x10000, CRC(100264a2) SHA1(d1ea4bf93f5472901ce95200f546ce9b58936aea) )
	ROM_LOAD16_BYTE( "tt05ef8a.rom", 0x20001, 0x10000, CRC(f787a948) SHA1(512b8cb2f5e9795171951e02c07cae957db41334) )

	ROM_REGION( 0x30000, "gfx1", ROMREGION_INVERT ) /* tiles */
	ROM_LOAD( "tt1574b3.rom", 0x00000, 0x10000, CRC(e9e630da) SHA1(e8471dedbb25475e4814d78b56f579fe9110461e) )
	ROM_LOAD( "tt16cf44.rom", 0x10000, 0x10000, CRC(4c467735) SHA1(8338b6605cbe2e076da0b3e3a47630409a79f002) )
	ROM_LOAD( "tt17d59e.rom", 0x20000, 0x10000, CRC(60c0f2fe) SHA1(3fea4ed757d47628f59ff940e40cb86b3b5b443b) )

	ROM_REGION16_BE( 0x80000, "gfx2", 0 ) /* sprites */
	ROM_LOAD16_BYTE( "12279.1b", 0x00001, 0x10000, CRC(7a169fb1) SHA1(1ec6da0d2cfcf727e61f61c847fd8b975b64f944) )
	ROM_LOAD16_BYTE( "12283.5b", 0x00000, 0x10000, CRC(ae0fa085) SHA1(ae9af92d4dd0c8a0f064d24e647522b588fbd7f7) )
	ROM_LOAD16_BYTE( "12278.2b", 0x20001, 0x10000, CRC(961d06b7) SHA1(b1a9dea63785bfa2c0e7b931387b91dfcd27d79b) )
	ROM_LOAD16_BYTE( "12282.6b", 0x20000, 0x10000, CRC(e8671ee1) SHA1(a3732938c370f1936d867aae9c3d1e9bbfb57ede) )
	ROM_LOAD16_BYTE( "12277.3b", 0x40001, 0x10000, CRC(f16b6ba2) SHA1(00cc04c7b5aad82d51d2d252e1e57bcdc5e2c9e3) )
	ROM_LOAD16_BYTE( "12281.7b", 0x40000, 0x10000, CRC(1ef1077f) SHA1(8ce6fd7d32a20b93b3f91aaa43fe22720da7236f) )
	ROM_LOAD16_BYTE( "12276.4b", 0x60001, 0x10000, CRC(838bd71f) SHA1(82d9d127438f5e1906b1cf40bf3b4727f2ee5685) )
	ROM_LOAD16_BYTE( "12280.8b", 0x60000, 0x10000, CRC(639a57cb) SHA1(84fd8b96758d38f9e1ba1a3c2cf8099ec0452784) )

	ROM_REGION( 0x30000, "soundcpu", 0 ) //* sound CPU */
	ROM_LOAD( "tt014d68.rom", 0x10000, 0x10000, CRC(d4aab1d9) SHA1(94885896d59da1ecabe2377a194fcf61eaae3765) )
	ROM_LOAD( "tt0246ff.rom", 0x20000, 0x10000, CRC(bb4bba8f) SHA1(b182a7e1d0425e93c2c1b93472aafd30a6af6907) )
ROM_END

ROM_START( dduxbl )
	ROM_REGION( 0x0c0000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "dduxb03.bin", 0x000000, 0x20000, CRC(e7526012) SHA1(a1798008bfa1ce9b87dc330f3817b1978052fcfd) )
	ROM_LOAD16_BYTE( "dduxb05.bin", 0x000001, 0x20000, CRC(459d1237) SHA1(55e9c0dc341c919d58cc789203642c397d7ac65e) )
	/* empty 0x40000 - 0x80000 */
	ROM_LOAD16_BYTE( "dduxb02.bin", 0x080000, 0x20000, CRC(d8ed3132) SHA1(a9d5ad8f79fb635cc234a99fad398688a5f15926) )
	ROM_LOAD16_BYTE( "dduxb04.bin", 0x080001, 0x20000, CRC(30c6cb92) SHA1(2e17c74eeb37c9731fc2e365cc0114f7383c0106) )

	ROM_REGION( 0x30000, "gfx1", ROMREGION_INVERT ) /* tiles */
	ROM_LOAD( "dduxb14.bin", 0x00000, 0x10000, CRC(664bd135) SHA1(674b06e01c2c8f5b8057dd24d470330c3f140473) )
	ROM_LOAD( "dduxb15.bin", 0x10000, 0x10000, CRC(ce0d2b30) SHA1(e60521c46f1650c9bdc76f2ceb91a6d61aaa0a09) )
	ROM_LOAD( "dduxb16.bin", 0x20000, 0x10000, CRC(6de95434) SHA1(7bed2a0261cf6c2fbb3756633f05f0bb2173977c) )

	ROM_REGION16_BE( 0x100000, "gfx2", 0 ) //* sprites */
	ROM_LOAD16_BYTE( "dduxb06.bin", 0x00000, 0x010000, CRC(b0079e99) SHA1(9bb4d3fa804a3d05a6e06b45a1280d7064e96ac6) )
	ROM_LOAD16_BYTE( "dduxb10.bin", 0x00001, 0x010000, CRC(0be3aee5) SHA1(48fc779b7398abbb82cd0d0d28705ece75b3c4e3) )
	ROM_LOAD16_BYTE( "dduxb07.bin", 0x20000, 0x010000, CRC(0217369c) SHA1(b6ec2fa1279a27a602d79e1073c54193745ea816) )
	ROM_LOAD16_BYTE( "dduxb11.bin", 0x20001, 0x010000, CRC(cfb2af18) SHA1(1ad18f933a7b797f0364d1f4a6c8549351b4c9a6) )
	ROM_LOAD16_BYTE( "dduxb08.bin", 0x40000, 0x010000, CRC(8844f336) SHA1(18c1baaad3bcc658d4a6d03de8c97378b5284e88) )
	ROM_LOAD16_BYTE( "dduxb12.bin", 0x40001, 0x010000, CRC(28ce9b15) SHA1(1640df9c8f21893c0647ad2f4210c714a06e6f37) )
	ROM_LOAD16_BYTE( "dduxb09.bin", 0x60000, 0x010000, CRC(6b64f665) SHA1(df07fcf2bbec6fa78f89b95272762aebd6f3ec0e) )
	ROM_LOAD16_BYTE( "dduxb13.bin", 0x60001, 0x010000, CRC(efe57759) SHA1(69b8969b20ab9480df2735bd2bcd527069196bd7) )

	ROM_REGION( 0x10000, "soundcpu", 0 ) /* sound CPU */
	ROM_LOAD( "dduxb01.bin", 0x0000, 0x8000, CRC(0dbef0d7) SHA1(8b9afb2fcb946cec467b1e691c267194b503f841) )
ROM_END

ROM_START( eswatbl )
	ROM_REGION( 0x080000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "eswat_c.rom", 0x000000, 0x10000, CRC(1028cc81) SHA1(24b4cd182419a44f3d6afa1c4273353024eb278f) )
	ROM_LOAD16_BYTE( "eswat_f.rom", 0x000001, 0x10000, CRC(f7b2d388) SHA1(8131ba8f4fa01751b9993c3c6c218c9bd3adb328) )
	ROM_LOAD16_BYTE( "eswat_b.rom", 0x020000, 0x10000, CRC(87c6b1b5) SHA1(a9f29e29a9c0e3daf272dce263a5fd7866642c77) )
	ROM_LOAD16_BYTE( "eswat_e.rom", 0x020001, 0x10000, CRC(937ddf9a) SHA1(9fc73f93e9c4221a4dc778593edc02cb405b2f78) )
	ROM_LOAD16_BYTE( "eswat_a.rom", 0x040000, 0x08000, CRC(2af4fc62) SHA1(f7b1539a5ab9560bd49dfecf44699abccfb649be) )
	ROM_LOAD16_BYTE( "eswat_d.rom", 0x040001, 0x08000, CRC(b4751e19) SHA1(57c9687dc864c163d13dbb89057cd42684a199cd) )

	ROM_REGION( 0xc0000, "gfx1", 0 ) /* tiles */
	ROM_LOAD( "mpr12624.b11", 0x00000, 0x40000, CRC(375a5ec4) SHA1(42b9116bdc0e0a5b1dd667ac1856b4c2252829ba) ) // ic19
	ROM_LOAD( "mpr12625.b12", 0x40000, 0x40000, CRC(3b8c757e) SHA1(0b66e8446d059a12e47e2a6fe8f0a333245bb95c) ) // ic20
	ROM_LOAD( "mpr12626.b13", 0x80000, 0x40000, CRC(3efca25c) SHA1(0d866bf53a16b52719f73081e933f4db27d72ece) ) // ic21

	ROM_REGION16_BE( 0x1c0000, "gfx2", 0 ) /* sprites */
	ROM_LOAD16_BYTE( "ic9",  0x000001, 0x20000, CRC(0d1530bf) SHA1(bb8626cd98761c1c20cee117d00315c85621ba6a) )
	ROM_CONTINUE(            0x100001, 0x20000 )
	ROM_LOAD16_BYTE( "ic12", 0x000000, 0x20000, CRC(18ff0799) SHA1(5417223378aef16ee2b4f438d1f8f11a23fe7265) )
	ROM_CONTINUE(            0x100000, 0x20000 )
	ROM_LOAD16_BYTE( "ic10", 0x040001, 0x20000, CRC(32069246) SHA1(4913009bc72bf4f8b171b14fe06457f5784cab15) )
	ROM_CONTINUE(            0x140001, 0x20000 )
	ROM_LOAD16_BYTE( "ic13", 0x040000, 0x20000, CRC(a3dfe436) SHA1(640ccc552114d403f35d441574d2f3e4f1d4a8f9) )
	ROM_CONTINUE(            0x140000, 0x20000 )
	ROM_LOAD16_BYTE( "ic11", 0x080001, 0x20000, CRC(f6b096e0) SHA1(695ad1adbdc29f4d614645867e16de038cf92709) )
	ROM_CONTINUE(            0x180001, 0x20000 )
	ROM_LOAD16_BYTE( "ic14", 0x080000, 0x20000, CRC(6773fef6) SHA1(91e646ea447be02254d060daf255d26afe0cc79e) )
	ROM_CONTINUE(            0x180000, 0x20000 )

	ROM_REGION( 0x50000, "soundcpu", 0 ) /* sound CPU */
	ROM_LOAD( "epr12617.a13",  0x00000, 0x08000, CRC(7efecf23) SHA1(2b87af7cfaab5942a3f7b38c987fcba01d3475ab) ) // ic8
	ROM_LOAD( "mpr12616.a11", 0x10000, 0x40000, CRC(254347c2) SHA1(bf2d83a69a5be375c7e42e9f7d6e65c1095a354c) ) // ic6
ROM_END

ROM_START( fpointbl )
	ROM_REGION( 0x0c0000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "flpoint.003", 0x000000, 0x10000, CRC(4d6df514) SHA1(168aa1629ab7152ba1984605155406b236954a2c) )
	ROM_LOAD16_BYTE( "flpoint.002", 0x000001, 0x10000, CRC(4dff2ee8) SHA1(bd157d8c168d45e7490a05d5e1e901d9bdda9599) )

	ROM_REGION( 0x30000, "gfx1", ROMREGION_INVERT ) /* tiles */
	ROM_LOAD( "flpoint.006", 0x00000, 0x10000, CRC(c539727d) SHA1(56674effe1d273128dddd2ff9e02974ec10f3fff) )
	ROM_LOAD( "flpoint.005", 0x10000, 0x10000, CRC(82c0b8b0) SHA1(e1e2e721cb8ad53df33065582dc90edeba9c3cab) )
	ROM_LOAD( "flpoint.004", 0x20000, 0x10000, CRC(522426ae) SHA1(90fd0a19b30a8a61dc4cfa66a64115596333dcc6) )

	ROM_REGION16_BE( 0x20000, "gfx2", 0 ) /* sprites */
	ROM_LOAD16_BYTE( "12596.bin", 0x00001, 0x010000, CRC(4a4041f3) SHA1(4c52b30223d8aa80ccdbb196098cb17e64ad6583) )
	ROM_LOAD16_BYTE( "12597.bin", 0x00000, 0x010000, CRC(6961e676) SHA1(7639d2da086b57a9a8d6100fdacf40d97d7c4772) )

	ROM_REGION( 0x10000, "soundcpu", 0 ) /* sound CPU */
	ROM_LOAD( "flpoint.001", 0x0000, 0x8000, CRC(c5b8e0fe) SHA1(6cf8c67151d8604326fc6dbf976c0635b452a844) )	// bootleg rom doesn't work, but should be correct!
ROM_END

ROM_START( fpointbj )
	ROM_REGION( 0x0c0000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "boot2.003", 0x000000, 0x10000, CRC(6c00d1b0) SHA1(fd0c47b8ca010a64d3ef91980f93854ebc98fbda) )
	ROM_LOAD16_BYTE( "boot2.002", 0x000001, 0x10000, CRC(c1fcd704) SHA1(697bef464e53fb9891ed15ee2d6210107b693b20) )

	ROM_REGION( 0x30000, "gfx1", ROMREGION_INVERT ) /* tiles */
	ROM_LOAD( "flpoint.006", 0x00000, 0x10000, CRC(c539727d) SHA1(56674effe1d273128dddd2ff9e02974ec10f3fff) )
	ROM_LOAD( "flpoint.005", 0x10000, 0x10000, CRC(82c0b8b0) SHA1(e1e2e721cb8ad53df33065582dc90edeba9c3cab) )
	ROM_LOAD( "flpoint.004", 0x20000, 0x10000, CRC(522426ae) SHA1(90fd0a19b30a8a61dc4cfa66a64115596333dcc6) )

	ROM_REGION16_BE( 0x20000, "gfx2", 0 ) /* sprites */
	ROM_LOAD16_BYTE( "12596.bin", 0x00001, 0x010000, CRC(4a4041f3) SHA1(4c52b30223d8aa80ccdbb196098cb17e64ad6583) )
	ROM_LOAD16_BYTE( "12597.bin", 0x00000, 0x010000, CRC(6961e676) SHA1(7639d2da086b57a9a8d6100fdacf40d97d7c4772) )

	ROM_REGION( 0x10000, "soundcpu", 0 ) /* sound CPU */
	ROM_LOAD( "flpoint.001", 0x0000, 0x8000, CRC(c5b8e0fe) SHA1(6cf8c67151d8604326fc6dbf976c0635b452a844) )	// bootleg rom doesn't work, but should be correct!

	/* stuff below isn't used but loaded because it was on the board .. */
	ROM_REGION( 0x0120, "proms", 0 )
	ROM_LOAD( "82s129.1",  0x0000, 0x0100, CRC(a7c22d96) SHA1(160deae8053b09c09328325246598b3518c7e20b) )
	ROM_LOAD( "82s123.2",  0x0100, 0x0020, CRC(58bcf8bd) SHA1(e4d3d179b08c0f3424a6bec0f15058fb1b56f8d8) )

	ROM_REGION( 0x0600, "plds", 0 )
	ROM_LOAD( "fpointbj_gal16v8_1.bin", 0x0000, 0x0117, CRC(ba7f292c) SHA1(c383700d05663c9c5f29d5d04d16b05cd6adceb8) )
	ROM_LOAD( "fpointbj_gal16v8_3.bin", 0x0200, 0x0117, CRC(ce1ab1e1) SHA1(dcfc0015d8595ee6cb6bb02c95655161a7f3b017) )
	ROM_LOAD( "fpointbj_gal20v8.bin", 0x0400, 0x019d, NO_DUMP ) /* Protected */
ROM_END

ROM_START( tetrisbl )
	ROM_REGION( 0x040000, "maincpu", ROMREGION_ERASEFF ) /* 68000 code */
	ROM_LOAD16_BYTE( "rom2.bin", 0x000000, 0x10000, CRC(4d165c38) SHA1(04706b1977ae18bd09bafaf8ea65f8e5f32e04b8) )
	ROM_LOAD16_BYTE( "rom1.bin", 0x000001, 0x10000, CRC(1e912131) SHA1(8f53504ac08942ee340489d84eab825e654d0a2c) )

	ROM_REGION( 0x30000, "gfx1", 0 ) /* tiles */
	ROM_LOAD( "epr12165.b9",  0x00000, 0x10000, CRC(62640221) SHA1(c311d3847a981d0e1609f9b3d80481565d32d78c) )
	ROM_LOAD( "epr12166.b10", 0x10000, 0x10000, CRC(9abd183b) SHA1(621b017cb34973f9227be383e26b5cd41aea9422) )
	ROM_LOAD( "epr12167.b11", 0x20000, 0x10000, CRC(2495fd4e) SHA1(2db94ead9223a67238a97e724668076fc43e5534) )

	ROM_REGION16_BE( 0x020000, "gfx2", 0 ) /* sprites */
	ROM_LOAD16_BYTE( "obj0-o.rom", 0x00001, 0x10000, CRC(2fb38880) SHA1(0e1b601bbda78d1887951c1f7e752531c281bc83) )
	ROM_LOAD16_BYTE( "obj0-e.rom", 0x00000, 0x10000, CRC(d6a02cba) SHA1(d80000f92e754e89c6ca7b7273feab448fc9a061) )

	ROM_REGION( 0x40000, "soundcpu", 0 ) /* sound CPU */
	ROM_LOAD( "epr12168.a7", 0x0000, 0x8000, CRC(bd9ba01b) SHA1(fafa7dc36cc057a50ae4cdf7a35f3594292336f4) )
ROM_END



/******************************
    Tetris-based HW
******************************/

/*
Program Roms contain

Designed and Programmed by A.M.T. Research & Development Department 03/30/1991.
Copying or Revising for Commercial Use Is Not Permitted.
*/

ROM_START( beautyb )
	ROM_REGION( 0x010000, "maincpu", ROMREGION_ERASEFF ) /* 68000 code */
	ROM_LOAD16_BYTE( "b13.u3", 0x00000, 0x8000, CRC(90c4489b) SHA1(240275ad6dfd02feab636ceb620264d339e79b6a) )
	ROM_LOAD16_BYTE( "b23.u2", 0x00001, 0x8000, CRC(79b8f9ed) SHA1(5926852ea00b60d91684dbea4687b67894a397a1) )

	ROM_REGION( 0x30000, "gfx1", ROMREGION_INVERT ) /* tiles */
	ROM_LOAD( "b43.bin", 0x20000, 0x10000, CRC(67fe3f0c) SHA1(c9479512dd7a80895740c7fbd2133ab4d4c679d6) )
	ROM_LOAD( "b53.bin", 0x10000, 0x10000, CRC(aca8e330) SHA1(912e636e3c1e238682ea29620e8e2c6089c77209) )
	ROM_LOAD( "b63.bin", 0x00000, 0x10000, CRC(f2af2fd5) SHA1(0a95ebb5eae7cdc6535533d73d06419c23d01ac3) )

	ROM_REGION( 0x020000, "gfx2", ROMREGION_ERASEFF ) /* sprites */
	/* no sprites on this */

	ROM_REGION( 0x40000, "soundcpu", 0 ) /* sound CPU */
	ROM_LOAD( "1.bin", 0x0000, 0x8000, CRC(bd9ba01b) SHA1(fafa7dc36cc057a50ae4cdf7a35f3594292336f4) )

	ROM_REGION( 0x0020, "proms", 0 )
	ROM_LOAD( "82s123.2",  0x0000, 0x0020, CRC(58bcf8bd) SHA1(e4d3d179b08c0f3424a6bec0f15058fb1b56f8d8) )

	ROM_REGION( 0x0144, "pals", 0 )
	ROM_LOAD( "pal16r4acn.1",  0x0000, 0x0104, CRC(826be9e7) SHA1(893fc49c38aa8e7d6e98f6320157ba627a5d1748) )
	ROM_LOAD( "pal16r4acn.2",  0x0000, 0x0104, CRC(b3084ffe) SHA1(086ad2f89bdd8524ae358fd49b0803f9bb4aff33) )
	ROM_LOAD( "pal16r4acn.3",  0x0000, 0x0104, CRC(741cd872) SHA1(d53bdcadcd25d44b6423e0740e88209f85c709bd) )
	ROM_LOAD( "pal20l8acn",    0x0000, 0x0144, CRC(36e30d71) SHA1(e38f0257f9beedccc9421eec78701a86465d16ad) )
	ROM_LOAD( "pal16l8ajc.u4", 0x0000, 0x0104, NO_DUMP)
ROM_END

ROM_START( iqpipe )
	ROM_REGION( 0x010000, "maincpu", ROMREGION_ERASEFF ) /* 68000 code */
	ROM_LOAD16_BYTE( "iqpipe.u3", 0x00000, 0x8000, CRC(4ef1a0ba) SHA1(b11412b6b9e1a5d2f44ed5b7ceaa011418e5eab5) )
	ROM_LOAD16_BYTE( "iqpipe.u2", 0x00001, 0x8000, CRC(1dacee68) SHA1(7a37362a679a2c4cbeadca63c2ef9a112c946c97) )

	ROM_REGION( 0x30000, "gfx1", ROMREGION_INVERT ) /* tiles */
	ROM_LOAD( "iqpipe.4", 0x20000, 0x10000, CRC(938b9a04) SHA1(98c61b0526e76d5de134d9e22be0af0d576a6749) )
	ROM_LOAD( "iqpipe.5", 0x10000, 0x10000, CRC(dfaedd39) SHA1(498f1c34fecd8de497fdce41bb683d00047a868a) )
	ROM_LOAD( "iqpipe.6", 0x00000, 0x10000, CRC(8e554f8d) SHA1(4b3b0e47c36f37947422f1c31063f11975108cd0) )

	ROM_REGION( 0x020000, "gfx2", ROMREGION_ERASEFF ) /* sprites */
	/* no sprites on this */

	ROM_REGION( 0x40000, "soundcpu", 0 ) /* sound CPU */
	ROM_LOAD( "iqpipe.1", 0x0000, 0x8000, CRC(bd9ba01b) SHA1(fafa7dc36cc057a50ae4cdf7a35f3594292336f4) )

	ROM_REGION( 0x0020, "proms", 0 )
	ROM_LOAD( "82s123.2",  0x0000, 0x0020, NO_DUMP ) //same as beauty block?

	ROM_REGION( 0x0144, "pals", 0 )
	ROM_LOAD( "iqpipe.a",  0x0000, 0x0104, CRC(e9cd78fb) SHA1(557d3e7ef3b25c1338b24722cac91bca788c02b8) )
	ROM_LOAD( "iqpipe.b",  0x0000, 0x0104, CRC(e9cd78fb) SHA1(557d3e7ef3b25c1338b24722cac91bca788c02b8) )
	ROM_LOAD( "iqpipe.c",  0x0000, 0x0104, CRC(e9cd78fb) SHA1(557d3e7ef3b25c1338b24722cac91bca788c02b8) )
	ROM_LOAD( "iqpipe.d",  0x0000, 0x0144, CRC(36e30d71) SHA1(e38f0257f9beedccc9421eec78701a86465d16ad) )
	ROM_LOAD( "iqpipe.u4", 0x0000, 0x0104, CRC(e9cd78fb) SHA1(557d3e7ef3b25c1338b24722cac91bca788c02b8) )
ROM_END


/******************************
    System 18 Bootlegs
******************************/

ROM_START( astormbl )
	ROM_REGION( 0x080000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "astorm.a6", 0x000000, 0x40000, CRC(7682ed3e) SHA1(b857352ad9c66488e91f60989472638c483e4ae8) )
	ROM_LOAD16_BYTE( "astorm.a5", 0x000001, 0x40000, CRC(efe9711e) SHA1(496fd9e30941fde1658fab7292a669ef7964cecb) )

	ROM_REGION( 0xc0000, "gfx1", 0 ) /* tiles */
	ROM_LOAD( "epr13073.bin", 0x00000, 0x40000, CRC(df5d0a61) SHA1(79ad71de348f280bad847566c507b7a31f022292) )
	ROM_LOAD( "epr13074.bin", 0x40000, 0x40000, CRC(787afab8) SHA1(a119042bb2dad54e9733bfba4eaab0ac5fc0f9e7) )
	ROM_LOAD( "epr13075.bin", 0x80000, 0x40000, CRC(4e01b477) SHA1(4178ce4a87ea427c3b0195e64acef6cddfb3485f) )

	ROM_REGION16_BE( 0x200000, "gfx2", 0 ) /* sprites */
	ROM_LOAD16_BYTE( "mpr13082.bin", 0x000001, 0x40000, CRC(a782b704) SHA1(ba15bdfbc267b8d86f03e5310ce60846ff846de3) )
	ROM_LOAD16_BYTE( "astorm.a11",   0x000000, 0x40000, CRC(7829c4f3) SHA1(3adb7aa7f70163d3848c98316e18b9783c41d663) )
	ROM_LOAD16_BYTE( "mpr13081.bin", 0x080001, 0x40000, CRC(eb510228) SHA1(4cd387b160ec7050e1300ebe708853742169e643) )
	ROM_LOAD16_BYTE( "mpr13088.bin", 0x080000, 0x40000, CRC(3b6b4c55) SHA1(970495c54b3e1893ee8060f6ca1338c2cbbd1074) )
	ROM_LOAD16_BYTE( "mpr13080.bin", 0x100001, 0x40000, CRC(e668eefb) SHA1(d4a087a238b4d3ac2d23fe148d6a73018e348a89) )
	ROM_LOAD16_BYTE( "mpr13087.bin", 0x100000, 0x40000, CRC(2293427d) SHA1(4fd07763ff060afd594e3f64fa4750577f56c80e) )
	ROM_LOAD16_BYTE( "epr13079.bin", 0x180001, 0x40000, CRC(de9221ed) SHA1(5e2e434d1aa547be1e5652fc906d2e18c5122023) )
	ROM_LOAD16_BYTE( "epr13086.bin", 0x180000, 0x40000, CRC(8c9a71c4) SHA1(40b774765ac888792aad46b6351a24b7ef40d2dc) )

	ROM_REGION( 0x100000, "soundcpu", 0 ) /* sound CPU */
	ROM_LOAD( "epr13083.bin", 0x10000, 0x20000, CRC(5df3af20) SHA1(e49105fcfd5bf37d14bd760f6adca5ce2412883d) )
	ROM_LOAD( "epr13076.bin", 0x30000, 0x40000, CRC(94e6c76e) SHA1(f99e58a9bf372c41af211bd9b9ea3ac5b924c6ed) )
	ROM_LOAD( "epr13077.bin", 0x70000, 0x40000, CRC(e2ec0d8d) SHA1(225b0d223b7282cba7710300a877fb4a2c6dbabb) )
	ROM_LOAD( "epr13078.bin", 0xb0000, 0x40000, CRC(15684dc5) SHA1(595051006de24f791dae937584e502ff2fa31d9c) )
ROM_END

/*

CPUs:
on main board:

1x MC68000P10 (main)
1x Z8400BB1-Z80BCPU (sound)
1x OKI M6295 (sound)
1x oscillator 24.000MHz (close to main)
1x oscillator 8.000MHz (close to sound)

ROMs
on main board:

9x M27C512 (1,2,3,4,5,6,7,8,10)
1x TMS27C256 (9)
21x NM27C010Q (11-31)

----------------------

on roms board:
6x NM27C010Q (32-37)
2x N82S123N

*/

ROM_START( astormb2 )
	ROM_REGION( 0x080000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "1.a4", 0x000000, 0x10000, CRC(cca0d0af) SHA1(26fdbbeb8444d05f0ca2056a7c7fb81b0f1f2b5a) )
	ROM_LOAD16_BYTE( "2.a3", 0x020000, 0x10000, CRC(f95eb883) SHA1(b25d9c0fd46a534e7612f4a3ffa708b73654ae2b) )
	ROM_LOAD16_BYTE( "3.a2", 0x040000, 0x10000, CRC(4206ecd4) SHA1(45c65d7727cfaf215a7081159f6931185e92b39a) ) // epr13182.bin [3/4]      IDENTICAL
	ROM_LOAD16_BYTE( "4.a1", 0x060000, 0x10000, CRC(23247c95) SHA1(e4d78c453d2cb77946dd1b5266de823968eade77) ) // epr13182.bin [4/4]      98.648071%
	ROM_LOAD16_BYTE( "5.a9", 0x000001, 0x10000, CRC(6143039e) SHA1(8a5143c1e2c637149e988c423fa30b31e29a1193) )
	ROM_LOAD16_BYTE( "6.a8", 0x020001, 0x10000, CRC(0fd17bec) SHA1(e9a5dd93394fdf1561a925e4111dfce51b717b14) )
	ROM_LOAD16_BYTE( "7.a7", 0x040001, 0x10000, CRC(c901e228) SHA1(f459ba819a4e5f5174ff1b3957fb648c93beed53) ) // epr13181.bin [3/4]      IDENTICAL
	ROM_LOAD16_BYTE( "8.a6", 0x060001, 0x10000, CRC(bfb9d607) SHA1(8c3e10c1397fa0807d8df4715c9eb1945c774924) ) // epr13181.bin [4/4]      98.587036%

	ROM_REGION( 0xc0000, "gfx1", 0 ) /* tiles */
	ROM_LOAD( "32.01",  0x00000, 0x20000, CRC(d2aeb4ab) SHA1(9338ec5dc48f5d2b20511628a281236fe4646ef4) ) // epr13073.bin [1/2]      IDENTICAL
	ROM_LOAD( "33.011", 0x20000, 0x20000, CRC(2193f0ae) SHA1(84070f74693699c1ffc1a47517a97b5d058d08ec) ) // epr13073.bin [2/2]      IDENTICAL
	ROM_LOAD( "34.02",  0x40000, 0x20000, CRC(849aa725) SHA1(0f949dfe8a6c5796edc86a05339da80a158a95ae) ) // epr13074.bin [1/2]      IDENTICAL
	ROM_LOAD( "35.021", 0x60000, 0x20000, CRC(3f190347) SHA1(131953ccefb95eeef1ea90499ce521c3749f95c1) ) // epr13074.bin [2/2]      IDENTICAL
	ROM_LOAD( "36.03",  0x80000, 0x20000, CRC(c0f9628d) SHA1(aeacf5e409adfa0b9c28c90d4e89eb1f56cd5f4d) ) // epr13075.bin [1/2]      IDENTICAL
	ROM_LOAD( "37.031", 0xa0000, 0x20000, CRC(95af904e) SHA1(6574fa874c355c368109b417aab7d0b05c9d215d) ) // epr13075.bin [2/2]      IDENTICAL

	ROM_REGION16_BE( 0x200000, "gfx2", 0 ) /* sprites */
	ROM_LOAD16_BYTE( "17.042", 0x000001, 0x20000, CRC(db08beb5) SHA1(c154d22c69b77637d6a9d0f2bffcfb47e6901ec8) ) // mpr13082.bin [1/2]      IDENTICAL
	ROM_LOAD16_BYTE( "16.043", 0x040001, 0x20000, CRC(41f78977) SHA1(9cf9fcf96722d148c4b2cf7aa33425b6efcd0379) ) // mpr13082.bin [2/2]      IDENTICAL
	ROM_LOAD16_BYTE( "29.012", 0x000000, 0x20000, CRC(22acf675) SHA1(80fd0d96017bf36d964a79f7e13e73fee7ed370a) ) // mpr13089.bin [1/2]      99.941254%
	ROM_LOAD16_BYTE( "28.013", 0x040000, 0x20000, CRC(32b37a3a) SHA1(70f268aa99a17739fd9d832b5f1d9e37247747e6) ) // mpr13089.bin [2/2]      IDENTICAL
	ROM_LOAD16_BYTE( "19.040", 0x080001, 0x20000, CRC(10c359ac) SHA1(9087cb824242ce5fc8eba45b61cca8b329c576e5) ) // mpr13081.bin [1/2]      IDENTICAL
	ROM_LOAD16_BYTE( "18.041", 0x0c0001, 0x20000, CRC(47146c1d) SHA1(cd5d92136f86128a9f304c4f8850f1efd652dd5c) ) // mpr13081.bin [2/2]      IDENTICAL
	ROM_LOAD16_BYTE( "31.010", 0x080000, 0x20000, CRC(e88fc39c) SHA1(f19c55c49771625a76e65b639a3b23969db8031d) ) // mpr13088.bin [1/2]      IDENTICAL
	ROM_LOAD16_BYTE( "30.011", 0x0c0000, 0x20000, CRC(6fe7e2a2) SHA1(94e5852377f72fd00daae302db4a5f93301213e4) ) // mpr13088.bin [2/2]      IDENTICAL
	ROM_LOAD16_BYTE( "21.032", 0x100001, 0x20000, CRC(c9e5a258) SHA1(809a3a3f88efe9c7a9dd9f6439ccb48fffb84df0) ) // mpr13080.bin [1/2]      IDENTICAL
	ROM_LOAD16_BYTE( "20.033", 0x140001, 0x20000, CRC(ddf8d00e) SHA1(0a9031063921bb03e7fd57eea369a1ddcfa85431) ) // mpr13080.bin [2/2]      IDENTICAL
	ROM_LOAD16_BYTE( "25.022", 0x100000, 0x20000, CRC(af8f3700) SHA1(3787f732eee5c6c9b6550bd4ce5387aff2c4072e) ) // mpr13087.bin [1/2]      IDENTICAL
	ROM_LOAD16_BYTE( "24.023", 0x140000, 0x20000, CRC(a092ecb6) SHA1(d7cc85eaea70c7947c497bc1d9743ab12a6fb43e) ) // mpr13087.bin [2/2]      IDENTICAL
	ROM_LOAD16_BYTE( "23.030", 0x180001, 0x20000, CRC(adc1b625) SHA1(496a1e92a833dbde37a0426165ff4250848b6ef4) ) // epr13079.bin [1/2]      IDENTICAL
	ROM_LOAD16_BYTE( "22.031", 0x1c0001, 0x20000, CRC(27c27f38) SHA1(439502250da4e376d2aa4bd9122455c6991e334d) ) // epr13079.bin [2/2]      IDENTICAL
	ROM_LOAD16_BYTE( "27.020", 0x180000, 0x20000, CRC(6c5312aa) SHA1(94b74c78f318fcc1881a2926cebc98033a7e535d) ) // epr13086.bin [1/2]      IDENTICAL
	ROM_LOAD16_BYTE( "26.021", 0x1c0000, 0x20000, CRC(c67fc986) SHA1(5fac826f9dde45201e3b93582dbe29c584a10229) ) // epr13086.bin [2/2]      99.987030%

	/* Sound HW is very different to the originals */
	ROM_REGION( 0x210000, "soundcpu", ROMREGION_ERASEFF ) /* Z80 sound CPU */
	ROM_LOAD( "9.a5", 0x10000, 0x08000, CRC(0a4638e9) SHA1(0470e03a194464ff53c7583637193b585f5fd79f) )

	ROM_REGION( 0x40000, "oki1", ROMREGION_ERASEFF ) /* Oki6295 Samples - fixed? samples */
	ROM_LOAD( "11.a10", 0x00000, 0x20000, CRC(7e0f752c) SHA1(a4070c3fa4848b5be223f9b927de4b6926dbb4e6) ) // contains sample table
	ROM_LOAD( "10.a11", 0x20000, 0x10000, CRC(722e5969) SHA1(9cf891c6533b2e2a5c4741aa4e405038a7bf4e97) )
	/* 0x30000- 0x3ffff banked? (guess) */

	ROM_REGION( 0xc0000, "oki2", ROMREGION_ERASEFF ) /* Oki6295 Samples - banked? samples*/
	ROM_LOAD( "12.a15", 0x00000, 0x20000, CRC(cb4517db) SHA1(4c93376c2b3e70001bbc283d4485bb55514f6ef9) )
	ROM_LOAD( "13.a14", 0x20000, 0x20000, CRC(c60d6f18) SHA1(c9610729f19ae8414efd785948a1e6fb079bfe8d) )
	ROM_LOAD( "14.a13", 0x40000, 0x20000, CRC(07e6b3a5) SHA1(32da2a9aeb840b76e6f0117ac342ff5d612762b4) )
	ROM_LOAD( "15.a12", 0x60000, 0x20000, CRC(dffde929) SHA1(037b32470747d155385e532ee574b1234b3c2b26) )

	ROM_REGION( 0x0120, "proms", 0 )
	ROM_LOAD( "n82s129n.129",  0x0000, 0x0100, CRC(a7c22d96) SHA1(160deae8053b09c09328325246598b3518c7e20b) )
	ROM_LOAD( "n82s123n.123",  0x0100, 0x0020, CRC(58bcf8bd) SHA1(e4d3d179b08c0f3424a6bec0f15058fb1b56f8d8) )
ROM_END


ROM_START( mwalkbl )
	ROM_REGION( 0x080000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "mwalkbl.01", 0x000000, 0x10000, CRC(f49cdb16) SHA1(34b7e98d31c3b9db2f0f055d7b249b0e5e5cb746) )
	ROM_LOAD16_BYTE( "mwalkbl.05", 0x000001, 0x10000, CRC(c483f29f) SHA1(8fdfa764d8e49754844a9dc001400d439f9af9f0) )
	ROM_LOAD16_BYTE( "mwalkbl.02", 0x020000, 0x10000, CRC(0bde1896) SHA1(42731ae90d56918dc50c0dcb53d092dcfb957159) )
	ROM_LOAD16_BYTE( "mwalkbl.06", 0x020001, 0x10000, CRC(5b9fc688) SHA1(53d8143c3876548f63b392f0ea16c0e7c30a7917) )
	ROM_LOAD16_BYTE( "mwalkbl.03", 0x040000, 0x10000, CRC(0c5fe15c) SHA1(626e3f37f019448c3c96bf73b2d2b5fe4b3716c0) )
	ROM_LOAD16_BYTE( "mwalkbl.07", 0x040001, 0x10000, CRC(9e600704) SHA1(efd3d450b26f81dc2b74f44b4aaf906fa017e437) )
	ROM_LOAD16_BYTE( "mwalkbl.04", 0x060000, 0x10000, CRC(64692f79) SHA1(ad7f32997b78863e3aa3214018cdd24e3ec9c5cb) )
	ROM_LOAD16_BYTE( "mwalkbl.08", 0x060001, 0x10000, CRC(546ca530) SHA1(51f74878fdc221fee026e2e6a7ca96f290c8947f) )

	ROM_REGION( 0xc0000, "gfx1", 0 ) /* tiles */
	ROM_LOAD( "mpr13216.b1", 0x00000, 0x40000, CRC(862d2c03) SHA1(3c5446d702a639b62a602c6d687f9875d8450218) )
	ROM_LOAD( "mpr13217.b2", 0x40000, 0x40000, CRC(7d1ac3ec) SHA1(8495357304f1df135bba77ef3b96e79a883b8ff0) )
	ROM_LOAD( "mpr13218.b3", 0x80000, 0x40000, CRC(56d3393c) SHA1(50a2d065060692c9ecaa56046a781cb21d93e554) )

	ROM_REGION16_BE( 0x200000, "gfx2", 0 ) /* sprites */
	ROM_LOAD16_BYTE( "mpr13224.b11", 0x000001, 0x40000, CRC(c59f107b) SHA1(10fa60fca6e34eda277c483bb1c0e81bb88c8a47) )
	ROM_LOAD16_BYTE( "mpr13231.a11", 0x000000, 0x40000, CRC(a5e96346) SHA1(a854f4dd5dc16975373255110fdb8ab3d121b1af) )
	ROM_LOAD16_BYTE( "mpr13223.b10", 0x080001, 0x40000, CRC(364f60ff) SHA1(9ac887ec0b2e32b504b7c6a5f3bb1ce3fe41a15a) )
	ROM_LOAD16_BYTE( "mpr13230.a10", 0x080000, 0x40000, CRC(9550091f) SHA1(bb6e898f7b540e130fd338c10f74609a7604cef4) )
	ROM_LOAD16_BYTE( "mpr13222.b9",  0x100001, 0x40000, CRC(523df3ed) SHA1(2e496125e75decd674c3a08404fbdb53791a965d) )
	ROM_LOAD16_BYTE( "mpr13229.a9",  0x100000, 0x40000, CRC(f40dc45d) SHA1(e9468cef428f52ecdf6837c6d9a9fea934e7676c) )
	ROM_LOAD16_BYTE( "epr13221.b8",  0x180001, 0x40000, CRC(9ae7546a) SHA1(5413b0131881b0b32bac8de51da9a299835014bb) )
	ROM_LOAD16_BYTE( "epr13228.a8",  0x180000, 0x40000, CRC(de3786be) SHA1(2279bb390aa3efab9aeee0a643e5cb6a4f5933b6) )

	ROM_REGION( 0x100000, "soundcpu", 0 ) /* sound CPU */
	ROM_LOAD( "epr13225.a4", 0x10000, 0x20000, CRC(56c2e82b) SHA1(d5755a1bb6e889d274dc60e883d4d65f12fdc877) )
	ROM_LOAD( "mpr13219.b4", 0x30000, 0x40000, CRC(19e2061f) SHA1(2dcf1718a43dab4da53b4f67722664e70ddd2169) )
	ROM_LOAD( "mpr13220.b5", 0x70000, 0x40000, CRC(58d4d9ce) SHA1(725e73a656845b02702ef131b4c0aa2a73cdd02e) )
	ROM_LOAD( "mpr13249.b6", 0xb0000, 0x40000, CRC(623edc5d) SHA1(c32d9f818d40f311877fbe6532d9e95b6045c3c4) )
ROM_END


// Shadow Dancer
ROM_START( shdancbl )
	ROM_REGION( 0x080000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "ic39", 0x000000, 0x10000, CRC(adc1781c) SHA1(b2ca2831a48779df7533e6b2a406ee539e1f650c) )
	ROM_LOAD16_BYTE( "ic53", 0x000001, 0x10000, CRC(1c1ac463) SHA1(21075f7afae372daef197f04f5f12d14479a8140) )
	ROM_LOAD16_BYTE( "ic38", 0x020000, 0x10000, CRC(cd6e155b) SHA1(e37b53cc431533091d26b37be9b8e30494de5faf) )
	ROM_LOAD16_BYTE( "ic52", 0x020001, 0x10000, CRC(bb3c49a4) SHA1(ab01a6de1a6d338d30f9cfea7b3bf80dda67f215) )
	ROM_LOAD16_BYTE( "ic37", 0x040000, 0x10000, CRC(1bd8d5c3) SHA1(4d663362c059e112ac6c742d80200be98d50d175) )
	ROM_LOAD16_BYTE( "ic51", 0x040001, 0x10000, CRC(ce2e71b4) SHA1(3e251319cd4c8c63c66e6b92b2eef514d79dba8e) )
	ROM_LOAD16_BYTE( "ic36", 0x060000, 0x10000, CRC(bb861290) SHA1(62ea8eec74c6b1f5530ee86f97ad821daeac26ad) )
	ROM_LOAD16_BYTE( "ic50", 0x060001, 0x10000, CRC(7f7b82b1) SHA1(675020b57ce689b2767ff83773e2b828cda5aeed) )

	ROM_REGION( 0xc0000, "gfx1", ROMREGION_INVERT ) /* tiles */
	ROM_LOAD( "ic4",  0x00000, 0x20000, CRC(f0a016fe) SHA1(1426f3fbf50a04a8c5e998e071ca0e78d15f37a8) )
	ROM_LOAD( "ic18", 0x20000, 0x20000, CRC(f6bee053) SHA1(39ee5edfcc67bb4855217c7428254f3e8c862ba0) )
	ROM_LOAD( "ic3",  0x40000, 0x20000, CRC(e07e6b5d) SHA1(bdeb1193415049d0c9261ca261073bdd9e251b88) )
	ROM_LOAD( "ic17", 0x60000, 0x20000, CRC(f59deba1) SHA1(21188d22fe607281bb7da1e1f418a33d4a315695) )
	ROM_LOAD( "ic2",  0x80000, 0x20000, CRC(60095070) SHA1(913c2ee51fb6f838f3c6cbd27032bdf754fbadf1) )
	ROM_LOAD( "ic16", 0xa0000, 0x20000, CRC(0f0d5dd3) SHA1(76812e2f831256a8b6598257dd84a7f07443642e) )

	ROM_REGION16_BE( 0x200000, "gfx2", 0 ) /* sprites */

	// 12719
	ROM_LOAD16_BYTE( "ic73", 0x000001, 0x10000, CRC(59e77c96) SHA1(08da058529ac83352a4528d3792a21edda348f7a) )
	ROM_LOAD16_BYTE( "ic74", 0x020001, 0x10000, CRC(90ea5407) SHA1(4bdd93c86cb35822517433d491aa8be6857dd36c) )
	ROM_LOAD16_BYTE( "ic75", 0x040001, 0x10000, CRC(27d2fa61) SHA1(0ba3cd9448e54ce9fc9433f3edd28de9a4e451e9) )
	ROM_LOAD16_BYTE( "ic76", 0x060001, 0x10000, CRC(f36db688) SHA1(a527298ce9ca1d9f5aa7b9eac93985f34ca8119f) )

	// 12726
	ROM_LOAD16_BYTE( "ic58", 0x000000, 0x10000, CRC(9cd5c8c7) SHA1(54c2d0a683bda37eb9a75f90f4ca5e620c09c4cf) )
	ROM_LOAD16_BYTE( "ic59", 0x020000, 0x10000, CRC(ff40e872) SHA1(bd2c4aac427d106a46318f4cb2eb05c34d3c70b6) )
	ROM_LOAD16_BYTE( "ic60", 0x040000, 0x10000, CRC(826d7245) SHA1(bb3394de058bd63b9939cd05f22c925e0cdc840a) )
	ROM_LOAD16_BYTE( "ic61", 0x060000, 0x10000, CRC(dcf8068b) SHA1(9c78de224df76fc90fb90f1bbd9b22dad0874f69) )

	// 12718
	ROM_LOAD16_BYTE( "ic77", 0x080001, 0x10000, CRC(f93470b7) SHA1(1041afa43aa8d0589d6def9743721cdbda617f78) )
//  ROM_LOAD16_BYTE( "ic78", 0x0A0001, 0x10000, CRC(4d523ea3) SHA1(053c30778017127dddeae0783af463aef17bcc9a) ) // corrupt? (bad sprite when dog attacts in attract mode)
	ROM_LOAD16_BYTE( "sdbl.78", 0x0A0001, 0x10000, CRC(e533be5d) SHA1(926d6ba3f7a3ac289b0ae40d7633c70b2819df4d) )
	ROM_LOAD16_BYTE( "ic95", 0x0C0001, 0x10000, CRC(828b8294) SHA1(f2cdb882fb0709a909e6ef98f0315aceeb8bf283) )
//  ROM_LOAD16_BYTE( "ic94", 0x0E0001, 0x10000, CRC(542b2d1e) SHA1(1ce91aea6c49e6e365a91c30ca3049682c2162da) )
	ROM_LOAD16_BYTE( "sdbl.94", 0x0E0001, 0x10000, CRC(e2fa2b41) SHA1(7186107734dac5763dee43addcea7c14fb0d9d74) )

	// 12725
	ROM_LOAD16_BYTE( "ic62", 0x080000, 0x10000, CRC(50ca8065) SHA1(8c0d6ae34b9da6c376df387e8fc8b1068bcb4dcb) )
	ROM_LOAD16_BYTE( "ic63", 0x0A0000, 0x10000, CRC(d1866aa9) SHA1(524c82a12a1c484a246b8d49d9f05a774d008108) )
	ROM_LOAD16_BYTE( "ic90", 0x0C0000, 0x10000, CRC(3602b758) SHA1(d25b6c8420e07d0f2ac3e1d8717f14738466df16) )
	ROM_LOAD16_BYTE( "ic89", 0x0E0000, 0x10000, CRC(1ba4be93) SHA1(6f4fe2016e375be3df477436f5cde7508a24ecd1) )

	// 12717
	ROM_LOAD16_BYTE( "ic79", 0x100001, 0x10000, CRC(f22548ee) SHA1(723cb7604784c6715817daa8c86c18c6bcd1388d) )
	ROM_LOAD16_BYTE( "ic80", 0x120001, 0x10000, CRC(6209f7f9) SHA1(09b33c99d972a62af8ef56dacfa6262f002aba0c) )
	ROM_LOAD16_BYTE( "ic81", 0x140001, 0x10000, CRC(34692f23) SHA1(56126a81ac279662e3e3423da5205f65a62c4600) )
	ROM_LOAD16_BYTE( "ic82", 0x160001, 0x10000, CRC(7ae40237) SHA1(fae97cfcfd3cd557da3330158831e4727c438745) )

	// 12724
	ROM_LOAD16_BYTE( "ic64", 0x100000, 0x10000, CRC(7a8b7bcc) SHA1(00cbbbc4b3db48ca3ac65ff56b02c7d63a1b898a) )
	ROM_LOAD16_BYTE( "ic65", 0x120000, 0x10000, CRC(90ffca14) SHA1(00962e5309a79ce34c6f420036054bc607595dfe) )
	ROM_LOAD16_BYTE( "ic66", 0x140000, 0x10000, CRC(5d655517) SHA1(2a1c197dde62bd7946ca7b5f1c2833bdbc2e2e32) )
	ROM_LOAD16_BYTE( "ic67", 0x160000, 0x10000, CRC(0e5d0855) SHA1(3c15088f7fdda5c2bba9c89d244bbcff022f05fd) )

	// 12716
	ROM_LOAD16_BYTE( "ic83", 0x180001, 0x10000, CRC(a9040a32) SHA1(7b0b375285f528b2833c50892b55b0d4c550506d) )
	ROM_LOAD16_BYTE( "ic84", 0x1A0001, 0x10000, CRC(d6810031) SHA1(a82857a9ac442fbe076cdafcf7390765391ed136) )
	ROM_LOAD16_BYTE( "ic92", 0x1C0001, 0x10000, CRC(b57d5cb5) SHA1(636f1a07a84d37cecbe388a2f585893c4611436c) )
	ROM_LOAD16_BYTE( "ic91", 0x1E0001, 0x10000, CRC(49def6c8) SHA1(d8b2cc1993f0808553f87bf56fdbe47374576c5a) )

	// 12723
	ROM_LOAD16_BYTE( "ic68", 0x180000, 0x10000, CRC(8d684e53) SHA1(00e82ddaf875a7452ff978b7b7eb87a1a5a8fb64) )
	ROM_LOAD16_BYTE( "ic69", 0x1A0000, 0x10000, CRC(c47d32e2) SHA1(92b21f51abdd7950fb09d965b1d71b7bffac31ec) )
	ROM_LOAD16_BYTE( "ic88", 0x1C0000, 0x10000, CRC(9de140e1) SHA1(f1125e056a898a4fa519b49ae866c5c742e36bf7) )
	ROM_LOAD16_BYTE( "ic87", 0x1E0000, 0x10000, CRC(8172a991) SHA1(6d12b1533a19cb02613b473cc8ba73ece1f2a2fc) )

	ROM_REGION( 0x30000, "soundcpu", 0 ) /* sound CPU */
	ROM_LOAD( "ic45", 0x10000, 0x10000, CRC(576b3a81) SHA1(b65356a3837ed3875634ab0cbcd61acce44f2bb9) )
	ROM_LOAD( "ic46", 0x20000, 0x10000, CRC(c84e8c84) SHA1(f57895bedb6152c30733e91e6f4795702a62ac3a) )
ROM_END


/*************************************
 *
 *  Driver initialization
 *
 *************************************/

static DRIVER_INIT( common )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)machine->driver_data;

	running_device* device = devtag_get_device(machine, "segaspr1");
	sega16sp_state *sega16sp = (sega16sp_state *)device->token;

	sega16sp->xoffs = 107;


	state->bg1_trans = 0;
	state->splittab_bg_x = 0;
	state->splittab_bg_y = 0;
	state->splittab_fg_x = 0;
	state->splittab_fg_y = 0;

	state->spritebank_type = 0;
	state->back_yscroll = 0;
	state->fore_yscroll = 0;
	state->text_yscroll = 0;



	state->sample_buffer = 0;
	state->sample_select = 0;

	state->soundbank_ptr = NULL;

	state->beautyb_unkx = 0;

	state->maincpu = devtag_get_device(machine, "maincpu");
	state->soundcpu = devtag_get_device(machine, "soundcpu");
}

/* Sys16A */
static DRIVER_INIT( shinobl )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)machine->driver_data;
	running_device* device = devtag_get_device(machine, "segaspr1");
	sega16sp_state *sega16sp = (sega16sp_state *)device->token;

	DRIVER_INIT_CALL(common);

	state->spritebank_type = 1;
	sega16sp->xoffs = 117;
}

static DRIVER_INIT( passsht )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)machine->driver_data;
	running_device* device = devtag_get_device(machine, "segaspr1");
	sega16sp_state *sega16sp = (sega16sp_state *)device->token;

	DRIVER_INIT_CALL(common);

	state->spritebank_type = 1;
	state->back_yscroll = 3;
	sega16sp->xoffs = 117;
}

static DRIVER_INIT( wb3bbl )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)machine->driver_data;
	running_device* device = devtag_get_device(machine, "segaspr1");
	sega16sp_state *sega16sp = (sega16sp_state *)device->token;


	DRIVER_INIT_CALL(common);

	state->spritebank_type = 1;
	state->back_yscroll = 2;
	state->fore_yscroll = 2;
	sega16sp->xoffs = 117;
}


/* Sys16B */
static DRIVER_INIT( goldnaxeb1 )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)machine->driver_data;
	int i;
	UINT8 *ROM = memory_region(machine, "maincpu");
	UINT8 *KEY = memory_region(machine, "decryption");
	const address_space *space = cputag_get_address_space(machine, "maincpu", ADDRESS_SPACE_PROGRAM);
	UINT8 data[0x1000];
	running_device* device = devtag_get_device(machine, "segaspr1");
	sega16sp_state *sega16sp = (sega16sp_state *)device->token;

	// the decryption key is in a rom (part of an MSDOS executable...)
	for (i = 0; i < 0x800; i++)
	{
		KEY[i] = KEY[i] ^ 0xff;
		data[(i * 2) + 0] = ((KEY[i] & 0x80) >> 1) | ((KEY[i] & 0x40) >> 2) | ((KEY[i] & 0x20) >> 3) | ((KEY[i] & 0x10) >> 4);
		data[(i * 2) + 1] = ((KEY[i] & 0x08) << 3) | ((KEY[i] & 0x04) << 2) | ((KEY[i] & 0x02) << 1) | ((KEY[i] & 0x01) << 0);
	}

	state->decrypted_region = auto_alloc_array(machine, UINT8, 0xc0000);
	memcpy(state->decrypted_region, ROM, 0xc0000);

	for (i = 0; i < 0x40000; i++)
	{
		state->decrypted_region[i] = ROM[i] ^ data[(i & 0xfff) ^ 1];
	}

	memory_set_decrypted_region(space, 0x00000, 0xbffff, state->decrypted_region);

	DRIVER_INIT_CALL(common);

	state->spritebank_type = 1;
	sega16sp->xoffs = 121;
}


static DRIVER_INIT( bayrouteb1 )
{
	// it has the same encryption as the golden axe bootleg!
	//
	// but also some protection, probably code provided in RAM by an MCU
	//
	// for now we use the code which is present in the unprotected bootleg set
	// and modify the rom to use it
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)machine->driver_data;
	UINT16 *ROM2;
	UINT16 *decrypted_region2;

	// decrypt
	DRIVER_INIT_CALL( goldnaxeb1 );

	ROM2 = (UINT16*)memory_region(machine, "maincpu");
	decrypted_region2 = (UINT16*)state->decrypted_region;

	// patch interrupt vector
	ROM2[0x0070/2] = 0x000b;
	ROM2[0x0072/2] = 0xf000;

	// patch check for code in RAM
	decrypted_region2[0x107e/2] = 0x48e7;
	decrypted_region2[0x1080/2] = 0x000b;
	decrypted_region2[0x1082/2] = 0xf000;
}

static DRIVER_INIT( bayrouteb2 )
{
	UINT8 *mem = memory_region(machine, "soundcpu");

	memcpy(mem, mem + 0x10000, 0x8000);

	DRIVER_INIT_CALL(common);
}

static DRIVER_INIT( goldnaxeb2 )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)machine->driver_data;
	running_device* device = devtag_get_device(machine, "segaspr1");
	sega16sp_state *sega16sp = (sega16sp_state *)device->token;

	DRIVER_INIT_CALL(common);

	state->spritebank_type = 1;
	sega16sp->xoffs = 121;
}

static DRIVER_INIT( tturfbl )
{
	UINT8 *mem = memory_region(machine, "soundcpu");

	memcpy(mem, mem + 0x10000, 0x8000);

	DRIVER_INIT_CALL(common);
}

static DRIVER_INIT( dduxbl )
{
	running_device* device = devtag_get_device(machine, "segaspr1");
	sega16sp_state *sega16sp = (sega16sp_state *)device->token;

	DRIVER_INIT_CALL(common);

	sega16sp->xoffs = 112;
}

static DRIVER_INIT( eswatbl )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)machine->driver_data;
	running_device* device = devtag_get_device(machine, "segaspr1");
	sega16sp_state *sega16sp = (sega16sp_state *)device->token;

	DRIVER_INIT_CALL(common);
	//state->splittab_fg_x = &sys16_textram[0x0f80];

	state->spritebank_type = 1;
	sega16sp->xoffs = 124;
}

static DRIVER_INIT( fpointbl )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)machine->driver_data;

	DRIVER_INIT_CALL(common);
	//sys16_video_config(fpoint_update_proc, -0xb8, NULL);

	state->back_yscroll = 2;
	state->fore_yscroll = 2;
}

/* Tetris-based */
static DRIVER_INIT( beautyb )
{
	UINT16*rom = (UINT16*)memory_region( machine, "maincpu" );
	int x;
	running_device* device = devtag_get_device(machine, "segaspr1");
	sega16sp_state *sega16sp = (sega16sp_state *)device->token;

	for (x = 0; x < 0x8000; x++)
	{
		rom[x] = rom[x] ^ 0x2400;

		if (x & 8) rom[x] = BITSWAP16(rom[x],15,14,10,12,  11,13,9,8,
		                            7,6,5,4,   3,2,1,0 );
	}

	DRIVER_INIT_CALL(common);

	sega16sp->xoffs = 112;
}


/* Sys18 */
static DRIVER_INIT( shdancbl )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)machine->driver_data;
	UINT8 *mem = memory_region(machine, "soundcpu");;

	/* Copy first 32K of IC45 to Z80 address space */
	memcpy(mem, mem + 0x10000, 0x8000);

	DRIVER_INIT_CALL(common);

	state->spritebank_type = 1;
	state->splittab_fg_x = &state->textram[0x0f80/2];
	state->splittab_bg_x = &state->textram[0x0fc0/2];
}

static DRIVER_INIT( mwalkbl )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)machine->driver_data;
	UINT8 *RAM =  memory_region(machine, "soundcpu");
	static const int mwalk_sound_info[]  =
	{
		0x0f, 0x00000, // ROM #1 = 128K
		0x1f, 0x20000, // ROM #2 = 256K
		0x1f, 0x60000, // ROM #3 = 256K
		0x1f, 0xA0000  // ROM #4 = 256K
	};

	memcpy(state->sound_info, mwalk_sound_info, sizeof(state->sound_info));
	memcpy(RAM, &RAM[0x10000], 0xa000);

	DRIVER_INIT_CALL(common);

	state->spritebank_type = 1;
	state->splittab_fg_x = &state->textram[0x0f80/2];
	state->splittab_bg_x = &state->textram[0x0fc0/2];
}

static DRIVER_INIT( astormbl )
{
	segas1x_bootleg_state *state = (segas1x_bootleg_state *)machine->driver_data;
	UINT8 *RAM =  memory_region(machine, "soundcpu");
	static const int astormbl_sound_info[]  =
	{
		0x0f, 0x00000, // ROM #1 = 128K
		0x1f, 0x20000, // ROM #2 = 256K
		0x1f, 0x60000, // ROM #3 = 256K
		0x1f, 0xA0000  // ROM #4 = 256K
	};

	memcpy(state->sound_info, astormbl_sound_info, sizeof(state->sound_info));
	memcpy(RAM, &RAM[0x10000], 0xa000);

	DRIVER_INIT_CALL(common);

	state->spritebank_type = 1;
	state->splittab_fg_x = &state->textram[0x0f80/2];
	state->splittab_bg_x = &state->textram[0x0fc0/2];
}

/*************************************
 *
 *  Game driver(s)
 *
 *************************************/

/* System 16A based bootlegs (less complex tilemap system) */
GAME( 1987, shinobld,    shinobi,   shinobib,    shinobi,   shinobl,    ROT0,   "[Sega] (Datsu bootleg)", "Shinobi (Datsu bootleg)", GAME_NOT_WORKING | GAME_IMPERFECT_GRAPHICS | GAME_NO_SOUND )
GAME( 1988, passshtb,    passsht,   passshtb,    passsht,   passsht,    ROT270, "bootleg", "Passing Shot (2 Players) (bootleg)", GAME_NOT_WORKING | GAME_IMPERFECT_GRAPHICS | GAME_IMPERFECT_SOUND )
GAME( 1988, passht4b,    passsht,   passsht4b,   passht4b,  shinobl,    ROT270, "bootleg", "Passing Shot (4 Players) (bootleg)", GAME_NOT_WORKING | GAME_IMPERFECT_GRAPHICS | GAME_NO_SOUND )
GAME( 1988, wb3bbl,      wb3,       wb3bb,       wb3b,      wb3bbl,     ROT0,   "bootleg", "Wonder Boy III - Monster Lair (bootleg)", GAME_NOT_WORKING )

/* System 16B based bootlegs */
GAME( 1989, bayrouteb1,  bayroute,  bayrouteb1,  bayroute,  bayrouteb1, ROT0,   "bootleg", "Bay Route (encrypted, protected bootleg)", GAME_NO_SOUND | GAME_NOT_WORKING ) // broken sprites (due to missing/wrong irq code?)
GAME( 1989, bayrouteb2,  bayroute,  bayrouteb2,  bayroute,  bayrouteb2, ROT0,   "bootleg", "Bay Route (Datsu bootleg)", GAME_IMPERFECT_SOUND | GAME_NOT_WORKING )
GAME( 1989, goldnaxeb1,  goldnaxe,  goldnaxeb1,  goldnaxe,  goldnaxeb1, ROT0,   "bootleg", "Golden Axe (encrypted bootleg)", GAME_NOT_WORKING|GAME_NO_SOUND )
GAME( 1989, goldnaxeb2,  goldnaxe,  goldnaxeb2,  goldnaxe,  goldnaxeb2, ROT0,   "bootleg", "Golden Axe (bootleg)", GAME_NOT_WORKING|GAME_NO_SOUND )
GAME( 1989, tturfbl,     tturf,     tturfbl,     tturf,     tturfbl,    ROT0,   "bootleg", "Tough Turf (bootleg)", GAME_NOT_WORKING | GAME_IMPERFECT_GRAPHICS | GAME_IMPERFECT_SOUND )
GAME( 1989, dduxbl,      ddux,      dduxbl,      ddux,      dduxbl,     ROT0,   "bootleg", "Dynamite Dux (bootleg)", GAME_NOT_WORKING )
GAME( 1989, eswatbl,     eswat,     eswatbl,     eswat,     eswatbl,    ROT0,   "bootleg", "E-Swat - Cyber Police (bootleg)", GAME_NOT_WORKING | GAME_IMPERFECT_SOUND )
GAME( 1989, fpointbl,    fpoint,    fpointbl,    fpointbl,  fpointbl,   ROT0,   "bootleg", "Flash Point (World, bootleg)", GAME_NOT_WORKING )
GAME( 1989, fpointbj,    fpoint,    fpointbl,    fpointbl,  fpointbl,   ROT0,   "bootleg", "Flash Point (Japan, bootleg)", GAME_NOT_WORKING )
GAME( 1988, tetrisbl,    tetris,    tetrisbl,    tetris,    dduxbl,     ROT0,   "bootleg", "Tetris (bootleg)", 0 )

/* Tetris-based hardware */
GAME( 1991, beautyb,     0,         beautyb,     tetris,    beautyb,    ROT0,   "AMT", "Beauty Block", GAME_NO_SOUND | GAME_NOT_WORKING )
GAME( 1991, iqpipe,      0,         beautyb,     tetris,    beautyb,    ROT0,   "AMT", "IQ Pipe", GAME_NO_SOUND | GAME_NOT_WORKING )

/* System 18 bootlegs */
GAME( 1990, astormbl,    astorm,    astormbl,    astormbl,  astormbl,   ROT0,   "bootleg", "Alien Storm (bootleg, set 1)", GAME_IMPERFECT_GRAPHICS | GAME_IMPERFECT_SOUND )
GAME( 1990, astormb2,    astorm,    astormbl,    astormbl,  astormbl,   ROT0,   "bootleg", "Alien Storm (bootleg, set 2)", GAME_IMPERFECT_GRAPHICS | GAME_NO_SOUND )
GAME( 1990, mwalkbl,     mwalk,     mwalkbl,     mwalkbl,   mwalkbl,    ROT0,   "bootleg", "Michael Jackson's Moonwalker (bootleg)", GAME_IMPERFECT_GRAPHICS | GAME_IMPERFECT_SOUND )
GAME( 1989, shdancbl,    shdancer,  shdancbl,    mwalkbl,   shdancbl,   ROT0,   "bootleg", "Shadow Dancer (bootleg)", GAME_IMPERFECT_GRAPHICS | GAME_IMPERFECT_SOUND )
