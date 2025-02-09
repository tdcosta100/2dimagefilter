/***************************************************************************

    Sega Y-board hardware

Games supported:

    G-LOC Air Battle
    G-LOC R360
    Galaxy Force 2
    Power Drift
    Rail Chase
    Strike Fighter

Known games currently not dumped:

    Galaxy Force

****************************************************************************

    Known bugs:
        * still seems to be some glitchiness in the games in general

***************************************************************************/

#include "emu.h"
#include "cpu/z80/z80.h"
#include "includes/segas16.h"
#include "cpu/m68000/m68000.h"
#include "machine/segaic16.h"
#include "sound/2151intf.h"
#include "sound/segapcm.h"
#include "video/segaic16.h"

#include "pdrift.lh"


#define MASTER_CLOCK		50000000
#define SOUND_CLOCK			32215900

/* use this to fiddle with the IRQ2 timing */
#define TWEAK_IRQ2_SCANLINE		(0)



/*************************************
 *
 *  Statics
 *
 *************************************/

static UINT16 *backupram;

/* callbacks to handle output */
typedef void (*yboard_output_callback)(UINT16 data);
static yboard_output_callback ybd_output_cb1, ybd_output_cb2;
static UINT16 pdrift_bank;


/*************************************
 *
 *  Configuration
 *
 *************************************/

static void yboard_generic_init( running_machine *machine )
{
	segas1x_state *state = (segas1x_state *)machine->driver_data;

	/* reset globals */
	state->vblank_irq_state = 0;
	state->timer_irq_state = 0;

	ybd_output_cb1 = NULL;
	ybd_output_cb2 = NULL;
}



/*************************************
 *
 *  Initialization & interrupts
 *
 *************************************/

static void update_main_irqs(running_machine *machine)
{
	segas1x_state *state = (segas1x_state *)machine->driver_data;

	cpu_set_input_line(state->maincpu, 2, state->timer_irq_state ? ASSERT_LINE : CLEAR_LINE);
	cpu_set_input_line(state->subx, 2, state->timer_irq_state ? ASSERT_LINE : CLEAR_LINE);
	cpu_set_input_line(state->suby, 2, state->timer_irq_state ? ASSERT_LINE : CLEAR_LINE);
	cpu_set_input_line(state->maincpu, 4, state->vblank_irq_state ? ASSERT_LINE : CLEAR_LINE);
	cpu_set_input_line(state->subx, 4, state->vblank_irq_state ? ASSERT_LINE : CLEAR_LINE);
	cpu_set_input_line(state->suby, 4, state->vblank_irq_state ? ASSERT_LINE : CLEAR_LINE);
	cpu_set_input_line(state->maincpu, 6, state->timer_irq_state && state->vblank_irq_state ? ASSERT_LINE : CLEAR_LINE);
	cpu_set_input_line(state->subx, 6, state->timer_irq_state && state->vblank_irq_state ? ASSERT_LINE : CLEAR_LINE);
	cpu_set_input_line(state->suby, 6, state->timer_irq_state && state->vblank_irq_state ? ASSERT_LINE : CLEAR_LINE);

	if (state->timer_irq_state || state->vblank_irq_state)
		cpuexec_boost_interleave(machine, attotime_zero, ATTOTIME_IN_USEC(50));
}


/*
    IRQ2 timing: the timing of this interrupt is VERY sensitive! If it is at the
    wrong time, many games will screw up their rendering. Also, since there is
    no ack on the interrupt, it only fires for a single scanline, so if it fires
    too early, it's possible it could be missed.

    As far as I can tell, the interrupt is not programmable. It comes from the
    pair of rotation/video sync chips. There is no obvious location in the
    rotation RAM where it is specified, so I am assuming it is a hard-coded
    interrupt.

    Through trial and error, here is what I have found. Scanline 170 seems to
    be the earliest we can fire it and have it work reliably. Firing it later
    seems to introduce some glitches. Firing it too early means it will get
    missed and even worse things happen.

    Enable the TWEAK_IRQ2_SCANLINE define at the top of this file to fiddle with
    the timing.

    pdrift:
        100-110 = glitchy logo
        120     = glitchy logo + flickering 16B during logo
        150-170 = ok, very little logo glitching
        180-200 = ok, slightly glitchy logo
        210-220 = ok, but no palette fade

    gforce2:
        140     = no palette fade up
        150-200 = ok

    strkfgtr:
        150-200 = ok
*/

static TIMER_DEVICE_CALLBACK( scanline_callback )
{
	segas1x_state *state = (segas1x_state *)timer->machine->driver_data;
	int scanline = param;

	/* on scanline 'irq2_scanline' generate an IRQ2 */
	if (scanline == state->irq2_scanline)
	{
		state->timer_irq_state = 1;
		scanline = state->irq2_scanline + 1;
	}

	/* on scanline 'irq2_scanline' + 1, clear the IRQ2 */
	else if (scanline == state->irq2_scanline + 1)
	{
		state->timer_irq_state = 0;
		scanline = 223;
	}

	/* on scanline 223 generate VBLANK for all CPUs */
	else if (scanline == 223)
	{
		state->vblank_irq_state = 1;
		scanline = 224;
	}

	/* on scanline 224 we turn it off */
	else if (scanline == 224)
	{
		state->vblank_irq_state = 0;
		scanline = state->irq2_scanline;
	}

	/* update IRQs on the main CPU */
	update_main_irqs(timer->machine);

	/* come back at the next appropriate scanline */
	timer_device_adjust_oneshot(state->interrupt_timer, video_screen_get_time_until_pos(timer->machine->primary_screen, scanline, 0), scanline);

#if TWEAK_IRQ2_SCANLINE
	if (scanline == 223)
	{
		int old = state->irq2_scanline;

		/* Q = -10 scanlines, W = -1 scanline, E = +1 scanline, R = +10 scanlines */
		if (input_code_pressed(timer->machine, KEYCODE_Q)) { while (input_code_pressed(timer->machine, KEYCODE_Q)) ; state->irq2_scanline -= 10; }
		if (input_code_pressed(timer->machine, KEYCODE_W)) { while (input_code_pressed(timer->machine, KEYCODE_W)) ; state->irq2_scanline -= 1; }
		if (input_code_pressed(timer->machine, KEYCODE_E)) { while (input_code_pressed(timer->machine, KEYCODE_E)) ; state->irq2_scanline += 1; }
		if (input_code_pressed(timer->machine, KEYCODE_R)) { while (input_code_pressed(timer->machine, KEYCODE_R)) ; state->irq2_scanline += 10; }
		if (old != state->irq2_scanline)
			popmessage("scanline = %d", state->irq2_scanline);
	}
#endif
}


static MACHINE_START( yboard )
{
	segas1x_state *state = (segas1x_state *)machine->driver_data;

	state->maincpu = devtag_get_device(machine, "maincpu");
	state->soundcpu = devtag_get_device(machine, "soundcpu");
	state->subx = devtag_get_device(machine, "subx");
	state->suby = devtag_get_device(machine, "suby");

	state_save_register_global(machine, state->vblank_irq_state);
	state_save_register_global(machine, state->timer_irq_state);
	state_save_register_global(machine, state->irq2_scanline);
	state_save_register_global_array(machine, state->misc_io_data);
	state_save_register_global_array(machine, state->analog_data);
}


static MACHINE_RESET( yboard )
{
	segas1x_state *state = (segas1x_state *)machine->driver_data;

	state->irq2_scanline = 170;

	state->interrupt_timer = devtag_get_device(machine, "int_timer");
	timer_device_adjust_oneshot(state->interrupt_timer, video_screen_get_time_until_pos(machine->primary_screen, 223, 0), 223);
}



/*************************************
 *
 *  Sound comm and interrupts
 *
 *************************************/

static void sound_cpu_irq(running_device *device, int state)
{
	segas1x_state *driver = (segas1x_state *)device->machine->driver_data;

	cpu_set_input_line(driver->soundcpu, 0, state);
}


static TIMER_CALLBACK( delayed_sound_data_w )
{
	segas1x_state *state = (segas1x_state *)machine->driver_data;
	const address_space *space = cpu_get_address_space(state->maincpu, ADDRESS_SPACE_PROGRAM);

	soundlatch_w(space, 0, param);
	cpu_set_input_line(state->soundcpu, INPUT_LINE_NMI, ASSERT_LINE);
}


static WRITE16_HANDLER( sound_data_w )
{
	if (ACCESSING_BITS_0_7)
		timer_call_after_resynch(space->machine, NULL, data & 0xff, delayed_sound_data_w);
}


static READ8_HANDLER( sound_data_r )
{
	segas1x_state *state = (segas1x_state *)space->machine->driver_data;
	cpu_set_input_line(state->soundcpu, INPUT_LINE_NMI, CLEAR_LINE);
	return soundlatch_r(space, offset);
}



/*************************************
 *
 *  I/O space
 *
 *************************************/

static READ16_HANDLER( io_chip_r )
{
	segas1x_state *state = (segas1x_state *)space->machine->driver_data;
	static const char *const portnames[] = { "P1", "GENERAL", "PORTC", "PORTD", "PORTE", "DSW", "COINAGE", "PORTH" };
	offset &= 0x1f/2;

	switch (offset)
	{
		/* I/O ports */
		case 0x00/2:
		case 0x02/2:
		case 0x04/2:
		case 0x06/2:
		case 0x08/2:
		case 0x0a/2:
		case 0x0c/2:
		case 0x0e/2:
			/* if the port is configured as an output, return the last thing written */
			if (state->misc_io_data[0x1e/2] & (1 << offset))
				return state->misc_io_data[offset];

			/* otherwise, return an input port */
			return input_port_read(space->machine, portnames[offset]);

		/* 'SEGA' protection */
		case 0x10/2:
			return 'S';
		case 0x12/2:
			return 'E';
		case 0x14/2:
			return 'G';
		case 0x16/2:
			return 'A';

		/* CNT register & mirror */
		case 0x18/2:
		case 0x1c/2:
			return state->misc_io_data[0x1c/2];

		/* port direction register & mirror */
		case 0x1a/2:
		case 0x1e/2:
			return state->misc_io_data[0x1e/2];
	}
	return 0xffff;
}


static WRITE16_HANDLER( io_chip_w )
{
	segas1x_state *state = (segas1x_state *)space->machine->driver_data;
	UINT8 old;

	/* generic implementation */
	offset &= 0x1f/2;
	old = state->misc_io_data[offset];
	state->misc_io_data[offset] = data;

	switch (offset)
	{
		/* I/O ports */
		case 0x00/2:
			break;
		case 0x02/2:
			break;
		case 0x04/2:
			break;
		case 0x06/2:
			if (ybd_output_cb1)
				ybd_output_cb1(data);
			break;
		case 0x0a/2:
		case 0x0c/2:
			break;

		/* miscellaneous output */
		case 0x08/2:


			/*
                D7 = /KILL
                D6 = CONT
                D5 = /WDCL
                D4 = /SRES
                D3 = XRES
                D2 = YRES
                D1-D0 = ADC0-1
        */
			segaic16_set_display_enable(space->machine, data & 0x80);
			if (((old ^ data) & 0x20) && !(data & 0x20))
				watchdog_reset_w(space, 0, 0);
			cpu_set_input_line(state->soundcpu, INPUT_LINE_RESET, (data & 0x10) ? CLEAR_LINE : ASSERT_LINE);
			cpu_set_input_line(state->subx, INPUT_LINE_RESET, (data & 0x08) ? ASSERT_LINE : CLEAR_LINE);
			cpu_set_input_line(state->suby, INPUT_LINE_RESET, (data & 0x04) ? ASSERT_LINE : CLEAR_LINE);
			break;

		/* mute */

		case 0x0e/2:
			if (ybd_output_cb2)
				ybd_output_cb2(data);

			/* D7 = /MUTE */
			/* D6-D0 = FLT31-25 */
			sound_global_enable(space->machine, data & 0x80);
			break;

		/* CNT register */
		case 0x1c/2:
			break;
	}
}


/*************************************
 *
 *  Analog ports
 *
 *************************************/

static READ16_HANDLER( analog_r )
{
	segas1x_state *state = (segas1x_state *)space->machine->driver_data;
	int result = 0xff;
	if (ACCESSING_BITS_0_7)
	{
		result = state->analog_data[offset & 3] & 0x80;
		state->analog_data[offset & 3] <<= 1;
	}
	return result;
}


static WRITE16_HANDLER( analog_w )
{
	segas1x_state *state = (segas1x_state *)space->machine->driver_data;
	static const char *const ports[] = { "ADC0", "ADC1", "ADC2", "ADC3", "ADC4", "ADC5", "ADC6" };
	int selected = ((offset & 3) == 3) ? (3 + (state->misc_io_data[0x08/2] & 3)) : (offset & 3);
	int value = input_port_read_safe(space->machine, ports[selected], 0xff);

	state->analog_data[offset & 3] = value;
}



/*************************************
 *
 *  Capacitor-backed RAM
 *
 *************************************/

static NVRAM_HANDLER( yboard )
{
	if (read_or_write)
		mame_fwrite(file, backupram, 0x4000);
	else if (file)
		mame_fread(file, backupram, 0x4000);
}



/*************************************
 *
 *  Main CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( main_map, ADDRESS_SPACE_PROGRAM, 16 )
	ADDRESS_MAP_UNMAP_HIGH
	ADDRESS_MAP_GLOBAL_MASK(0x1fffff)
	AM_RANGE(0x000000, 0x07ffff) AM_ROM
	AM_RANGE(0x080000, 0x080007) AM_MIRROR(0x001ff8) AM_DEVREADWRITE("5248_main", segaic16_multiply_r, segaic16_multiply_w)
	AM_RANGE(0x082000, 0x083fff) AM_WRITE(sound_data_w)
	AM_RANGE(0x084000, 0x08401f) AM_MIRROR(0x001fe0) AM_DEVREADWRITE("5249_main", segaic16_divide_r, segaic16_divide_w)
//  AM_RANGE(0x086000, 0x087fff) /DEA0
	AM_RANGE(0x0c0000, 0x0cffff) AM_RAM AM_SHARE("share1")
	AM_RANGE(0x100000, 0x10001f) AM_READWRITE(io_chip_r, io_chip_w)
	AM_RANGE(0x100040, 0x100047) AM_READWRITE(analog_r, analog_w)
	AM_RANGE(0x1f0000, 0x1fffff) AM_RAM
ADDRESS_MAP_END



/*************************************
 *
 *  Sub CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( subx_map, ADDRESS_SPACE_PROGRAM, 16 )
	ADDRESS_MAP_UNMAP_HIGH
	ADDRESS_MAP_GLOBAL_MASK(0x1fffff)
	AM_RANGE(0x000000, 0x03ffff) AM_ROM
	AM_RANGE(0x080000, 0x080007) AM_MIRROR(0x001ff8) AM_DEVREADWRITE("5248_subx", segaic16_multiply_r, segaic16_multiply_w)
	AM_RANGE(0x084000, 0x08401f) AM_MIRROR(0x001fe0) AM_DEVREADWRITE("5249_subx", segaic16_divide_r, segaic16_divide_w)
	AM_RANGE(0x0c0000, 0x0cffff) AM_RAM AM_SHARE("share1")
	AM_RANGE(0x180000, 0x18ffff) AM_RAM AM_BASE(&segaic16_spriteram_1)
	AM_RANGE(0x1f8000, 0x1fbfff) AM_RAM
	AM_RANGE(0x1fc000, 0x1fffff) AM_RAM AM_BASE(&backupram)
ADDRESS_MAP_END


static ADDRESS_MAP_START( suby_map, ADDRESS_SPACE_PROGRAM, 16 )
	ADDRESS_MAP_UNMAP_HIGH
	ADDRESS_MAP_GLOBAL_MASK(0x1fffff)
	AM_RANGE(0x000000, 0x03ffff) AM_ROM
	AM_RANGE(0x080000, 0x080007) AM_MIRROR(0x001ff8) AM_DEVREADWRITE("5248_suby", segaic16_multiply_r, segaic16_multiply_w)
	AM_RANGE(0x084000, 0x08401f) AM_MIRROR(0x001fe0) AM_DEVREADWRITE("5249_suby", segaic16_divide_r, segaic16_divide_w)
	AM_RANGE(0x0c0000, 0x0cffff) AM_RAM AM_SHARE("share1")
	AM_RANGE(0x180000, 0x1807ff) AM_MIRROR(0x007800) AM_RAM AM_BASE(&segaic16_rotateram_0)
	AM_RANGE(0x188000, 0x188fff) AM_MIRROR(0x007000) AM_RAM AM_BASE(&segaic16_spriteram_0)
	AM_RANGE(0x190000, 0x193fff) AM_MIRROR(0x004000) AM_RAM_WRITE(segaic16_paletteram_w) AM_BASE(&segaic16_paletteram)
	AM_RANGE(0x198000, 0x19ffff) AM_READ(segaic16_rotate_control_0_r)
	AM_RANGE(0x1f0000, 0x1fffff) AM_RAM
ADDRESS_MAP_END



/*************************************
 *
 *  Sound CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( sound_map, ADDRESS_SPACE_PROGRAM, 8 )
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x0000, 0xefff) AM_ROM
	AM_RANGE(0xf000, 0xf0ff) AM_MIRROR(0x0700) AM_DEVREADWRITE("pcm", sega_pcm_r, sega_pcm_w)
	AM_RANGE(0xf800, 0xffff) AM_RAM
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_portmap, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_UNMAP_HIGH
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x00, 0x01) AM_MIRROR(0x3e) AM_DEVREADWRITE("ymsnd", ym2151_r, ym2151_w)
	AM_RANGE(0x40, 0x40) AM_MIRROR(0x3f) AM_READ(sound_data_r)
ADDRESS_MAP_END



/*************************************
 *
 *  Generic port definitions
 *
 *************************************/

static INPUT_PORTS_START( yboard_generic )
	PORT_START("P1")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("GENERAL")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )	/* afterburner (gloc) */
	PORT_SERVICE_NO_TOGGLE( 0x02, IP_ACTIVE_LOW )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )	/* cannon trigger */
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )	/* missile button or gearshift */
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN2 )

	PORT_START("PORTC")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("PORTD")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("PORTE")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("DSW")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) ) PORT_DIPLOCATION("SWB:1")
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) ) PORT_DIPLOCATION("SWB:2")
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) ) PORT_DIPLOCATION("SWB:3")
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) ) PORT_DIPLOCATION("SWB:4")
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) ) PORT_DIPLOCATION("SWB:5")
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) ) PORT_DIPLOCATION("SWB:6")
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) ) PORT_DIPLOCATION("SWB:7")
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) ) PORT_DIPLOCATION("SWB:8")
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("COINAGE")
	PORT_DIPNAME( 0x0f, 0x0f, DEF_STR( Coin_A ) ) PORT_DIPLOCATION("SWA:1,2,3,4")
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
	PORT_DIPNAME( 0xf0, 0xf0, DEF_STR( Coin_B ) ) PORT_DIPLOCATION("SWA:5,6,7,8")
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

	PORT_START("PORTH")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END



/*************************************
 *
 *  Game-specific port definitions
 *
 *************************************/

static INPUT_PORTS_START( gforce2 )
	PORT_INCLUDE( yboard_generic )

	PORT_MODIFY("GENERAL")
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("Shoot")
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("Missile")

	PORT_MODIFY("DSW")
	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SWB:1")
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x06, 0x06, "Energy Timer" ) PORT_DIPLOCATION("SWB:2,3")
	PORT_DIPSETTING(    0x04, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x06, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x08, 0x08, "Shield Strength" ) PORT_DIPLOCATION("SWB:4")
	PORT_DIPSETTING(    0x08, "Weak" )
	PORT_DIPSETTING(    0x00, "Strong" )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SWB:5,6")
	PORT_DIPSETTING(    0x20, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0xc0, 0x40, DEF_STR( Cabinet ) ) PORT_DIPLOCATION("SWB:7,8")
	PORT_DIPSETTING(    0xc0, "Super Deluxe" )
	PORT_DIPSETTING(    0x80, "Deluxe" )
	PORT_DIPSETTING(    0x40, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x00, "City" )

	PORT_START("ADC0")	/* stick X */
	PORT_BIT( 0xff, 0x80, IPT_AD_STICK_X ) PORT_MINMAX(0x01,0xff) PORT_SENSITIVITY(100) PORT_KEYDELTA(4)

	PORT_START("ADC1")	/* stick Y */
	PORT_BIT( 0xff, 0x80, IPT_AD_STICK_Y ) PORT_MINMAX(0x01,0xff) PORT_SENSITIVITY(100) PORT_KEYDELTA(4) PORT_REVERSE

	PORT_START("ADC2")	/* throttle */
	PORT_BIT( 0xff, 0x80, IPT_AD_STICK_Z ) PORT_MINMAX(0x01,0xff) PORT_SENSITIVITY(100) PORT_KEYDELTA(79)
INPUT_PORTS_END


static INPUT_PORTS_START( gloc )
	PORT_INCLUDE( yboard_generic )

	PORT_MODIFY("GENERAL")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("After Burner")
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("Vulcan")
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("Missile")

	PORT_MODIFY("DSW")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SWB:1,2")
	PORT_DIPSETTING(    0x02, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x03, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SWB:3")
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x18, 0x08, DEF_STR( Cabinet ) ) PORT_DIPLOCATION("SWB:4,5")
	PORT_DIPSETTING(    0x18, "Moving" )
	PORT_DIPSETTING(    0x10, "Cockpit" )
	PORT_DIPSETTING(    0x08, DEF_STR( Upright ) )
//  PORT_DIPSETTING(    0x00, DEF_STR( Unused ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Allow_Continue ) ) PORT_DIPLOCATION("SWB:6")
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Yes ) )
	PORT_DIPNAME( 0xc0, 0x40, "Credits" ) PORT_DIPLOCATION("SWB:7,8")
	PORT_DIPSETTING(    0x40, "1 to Start, 1 to Continue" )
	PORT_DIPSETTING(    0xc0, "2 to Start, 1 to Continue" )
	PORT_DIPSETTING(    0x80, "3 to Start, 2 to Continue" )
	PORT_DIPSETTING(    0x00, "4 to Start, 3 to Continue" )

	PORT_START("ADC3")	/* stick Y */
	PORT_BIT( 0xff, 0x80, IPT_AD_STICK_Y ) PORT_MINMAX(0x40,0xc0) PORT_SENSITIVITY(100) PORT_KEYDELTA(4) PORT_REVERSE

	PORT_START("ADC4")	/* throttle */
	PORT_BIT( 0xff, 0x80, IPT_AD_STICK_Z ) PORT_SENSITIVITY(100) PORT_KEYDELTA(79)

	PORT_START("ADC5")	/* stick X */
	PORT_BIT( 0xff, 0x80, IPT_AD_STICK_X ) PORT_MINMAX(0x20,0xe0) PORT_SENSITIVITY(100) PORT_KEYDELTA(4)
INPUT_PORTS_END


static INPUT_PORTS_START( glocr360 )
	PORT_INCLUDE( yboard_generic )

	PORT_MODIFY("GENERAL")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("After Burner")
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("Vulcan")
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("Missile")

	PORT_MODIFY("DSW")
	PORT_DIPNAME( 0x03, 0x03, "Game Type" ) PORT_DIPLOCATION("SWB:1,2")
	PORT_DIPSETTING(    0x02, "Fighting Only" )
	PORT_DIPSETTING(    0x03, "Fight/Experience" )
	PORT_DIPSETTING(    0x01, "Experience Only" )
//  PORT_DIPSETTING(    0x00, DEF_STR( Unused ) )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SWB:3")
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, "Ever Off" ) PORT_DIPLOCATION("SWB:4")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )
	PORT_DIPNAME( 0xf0, 0xf0, "Initial Credit" ) PORT_DIPLOCATION("SWB:5,6,7,8")
	PORT_DIPSETTING(    0xf0, "1" )
	PORT_DIPSETTING(    0xe0, "2" )
	PORT_DIPSETTING(    0xd0, "3" )
	PORT_DIPSETTING(    0xc0, "4" )
	PORT_DIPSETTING(    0xb0, "5" )
	PORT_DIPSETTING(    0xa0, "6" )
	PORT_DIPSETTING(    0x90, "8" )
	PORT_DIPSETTING(    0x80, "10" )
	PORT_DIPSETTING(    0x70, "12" )
//  PORT_DIPSETTING(    0x00, "1" )
//  PORT_DIPSETTING(    0x10, "1" )
//  PORT_DIPSETTING(    0x20, "1" )
//  PORT_DIPSETTING(    0x30, "1" )
//  PORT_DIPSETTING(    0x40, "1" )
//  PORT_DIPSETTING(    0x50, "1" )
//  PORT_DIPSETTING(    0x60, "1" )

	PORT_MODIFY("COINAGE")
	PORT_DIPNAME( 0x0f, 0x0f, DEF_STR( Coin_A ) ) PORT_DIPLOCATION("SWA:1,2,3,4")
//  PORT_DIPSETTING(    0x07, DEF_STR( 1C_1C ) )
//  PORT_DIPSETTING(    0x05, DEF_STR( 1C_1C ) )
//  PORT_DIPSETTING(    0x04, DEF_STR( 1C_1C ) )
//  PORT_DIPSETTING(    0x01, DEF_STR( 1C_1C ) )
//  PORT_DIPSETTING(    0x02, DEF_STR( 1C_1C ) )
//  PORT_DIPSETTING(    0x03, DEF_STR( 1C_1C ) )
//  PORT_DIPSETTING(    0x06, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x0f, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x0e, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x0d, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x0b, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x0a, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x09, DEF_STR( 1C_7C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 1C_8C ) )
	PORT_DIPSETTING(    0x00, "Free Play (if Coin B too) or 1/1" )
	PORT_DIPNAME( 0xf0, 0xf0, DEF_STR( Coin_B ) ) PORT_DIPLOCATION("SWA:5,6,7,8")
//  PORT_DIPSETTING(    0x70, DEF_STR( 1C_1C ) )
//  PORT_DIPSETTING(    0x50, DEF_STR( 1C_1C ) )
//  PORT_DIPSETTING(    0x40, DEF_STR( 1C_1C ) )
//  PORT_DIPSETTING(    0x10, DEF_STR( 1C_1C ) )
//  PORT_DIPSETTING(    0x20, DEF_STR( 1C_1C ) )
//  PORT_DIPSETTING(    0x30, DEF_STR( 1C_1C ) )
//  PORT_DIPSETTING(    0x60, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0xf0, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0xe0, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0xd0, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0xb0, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0xa0, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x90, DEF_STR( 1C_7C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_8C ) )
	PORT_DIPSETTING(    0x00, "Free Play (if Coin A too) or 1/1" )

	PORT_START("ADC0")	/* moving pitch */
	PORT_BIT( 0xff, 0x7f, IPT_AD_STICK_X ) PORT_SENSITIVITY(100) PORT_KEYDELTA(4) PORT_PLAYER(2)

	PORT_START("ADC2")	/* moving roll */
	PORT_BIT( 0xff, 0x7f, IPT_AD_STICK_X ) PORT_SENSITIVITY(100) PORT_KEYDELTA(4) PORT_PLAYER(3)

	PORT_START("ADC3")	/* stick Y */
	PORT_BIT( 0xff, 0x7f, IPT_AD_STICK_Y ) PORT_SENSITIVITY(100) PORT_KEYDELTA(4) PORT_REVERSE

	PORT_START("ADC5")	/* stick X */
	PORT_BIT( 0xff, 0x7f, IPT_AD_STICK_X ) PORT_SENSITIVITY(100) PORT_KEYDELTA(4)
INPUT_PORTS_END


static INPUT_PORTS_START( pdrift )
	PORT_INCLUDE( yboard_generic )

	PORT_MODIFY("GENERAL")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_NAME("Gear Shift") PORT_CODE(KEYCODE_SPACE) PORT_TOGGLE

	PORT_MODIFY("DSW")
	PORT_DIPNAME( 0x03, 0x02, DEF_STR( Cabinet ) ) PORT_DIPLOCATION("SWB:1,2")
	PORT_DIPSETTING(    0x03, "Moving" )
	PORT_DIPSETTING(    0x02, "Upright/Sit Down" )
	PORT_DIPSETTING(    0x01, "Mini Upright" )
//  PORT_DIPSETTING(    0x00, DEF_STR( Unused ) )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SWB:3")
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x18, 0x08, "Credits" ) PORT_DIPLOCATION("SWB:4,5")
	PORT_DIPSETTING(    0x08, "1 to Start, 1 to Continue" )
	PORT_DIPSETTING(    0x18, "2 to Start, 1 to Continue" )
	PORT_DIPSETTING(    0x00, "2 to Start, 2 to Continue" )
	PORT_DIPSETTING(    0x10, "3 to Start, 2 to Continue" )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Allow_Continue ) ) PORT_DIPLOCATION("SWB:6")
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Yes ) )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SWB:7,8")
	PORT_DIPSETTING(    0x80, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )

	PORT_START("ADC3")	/* brake */
	PORT_BIT( 0xff, 0x00, IPT_PEDAL2 ) PORT_SENSITIVITY(100) PORT_KEYDELTA(40)

	PORT_START("ADC4")	/* gas pedal */
	PORT_BIT( 0xff, 0x00, IPT_PEDAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(20)

	PORT_START("ADC5")	/* steering */
	PORT_BIT( 0xff, 0x80, IPT_PADDLE ) PORT_MINMAX(0x20,0xe0) PORT_SENSITIVITY(100) PORT_KEYDELTA(4)
INPUT_PORTS_END


static INPUT_PORTS_START( pdrifte )
	PORT_INCLUDE( pdrift )

	PORT_MODIFY("DSW")
	PORT_DIPNAME( 0x03, 0x02, DEF_STR( Cabinet ) ) PORT_DIPLOCATION("SWB:1,2")
	PORT_DIPSETTING(    0x03, "Moving" )
	PORT_DIPSETTING(    0x02, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x01, "Mini Upright" )
//  PORT_DIPSETTING(    0x00, DEF_STR( Unused ) )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SWB:3")
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
    PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unused ) ) PORT_DIPLOCATION("SWB:4")
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x00, "Initial Credit" ) PORT_DIPLOCATION("SWB:5")
	PORT_DIPSETTING(    0x00, "1" )
	PORT_DIPSETTING(    0x10, "2" )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Allow_Continue ) ) PORT_DIPLOCATION("SWB:6")
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Yes ) )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SWB:7,8")
	PORT_DIPSETTING(    0x80, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
INPUT_PORTS_END


static INPUT_PORTS_START( pdriftj )
	PORT_INCLUDE( pdrift )

	PORT_MODIFY("DSW")
	PORT_DIPNAME( 0x03, 0x02, DEF_STR( Cabinet ) ) PORT_DIPLOCATION("SWB:1,2")
	PORT_DIPSETTING(    0x03, "Moving" )
	PORT_DIPSETTING(    0x02, "Upright/Sit Down" )
	PORT_DIPSETTING(    0x01, "Mini Upright" )
//  PORT_DIPSETTING(    0x00, DEF_STR( Unused ) )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SWB:3")
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
    PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unused ) ) PORT_DIPLOCATION("SWB:4")
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x00, "Initial Credit" ) PORT_DIPLOCATION("SWB:5")
	PORT_DIPSETTING(    0x00, "1" )
	PORT_DIPSETTING(    0x10, "2" )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Allow_Continue ) ) PORT_DIPLOCATION("SWB:6")
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Yes ) )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SWB:7,8")
	PORT_DIPSETTING(    0x80, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
INPUT_PORTS_END


static INPUT_PORTS_START( rchase )
	PORT_INCLUDE( yboard_generic )

	PORT_MODIFY("GENERAL")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_SERVICE_NO_TOGGLE( 0x04, IP_ACTIVE_LOW )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START1 )

	PORT_MODIFY("COINAGE")
	PORT_DIPNAME( 0x03, 0x03, "Coin to Credit" ) PORT_DIPLOCATION("SWA:1,2")
	PORT_DIPSETTING(    0x00, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_1C ) )
	PORT_DIPNAME( 0x04, 0x04, "Coin #1 multiplier" ) PORT_DIPLOCATION("SWA:3")
	PORT_DIPSETTING(    0x04, "x1" )
	PORT_DIPSETTING(    0x00, "x2" )
	PORT_DIPNAME( 0x18, 0x18, "Coin #2 multiplier" ) PORT_DIPLOCATION("SWA:4,5")
	PORT_DIPSETTING(    0x18, "x1" )
	PORT_DIPSETTING(    0x10, "x4" )
	PORT_DIPSETTING(    0x08, "x5" )
	PORT_DIPSETTING(    0x00, "x6" )
	PORT_DIPNAME( 0xe0, 0xe0, "Bonus Adder" ) PORT_DIPLOCATION("SWA:6,7,8")
	PORT_DIPSETTING(    0xe0, DEF_STR( None ) )
	PORT_DIPSETTING(    0xc0, "2 gives +1" )
	PORT_DIPSETTING(    0xa0, "3 gives +1" )
	PORT_DIPSETTING(    0x80, "4 gives +1" )
	PORT_DIPSETTING(    0x60, "5 gives +1" )
	PORT_DIPSETTING(    0x40, "6 gives +1" )
	PORT_DIPSETTING(    0x20, "7 gives +1" )
	PORT_DIPSETTING(    0x00, "Error" )

	PORT_MODIFY("DSW")
	PORT_DIPNAME( 0x03, 0x03, "Credits" ) PORT_DIPLOCATION("SWB:1,2")
	PORT_DIPSETTING(    0x03, "1 to Start, 1 to Continue" )
	PORT_DIPSETTING(    0x02, "2 to Start, 1 to Continue" )
	PORT_DIPSETTING(    0x01, "3 to Start, 2 to Continue" )
	PORT_DIPSETTING(    0x00, "4 to Start, 3 to Continue" )
	PORT_DIPNAME( 0x04, 0x00, "Coin Chute" ) PORT_DIPLOCATION("SWB:3")
	PORT_DIPSETTING(    0x04, DEF_STR( Single ) )
	PORT_DIPSETTING(    0x00, "Twin" )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Cabinet ) ) PORT_DIPLOCATION("SWB:4")
	PORT_DIPSETTING(    0x08, "Moving" )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPNAME( 0x60, 0x60, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SWB:6,7")
	PORT_DIPSETTING(    0x40, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x60, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SWB:8")
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("ADC0")
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_X ) PORT_SENSITIVITY(50) PORT_KEYDELTA(5)

	PORT_START("ADC1")
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_Y ) PORT_SENSITIVITY(50) PORT_KEYDELTA(5)

	PORT_START("ADC2")
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_X ) PORT_SENSITIVITY(50) PORT_KEYDELTA(5) PORT_PLAYER(2)

	PORT_START("ADC3")
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_Y ) PORT_SENSITIVITY(50) PORT_KEYDELTA(5) PORT_PLAYER(2)
INPUT_PORTS_END


static INPUT_PORTS_START( strkfgtr )
	PORT_INCLUDE( gloc )

	PORT_MODIFY("DSW")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SWB:1,2")
	PORT_DIPSETTING(    0x02, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x03, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SWB:3")
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Allow_Continue ) ) PORT_DIPLOCATION("SWB:4")
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x30, 0x10, "Credits" ) PORT_DIPLOCATION("SWB:5,6")
	PORT_DIPSETTING(    0x10, "1 to Start, 1 to Continue" )
	PORT_DIPSETTING(    0x30, "2 to Start, 1 to Continue" )
	PORT_DIPSETTING(    0x20, "3 to Start, 2 to Continue" )
	PORT_DIPSETTING(    0x00, "4 to Start, 3 to Continue" )
	PORT_DIPNAME( 0xc0, 0x40, DEF_STR( Cabinet ) ) PORT_DIPLOCATION("SWB:7,8")
	PORT_DIPSETTING(    0xc0, "Moving" )
	PORT_DIPSETTING(    0x80, "Cockpit" )
	PORT_DIPSETTING(    0x40, DEF_STR( Upright ) )
//  PORT_DIPSETTING(    0x00, DEF_STR( Unused ) )
INPUT_PORTS_END



/*************************************
 *
 *  Sound definitions
 *
 *************************************/

static const ym2151_interface ym2151_config =
{
	sound_cpu_irq
};


static const sega_pcm_interface segapcm_interface =
{
	BANK_12M | BANK_MASKF8
};



/*************************************
 *
 *  Generic machine drivers
 *
 *************************************/

static MACHINE_DRIVER_START( yboard )

	/* driver data */
	MDRV_DRIVER_DATA(segas1x_state)

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", M68000, MASTER_CLOCK/4)
	MDRV_CPU_PROGRAM_MAP(main_map)

	MDRV_CPU_ADD("subx", M68000, MASTER_CLOCK/4)
	MDRV_CPU_PROGRAM_MAP(subx_map)

	MDRV_CPU_ADD("suby", M68000, MASTER_CLOCK/4)
	MDRV_CPU_PROGRAM_MAP(suby_map)

	MDRV_CPU_ADD("soundcpu", Z80, SOUND_CLOCK/8)
	MDRV_CPU_PROGRAM_MAP(sound_map)
	MDRV_CPU_IO_MAP(sound_portmap)

	MDRV_MACHINE_START(yboard)
	MDRV_MACHINE_RESET(yboard)
	MDRV_NVRAM_HANDLER(yboard)
	MDRV_QUANTUM_TIME(HZ(6000))

	MDRV_TIMER_ADD("int_timer", scanline_callback)

	MDRV_315_5248_ADD("5248_main")
	MDRV_315_5248_ADD("5248_subx")
	MDRV_315_5248_ADD("5248_suby")
	MDRV_315_5249_ADD("5249_main")
	MDRV_315_5249_ADD("5249_subx")
	MDRV_315_5249_ADD("5249_suby")

	/* video hardware */
	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(342,262)	/* to be verified */
	MDRV_SCREEN_VISIBLE_AREA(0*8, 40*8-1, 0*8, 28*8-1)

	MDRV_SEGA16SP_ADD_YBOARD_16B("segaspr1")
	MDRV_SEGA16SP_ADD_YBOARD("segaspr2")

	MDRV_PALETTE_LENGTH(8192*3)

	MDRV_VIDEO_START(yboard)
	MDRV_VIDEO_UPDATE(yboard)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")

	MDRV_SOUND_ADD("ymsnd", YM2151, SOUND_CLOCK/8)
	MDRV_SOUND_CONFIG(ym2151_config)
	MDRV_SOUND_ROUTE(0, "lspeaker", 0.43)
	MDRV_SOUND_ROUTE(1, "rspeaker", 0.43)

	MDRV_SOUND_ADD("pcm", SEGAPCM, SOUND_CLOCK/8)
	MDRV_SOUND_CONFIG(segapcm_interface)
	MDRV_SOUND_ROUTE(0, "lspeaker", 1.0)
	MDRV_SOUND_ROUTE(1, "rspeaker", 1.0)
MACHINE_DRIVER_END



/*************************************
 *
 *  ROM definition(s)
 *
 *************************************/

/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Galaxy Force, Sega Y-board
    Sega Game ID: 831-6614 (two PCB board stack)

    NOTE: An original PCB is very hard to locate intact.  Most of these boards were upgraded to Galaxy Force 2 through a
          chip swap upgrade.

*/

/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Galaxy Force 2, Sega Y-board
    CPU: 68000 (317-????)
*/
ROM_START( gforce2 )
	ROM_REGION( 0x080000, "maincpu", 0 ) // M
	ROM_LOAD16_BYTE( "epr-11688.bin",  0x000000, 0x20000, CRC(c845f2df) SHA1(17586a5f83170e99f28cd35b28e85e503e7bbe75) )
	ROM_LOAD16_BYTE( "epr-11687.bin",  0x000001, 0x20000, CRC(1cbefbbf) SHA1(28d473707cf042baca9d3a75ef22a5beb8c993f7) )

	ROM_REGION( 0x040000, "subx", 0 ) // X
	ROM_LOAD16_BYTE( "epr-11875.bin",  0x000000, 0x20000, CRC(c81701c6) SHA1(00c269f18c5eded7a0e4b6354779cd80db827409) )
	ROM_LOAD16_BYTE( "epr-11874.bin",  0x000001, 0x20000, CRC(5301fd79) SHA1(60a751c168d519cd45a9575e138514d580bce9b6) )

	ROM_REGION( 0x040000, "suby", 0 ) // Y
	ROM_LOAD16_BYTE( "epr-11816.bin",  0x000000, 0x20000, CRC(317dd0c2) SHA1(7f1c7dcfb111385e2a94912975c2f9bfe78445ac) )
	ROM_LOAD16_BYTE( "epr-11815.bin",  0x000001, 0x20000, CRC(f1fb22f1) SHA1(da3ce521b0a19b391913c35af34084d29edceca7) )

	ROM_REGION16_BE( 0x080000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "epr-11467.bin",  0x000000, 0x20000, CRC(6e60e736) SHA1(73086744cb2fe1ce162f851cb879755c21819b33) )
	ROM_LOAD16_BYTE( "epr-11468.bin",  0x000001, 0x20000, CRC(74ca9ca5) SHA1(c6f27ce43ef270088e6155c8240fd15afa5729fd) )
	ROM_LOAD16_BYTE( "epr-11694.bin",  0x040000, 0x20000, CRC(7e297b84) SHA1(bbf1aa2b0b6b1f9fdaf9bea77d24b7f4f9320696) )
	ROM_LOAD16_BYTE( "epr-11695.bin",  0x040001, 0x20000, CRC(38a864be) SHA1(ef7d89511713d695f6a454c42f079d3507d9690d) )

	ROM_REGION64_BE( 0x400000, "gfx1", 0)
	ROMX_LOAD( "epr-11469.bin",  0x000000, 0x20000, CRC(ed7a2299) SHA1(1aecf9ccba1fed0b7908008e798c522251a08b0f), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11470.bin",  0x000001, 0x20000, CRC(34dea550) SHA1(da95b8346c3530573461553629af4cc493bbb4af), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11477.bin",  0x000002, 0x20000, CRC(a2784653) SHA1(00a123d1fc8116ca678d6d8dbf1a5450feee014d), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11478.bin",  0x000003, 0x20000, CRC(8b778993) SHA1(015ae757d26cd6e69bdf79e237f62743a8f41e0c), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11471.bin",  0x000004, 0x20000, CRC(f1974069) SHA1(c8beb1a2ce430e3e6478c87c7053a58f8f31a140), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11472.bin",  0x000005, 0x20000, CRC(0d24409a) SHA1(ee00e23b0c548918b89dd48ed3f8b0370388659e), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11479.bin",  0x000006, 0x20000, CRC(ecd6138a) SHA1(bbc157a1f9b7e24d16e6f126d95c448736e83791), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11480.bin",  0x000007, 0x20000, CRC(64ad66c5) SHA1(aaab6999aa88b3340b16ee1188a8432477e16625), ROM_SKIP(7) )

	ROMX_LOAD( "epr-11473.bin",  0x100000, 0x20000, CRC(0538c6ec) SHA1(9397db188f12cf8cd91841794134760f30f83893), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11474.bin",  0x100001, 0x20000, CRC(eb923c50) SHA1(f997d2bc7f674eae4243eaf640d8faffa41d9521), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11481.bin",  0x100002, 0x20000, CRC(78e652b6) SHA1(8c70609172908131fef8cbe05f810dbd3d120eba), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11482.bin",  0x100003, 0x20000, CRC(2f879766) SHA1(ca5835d0bb77fe7de83ad336a82111cb2f96cd41), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11475.bin",  0x100004, 0x20000, CRC(69cfec89) SHA1(db2a9b03fff727f198fb5cbfb9c281ac3bbc5623), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11476.bin",  0x100005, 0x20000, CRC(a60b9b79) SHA1(6e9aa51923c12f5658bd17deb6c032e08c91dade), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11483.bin",  0x100006, 0x20000, CRC(d5d3a505) SHA1(fa7662346b954d3faf0e8fcf138004231676845c), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11484.bin",  0x100007, 0x20000, CRC(b8a56a50) SHA1(ccbc391d6f60b88630d7a93b8bb6e365a8d59ed8), ROM_SKIP(7) )

	ROMX_LOAD( "epr-11696.bin",  0x200000, 0x20000, CRC(99e8e49e) SHA1(ab6e1d74af412ec2f939043d7dc26f4b2e34a528), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11697.bin",  0x200001, 0x20000, CRC(7545c52e) SHA1(bdb0ccf233e10e9449aa367db5a5b5f209bee969), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11700.bin",  0x200002, 0x20000, CRC(e13839c1) SHA1(4975b5314797d11c782b0478eaa84eaf9980b1cd), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11701.bin",  0x200003, 0x20000, CRC(9fb3d365) SHA1(bfcf4abfa91aa41b16f01b7bafe97cd865167fbc), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11698.bin",  0x200004, 0x20000, CRC(cfeba3e2) SHA1(826b73858ca5ea05688246d36b1f607356974ca9), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11699.bin",  0x200005, 0x20000, CRC(4a00534a) SHA1(9a637b45b140420937b232bf690ef8bc3d43f5ad), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11702.bin",  0x200006, 0x20000, CRC(2a09c627) SHA1(c6b0a618b4ddd9d227d3472544b62ebef966b041), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11703.bin",  0x200007, 0x20000, CRC(43bb7d9f) SHA1(e36d208937f56f0af14292184c80ca939c32c378), ROM_SKIP(7) )

	ROMX_LOAD( "epr-11524.bin",  0x300000, 0x20000, CRC(5d35849f) SHA1(b86fb230ed0901dfdb525e7a47d9c9ad8031a3f6), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11525.bin",  0x300001, 0x20000, CRC(9ae47552) SHA1(43268dd5f79282aabd8972994437de1b2d2acc40), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11532.bin",  0x300002, 0x20000, CRC(b3565ddb) SHA1(3208744d43a11f4de8a6b626cc9113ad63d36807), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11533.bin",  0x300003, 0x20000, CRC(f5d16e8a) SHA1(3b5c1582794a0e69707264879b544ac48afd9337), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11526.bin",  0x300004, 0x20000, CRC(094cb3f0) SHA1(4b30e24dfd33c9922ffd5665cd9631b1f24243da), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11527.bin",  0x300005, 0x20000, CRC(e821a144) SHA1(eeec0e51798645cce8f9a7e686178268398977fb), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11534.bin",  0x300006, 0x20000, CRC(b7f0ad7c) SHA1(987b198692db365f3a81e88e5eec526775c318c6), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11535.bin",  0x300007, 0x20000, CRC(95da7a46) SHA1(ffe6cd80eecc7c53a1155396ede0720d975d2c3b), ROM_SKIP(7) )

	ROM_REGION( 0x10000, "soundcpu", 0 )		/* Z80 sound CPU */
	ROM_LOAD( "epr-11693.bin",	0x000000, 0x10000, CRC(0497785c) SHA1(f077e71efdc96d6eb3f1a1f989650466554fb13a) ) // 11516A

	ROM_REGION( 0x200000, "pcm", ROMREGION_ERASEFF )	/* SegaPCM samples */
	ROM_LOAD( "epr-11465.bin", 0x000000, 0x80000, CRC(e1436dab) SHA1(363f4c111de38cb1d82b245e4fcc65308f506e6a) )
	ROM_LOAD( "epr-11516.bin", 0x080000, 0x20000, CRC(19d0e17f) SHA1(7171131226cf1fe260a2db310fad2ec264adca26) )
	ROM_RELOAD(              0x0a0000, 0x20000 )
	ROM_RELOAD(              0x0c0000, 0x20000 )
	ROM_RELOAD(              0x0e0000, 0x20000 )
	ROM_LOAD( "epr-11814.bin", 0x100000, 0x20000, CRC(0b05d376) SHA1(8d0c0be2fd9dbc714c82c7cd1c439d5ff65e1317) ) // 11517
	ROM_RELOAD(              0x120000, 0x20000 )
	ROM_RELOAD(              0x140000, 0x20000 )
	ROM_RELOAD(              0x160000, 0x20000 )
ROM_END

/**************************************************************************************************************************
    Galaxy Force 2, Sega Y-board
    CPU: 68000 (317-????)
*/
ROM_START( gforce2j )
	ROM_REGION( 0x080000, "maincpu", 0 ) // M
	ROM_LOAD16_BYTE( "epr-11511.bin",  0x000000, 0x20000, CRC(d80a86d6) SHA1(e1beecb2af2d9960514639f30dbf923c9d5d5a89) )
	ROM_LOAD16_BYTE( "epr-11510.bin",  0x000001, 0x20000, CRC(d2b1bef4) SHA1(191c0387f4e47c2d675a6d6984907ca3a9d4156a) )

	ROM_REGION( 0x040000, "subx", 0 ) // X
	ROM_LOAD16_BYTE( "epr-11515.bin",  0x000000, 0x20000, CRC(d85875cf) SHA1(11fe1e008b2ced564aa5f82130cc7872fefeaf8d) )
	ROM_LOAD16_BYTE( "epr-11514.bin",  0x000001, 0x20000, CRC(3dcc6919) SHA1(0f701854734880aa98e890d8e2d13c62216dfb53) )

	ROM_REGION( 0x040000, "suby", 0 ) // Y
	ROM_LOAD16_BYTE( "epr-11513.bin",  0x000000, 0x20000, CRC(e18bc177) SHA1(3fb179c9074954fc9b64da1463f542d60a99ec84) )
	ROM_LOAD16_BYTE( "epr-11512.bin",  0x000001, 0x20000, CRC(6010e63e) SHA1(00aa5e8516f094409a407744b84ef183393b8b19) )

	ROM_REGION16_BE( 0x080000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "epr-11467.bin",  0x000000, 0x20000, CRC(6e60e736) SHA1(73086744cb2fe1ce162f851cb879755c21819b33) )
	ROM_LOAD16_BYTE( "epr-11468.bin",  0x000001, 0x20000, CRC(74ca9ca5) SHA1(c6f27ce43ef270088e6155c8240fd15afa5729fd) )
	ROM_LOAD16_BYTE( "epr-11694.bin",  0x040000, 0x20000, CRC(7e297b84) SHA1(bbf1aa2b0b6b1f9fdaf9bea77d24b7f4f9320696) )
	ROM_LOAD16_BYTE( "epr-11695.bin",  0x040001, 0x20000, CRC(38a864be) SHA1(ef7d89511713d695f6a454c42f079d3507d9690d) )

	ROM_REGION64_BE( 0x400000, "gfx1", 0)
	ROMX_LOAD( "epr-11469.bin",  0x000000, 0x20000, CRC(ed7a2299) SHA1(1aecf9ccba1fed0b7908008e798c522251a08b0f), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11470.bin",  0x000001, 0x20000, CRC(34dea550) SHA1(da95b8346c3530573461553629af4cc493bbb4af), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11477.bin",  0x000002, 0x20000, CRC(a2784653) SHA1(00a123d1fc8116ca678d6d8dbf1a5450feee014d), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11478.bin",  0x000003, 0x20000, CRC(8b778993) SHA1(015ae757d26cd6e69bdf79e237f62743a8f41e0c), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11471.bin",  0x000004, 0x20000, CRC(f1974069) SHA1(c8beb1a2ce430e3e6478c87c7053a58f8f31a140), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11472.bin",  0x000005, 0x20000, CRC(0d24409a) SHA1(ee00e23b0c548918b89dd48ed3f8b0370388659e), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11479.bin",  0x000006, 0x20000, CRC(ecd6138a) SHA1(bbc157a1f9b7e24d16e6f126d95c448736e83791), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11480.bin",  0x000007, 0x20000, CRC(64ad66c5) SHA1(aaab6999aa88b3340b16ee1188a8432477e16625), ROM_SKIP(7) )

	ROMX_LOAD( "epr-11473.bin",  0x100000, 0x20000, CRC(0538c6ec) SHA1(9397db188f12cf8cd91841794134760f30f83893), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11474.bin",  0x100001, 0x20000, CRC(eb923c50) SHA1(f997d2bc7f674eae4243eaf640d8faffa41d9521), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11481.bin",  0x100002, 0x20000, CRC(78e652b6) SHA1(8c70609172908131fef8cbe05f810dbd3d120eba), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11482.bin",  0x100003, 0x20000, CRC(2f879766) SHA1(ca5835d0bb77fe7de83ad336a82111cb2f96cd41), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11475.bin",  0x100004, 0x20000, CRC(69cfec89) SHA1(db2a9b03fff727f198fb5cbfb9c281ac3bbc5623), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11476.bin",  0x100005, 0x20000, CRC(a60b9b79) SHA1(6e9aa51923c12f5658bd17deb6c032e08c91dade), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11483.bin",  0x100006, 0x20000, CRC(d5d3a505) SHA1(fa7662346b954d3faf0e8fcf138004231676845c), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11484.bin",  0x100007, 0x20000, CRC(b8a56a50) SHA1(ccbc391d6f60b88630d7a93b8bb6e365a8d59ed8), ROM_SKIP(7) )

	ROMX_LOAD( "epr-11696.bin",  0x200000, 0x20000, CRC(99e8e49e) SHA1(ab6e1d74af412ec2f939043d7dc26f4b2e34a528), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11697.bin",  0x200001, 0x20000, CRC(7545c52e) SHA1(bdb0ccf233e10e9449aa367db5a5b5f209bee969), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11700.bin",  0x200002, 0x20000, CRC(e13839c1) SHA1(4975b5314797d11c782b0478eaa84eaf9980b1cd), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11701.bin",  0x200003, 0x20000, CRC(9fb3d365) SHA1(bfcf4abfa91aa41b16f01b7bafe97cd865167fbc), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11698.bin",  0x200004, 0x20000, CRC(cfeba3e2) SHA1(826b73858ca5ea05688246d36b1f607356974ca9), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11699.bin",  0x200005, 0x20000, CRC(4a00534a) SHA1(9a637b45b140420937b232bf690ef8bc3d43f5ad), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11702.bin",  0x200006, 0x20000, CRC(2a09c627) SHA1(c6b0a618b4ddd9d227d3472544b62ebef966b041), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11703.bin",  0x200007, 0x20000, CRC(43bb7d9f) SHA1(e36d208937f56f0af14292184c80ca939c32c378), ROM_SKIP(7) )

	ROMX_LOAD( "epr-11524.bin",  0x300000, 0x20000, CRC(5d35849f) SHA1(b86fb230ed0901dfdb525e7a47d9c9ad8031a3f6), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11525.bin",  0x300001, 0x20000, CRC(9ae47552) SHA1(43268dd5f79282aabd8972994437de1b2d2acc40), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11532.bin",  0x300002, 0x20000, CRC(b3565ddb) SHA1(3208744d43a11f4de8a6b626cc9113ad63d36807), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11533.bin",  0x300003, 0x20000, CRC(f5d16e8a) SHA1(3b5c1582794a0e69707264879b544ac48afd9337), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11526.bin",  0x300004, 0x20000, CRC(094cb3f0) SHA1(4b30e24dfd33c9922ffd5665cd9631b1f24243da), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11527.bin",  0x300005, 0x20000, CRC(e821a144) SHA1(eeec0e51798645cce8f9a7e686178268398977fb), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11534.bin",  0x300006, 0x20000, CRC(b7f0ad7c) SHA1(987b198692db365f3a81e88e5eec526775c318c6), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11535.bin",  0x300007, 0x20000, CRC(95da7a46) SHA1(ffe6cd80eecc7c53a1155396ede0720d975d2c3b), ROM_SKIP(7) )

	ROM_REGION( 0x10000, "soundcpu", 0 )		/* Z80 sound CPU */
	ROM_LOAD( "epr-11693.bin",	0x000000, 0x10000, CRC(0497785c) SHA1(f077e71efdc96d6eb3f1a1f989650466554fb13a) ) // 11516A

	ROM_REGION( 0x200000, "pcm", ROMREGION_ERASEFF )	/* SegaPCM samples */
	ROM_LOAD( "epr-11465.bin", 0x000000, 0x80000, CRC(e1436dab) SHA1(363f4c111de38cb1d82b245e4fcc65308f506e6a) )
	ROM_LOAD( "epr-11516.bin", 0x080000, 0x20000, CRC(19d0e17f) SHA1(7171131226cf1fe260a2db310fad2ec264adca26) )
	ROM_RELOAD(              0x0a0000, 0x20000 )
	ROM_RELOAD(              0x0c0000, 0x20000 )
	ROM_RELOAD(              0x0e0000, 0x20000 )
	ROM_LOAD( "epr-11814.bin", 0x100000, 0x20000, CRC(0b05d376) SHA1(8d0c0be2fd9dbc714c82c7cd1c439d5ff65e1317) ) // 11517
	ROM_RELOAD(              0x120000, 0x20000 )
	ROM_RELOAD(              0x140000, 0x20000 )
	ROM_RELOAD(              0x160000, 0x20000 )


ROM_END


/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    G-Loc, Sega Y-board
    CPU: 68000 (317-????)
*/
ROM_START( gloc )
	ROM_REGION( 0x080000, "maincpu", 0 ) // M
	ROM_LOAD16_BYTE( "epr-13170.25",  0x000000, 0x20000, CRC(45189229) SHA1(01d18f6e4887633475baf610f455fad4ed7981e4) )
	ROM_LOAD16_BYTE( "epr-13169.24",  0x000001, 0x20000, CRC(1b47cd6e) SHA1(694f489766f25e4c1f1fef6db79de347688ad80a) )
	ROM_LOAD16_BYTE( "epr-13028.27",  0x040000, 0x20000, CRC(b6aa2edf) SHA1(07259fc48cd0f63fbd0a8dadf2294575cd790c85) )
	ROM_LOAD16_BYTE( "epr-13027.26",  0x040001, 0x20000, CRC(6463c87a) SHA1(882d980a1568ca777364822295e173224509f842) )

	ROM_REGION( 0x040000, "subx", 0 ) // X
	ROM_LOAD16_BYTE( "epr-13032.81",  0x000000, 0x20000, CRC(7da09c4e) SHA1(09ec269c07f07549aa9851585eac0a5195e25bf9) )
	ROM_LOAD16_BYTE( "epr-13031.80",  0x000001, 0x20000, CRC(f3c7e3f4) SHA1(927c0cf05e7a72d79fdf19bbe7b18bf167feccd6) )

	ROM_REGION( 0x040000, "suby", 0 ) // Y
	ROM_LOAD16_BYTE( "epr-13030.54",  0x000000, 0x20000, CRC(81abcabf) SHA1(cb4e817d66a7f384aa9757758c51cd1bf7347dd0) )
	ROM_LOAD16_BYTE( "epr-13029.53",  0x000001, 0x20000, CRC(f3638efb) SHA1(f82a46fc8616cbe0235746161c587e54adecfe50) )

	ROM_REGION16_BE( 0x200000, "gfx2", 0)
	ROM_LOAD16_BYTE( "epr-13039.16",  0x000000, 0x80000, CRC(d7e1266d) SHA1(b0fc4cc60a7e876ae2af343bba6da3fb926ea9c5) )
	ROM_LOAD16_BYTE( "epr-13037.14",  0x000001, 0x80000, CRC(b801a250) SHA1(7d1f6a1f2022a4f302f22d11fa79057cf8134ad2) )
	ROM_LOAD16_BYTE( "epr-13040.17",  0x100000, 0x80000, CRC(4aeb3a85) SHA1(5521fd2d3956839bdbe7b70a9e60cd9fb72a42f1) )
	ROM_LOAD16_BYTE( "epr-13038.15",  0x100001, 0x80000, CRC(0b2edb6d) SHA1(04944d6e6f020cd6d33641110847706516630227) )

	ROM_REGION64_BE( 0x1000000, "gfx1", 0 )
	ROMX_LOAD( "epr-13048.67",  0x000000, 0x80000, CRC(fe1eb0dd) SHA1(5e292fc0b83505eb289e026d4be24c9038ef1418), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13056.75",  0x000001, 0x80000, CRC(5904f8e6) SHA1(fbb01dadc796624c360d44b7631e3f1f285abf2e), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13044.63",  0x000002, 0x80000, CRC(4d931f89) SHA1(ff603f4347e4728a2849d9f480893ad0af7abc5c), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13052.71",  0x000003, 0x80000, CRC(0291f040) SHA1(610dee2a31445f4a054111b7005278560a9c0702), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13064.86",  0x000004, 0x80000, CRC(5f8e651b) SHA1(f1a957e68dea40c23f6a5a208358ec6d6515fe60), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13072.114", 0x000005, 0x80000, CRC(6b85641a) SHA1(143a4684d5f303cd30880a2d5728dccbdd168da4), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13060.82",  0x000006, 0x80000, CRC(ee16ad97) SHA1(6af38cfaf694f686f8e4223fb0b13cd350a8b9e5), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13068.110", 0x000007, 0x80000, CRC(64d52bbb) SHA1(b6eab546edb2443e5da6c94ec811ec5084212e60), ROM_SKIP(7) )

	ROMX_LOAD( "epr-13047.66",  0x400000, 0x80000, CRC(53340832) SHA1(8ece8a71ea8ed80458121622307a137fb13931f6), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13055.74",  0x400001, 0x80000, CRC(39b6b665) SHA1(d915db1d9bfe0c6ad3f7b447ce0cfdb42ec66ffe), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13043.62",  0x400002, 0x80000, CRC(208f16fd) SHA1(ce96708ea9886af4aba8730cbb98c0ca72b96f57), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13051.70",  0x400003, 0x80000, CRC(ad62cbd4) SHA1(09c008ce5cb97575a4312d2f22566bda72ecc4e2), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13063.85",  0x400004, 0x80000, CRC(c580bf6d) SHA1(cb72970377ad2acce499059aa8155711b8da8a11), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13071.113", 0x400005, 0x80000, CRC(df99ef99) SHA1(12648844c6e78dbd573b7bf0c981edb4d3012b58), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13059.81",  0x400006, 0x80000, CRC(4c982558) SHA1(e04902af2740ca098cd6bbf1f57cb25562754a76), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13067.109", 0x400007, 0x80000, CRC(f97f6119) SHA1(6f91fc28a1260ca4f1c695863717b27d1e45dc32), ROM_SKIP(7) )

	ROMX_LOAD( "epr-13046.65",  0x800000, 0x80000, CRC(c75a86e9) SHA1(8a180e1e2dd06eb81e2aa4ef73b83879cf6afc1b), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13054.73",  0x800001, 0x80000, CRC(2934549a) SHA1(058b2966141d0db6bb8557d65c77b3458aca9358), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13042.61",  0x800002, 0x80000, CRC(53ed97af) SHA1(22dffa434eb98e5bca1e429b69553a3540dc54a7), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13050.69",  0x800003, 0x80000, CRC(04429068) SHA1(d7d8738809fd959ed428796b2bd1b589b74522c6), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13062.84",  0x800004, 0x80000, CRC(4fdb4ee3) SHA1(d76065b9abe5c3cf692567d3a8746a231748340d), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13070.112", 0x800005, 0x80000, CRC(52ea130e) SHA1(860cb3a1701066e595518c49b696b7b7a3994ada), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13058.80",  0x800006, 0x80000, CRC(19ff1626) SHA1(029e231c3322467b5e2e52eea11df4f645460468), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13066.108", 0x800007, 0x80000, CRC(bc70a250) SHA1(25189854cc01855b6e3589b85490f30dda029f86), ROM_SKIP(7) )

	ROMX_LOAD( "epr-13045.64",  0xc00000, 0x80000, CRC(54d5bc6d) SHA1(18a301c9e6c4a352f300a438d85c6e6952bf0738), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13053.72",  0xc00001, 0x80000, CRC(9502af13) SHA1(1a8c0fcd10f4c86af69c0107f486ca2eb8863f93), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13041.60",  0xc00002, 0x80000, CRC(d0a7402c) SHA1(8932503c570ec49fdb4706f4015608bd060bafa0), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13049.68",  0xc00003, 0x80000, CRC(5b9c0b6c) SHA1(17f2460b7dc0bd34dca3f90f2b553df4a7149147), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13061.83",  0xc00004, 0x80000, CRC(7b95ec3b) SHA1(284aba4effd9d376a7a8f510a6f675fcb3393d09), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13069.111", 0xc00005, 0x80000, CRC(e1f538f0) SHA1(55dc85faed1d5a7f2d586bac7e524c3fef3c53b4), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13057.79",  0xc00006, 0x80000, CRC(73baefee) SHA1(6e86edc8229dd6112034a7df79f7341a4120dc6b), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13065.107", 0xc00007, 0x80000, CRC(8937a655) SHA1(d38726a8a6fe68a002ac8d17f70ab83c2f814aa2), ROM_SKIP(7) )

	ROM_REGION( 0x10000, "soundcpu", 0 )		/* Z80 sound CPU */
	ROM_LOAD( "epr-13033.102",    0x000000, 0x10000, CRC(6df5e827) SHA1(ec260886a27ba00690490500fcf4ebf07fb35205) )

	ROM_REGION( 0x200000, "pcm", ROMREGION_ERASEFF )	/* SegaPCM samples */
	ROM_LOAD( "epr-13036.107", 0x000000, 0x80000, CRC(7890c26c) SHA1(97e0678bb571de5cf732804f8909e5cbd24980f1) )
	ROM_LOAD( "epr-13035.106", 0x080000, 0x80000, CRC(009fa13e) SHA1(c7b224b471696b12332fc7c403c127b19c297df7) )
	ROM_LOAD( "epr-13034.105", 0x100000, 0x80000, CRC(cd22d95d) SHA1(857aa320df0b3fb44fc8a5526ba5ee82cc74fe63) )
ROM_END


ROM_START( glocr360 )
	ROM_REGION( 0x080000, "maincpu", 0 ) // M
	ROM_LOAD16_BYTE( "epr-13623.25",   0x000000, 0x20000, CRC(58ad10e7) SHA1(3760ede1d1c089f6c8b2ec88b2f25bce67add467) )
	ROM_LOAD16_BYTE( "epr-13622.24",  0x000001, 0x20000, CRC(c4e68dbf) SHA1(f85e6fdf159e19342e5a9278f004e95752af7d55) )
	ROM_LOAD16_BYTE( "epr-13323a.27",  0x040000, 0x20000, CRC(02e24a33) SHA1(4955b13e5e90945dfb9066597b16df63c2a09552) )
	ROM_LOAD16_BYTE( "epr-13322a.26",  0x040001, 0x20000, CRC(94f67740) SHA1(3d1be8dc9c370cd024fae19bb0b2663995d13d0e) )

	ROM_REGION( 0x040000, "subx", 0 ) // X
	ROM_LOAD16_BYTE( "epr-13327.81",  0x000000, 0x20000, CRC(627036f9) SHA1(bae8a2b1a90088532e5d487e36265de9a1f38f84) )
	ROM_LOAD16_BYTE( "epr-13326.80",  0x000001, 0x20000, CRC(162ac233) SHA1(863b93a38906e3d7f3446c2ee4914205f8123022) )

	ROM_REGION( 0x040000, "suby", 0 ) // Y
	ROM_LOAD16_BYTE( "epr-13325a.54",  0x000000, 0x20000, CRC(aba307e5) SHA1(a27a7d3699a95d7c6265a23157b2fefd362003dd) )
	ROM_LOAD16_BYTE( "epr-13324a.53",  0x000001, 0x20000, CRC(eb1b19e5) SHA1(3d1d7299cb3befc22afc0db0376d7f94dec37367) )

	ROM_REGION16_BE( 0x200000, "gfx2", 0)
	ROM_LOAD16_BYTE( "epr-13039.16",  0x000000, 0x80000, CRC(d7e1266d) SHA1(b0fc4cc60a7e876ae2af343bba6da3fb926ea9c5) )
	ROM_LOAD16_BYTE( "epr-13037.14",  0x000001, 0x80000, CRC(b801a250) SHA1(7d1f6a1f2022a4f302f22d11fa79057cf8134ad2) )
	ROM_LOAD16_BYTE( "epr-13040.17",  0x100000, 0x80000, CRC(4aeb3a85) SHA1(5521fd2d3956839bdbe7b70a9e60cd9fb72a42f1) )
	ROM_LOAD16_BYTE( "epr-13038.15",  0x100001, 0x80000, CRC(0b2edb6d) SHA1(04944d6e6f020cd6d33641110847706516630227) )

	ROM_REGION64_BE( 0x1000000, "gfx1", 0 )
	ROMX_LOAD( "epr-13048.67",  0x000000, 0x80000, CRC(fe1eb0dd) SHA1(5e292fc0b83505eb289e026d4be24c9038ef1418), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13056.75",  0x000001, 0x80000, CRC(5904f8e6) SHA1(fbb01dadc796624c360d44b7631e3f1f285abf2e), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13044.63",  0x000002, 0x80000, CRC(4d931f89) SHA1(ff603f4347e4728a2849d9f480893ad0af7abc5c), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13052.71",  0x000003, 0x80000, CRC(0291f040) SHA1(610dee2a31445f4a054111b7005278560a9c0702), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13064.86",  0x000004, 0x80000, CRC(5f8e651b) SHA1(f1a957e68dea40c23f6a5a208358ec6d6515fe60), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13072.114", 0x000005, 0x80000, CRC(6b85641a) SHA1(143a4684d5f303cd30880a2d5728dccbdd168da4), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13060.82",  0x000006, 0x80000, CRC(ee16ad97) SHA1(6af38cfaf694f686f8e4223fb0b13cd350a8b9e5), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13068.110", 0x000007, 0x80000, CRC(64d52bbb) SHA1(b6eab546edb2443e5da6c94ec811ec5084212e60), ROM_SKIP(7) )

	ROMX_LOAD( "epr-13047.66",  0x400000, 0x80000, CRC(53340832) SHA1(8ece8a71ea8ed80458121622307a137fb13931f6), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13055.74",  0x400001, 0x80000, CRC(39b6b665) SHA1(d915db1d9bfe0c6ad3f7b447ce0cfdb42ec66ffe), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13043.62",  0x400002, 0x80000, CRC(208f16fd) SHA1(ce96708ea9886af4aba8730cbb98c0ca72b96f57), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13051.70",  0x400003, 0x80000, CRC(ad62cbd4) SHA1(09c008ce5cb97575a4312d2f22566bda72ecc4e2), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13063.85",  0x400004, 0x80000, CRC(c580bf6d) SHA1(cb72970377ad2acce499059aa8155711b8da8a11), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13071.113", 0x400005, 0x80000, CRC(df99ef99) SHA1(12648844c6e78dbd573b7bf0c981edb4d3012b58), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13059.81",  0x400006, 0x80000, CRC(4c982558) SHA1(e04902af2740ca098cd6bbf1f57cb25562754a76), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13067.109", 0x400007, 0x80000, CRC(f97f6119) SHA1(6f91fc28a1260ca4f1c695863717b27d1e45dc32), ROM_SKIP(7) )

	ROMX_LOAD( "epr-13331.65",  0x800000, 0x80000, CRC(8ea8febe) SHA1(c5b68f955807d7d901b773ba8fba5c9a2d29afd1), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13333.73",  0x800001, 0x80000, CRC(5bcd37d4) SHA1(df25d3ef4acd3aea2484ff5760a63245eafcb66c), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13330.61",  0x800002, 0x80000, CRC(1e325d52) SHA1(91ffdb5d9926ea573550079ca0486a71baf70d07), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13332.69",  0x800003, 0x80000, CRC(8fd8067e) SHA1(ac438ae60fa08418b5590f5656e2d1ecc3e6eb15), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13335.84",  0x800004, 0x80000, CRC(98ea420b) SHA1(fc138a45e287e0c7c3ec1cc75aa6dabc9dca8d42), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13337.112", 0x800005, 0x80000, CRC(f55f00a4) SHA1(77f1e946c7bce69aa9f1f3e59af1e033338c9d95), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13334.80",  0x800006, 0x80000, CRC(72725060) SHA1(6002eca365a69816ac6e7ccba65ae73967e83a89), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13336.108", 0x800007, 0x80000, CRC(e2d4d477) SHA1(cdeefeb6d66433c121cac4e77453f6265bacf9e2), ROM_SKIP(7) )

	ROMX_LOAD( "epr-13045.64",  0xc00000, 0x80000, CRC(54d5bc6d) SHA1(18a301c9e6c4a352f300a438d85c6e6952bf0738), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13053.72",  0xc00001, 0x80000, CRC(9502af13) SHA1(1a8c0fcd10f4c86af69c0107f486ca2eb8863f93), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13041.60",  0xc00002, 0x80000, CRC(d0a7402c) SHA1(8932503c570ec49fdb4706f4015608bd060bafa0), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13049.68",  0xc00003, 0x80000, CRC(5b9c0b6c) SHA1(17f2460b7dc0bd34dca3f90f2b553df4a7149147), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13061.83",  0xc00004, 0x80000, CRC(7b95ec3b) SHA1(284aba4effd9d376a7a8f510a6f675fcb3393d09), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13069.111", 0xc00005, 0x80000, CRC(e1f538f0) SHA1(55dc85faed1d5a7f2d586bac7e524c3fef3c53b4), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13057.79",  0xc00006, 0x80000, CRC(73baefee) SHA1(6e86edc8229dd6112034a7df79f7341a4120dc6b), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13065.107", 0xc00007, 0x80000, CRC(8937a655) SHA1(d38726a8a6fe68a002ac8d17f70ab83c2f814aa2), ROM_SKIP(7) )

	ROM_REGION( 0x10000, "soundcpu", 0 )		/* Z80 sound CPU */
	ROM_LOAD( "epr-13624.102",  0x000000, 0x10000, CRC(eff33f2d) SHA1(87ea22042e5ee1df28544e4959e16f54cfe17f23) )

	ROM_REGION( 0x200000, "pcm", ROMREGION_ERASEFF )	/* SegaPCM samples */
	ROM_LOAD( "epr-13036.107",  0x000000, 0x80000, CRC(7890c26c) SHA1(97e0678bb571de5cf732804f8909e5cbd24980f1) )
	ROM_LOAD( "epr-13035.106",  0x080000, 0x80000, CRC(009fa13e) SHA1(c7b224b471696b12332fc7c403c127b19c297df7) )
	ROM_LOAD( "epr-13625.105",  0x100000, 0x80000, CRC(fae71fd2) SHA1(c8468486b1ac74d3a6254d538f05034b1533e40b) )
ROM_END

/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Power Drift, Sega Y-board
    CPU: 68000 (317-????)
     CPU BD POWER DRIFT   837-6695-08 (or 837-6695-09)
     VIDEO BD POWER DRIFT 837-6696-01 (or 837-6696-02)
*/
ROM_START( pdrift )
	ROM_REGION( 0x080000, "maincpu", 0 ) // M
	ROM_LOAD16_BYTE( "epr-12017.25",  0x000000, 0x20000, CRC(31190322) SHA1(18df4bd6078b1c76c7061c05a476a7cbf7d0b37b) )
	ROM_LOAD16_BYTE( "epr-12016.24",  0x000001, 0x20000, CRC(499f64a6) SHA1(506652bdb0f3d23a2cf7ff29732b8ec77c389b46) )
	ROM_LOAD16_BYTE( "epr-11748.27",  0x040000, 0x20000, CRC(82a76cab) SHA1(f8d3fe059e18896cd0e64711f1a3ee8b6372b4e0) )
	ROM_LOAD16_BYTE( "epr-11747.26",  0x040001, 0x20000, CRC(9796ece5) SHA1(f84f5689c2edc0853ff173ce20f93f89758b2f31) )

	ROM_REGION( 0x040000, "subx", 0 ) // X
	ROM_LOAD16_BYTE( "epr-11905.81",  0x000000, 0x20000, CRC(1cf68109) SHA1(198c1f9d4278586d1e0f50acaaf08e60ba464f8a) )
	ROM_LOAD16_BYTE( "epr-11904.80",  0x000001, 0x20000, CRC(bb993681) SHA1(c5107e8d52cba3a0fc813ffa9e3752e0f9c3c0df) )

	ROM_REGION( 0x040000, "suby", 0 ) // Y
	ROM_LOAD16_BYTE( "epr-12019a.54", 0x000000, 0x20000, CRC(11188a30) SHA1(42dd0344d92529848b53a8cec4c145237ccd5b51) )
	ROM_LOAD16_BYTE( "epr-12018a.53", 0x000001, 0x20000, CRC(1c582e1f) SHA1(c32d2f921554bddd7dedcb81e231aa91f50fa27b) )

	ROM_REGION16_BE( 0x080000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "epr-11789.16",  0x000000, 0x20000, CRC(b86f8d2b) SHA1(a053f2021841fd0ef89fd3f28050a698b36c435e) )
	ROM_LOAD16_BYTE( "epr-11791.14",  0x000001, 0x20000, CRC(36b2910a) SHA1(9948b91837f944a7a606542fa685525e74bbe398) )
	ROM_LOAD16_BYTE( "epr-11790.17",  0x040000, 0x20000, CRC(2a564e66) SHA1(5f30fc15bfd017d75cfffe1e9e62ed0bcf32a98e) )
	ROM_LOAD16_BYTE( "epr-11792.15",  0x040001, 0x20000, CRC(c85caf6e) SHA1(2411ea99ec7f6e2b0b4f219e86ff2172539ad2c4) )

	ROM_REGION64_BE( 0x400000, "gfx1", 0)
	ROMX_LOAD( "epr-11757.67",  0x000000, 0x20000, CRC(e46dc478) SHA1(baf79e230aef3d63fb50373b2b1626f7c56ee94f), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11758.75",  0x000001, 0x20000, CRC(5b435c87) SHA1(6b42b08e73957c36cd8faa896ca14461d00afd29), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11773.63",  0x000002, 0x20000, CRC(1b5d5758) SHA1(54f58a274740a0566e0553d145c0c284ffd1d36b), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11774.71",  0x000003, 0x20000, CRC(2ca0c170) SHA1(7de74c045bf084659ba70da9458d720125ff25ae), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11759.86",  0x000004, 0x20000, CRC(ac8111f6) SHA1(6412716dc97ae697b438d9c9cd554d1087416bc2), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11760.114", 0x000005, 0x20000, CRC(91282af9) SHA1(fddee7982949b7da724c7830e7bd139aeb84672d), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11775.82",  0x000006, 0x20000, CRC(48225793) SHA1(ee003c2ea24c14e0968da94bac139735660932fe), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11776.110", 0x000007, 0x20000, CRC(78c46198) SHA1(d299e631843da47cb7a46103d52a3dabfab71746), ROM_SKIP(7) )

	ROMX_LOAD( "epr-11761.66",  0x100000, 0x20000, CRC(baa5d065) SHA1(56dc71814e3f0f327781b0c1587038351c60f7b7), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11762.74",  0x100001, 0x20000, CRC(1d1af7a5) SHA1(86c02565b5aca201588c98678fb0c54faa8d4d6b), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11777.62",  0x100002, 0x20000, CRC(9662dd32) SHA1(454ec914b6c936f692bf90d2232c8169acec470a), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11778.70",  0x100003, 0x20000, CRC(2dfb7494) SHA1(4b9f1609e425c5e634e95dbc2d0ca820dd9212bc), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11763.85",  0x100004, 0x20000, CRC(1ee23407) SHA1(776c868e0e4e601fd6d0a83561b064b4be0560e2), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11764.113", 0x100005, 0x20000, CRC(e859305e) SHA1(aafcc3209a4fb6e0e8169ae6cce386b370b824f7), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11779.81",  0x100006, 0x20000, CRC(a49cd793) SHA1(efe77949be39a2ff88b50bfb2b4664b9267d9a09), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11780.109", 0x100007, 0x20000, CRC(d514ed81) SHA1(fbac3ad085363972a79e77aebb7fdae2200e7cda), ROM_SKIP(7) )

	ROMX_LOAD( "epr-11765.65",  0x200000, 0x20000, CRC(649e2dff) SHA1(a6c61b71d08b31a0ca175ab0404e2eaf1d09ccc2), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11766.73",  0x200001, 0x20000, CRC(d92fb7fc) SHA1(2f5c2d88ae0766351b9efe8ffcbebc88fc3a6c59), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11781.61",  0x200002, 0x20000, CRC(9692d4cd) SHA1(967351ba2c781ca865e3c1ee9eeef1aad2247c27), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11782.69",  0x200003, 0x20000, CRC(c913bb43) SHA1(9bc15a3180cf4c3134bb55e99e6092f0faf95c56), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11767.84",  0x200004, 0x20000, CRC(1f8ad054) SHA1(289f5795116ee29540f28e35c3b4f72adeca7891), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11768.112", 0x200005, 0x20000, CRC(db2c4053) SHA1(a5b6daa6deb7afb0019e289acb81c82d507ec93a), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11783.80",  0x200006, 0x20000, CRC(6d189007) SHA1(dd871ea3166fdcb59d49707d35dde8b6c7fdc76b), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11784.108", 0x200007, 0x20000, CRC(57f5fd64) SHA1(6aff54d3f3f76ce0f1a93485d1a35a3987d456d9), ROM_SKIP(7) )

	ROMX_LOAD( "epr-11769.64",  0x300000, 0x20000, CRC(28f0ab51) SHA1(d7cb7b83e5d85eb59d34cfd5c0d8e6c7ff81e24c), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11770.72",  0x300001, 0x20000, CRC(d7557ea9) SHA1(62430505d399ee2cc0f94e03144860056345573c), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11785.60",  0x300002, 0x20000, CRC(e6ef32c4) SHA1(869ba3816f5e3125f613f3b284fec74cd19db79e), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11786.68",  0x300003, 0x20000, CRC(2066b49d) SHA1(905ce70c921043d07591422a87fedd6e897ff38e), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11771.83",  0x300004, 0x20000, CRC(67635618) SHA1(f690ace026130ecb95532c92f2ad3741d0d167c1), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11772.111", 0x300005, 0x20000, CRC(0f798d3a) SHA1(71565ce28b93ae50d64af8c965fba6408a07f031), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11787.79",  0x300006, 0x20000, CRC(e631dc12) SHA1(3fd6db2eb297890b35dec566b6a90fc2d96bd085), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11788.107", 0x300007, 0x20000, CRC(8464c66e) SHA1(af93cbcc50acbd929d0298fb9a75da0369e13ff7), ROM_SKIP(7) )

	ROM_REGION( 0x10000, "soundcpu", 0 )		/* Z80 sound CPU */
	ROM_LOAD( "epr-11899.102",  0x000000, 0x10000, CRC(ed9fa889) SHA1(25d1a069254b34c31d8ee82d301ada895e8dc391) )

	ROM_REGION( 0x200000, "pcm", ROMREGION_ERASEFF )	/* SegaPCM samples */
	ROM_LOAD( "mpr-11754.107",  0x000000, 0x80000, CRC(ebeb8484) SHA1(269f33cb1a9be126bada858e25291385d48686a2) )
	ROM_LOAD( "epr-11755.105",  0x080000, 0x20000, CRC(12e43f8a) SHA1(0f9a11ba6b7c1a352daa1146a01ce147945e91e4) )
	ROM_RELOAD(                 0x0a0000, 0x20000 )
	ROM_RELOAD(                 0x0c0000, 0x20000 )
	ROM_RELOAD(                 0x0e0000, 0x20000 )
	ROM_LOAD( "epr-11756.106",  0x100000, 0x20000, CRC(c2db1244) SHA1(c98fe17c9f04a639a862cc2a86fab17d1f5d025c) )
	ROM_RELOAD(                 0x120000, 0x20000 )
	ROM_RELOAD(                 0x140000, 0x20000 )
	ROM_RELOAD(                 0x160000, 0x20000 )

	ROM_REGION( 0x100000, "user1", 0 )
	/* These are mpr-11754.107 split into 4 roms. They would be located on a Sega 839-0221 daughter card. */
	ROM_LOAD( "epr-11895.ic1", 0x000000, 0x20000, CRC(ee99a6fd) SHA1(4444826e751d9186e6d46b081e47cd99ee3cf853) )
	ROM_LOAD( "epr-11896.ic2", 0x000000, 0x20000, CRC(4bebc015) SHA1(307022ea1c1ee87c9ef3782526888c48c3c69fd2) )
	ROM_LOAD( "epr-11897.ic3", 0x000000, 0x20000, CRC(4463cb95) SHA1(e86fd4611cf83fe72d59950a60fc8c3a7381a1c7) )
	ROM_LOAD( "epr-11898.ic4", 0x000000, 0x20000, CRC(5d19d767) SHA1(d335cd3ef57c75e388df04b04fc3e2881a3902cf) )
ROM_END

ROM_START( pdrifta )
	ROM_REGION( 0x080000, "maincpu", 0 ) // M
	ROM_LOAD16_BYTE( "epr-12017.25",  0x000000, 0x20000, CRC(31190322) SHA1(18df4bd6078b1c76c7061c05a476a7cbf7d0b37b) )
	ROM_LOAD16_BYTE( "epr-12016.24",  0x000001, 0x20000, CRC(499f64a6) SHA1(506652bdb0f3d23a2cf7ff29732b8ec77c389b46) )
	ROM_LOAD16_BYTE( "epr-11748.27",  0x040000, 0x20000, CRC(82a76cab) SHA1(f8d3fe059e18896cd0e64711f1a3ee8b6372b4e0) )
	ROM_LOAD16_BYTE( "epr-11747.26",  0x040001, 0x20000, CRC(9796ece5) SHA1(f84f5689c2edc0853ff173ce20f93f89758b2f31) )

	ROM_REGION( 0x040000, "subx", 0 ) // X
	ROM_LOAD16_BYTE( "epr-11905.81",  0x000000, 0x20000, CRC(1cf68109) SHA1(198c1f9d4278586d1e0f50acaaf08e60ba464f8a) )
	ROM_LOAD16_BYTE( "epr-11904.80",  0x000001, 0x20000, CRC(bb993681) SHA1(c5107e8d52cba3a0fc813ffa9e3752e0f9c3c0df) )

	ROM_REGION( 0x040000, "suby", 0 ) // Y
	ROM_LOAD16_BYTE( "epr-12019.54", 0x000000, 0x20000, CRC(e514d7b6) SHA1(27ae99f5f3e8d2f248916f7a252e2c0686638df5) )
	ROM_LOAD16_BYTE( "epr-12018.53", 0x000001, 0x20000, CRC(0a3f7faf) SHA1(fe20a164a7a2c9e9bf0e7aade75b0488bdc93d79) )

	ROM_REGION16_BE( 0x080000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "epr-11789.16",  0x000000, 0x20000, CRC(b86f8d2b) SHA1(a053f2021841fd0ef89fd3f28050a698b36c435e) )
	ROM_LOAD16_BYTE( "epr-11791.14",  0x000001, 0x20000, CRC(36b2910a) SHA1(9948b91837f944a7a606542fa685525e74bbe398) )
	ROM_LOAD16_BYTE( "epr-11790.17",  0x040000, 0x20000, CRC(2a564e66) SHA1(5f30fc15bfd017d75cfffe1e9e62ed0bcf32a98e) )
	ROM_LOAD16_BYTE( "epr-11792.15",  0x040001, 0x20000, CRC(c85caf6e) SHA1(2411ea99ec7f6e2b0b4f219e86ff2172539ad2c4) )

	ROM_REGION64_BE( 0x400000, "gfx1", 0)
	ROMX_LOAD( "epr-11757.67",  0x000000, 0x20000, CRC(e46dc478) SHA1(baf79e230aef3d63fb50373b2b1626f7c56ee94f), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11758.75",  0x000001, 0x20000, CRC(5b435c87) SHA1(6b42b08e73957c36cd8faa896ca14461d00afd29), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11773.63",  0x000002, 0x20000, CRC(1b5d5758) SHA1(54f58a274740a0566e0553d145c0c284ffd1d36b), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11774.71",  0x000003, 0x20000, CRC(2ca0c170) SHA1(7de74c045bf084659ba70da9458d720125ff25ae), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11759.86",  0x000004, 0x20000, CRC(ac8111f6) SHA1(6412716dc97ae697b438d9c9cd554d1087416bc2), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11760.114", 0x000005, 0x20000, CRC(91282af9) SHA1(fddee7982949b7da724c7830e7bd139aeb84672d), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11775.82",  0x000006, 0x20000, CRC(48225793) SHA1(ee003c2ea24c14e0968da94bac139735660932fe), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11776.110", 0x000007, 0x20000, CRC(78c46198) SHA1(d299e631843da47cb7a46103d52a3dabfab71746), ROM_SKIP(7) )

	ROMX_LOAD( "epr-11761.66",  0x100000, 0x20000, CRC(baa5d065) SHA1(56dc71814e3f0f327781b0c1587038351c60f7b7), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11762.74",  0x100001, 0x20000, CRC(1d1af7a5) SHA1(86c02565b5aca201588c98678fb0c54faa8d4d6b), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11777.62",  0x100002, 0x20000, CRC(9662dd32) SHA1(454ec914b6c936f692bf90d2232c8169acec470a), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11778.70",  0x100003, 0x20000, CRC(2dfb7494) SHA1(4b9f1609e425c5e634e95dbc2d0ca820dd9212bc), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11763.85",  0x100004, 0x20000, CRC(1ee23407) SHA1(776c868e0e4e601fd6d0a83561b064b4be0560e2), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11764.113", 0x100005, 0x20000, CRC(e859305e) SHA1(aafcc3209a4fb6e0e8169ae6cce386b370b824f7), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11779.81",  0x100006, 0x20000, CRC(a49cd793) SHA1(efe77949be39a2ff88b50bfb2b4664b9267d9a09), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11780.109", 0x100007, 0x20000, CRC(d514ed81) SHA1(fbac3ad085363972a79e77aebb7fdae2200e7cda), ROM_SKIP(7) )

	ROMX_LOAD( "epr-11765.65",  0x200000, 0x20000, CRC(649e2dff) SHA1(a6c61b71d08b31a0ca175ab0404e2eaf1d09ccc2), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11766.73",  0x200001, 0x20000, CRC(d92fb7fc) SHA1(2f5c2d88ae0766351b9efe8ffcbebc88fc3a6c59), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11781.61",  0x200002, 0x20000, CRC(9692d4cd) SHA1(967351ba2c781ca865e3c1ee9eeef1aad2247c27), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11782.69",  0x200003, 0x20000, CRC(c913bb43) SHA1(9bc15a3180cf4c3134bb55e99e6092f0faf95c56), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11767.84",  0x200004, 0x20000, CRC(1f8ad054) SHA1(289f5795116ee29540f28e35c3b4f72adeca7891), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11768.112", 0x200005, 0x20000, CRC(db2c4053) SHA1(a5b6daa6deb7afb0019e289acb81c82d507ec93a), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11783.80",  0x200006, 0x20000, CRC(6d189007) SHA1(dd871ea3166fdcb59d49707d35dde8b6c7fdc76b), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11784.108", 0x200007, 0x20000, CRC(57f5fd64) SHA1(6aff54d3f3f76ce0f1a93485d1a35a3987d456d9), ROM_SKIP(7) )

	ROMX_LOAD( "epr-11769.64",  0x300000, 0x20000, CRC(28f0ab51) SHA1(d7cb7b83e5d85eb59d34cfd5c0d8e6c7ff81e24c), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11770.72",  0x300001, 0x20000, CRC(d7557ea9) SHA1(62430505d399ee2cc0f94e03144860056345573c), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11785.60",  0x300002, 0x20000, CRC(e6ef32c4) SHA1(869ba3816f5e3125f613f3b284fec74cd19db79e), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11786.68",  0x300003, 0x20000, CRC(2066b49d) SHA1(905ce70c921043d07591422a87fedd6e897ff38e), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11771.83",  0x300004, 0x20000, CRC(67635618) SHA1(f690ace026130ecb95532c92f2ad3741d0d167c1), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11772.111", 0x300005, 0x20000, CRC(0f798d3a) SHA1(71565ce28b93ae50d64af8c965fba6408a07f031), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11787.79",  0x300006, 0x20000, CRC(e631dc12) SHA1(3fd6db2eb297890b35dec566b6a90fc2d96bd085), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11788.107", 0x300007, 0x20000, CRC(8464c66e) SHA1(af93cbcc50acbd929d0298fb9a75da0369e13ff7), ROM_SKIP(7) )

	ROM_REGION( 0x10000, "soundcpu", 0 )		/* Z80 sound CPU */
	ROM_LOAD( "epr-11899.102",  0x000000, 0x10000, CRC(ed9fa889) SHA1(25d1a069254b34c31d8ee82d301ada895e8dc391) )

	ROM_REGION( 0x200000, "pcm", ROMREGION_ERASEFF )	/* SegaPCM samples */
	ROM_LOAD( "mpr-11754.107",  0x000000, 0x80000, CRC(ebeb8484) SHA1(269f33cb1a9be126bada858e25291385d48686a2) )
	ROM_LOAD( "epr-11755.105",  0x080000, 0x20000, CRC(12e43f8a) SHA1(0f9a11ba6b7c1a352daa1146a01ce147945e91e4) )
	ROM_RELOAD(                 0x0a0000, 0x20000 )
	ROM_RELOAD(                 0x0c0000, 0x20000 )
	ROM_RELOAD(                 0x0e0000, 0x20000 )
	ROM_LOAD( "epr-11756.106",  0x100000, 0x20000, CRC(c2db1244) SHA1(c98fe17c9f04a639a862cc2a86fab17d1f5d025c) )
	ROM_RELOAD(                 0x120000, 0x20000 )
	ROM_RELOAD(                 0x140000, 0x20000 )
	ROM_RELOAD(                 0x160000, 0x20000 )

	ROM_REGION( 0x100000, "user1", 0 )
	/* These are mpr-11754.107 split into 4 roms. They would be located on a Sega 839-0221 daughter card. */
	ROM_LOAD( "epr-11895.ic1", 0x000000, 0x20000, CRC(ee99a6fd) SHA1(4444826e751d9186e6d46b081e47cd99ee3cf853) )
	ROM_LOAD( "epr-11896.ic2", 0x000000, 0x20000, CRC(4bebc015) SHA1(307022ea1c1ee87c9ef3782526888c48c3c69fd2) )
	ROM_LOAD( "epr-11897.ic3", 0x000000, 0x20000, CRC(4463cb95) SHA1(e86fd4611cf83fe72d59950a60fc8c3a7381a1c7) )
	ROM_LOAD( "epr-11898.ic4", 0x000000, 0x20000, CRC(5d19d767) SHA1(d335cd3ef57c75e388df04b04fc3e2881a3902cf) )
ROM_END

ROM_START( pdrifte ) /* Earlier set based on eprom numbers & Sega Eprom/Mask Rom Locations sheet 421-7708 */
	ROM_REGION( 0x080000, "maincpu", 0 ) // M
	ROM_LOAD16_BYTE( "epr-11901.25",  0x000000, 0x20000, CRC(16744be8) SHA1(51d06f1b590621d37b2eb8f6cc88f17b248402ad) )
	ROM_LOAD16_BYTE( "epr-11900.24",  0x000001, 0x20000, CRC(0a170d06) SHA1(525dec21708fb33c6c7e6d86cb82fd16f6bddfa0) )
	ROM_LOAD16_BYTE( "epr-11748.27",  0x040000, 0x20000, CRC(82a76cab) SHA1(f8d3fe059e18896cd0e64711f1a3ee8b6372b4e0) )
	ROM_LOAD16_BYTE( "epr-11747.26",  0x040001, 0x20000, CRC(9796ece5) SHA1(f84f5689c2edc0853ff173ce20f93f89758b2f31) )

	ROM_REGION( 0x040000, "subx", 0 ) // X
	ROM_LOAD16_BYTE( "epr-11905.81",  0x000000, 0x20000, CRC(1cf68109) SHA1(198c1f9d4278586d1e0f50acaaf08e60ba464f8a) )
	ROM_LOAD16_BYTE( "epr-11904.80",  0x000001, 0x20000, CRC(bb993681) SHA1(c5107e8d52cba3a0fc813ffa9e3752e0f9c3c0df) )

	ROM_REGION( 0x040000, "suby", 0 ) // Y
	ROM_LOAD16_BYTE( "epr-11903.54",  0x000000, 0x20000, CRC(d004f411) SHA1(212a985275647fae24b580ebaffc1230c06318ac) )
	ROM_LOAD16_BYTE( "epr-11902.53",  0x000001, 0x20000, CRC(e8028e08) SHA1(de4ee5011e9552e624b6223e0e1ef00bc271a811) )

	ROM_REGION16_BE( 0x080000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "epr-11789.16",  0x000000, 0x20000, CRC(b86f8d2b) SHA1(a053f2021841fd0ef89fd3f28050a698b36c435e) )
	ROM_LOAD16_BYTE( "epr-11791.14",  0x000001, 0x20000, CRC(36b2910a) SHA1(9948b91837f944a7a606542fa685525e74bbe398) )
	ROM_LOAD16_BYTE( "epr-11790.17",  0x040000, 0x20000, CRC(2a564e66) SHA1(5f30fc15bfd017d75cfffe1e9e62ed0bcf32a98e) )
	ROM_LOAD16_BYTE( "epr-11792.15",  0x040001, 0x20000, CRC(c85caf6e) SHA1(2411ea99ec7f6e2b0b4f219e86ff2172539ad2c4) )

	ROM_REGION64_BE( 0x400000, "gfx1", 0)
	ROMX_LOAD( "epr-11757.67",  0x000000, 0x20000, CRC(e46dc478) SHA1(baf79e230aef3d63fb50373b2b1626f7c56ee94f), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11758.75",  0x000001, 0x20000, CRC(5b435c87) SHA1(6b42b08e73957c36cd8faa896ca14461d00afd29), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11773.63",  0x000002, 0x20000, CRC(1b5d5758) SHA1(54f58a274740a0566e0553d145c0c284ffd1d36b), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11774.71",  0x000003, 0x20000, CRC(2ca0c170) SHA1(7de74c045bf084659ba70da9458d720125ff25ae), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11759.86",  0x000004, 0x20000, CRC(ac8111f6) SHA1(6412716dc97ae697b438d9c9cd554d1087416bc2), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11760.114", 0x000005, 0x20000, CRC(91282af9) SHA1(fddee7982949b7da724c7830e7bd139aeb84672d), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11775.82",  0x000006, 0x20000, CRC(48225793) SHA1(ee003c2ea24c14e0968da94bac139735660932fe), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11776.110", 0x000007, 0x20000, CRC(78c46198) SHA1(d299e631843da47cb7a46103d52a3dabfab71746), ROM_SKIP(7) )

	ROMX_LOAD( "epr-11761.66",  0x100000, 0x20000, CRC(baa5d065) SHA1(56dc71814e3f0f327781b0c1587038351c60f7b7), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11762.74",  0x100001, 0x20000, CRC(1d1af7a5) SHA1(86c02565b5aca201588c98678fb0c54faa8d4d6b), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11777.62",  0x100002, 0x20000, CRC(9662dd32) SHA1(454ec914b6c936f692bf90d2232c8169acec470a), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11778.70",  0x100003, 0x20000, CRC(2dfb7494) SHA1(4b9f1609e425c5e634e95dbc2d0ca820dd9212bc), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11763.85",  0x100004, 0x20000, CRC(1ee23407) SHA1(776c868e0e4e601fd6d0a83561b064b4be0560e2), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11764.113", 0x100005, 0x20000, CRC(e859305e) SHA1(aafcc3209a4fb6e0e8169ae6cce386b370b824f7), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11779.81",  0x100006, 0x20000, CRC(a49cd793) SHA1(efe77949be39a2ff88b50bfb2b4664b9267d9a09), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11780.109", 0x100007, 0x20000, CRC(d514ed81) SHA1(fbac3ad085363972a79e77aebb7fdae2200e7cda), ROM_SKIP(7) )

	ROMX_LOAD( "epr-11765.65",  0x200000, 0x20000, CRC(649e2dff) SHA1(a6c61b71d08b31a0ca175ab0404e2eaf1d09ccc2), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11766.73",  0x200001, 0x20000, CRC(d92fb7fc) SHA1(2f5c2d88ae0766351b9efe8ffcbebc88fc3a6c59), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11781.61",  0x200002, 0x20000, CRC(9692d4cd) SHA1(967351ba2c781ca865e3c1ee9eeef1aad2247c27), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11782.69",  0x200003, 0x20000, CRC(c913bb43) SHA1(9bc15a3180cf4c3134bb55e99e6092f0faf95c56), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11767.84",  0x200004, 0x20000, CRC(1f8ad054) SHA1(289f5795116ee29540f28e35c3b4f72adeca7891), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11768.112", 0x200005, 0x20000, CRC(db2c4053) SHA1(a5b6daa6deb7afb0019e289acb81c82d507ec93a), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11783.80",  0x200006, 0x20000, CRC(6d189007) SHA1(dd871ea3166fdcb59d49707d35dde8b6c7fdc76b), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11784.108", 0x200007, 0x20000, CRC(57f5fd64) SHA1(6aff54d3f3f76ce0f1a93485d1a35a3987d456d9), ROM_SKIP(7) )

	ROMX_LOAD( "epr-11769.64",  0x300000, 0x20000, CRC(28f0ab51) SHA1(d7cb7b83e5d85eb59d34cfd5c0d8e6c7ff81e24c), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11770.72",  0x300001, 0x20000, CRC(d7557ea9) SHA1(62430505d399ee2cc0f94e03144860056345573c), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11785.60",  0x300002, 0x20000, CRC(e6ef32c4) SHA1(869ba3816f5e3125f613f3b284fec74cd19db79e), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11786.68",  0x300003, 0x20000, CRC(2066b49d) SHA1(905ce70c921043d07591422a87fedd6e897ff38e), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11771.83",  0x300004, 0x20000, CRC(67635618) SHA1(f690ace026130ecb95532c92f2ad3741d0d167c1), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11772.111", 0x300005, 0x20000, CRC(0f798d3a) SHA1(71565ce28b93ae50d64af8c965fba6408a07f031), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11787.79",  0x300006, 0x20000, CRC(e631dc12) SHA1(3fd6db2eb297890b35dec566b6a90fc2d96bd085), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11788.107", 0x300007, 0x20000, CRC(8464c66e) SHA1(af93cbcc50acbd929d0298fb9a75da0369e13ff7), ROM_SKIP(7) )

	ROM_REGION( 0x10000, "soundcpu", 0 )		/* Z80 sound CPU */
	ROM_LOAD( "epr-11899.102",  0x000000, 0x10000, CRC(ed9fa889) SHA1(25d1a069254b34c31d8ee82d301ada895e8dc391) )

	ROM_REGION( 0x200000, "pcm", ROMREGION_ERASEFF )	/* SegaPCM samples */
	ROM_LOAD( "mpr-11754.107",  0x000000, 0x80000, CRC(ebeb8484) SHA1(269f33cb1a9be126bada858e25291385d48686a2) )
	ROM_LOAD( "epr-11755.105",  0x080000, 0x20000, CRC(12e43f8a) SHA1(0f9a11ba6b7c1a352daa1146a01ce147945e91e4) )
	ROM_RELOAD(                 0x0a0000, 0x20000 )
	ROM_RELOAD(                 0x0c0000, 0x20000 )
	ROM_RELOAD(                 0x0e0000, 0x20000 )
	ROM_LOAD( "epr-11756.106",  0x100000, 0x20000, CRC(c2db1244) SHA1(c98fe17c9f04a639a862cc2a86fab17d1f5d025c) )
	ROM_RELOAD(                 0x120000, 0x20000 )
	ROM_RELOAD(                 0x140000, 0x20000 )
	ROM_RELOAD(                 0x160000, 0x20000 )

	ROM_REGION( 0x100000, "user1", 0 )
	/* These are mpr-11754.107 split into 4 roms. They would be located on a Sega 839-0221 daughter card. */
	ROM_LOAD( "epr-11895.ic1", 0x000000, 0x20000, CRC(ee99a6fd) SHA1(4444826e751d9186e6d46b081e47cd99ee3cf853) )
	ROM_LOAD( "epr-11896.ic2", 0x000000, 0x20000, CRC(4bebc015) SHA1(307022ea1c1ee87c9ef3782526888c48c3c69fd2) )
	ROM_LOAD( "epr-11897.ic3", 0x000000, 0x20000, CRC(4463cb95) SHA1(e86fd4611cf83fe72d59950a60fc8c3a7381a1c7) )
	ROM_LOAD( "epr-11898.ic4", 0x000000, 0x20000, CRC(5d19d767) SHA1(d335cd3ef57c75e388df04b04fc3e2881a3902cf) )
ROM_END

/**************************************************************************************************************************
    Power Drift, Sega Y-board
    CPU: 68000 (317-????)
     CPU BD POWER DRIFT   837-6695-08 (or 837-6695-09)
     VIDEO BD POWER DRIFT 837-6696-01 (or 837-6696-02)
*/
ROM_START( pdriftj )
	ROM_REGION( 0x080000, "maincpu", 0 ) // M
	ROM_LOAD16_BYTE( "epr-11746a.25", 0x000000, 0x20000, CRC(b0f1caf4) SHA1(1d7e70e740ef513728a72ff6e7b1c4b78e3cb0d5) )
	ROM_LOAD16_BYTE( "epr-11745a.24", 0x000001, 0x20000, CRC(a89720cd) SHA1(a0393546797f124e1c03bd12945dc6358bb94ba9) )
	ROM_LOAD16_BYTE( "epr-11748.27",  0x040000, 0x20000, CRC(82a76cab) SHA1(f8d3fe059e18896cd0e64711f1a3ee8b6372b4e0) )
	ROM_LOAD16_BYTE( "epr-11747.26",  0x040001, 0x20000, CRC(9796ece5) SHA1(f84f5689c2edc0853ff173ce20f93f89758b2f31) )

	ROM_REGION( 0x040000, "subx", 0 ) // X
	ROM_LOAD16_BYTE( "epr-11752.81",  0x000000, 0x20000, CRC(b6bb8111) SHA1(475ce4e3d92747a9012a0ab03838ece61f6d33e0) )
	ROM_LOAD16_BYTE( "epr-11751.80",  0x000001, 0x20000, CRC(7f0d0311) SHA1(7917be201ff44c6b895fc8e9e296e8b1ecf8d639) )

	ROM_REGION( 0x040000, "suby", 0 ) // Y
	ROM_LOAD16_BYTE( "epr-11750b.54", 0x000000, 0x20000, CRC(bc14ce30) SHA1(9bbadee0946e0abaac4f0d2625ba5550f11fa8a9) )
	ROM_LOAD16_BYTE( "epr-11749b.53", 0x000001, 0x20000, CRC(9e385568) SHA1(74e22eaed645cc80b1eb0c52912186066e58b9d2) )

	ROM_REGION16_BE( 0x080000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "epr-11789.16",  0x000000, 0x20000, CRC(b86f8d2b) SHA1(a053f2021841fd0ef89fd3f28050a698b36c435e) )
	ROM_LOAD16_BYTE( "epr-11791.14",  0x000001, 0x20000, CRC(36b2910a) SHA1(9948b91837f944a7a606542fa685525e74bbe398) )
	ROM_LOAD16_BYTE( "epr-11790.17",  0x040000, 0x20000, CRC(2a564e66) SHA1(5f30fc15bfd017d75cfffe1e9e62ed0bcf32a98e) )
	ROM_LOAD16_BYTE( "epr-11792.15",  0x040001, 0x20000, CRC(c85caf6e) SHA1(2411ea99ec7f6e2b0b4f219e86ff2172539ad2c4) )

	ROM_REGION64_BE( 0x400000, "gfx1", 0)
	ROMX_LOAD( "epr-11757.67",  0x000000, 0x20000, CRC(e46dc478) SHA1(baf79e230aef3d63fb50373b2b1626f7c56ee94f), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11758.75",  0x000001, 0x20000, CRC(5b435c87) SHA1(6b42b08e73957c36cd8faa896ca14461d00afd29), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11773.63",  0x000002, 0x20000, CRC(1b5d5758) SHA1(54f58a274740a0566e0553d145c0c284ffd1d36b), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11774.71",  0x000003, 0x20000, CRC(2ca0c170) SHA1(7de74c045bf084659ba70da9458d720125ff25ae), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11759.86",  0x000004, 0x20000, CRC(ac8111f6) SHA1(6412716dc97ae697b438d9c9cd554d1087416bc2), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11760.114", 0x000005, 0x20000, CRC(91282af9) SHA1(fddee7982949b7da724c7830e7bd139aeb84672d), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11775.82",  0x000006, 0x20000, CRC(48225793) SHA1(ee003c2ea24c14e0968da94bac139735660932fe), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11776.110", 0x000007, 0x20000, CRC(78c46198) SHA1(d299e631843da47cb7a46103d52a3dabfab71746), ROM_SKIP(7) )

	ROMX_LOAD( "epr-11761.66",  0x100000, 0x20000, CRC(baa5d065) SHA1(56dc71814e3f0f327781b0c1587038351c60f7b7), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11762.74",  0x100001, 0x20000, CRC(1d1af7a5) SHA1(86c02565b5aca201588c98678fb0c54faa8d4d6b), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11777.62",  0x100002, 0x20000, CRC(9662dd32) SHA1(454ec914b6c936f692bf90d2232c8169acec470a), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11778.70",  0x100003, 0x20000, CRC(2dfb7494) SHA1(4b9f1609e425c5e634e95dbc2d0ca820dd9212bc), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11763.85",  0x100004, 0x20000, CRC(1ee23407) SHA1(776c868e0e4e601fd6d0a83561b064b4be0560e2), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11764.113", 0x100005, 0x20000, CRC(e859305e) SHA1(aafcc3209a4fb6e0e8169ae6cce386b370b824f7), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11779.81",  0x100006, 0x20000, CRC(a49cd793) SHA1(efe77949be39a2ff88b50bfb2b4664b9267d9a09), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11780.109", 0x100007, 0x20000, CRC(d514ed81) SHA1(fbac3ad085363972a79e77aebb7fdae2200e7cda), ROM_SKIP(7) )

	ROMX_LOAD( "epr-11765.65",  0x200000, 0x20000, CRC(649e2dff) SHA1(a6c61b71d08b31a0ca175ab0404e2eaf1d09ccc2), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11766.73",  0x200001, 0x20000, CRC(d92fb7fc) SHA1(2f5c2d88ae0766351b9efe8ffcbebc88fc3a6c59), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11781.61",  0x200002, 0x20000, CRC(9692d4cd) SHA1(967351ba2c781ca865e3c1ee9eeef1aad2247c27), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11782.69",  0x200003, 0x20000, CRC(c913bb43) SHA1(9bc15a3180cf4c3134bb55e99e6092f0faf95c56), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11767.84",  0x200004, 0x20000, CRC(1f8ad054) SHA1(289f5795116ee29540f28e35c3b4f72adeca7891), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11768.112", 0x200005, 0x20000, CRC(db2c4053) SHA1(a5b6daa6deb7afb0019e289acb81c82d507ec93a), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11783.80",  0x200006, 0x20000, CRC(6d189007) SHA1(dd871ea3166fdcb59d49707d35dde8b6c7fdc76b), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11784.108", 0x200007, 0x20000, CRC(57f5fd64) SHA1(6aff54d3f3f76ce0f1a93485d1a35a3987d456d9), ROM_SKIP(7) )

	ROMX_LOAD( "epr-11769.64",  0x300000, 0x20000, CRC(28f0ab51) SHA1(d7cb7b83e5d85eb59d34cfd5c0d8e6c7ff81e24c), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11770.72",  0x300001, 0x20000, CRC(d7557ea9) SHA1(62430505d399ee2cc0f94e03144860056345573c), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11785.60",  0x300002, 0x20000, CRC(e6ef32c4) SHA1(869ba3816f5e3125f613f3b284fec74cd19db79e), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11786.68",  0x300003, 0x20000, CRC(2066b49d) SHA1(905ce70c921043d07591422a87fedd6e897ff38e), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11771.83",  0x300004, 0x20000, CRC(67635618) SHA1(f690ace026130ecb95532c92f2ad3741d0d167c1), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11772.111", 0x300005, 0x20000, CRC(0f798d3a) SHA1(71565ce28b93ae50d64af8c965fba6408a07f031), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11787.79",  0x300006, 0x20000, CRC(e631dc12) SHA1(3fd6db2eb297890b35dec566b6a90fc2d96bd085), ROM_SKIP(7) )
	ROMX_LOAD( "epr-11788.107", 0x300007, 0x20000, CRC(8464c66e) SHA1(af93cbcc50acbd929d0298fb9a75da0369e13ff7), ROM_SKIP(7) )

	ROM_REGION( 0x10000, "soundcpu", 0 )		/* Z80 sound CPU */
	ROM_LOAD( "epr-11899.102",  0x000000, 0x10000, CRC(ed9fa889) SHA1(25d1a069254b34c31d8ee82d301ada895e8dc391) )

	ROM_REGION( 0x200000, "pcm", ROMREGION_ERASEFF )	/* SegaPCM samples */
	ROM_LOAD( "mpr-11754.107",  0x000000, 0x80000, CRC(ebeb8484) SHA1(269f33cb1a9be126bada858e25291385d48686a2) )
	ROM_LOAD( "epr-11755.105",  0x080000, 0x20000, CRC(12e43f8a) SHA1(0f9a11ba6b7c1a352daa1146a01ce147945e91e4) )
	ROM_RELOAD(                 0x0a0000, 0x20000 )
	ROM_RELOAD(                 0x0c0000, 0x20000 )
	ROM_RELOAD(                 0x0e0000, 0x20000 )
	ROM_LOAD( "epr-11756.106",  0x100000, 0x20000, CRC(c2db1244) SHA1(c98fe17c9f04a639a862cc2a86fab17d1f5d025c) )
	ROM_RELOAD(                 0x120000, 0x20000 )
	ROM_RELOAD(                 0x140000, 0x20000 )
	ROM_RELOAD(                 0x160000, 0x20000 )

	ROM_REGION( 0x100000, "user1", 0 )
	/* These are mpr-11754.107 split into 4 roms. They would be located on a Sega 839-0221 daughter card. */
	ROM_LOAD( "epr-11895.ic1", 0x000000, 0x20000, CRC(ee99a6fd) SHA1(4444826e751d9186e6d46b081e47cd99ee3cf853) )
	ROM_LOAD( "epr-11896.ic2", 0x000000, 0x20000, CRC(4bebc015) SHA1(307022ea1c1ee87c9ef3782526888c48c3c69fd2) )
	ROM_LOAD( "epr-11897.ic3", 0x000000, 0x20000, CRC(4463cb95) SHA1(e86fd4611cf83fe72d59950a60fc8c3a7381a1c7) )
	ROM_LOAD( "epr-11898.ic4", 0x000000, 0x20000, CRC(5d19d767) SHA1(d335cd3ef57c75e388df04b04fc3e2881a3902cf) )
ROM_END


/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Rail Chase World, Sega Y-board
    CPU: 68000 (317-????)
    CPU BD      837-8073-05
    VIDEO BD    837-8074-01
    GAME BD     834-8072-05
*/
ROM_START( rchase )
	ROM_REGION( 0x080000, "maincpu", 0 ) // M
	ROM_LOAD16_BYTE( "epr-13986.25",  0x000000, 0x20000, CRC(388b2365) SHA1(0f006f9120b96b8d8be968878ce1d6dd853cd977) )
	ROM_LOAD16_BYTE( "epr-13985.24",  0x000001, 0x20000, CRC(14dba5d4) SHA1(ad09c55273ab1105630f2f76019aedc234fb9292) )
	ROM_LOAD16_BYTE( "epr-13988.27",  0x040000, 0x20000, CRC(dc1cd5a4) SHA1(3b2b6afbeb7daa7c0cc75279bc495221c2508e25) )
	ROM_LOAD16_BYTE( "epr-13987.26",  0x040001, 0x20000, CRC(43be9e60) SHA1(107a9c126c2bff9030fe621f6b4ab8f29c994ef2) )

	ROM_REGION( 0x040000, "subx", 0 ) // X
	ROM_LOAD16_BYTE( "epr-13992a.81",  0x000000, 0x20000, CRC(c5d525b6) SHA1(1fab7f761be67b67ee346a1af1f1fe12aef87dc5) )
	ROM_LOAD16_BYTE( "epr-13991a.80",  0x000001, 0x20000, CRC(299e3c7c) SHA1(e4903816ec364e9352abd1180e8a609fed75e1a7) )

	ROM_REGION( 0x040000, "suby", 0 ) // Y
	ROM_LOAD16_BYTE( "epr-14092.54",  0x000000, 0x20000, CRC(18eb23c5) SHA1(53e5681c7450a3879ed80c1680168d6295caa887) )
	ROM_LOAD16_BYTE( "epr-14091.53",  0x000001, 0x20000, CRC(72a56f71) SHA1(d45d3072ea92b5dde5c70138e56e7f0ca248880e) )

	ROM_REGION16_BE( 0x080000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "mpr-13999.16", 0x000000, 0x40000, CRC(9a1dd53c) SHA1(cb01f2c64554914ea693879dfcb498181a1e7a9a) )
	ROM_LOAD16_BYTE( "mpr-13997.14", 0x000001, 0x40000, CRC(1fdf1b87) SHA1(ed46af0f72081d545015b73a8d12240664f29506) )

	ROM_REGION64_BE( 0xc00000, "gfx1", 0)
	ROMX_LOAD( "mpr-14021.67",  0x000000, 0x80000, CRC(9fa88781) SHA1(a035fd0fe1d37a589adf3a5029c20d237d5cc827), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14022.75",  0x000001, 0x80000, CRC(49e824bb) SHA1(c1330719b5718aa664b5788244d8cb7b7103a57c), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14009.63",  0x000002, 0x80000, CRC(35b5187e) SHA1(6f0f6471c4135d07a2c852cdc50322b99176712e), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14010.71",  0x000003, 0x80000, CRC(9a538b9b) SHA1(cd84a39bd3858fa6c1d8eb4a349d939261cea6b6), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14023.86",  0x000004, 0x80000, CRC(e11c6c67) SHA1(839e71690e75e47d11b758f5b525452bcc75b823), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14024.114", 0x000005, 0x80000, CRC(16344535) SHA1(a9bd101ae93c24a2e8002ad6a111cf0d0d3b1a64), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14011.82",  0x000006, 0x80000, CRC(78e9983b) SHA1(c0f6577b55acda2cc8cdf0884d5c0517f79de4e9), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14012.110", 0x000007, 0x80000, CRC(e9daa1a4) SHA1(24ad782e88a586fbb31f7ad86be4cdeb38823102), ROM_SKIP(7) )

	ROMX_LOAD( "mpr-14017.66",  0x400000, 0x80000, CRC(b83df159) SHA1(f0cf99e6ddae1d26fd68240a731f3e28e9c6073b), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14018.74",  0x400001, 0x80000, CRC(76dbe9ce) SHA1(2f5af8d015cf8fb90a0862bc37235bc20d4dac0d), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14005.62",  0x400002, 0x80000, CRC(9e998209) SHA1(c0d39d11d554fd6a43db77ccf96ac04dd634edff), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14006.70",  0x400003, 0x80000, CRC(2caddf1a) SHA1(a2e891e65c7cd156a3131a084ff51ae9b1663bc0), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14019.85",  0x400004, 0x80000, CRC(b15e19ff) SHA1(947c35301875b4842835f2ba6aca216f087a3fc7), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14020.113", 0x400005, 0x80000, CRC(84c7008f) SHA1(b92f3c636c5d91c5b1c6090a48be7bb1be6b927e), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14007.81",  0x400006, 0x80000, CRC(c3cf5faa) SHA1(02bca1b248fcb6313cd529ca2aa0f7516177166b), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14008.109", 0x400007, 0x80000, CRC(7e91beb2) SHA1(bc1a7ce68d6b825daf93ca437dda803857cee0a2), ROM_SKIP(7) )

	ROMX_LOAD( "mpr-14013.65",  0x800000, 0x80000, CRC(31dbb2c3) SHA1(3efeff785d78056e2615dc2267f7bb80a6d4c663), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14014.73",  0x800001, 0x80000, CRC(7e68257d) SHA1(600d1066cfa83f5df46e240a473a8f04179e70f8), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14001.61",  0x800002, 0x80000, CRC(71031ad0) SHA1(02b6240461d66907199f846310e72d37faa5fb50), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14002.69",  0x800003, 0x80000, CRC(27e70a5e) SHA1(fa767f6cc8e46c0e804c37666a8499376c09b025), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14015.84",  0x800004, 0x80000, CRC(7540bf85) SHA1(e8f9208aea6ecedb6ae810a362476cdf1d424319), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14016.112", 0x800005, 0x80000, CRC(7d87b94d) SHA1(4ef5b7b114c25b9e274e3f3dd7e3d7bd29d2d8b9), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14003.80",  0x800006, 0x80000, CRC(87725d74) SHA1(d284512ad15362a886072aaa1a3af98f7a0bddf9), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14004.108", 0x800007, 0x80000, CRC(73477291) SHA1(1fe9d7666d89ee55a0178dceb7cfea7ce94b9e18), ROM_SKIP(7) )

	ROM_REGION( 0x10000, "soundcpu", 0 )		/* Z80 sound CPU */
	ROM_LOAD( "epr-13993.102", 0x000000, 0x10000,  CRC(7cc3b543) SHA1(c5e6a2dca891d0b6528e6d66ccd18b24ed4a9464) )

	ROM_REGION( 0x200000, "pcm", ROMREGION_ERASEFF )	/* SegaPCM samples */
	ROM_LOAD( "mpr-13996.107", 0x000000, 0x80000, CRC(345f5a41) SHA1(d414c3485ba31863c2b36282756709e06a41d262) )
	ROM_LOAD( "mpr-13995.106", 0x080000, 0x80000, CRC(f604c270) SHA1(02023786fec2f2702c2f19f51aff5b7e4928ae91) )
	ROM_LOAD( "mpr-13994.105", 0x100000, 0x80000, CRC(76095538) SHA1(aab830e3675116c475fe69e0e991118c045b131b) )
ROM_END

/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Rail Chase Japan, Sega Y-board
    CPU: 68000 (317-????)
*/

ROM_START( rchasej )
	ROM_REGION( 0x080000, "maincpu", 0 ) // M
	ROM_LOAD16_BYTE( "epr-13986.25",  0x000000, 0x20000, CRC(388b2365) SHA1(0f006f9120b96b8d8be968878ce1d6dd853cd977) )
	ROM_LOAD16_BYTE( "epr-13985.24",  0x000001, 0x20000, CRC(14dba5d4) SHA1(ad09c55273ab1105630f2f76019aedc234fb9292) )
	ROM_LOAD16_BYTE( "epr-13988.27",  0x040000, 0x20000, CRC(dc1cd5a4) SHA1(3b2b6afbeb7daa7c0cc75279bc495221c2508e25) )
	ROM_LOAD16_BYTE( "epr-13987.26",  0x040001, 0x20000, CRC(43be9e60) SHA1(107a9c126c2bff9030fe621f6b4ab8f29c994ef2) )

	ROM_REGION( 0x040000, "subx", 0 ) // X
	ROM_LOAD16_BYTE( "epr-13992a.81",  0x000000, 0x20000, CRC(c5d525b6) SHA1(1fab7f761be67b67ee346a1af1f1fe12aef87dc5) )
	ROM_LOAD16_BYTE( "epr-13991a.80",  0x000001, 0x20000, CRC(299e3c7c) SHA1(e4903816ec364e9352abd1180e8a609fed75e1a7) )

	ROM_REGION( 0x040000, "suby", 0 ) // Y
	ROM_LOAD16_BYTE( "epr-13990.54.verify",  0x000000, 0x20000, CRC(18eb23c5) SHA1(53e5681c7450a3879ed80c1680168d6295caa887) ) /* Need to verify EPR #, same as epr-14092.54 above */
	ROM_LOAD16_BYTE( "epr-13989.53.verify",  0x000001, 0x20000, CRC(8f4f824e) SHA1(d470f23ce2dca4e75b7b714175d47338c41bb721) ) /* Need to verify EPR # */

	ROM_REGION16_BE( 0x080000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "mpr-13999.16", 0x000000, 0x40000, CRC(9a1dd53c) SHA1(cb01f2c64554914ea693879dfcb498181a1e7a9a) )
	ROM_LOAD16_BYTE( "mpr-13997.14", 0x000001, 0x40000, CRC(1fdf1b87) SHA1(ed46af0f72081d545015b73a8d12240664f29506) )

	ROM_REGION64_BE( 0xc00000, "gfx1", 0)
	ROMX_LOAD( "mpr-14021.67",  0x000000, 0x80000, CRC(9fa88781) SHA1(a035fd0fe1d37a589adf3a5029c20d237d5cc827), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14022.75",  0x000001, 0x80000, CRC(49e824bb) SHA1(c1330719b5718aa664b5788244d8cb7b7103a57c), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14009.63",  0x000002, 0x80000, CRC(35b5187e) SHA1(6f0f6471c4135d07a2c852cdc50322b99176712e), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14010.71",  0x000003, 0x80000, CRC(9a538b9b) SHA1(cd84a39bd3858fa6c1d8eb4a349d939261cea6b6), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14023.86",  0x000004, 0x80000, CRC(e11c6c67) SHA1(839e71690e75e47d11b758f5b525452bcc75b823), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14024.114", 0x000005, 0x80000, CRC(16344535) SHA1(a9bd101ae93c24a2e8002ad6a111cf0d0d3b1a64), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14011.82",  0x000006, 0x80000, CRC(78e9983b) SHA1(c0f6577b55acda2cc8cdf0884d5c0517f79de4e9), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14012.110", 0x000007, 0x80000, CRC(e9daa1a4) SHA1(24ad782e88a586fbb31f7ad86be4cdeb38823102), ROM_SKIP(7) )

	ROMX_LOAD( "mpr-14017.66",  0x400000, 0x80000, CRC(b83df159) SHA1(f0cf99e6ddae1d26fd68240a731f3e28e9c6073b), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14018.74",  0x400001, 0x80000, CRC(76dbe9ce) SHA1(2f5af8d015cf8fb90a0862bc37235bc20d4dac0d), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14005.62",  0x400002, 0x80000, CRC(9e998209) SHA1(c0d39d11d554fd6a43db77ccf96ac04dd634edff), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14006.70",  0x400003, 0x80000, CRC(2caddf1a) SHA1(a2e891e65c7cd156a3131a084ff51ae9b1663bc0), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14019.85",  0x400004, 0x80000, CRC(b15e19ff) SHA1(947c35301875b4842835f2ba6aca216f087a3fc7), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14020.113", 0x400005, 0x80000, CRC(84c7008f) SHA1(b92f3c636c5d91c5b1c6090a48be7bb1be6b927e), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14007.81",  0x400006, 0x80000, CRC(c3cf5faa) SHA1(02bca1b248fcb6313cd529ca2aa0f7516177166b), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14008.109", 0x400007, 0x80000, CRC(7e91beb2) SHA1(bc1a7ce68d6b825daf93ca437dda803857cee0a2), ROM_SKIP(7) )

	ROMX_LOAD( "mpr-14013.65",  0x800000, 0x80000, CRC(31dbb2c3) SHA1(3efeff785d78056e2615dc2267f7bb80a6d4c663), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14014.73",  0x800001, 0x80000, CRC(7e68257d) SHA1(600d1066cfa83f5df46e240a473a8f04179e70f8), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14001.61",  0x800002, 0x80000, CRC(71031ad0) SHA1(02b6240461d66907199f846310e72d37faa5fb50), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14002.69",  0x800003, 0x80000, CRC(27e70a5e) SHA1(fa767f6cc8e46c0e804c37666a8499376c09b025), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14015.84",  0x800004, 0x80000, CRC(7540bf85) SHA1(e8f9208aea6ecedb6ae810a362476cdf1d424319), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14016.112", 0x800005, 0x80000, CRC(7d87b94d) SHA1(4ef5b7b114c25b9e274e3f3dd7e3d7bd29d2d8b9), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14003.80",  0x800006, 0x80000, CRC(87725d74) SHA1(d284512ad15362a886072aaa1a3af98f7a0bddf9), ROM_SKIP(7) )
	ROMX_LOAD( "mpr-14004.108", 0x800007, 0x80000, CRC(73477291) SHA1(1fe9d7666d89ee55a0178dceb7cfea7ce94b9e18), ROM_SKIP(7) )

	ROM_REGION( 0x10000, "soundcpu", 0 )		/* Z80 sound CPU */
	ROM_LOAD( "epr-13993.102", 0x000000, 0x10000,  CRC(7cc3b543) SHA1(c5e6a2dca891d0b6528e6d66ccd18b24ed4a9464) )

	ROM_REGION( 0x200000, "pcm", ROMREGION_ERASEFF )	/* SegaPCM samples */
	ROM_LOAD( "mpr-13996.107", 0x000000, 0x80000, CRC(345f5a41) SHA1(d414c3485ba31863c2b36282756709e06a41d262) )
	ROM_LOAD( "mpr-13995.106", 0x080000, 0x80000, CRC(f604c270) SHA1(02023786fec2f2702c2f19f51aff5b7e4928ae91) )
	ROM_LOAD( "mpr-13994.105", 0x100000, 0x80000, CRC(76095538) SHA1(aab830e3675116c475fe69e0e991118c045b131b) )
ROM_END



/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Strike Fighter, Sega Y-board
    CPU: 68000 (317-????)
*/
ROM_START( strkfgtr )
	ROM_REGION( 0x080000, "maincpu", 0 ) // M
	ROM_LOAD16_BYTE( "epr-13824.25",  0x000000, 0x20000, CRC(2cf2610c) SHA1(451511842ef61a0404094ecab4f9dc548790d06f) )
	ROM_LOAD16_BYTE( "epr-13823.24",  0x000001, 0x20000, CRC(2c98242f) SHA1(1b1dab94b3a4842c7fc2f1d1869771341b5f840a) )
	ROM_LOAD16_BYTE( "epr-13826.27",  0x040000, 0x20000, CRC(3d34ea55) SHA1(7c492e479f6ca41af70616639b7391d111003efe) )
	ROM_LOAD16_BYTE( "epr-13825.26",  0x040001, 0x20000, CRC(fe218d83) SHA1(8e33c966a8148df3c3dab76ae3ea187b5ada2155) )

	ROM_REGION( 0x040000, "subx", 0 ) // X
	ROM_LOAD16_BYTE( "epr-13830.81",  0x000000, 0x20000, CRC(f9adc9d1) SHA1(63f24ac5345ce1cc4e67193eb01737c60f747d9f) )
	ROM_LOAD16_BYTE( "epr-13829.80",  0x000001, 0x20000, CRC(c5cd85dd) SHA1(34c3033dca7fe8c77dbf23ad01684967b26f0284) )

	ROM_REGION( 0x040000, "suby", 0 ) // Y
	ROM_LOAD16_BYTE( "epr-13828.54",  0x000000, 0x20000, CRC(2470cf5f) SHA1(eb1a732228fe7ad9cf0747d2b53e391c5a733667) )
	ROM_LOAD16_BYTE( "epr-13827.53",  0x000001, 0x20000, CRC(a9d0cf7d) SHA1(c40c73c9e9105ed6503b77b65a6423a26057d810) )

	ROM_REGION16_BE( 0x200000, "gfx2", 0)
	ROM_LOAD16_BYTE( "epr-13833.16",  0x000000, 0x80000, CRC(6148e11a) SHA1(5802208cf0415f6af39de162e9f12e7c205915f7) )
	ROM_LOAD16_BYTE( "epr-13832.14",  0x000001, 0x80000, CRC(41679754) SHA1(58d46f33a4318bbc9e2a20eb5550a66ee0b2e62f) )
	ROM_LOAD16_BYTE( "epr-13040.17",  0x100000, 0x80000, CRC(4aeb3a85) SHA1(5521fd2d3956839bdbe7b70a9e60cd9fb72a42f1) )
	ROM_LOAD16_BYTE( "epr-13038.15",  0x100001, 0x80000, CRC(0b2edb6d) SHA1(04944d6e6f020cd6d33641110847706516630227) )

	ROM_REGION64_BE( 0x1000000, "gfx1", 0 )
	ROMX_LOAD( "epr-13048.67",  0x000000, 0x80000, CRC(fe1eb0dd) SHA1(5e292fc0b83505eb289e026d4be24c9038ef1418), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13056.75",  0x000001, 0x80000, CRC(5904f8e6) SHA1(fbb01dadc796624c360d44b7631e3f1f285abf2e), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13044.63",  0x000002, 0x80000, CRC(4d931f89) SHA1(ff603f4347e4728a2849d9f480893ad0af7abc5c), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13052.71",  0x000003, 0x80000, CRC(0291f040) SHA1(610dee2a31445f4a054111b7005278560a9c0702), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13064.86",  0x000004, 0x80000, CRC(5f8e651b) SHA1(f1a957e68dea40c23f6a5a208358ec6d6515fe60), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13072.114", 0x000005, 0x80000, CRC(6b85641a) SHA1(143a4684d5f303cd30880a2d5728dccbdd168da4), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13060.82",  0x000006, 0x80000, CRC(ee16ad97) SHA1(6af38cfaf694f686f8e4223fb0b13cd350a8b9e5), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13068.110", 0x000007, 0x80000, CRC(64d52bbb) SHA1(b6eab546edb2443e5da6c94ec811ec5084212e60), ROM_SKIP(7) )

	ROMX_LOAD( "epr-13047.66",  0x400000, 0x80000, CRC(53340832) SHA1(8ece8a71ea8ed80458121622307a137fb13931f6), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13055.74",  0x400001, 0x80000, CRC(39b6b665) SHA1(d915db1d9bfe0c6ad3f7b447ce0cfdb42ec66ffe), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13043.62",  0x400002, 0x80000, CRC(208f16fd) SHA1(ce96708ea9886af4aba8730cbb98c0ca72b96f57), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13051.70",  0x400003, 0x80000, CRC(ad62cbd4) SHA1(09c008ce5cb97575a4312d2f22566bda72ecc4e2), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13063.85",  0x400004, 0x80000, CRC(c580bf6d) SHA1(cb72970377ad2acce499059aa8155711b8da8a11), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13071.113", 0x400005, 0x80000, CRC(df99ef99) SHA1(12648844c6e78dbd573b7bf0c981edb4d3012b58), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13059.81",  0x400006, 0x80000, CRC(4c982558) SHA1(e04902af2740ca098cd6bbf1f57cb25562754a76), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13067.109", 0x400007, 0x80000, CRC(f97f6119) SHA1(6f91fc28a1260ca4f1c695863717b27d1e45dc32), ROM_SKIP(7) )

	ROMX_LOAD( "epr-13046.65",  0x800000, 0x80000, CRC(c75a86e9) SHA1(8a180e1e2dd06eb81e2aa4ef73b83879cf6afc1b), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13054.73",  0x800001, 0x80000, CRC(2934549a) SHA1(058b2966141d0db6bb8557d65c77b3458aca9358), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13042.61",  0x800002, 0x80000, CRC(53ed97af) SHA1(22dffa434eb98e5bca1e429b69553a3540dc54a7), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13050.69",  0x800003, 0x80000, CRC(04429068) SHA1(d7d8738809fd959ed428796b2bd1b589b74522c6), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13062.84",  0x800004, 0x80000, CRC(4fdb4ee3) SHA1(d76065b9abe5c3cf692567d3a8746a231748340d), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13070.112", 0x800005, 0x80000, CRC(52ea130e) SHA1(860cb3a1701066e595518c49b696b7b7a3994ada), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13058.80",  0x800006, 0x80000, CRC(19ff1626) SHA1(029e231c3322467b5e2e52eea11df4f645460468), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13066.108", 0x800007, 0x80000, CRC(bc70a250) SHA1(25189854cc01855b6e3589b85490f30dda029f86), ROM_SKIP(7) )

	ROMX_LOAD( "epr-13045.64",  0xc00000, 0x80000, CRC(54d5bc6d) SHA1(18a301c9e6c4a352f300a438d85c6e6952bf0738), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13053.72",  0xc00001, 0x80000, CRC(9502af13) SHA1(1a8c0fcd10f4c86af69c0107f486ca2eb8863f93), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13041.60",  0xc00002, 0x80000, CRC(d0a7402c) SHA1(8932503c570ec49fdb4706f4015608bd060bafa0), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13049.68",  0xc00003, 0x80000, CRC(5b9c0b6c) SHA1(17f2460b7dc0bd34dca3f90f2b553df4a7149147), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13061.83",  0xc00004, 0x80000, CRC(7b95ec3b) SHA1(284aba4effd9d376a7a8f510a6f675fcb3393d09), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13069.111", 0xc00005, 0x80000, CRC(e1f538f0) SHA1(55dc85faed1d5a7f2d586bac7e524c3fef3c53b4), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13057.79",  0xc00006, 0x80000, CRC(73baefee) SHA1(6e86edc8229dd6112034a7df79f7341a4120dc6b), ROM_SKIP(7) )
	ROMX_LOAD( "epr-13065.107", 0xc00007, 0x80000, CRC(8937a655) SHA1(d38726a8a6fe68a002ac8d17f70ab83c2f814aa2), ROM_SKIP(7) )

	ROM_REGION( 0x10000, "soundcpu", 0 )		/* Z80 sound CPU */
	ROM_LOAD( "epr-13831.102", 0x000000, 0x10000, CRC(dabbcea1) SHA1(79dab7d4a0f80a6acc05672a422ace9c487e26ae) )

	ROM_REGION( 0x200000, "pcm", ROMREGION_ERASEFF )	/* SegaPCM samples */
	ROM_LOAD( "mpr-13036.107", 0x000000, 0x80000, CRC(7890c26c) SHA1(97e0678bb571de5cf732804f8909e5cbd24980f1) )
	ROM_LOAD( "mpr-13035.106", 0x080000, 0x80000, CRC(009fa13e) SHA1(c7b224b471696b12332fc7c403c127b19c297df7) )
	ROM_LOAD( "mpr-13034.105", 0x100000, 0x80000, CRC(cd22d95d) SHA1(857aa320df0b3fb44fc8a5526ba5ee82cc74fe63) )
ROM_END



/*************************************
 *
 *  Output callbacks
 *
 *  TODO: kokoroj2 and jpark (SW2)
 *
 *  Additional notes:
 *    - about jpark: the compression switch is broken/inoperative
 *      and because of that all piston data, which is in this
 *      section is frozen. bits x01, x04 and x10 when which == 0
 *      (IO chip 0), seem to have something to do with the sensor
 *      switches we need to fix
 *************************************/

static void pdrift_output_cb1( UINT16 data )
{
	/* Note:  this is an approximation to get a relatively accurate bank value.  It is obviously not 100% */

	/* for some stupid reason the data is set to all on in the debug menu so we need to check for that */
	if (data != 255)
	{
		/* this is a cheap hack to get some usable positional data in the service menu */
		/* these values probably manually turn the motor left and right */
		if (((data == 162) || (data == 161) || (data == 160)))
		{
			if ((data == 162))
			/* moving left */
			{
				/* in this rare instance, the bottom bits are used for positional data */
				output_set_value("bank_data_raw", data);
				output_set_value("vibration_motor", 0);
				switch (pdrift_bank)
				/* we want to go left one step at a time */
				{
					case 1:
						/* all left */
						output_set_value("bank_motor_position", 1);
						pdrift_bank = 1;
						break;
					case 2:
						output_set_value("bank_motor_position", 1);
						pdrift_bank = 1;
						break;
					case 3:
						output_set_value("bank_motor_position", 2);
						pdrift_bank = 2;
						break;
					case 4:
						/* centered */
						output_set_value("bank_motor_position", 3);
						pdrift_bank = 3;
						break;
					case 5:
						output_set_value("bank_motor_position", 4);
						pdrift_bank = 4;
						break;
					case 6:
						output_set_value("bank_motor_position", 5);
						pdrift_bank = 5;
						break;
					case 7:
						/* all right */
						output_set_value("bank_motor_position", 6);
						pdrift_bank = 6;
						break;
					default:
						output_set_value("bank_motor_position", 4);
						pdrift_bank = 4;
						break;

				}
			}

			if ((data == 161))
			/* moving right */
			{
				/* in this rare instance, the bottom bits are used for positional data */
				output_set_value("bank_data_raw", data);
				output_set_value("vibration_motor", 0);
				switch (pdrift_bank)
				/* we want to go right one step at a time */
				{
					case 1:
						/* all left */
						output_set_value("bank_motor_position", 2);
						pdrift_bank = 2;
						break;
					case 2:
						output_set_value("bank_motor_position", 3);
						pdrift_bank = 3;
						break;
					case 3:
						output_set_value("bank_motor_position", 4);
						pdrift_bank = 4;
						break;
					case 4:
						/* centered */
						output_set_value("bank_motor_position", 5);
						pdrift_bank = 5;
						break;
					case 5:
						output_set_value("bank_motor_position", 6);
						pdrift_bank = 6;
						break;
					case 6:
						output_set_value("bank_motor_position", 7);
						pdrift_bank = 7;
						break;
					case 7:
						/* all right */
						output_set_value("bank_motor_position", 7);
						pdrift_bank = 7;
						break;
					default:
						output_set_value("bank_motor_position", 4);
						pdrift_bank = 4;
						break;

				}

			}
		}
		else
		{
			/* the vibration value uses the first few bits to give a number between 0 and 7 */
			output_set_value("vibration_motor", data & 7);
			/* normalize the data and subtract the vibration value from it*/

			pdrift_bank = (data - (data & 7));
			output_set_value("bank_data_raw", pdrift_bank);

			/* position values from left to right */
			/* 56 48 40 120 72 80 88 */

			/* the normalized values we'll use */
			/*   1  2  3   4  5  6  7 */

			switch (pdrift_bank)
			{
				case 56:
					/* all left */
					output_set_value("bank_motor_position", 1);
					break;
				case 48:
					output_set_value("bank_motor_position", 2);
					break;
				case 40:
					output_set_value("bank_motor_position", 3);
					break;
				case 120:
					/* centered */
					output_set_value("bank_motor_position", 4);
					break;
				case 72:
					output_set_value("bank_motor_position", 5);
					break;
				case 80:
					output_set_value("bank_motor_position", 6);
					break;
				case 88:
					/* all right */
					output_set_value("bank_motor_position", 7);
					break;
					/* these are the only valid values but 24 pops up sometimes when we crash */

			}
		}
	}
}

static void gloc_output_cb1( UINT16 data )
{
	if ((data < 32))
	{
		output_set_value("right_motor_position", data);

		/* normalization here prevents strange data from being transferred */
		/* we do this because for some odd reason */
		/* gloc starts with one piston all up and one all down.... at least data-wise it does */
		if ((data > 1) && (data < 29))
			output_set_value("right_motor_position_nor", data);
	}

	if ((data < 40) && (data > 31))
		output_set_value("right_motor_speed", data - 32);

	if ((data < 96) && (data > 63))
	{
		output_set_value("left_motor_position", data);
		/* normalized version... you know... for the kids */
		if (((data - 64) > 1) && ((data - 64) < 29))
			output_set_value("left_motor_position_nor", data - 64);
	}

	if ((data < 104) && (data > 95))
		output_set_value("left_motor_speed", data - 96);
}

static void pdrift_output_cb2( UINT16 data )
{
	output_set_value("start_lamp", BIT(data, 2));
	output_set_value("upright_wheel_motor", BIT(data, 1));
}

static void gforce2_output_cb2( UINT16 data )
{
	output_set_value("start_lamp", BIT(data, 2));
}

static void gloc_output_cb2( UINT16 data )
{
	output_set_value("start_lamp", BIT(data, 2));
	output_set_value("danger_lamp", BIT(data, 5));
	output_set_value("crash_lamp", BIT(data, 6));
}

static void r360_output_cb2( UINT16 data )
{
	/* r360 cabinet */
	output_set_value("start_lamp", BIT(data, 2));
	/* even though the same ouput is used, I've split them to avoid confusion. */
	output_set_value("emergency_stop_lamp", BIT(data, 2));
}

static void rchase_output_cb2( UINT16 data )
{
	output_set_value("left_start_lamp", BIT(data, 2));
	output_set_value("right_start_lamp", BIT(data, 1));

	output_set_value("P1_Gun_Recoil", BIT(data, 6));
	output_set_value("P2_Gun_Recoil", BIT(data, 5));
}

/*************************************
 *
 *  Game-Specific driver initialization
 *
 *************************************/

static DRIVER_INIT( gforce2 )
{
	yboard_generic_init(machine);
	ybd_output_cb2 = gforce2_output_cb2;
}

static DRIVER_INIT( pdrift )
{
	/* because some of the output data isn't fully understood we need to "center" the motor */
	yboard_generic_init(machine);

	ybd_output_cb1 = pdrift_output_cb1;
	ybd_output_cb2 = pdrift_output_cb2;
}

static DRIVER_INIT( gloc )
{
	/* because some of the output data isn't fully understood we need to "center" the rams */
	output_set_value("left_motor_position_nor", 16);
	output_set_value("right_motor_position_nor", 16);
	yboard_generic_init(machine);

	ybd_output_cb1 = gloc_output_cb1;
	ybd_output_cb2 = gloc_output_cb2;
}

static DRIVER_INIT( r360 )
{
	yboard_generic_init(machine);
	ybd_output_cb2 = r360_output_cb2;
}

static DRIVER_INIT( rchase )
{
	yboard_generic_init(machine);
	ybd_output_cb2 = rchase_output_cb2;
}

/*************************************
 *
 *  Game driver(s)
 *
 *************************************/

GAME( 1988, gforce2,  0,       yboard, gforce2,  gforce2, ROT0, "Sega", "Galaxy Force 2" , GAME_SUPPORTS_SAVE )
GAME( 1988, gforce2j, gforce2, yboard, gforce2,  gforce2, ROT0, "Sega", "Galaxy Force 2 (Japan)" , GAME_SUPPORTS_SAVE )
GAME( 1990, gloc,     0,       yboard, gloc,     gloc, ROT0, "Sega", "G-LOC Air Battle (US)" , GAME_SUPPORTS_SAVE )
GAME( 1990, glocr360, gloc,    yboard, glocr360, r360, ROT0, "Sega", "G-LOC R360", GAME_SUPPORTS_SAVE )
GAMEL(1988, pdrift,   0,       yboard, pdrift,   pdrift, ROT0, "Sega", "Power Drift (World, Rev A)", GAME_SUPPORTS_SAVE, layout_pdrift )
GAMEL(1988, pdrifta,  pdrift,  yboard, pdrift,   pdrift, ROT0, "Sega", "Power Drift (World)", GAME_SUPPORTS_SAVE, layout_pdrift )
GAMEL(1988, pdrifte,  pdrift,  yboard, pdrifte,  pdrift, ROT0, "Sega", "Power Drift (World, Earlier)", GAME_SUPPORTS_SAVE, layout_pdrift )
GAMEL(1988, pdriftj,  pdrift,  yboard, pdriftj,  pdrift, ROT0, "Sega", "Power Drift (Japan)", GAME_SUPPORTS_SAVE, layout_pdrift )
GAME( 1991, rchase,   0,       yboard, rchase,   rchase, ROT0, "Sega", "Rail Chase (World)", GAME_SUPPORTS_SAVE )
GAME( 1991, rchasej,  rchase,  yboard, rchase,   rchase, ROT0, "Sega", "Rail Chase (Japan)", GAME_SUPPORTS_SAVE )
GAME( 1991, strkfgtr, 0,       yboard, strkfgtr, gloc, ROT0, "Sega", "Strike Fighter (Japan)", GAME_SUPPORTS_SAVE )
