/***************************************************************************
    Sega Out Run hardware
****************************************************************************

    Known bugs:
        * LED connected to stop lights no longer working

    To do for each game:
        * verify analog input min/max
        * verify protection

--

Guru's Outrun & Super Hang On Board Notes:

Outrun & Super Hang On
Sega, 1986, 1987

PCB Layouts
-----------

CPU Board: The CPU board used on Outrun and Super Hang On is identical. Only the ROMs are changed,
all other chips including PALs/PLDs and custom chips are the same.
The non-encrypted versions are documented below as they are easy to repair and that version is used to
resurrect dead FD1089/1094 versions :-)

171-5376-01
837-6063-01 SEGA 1986
Sticker: 837-6278-02 (for Super Hang On)
Sticker: 834-6277-02 SUPER HANG ON
Sticker: 834-6065-02 OUT RUN
Sticker: 834-6065-04 OUT RUN (REV B)
|-----------------------------------------------------------------------------------|
|                        IC71   TMM2115                      J     5.5V_0.1F        |
|                               TMM2115       16MHz                                 |
|                        IC70     |--------|          MF6CN-50                    N |
|                                 |315-5218|          MF6CN-50   4066  LM324        |
|                        IC69     |        |                                        |
|                                 |--------|          MP7633JN                    P |
|                        IC68                                                       |
|                                          Z80A              TL084               LED|
|                        IC67              IC88    315-5224           |-----------| |
|                                       TMM2115              YM3012   |SEGA       | |
|                        IC66                      YM2151             |315-5195   | |
| 315-5222 315-5155 315-5155            40MHz      MB3771             |           | |
| IC11          IC47                                                  |           | |
|                                                                     |-----------| |
|                           315-5225 315-5226                                       |
|                           |---------------------|         |---------------------| |
|          315-5223A        |        68000        |         |        68000        | |
|                           |---------------------|         |---------------------| |
|                                                                                   |
|                                                                                   |
|                                                                                   |
|                                                                                   |
|                               IC58        IC76                IC118       IC133   |
|                                                                                   |
|                               IC57        IC75                IC117       IC132   |
|           TMM2115   TMM2115                                                       |
|                               IC56        IC74                IC116       IC131   |
|           TMM2115   TMM2115                                                       |
|                               TMM2063     TMM2063             TMM2063     TMM2063 |
|                                                                                   |
|                               TMM2063     TMM2063             TMM2062     TMM2063 |
|                                                                                   |
|                                                                                   |
|         A                         B                                 C             |
|-----------------------------------------------------------------------------------|
Notes:
      68000     - Clock Input 10.000MHz [40/4]
      Z80A      - Clock Input 4.000MHz [16/4]
      YM2151    - Yamaha YM2151 FM Operator Type M (OPM) Sound Generator IC. Clock Input 4.000MHz [16/4]
      YM3012    - Yamaha YM3012 2-Channel Serial Input Floating Point Digital to Analog Convertor (DIP16)
      TMM2063   - Toshiba TMM2063 8kx8 SRAM (NDIP28)
      TMM2115   - Toshiba TMM2115 2kx8 SRAM (NDIP24)
      TL084     - Texas Instruments TL084 Quad JFET-Input General-Purpose Operational Amplifier (DIP14)
      LM324     - National Semiconductor LM324 Low Power Quad Operational Amplifier (DIP14)
      MP7633JN  - Exar Corporation MP7633JN 15V CMOS 10-Bit Multiplying Digital-to-Analog Converter (DIP16)
                  Also equivalent to National Semiconductor DAC1022LCN and AD7520JN / AD7530JN
      MF6CN-50  - National Semiconductor MF6CN-50 6th Order Switched Capacitor Butterworth Lowpass Filter (DIP14)
      5.5V_0.1F - 0.1 Farad Super Cap for Capacitor Backed RAM
      4066      - NEC D74HC4066 Quad Bilateral Switch (DIP14)
      MB3771    - Fujitsu MB3771 Master Reset IC (DIP8)
      J         - 10 Pin Connector for 5V Input and GND
      N         - 6 Pin Connector for Unamplified Stereo Sound Output
      P         - 4 Pin Connector (not used)
      A/B/C     - 50 Pin Connectors (x3) for joining CPU Board to Video Board
      315-5155  - Sega custom PAL (Road Bit Extraction) (DIP20)
      315-5195  - Sega Memory Mapper IC (in PGA package)
      315-5218  - Sega PCM Sound Controller IC (QFP100).
                  Clock input 16.000MHz on pin 80
                  Clock outputs: pin 2 - 4.000MHz, pin 80 - 500.000kHz, pin 89 - 62.500KHz
      315-5222  - Signetics PLS153N (Road Mixing) (DIP20)
      315-5223A - Signetics CK2605 (DIP20)
      315-5224  - Signetics CK2605 (DIP20)
      315-5225  - MMI PAL16R4 (DIP20)
      315-5226  - MMI PAL16R4 (DIP20)

      Measurements
      ------------
      OSC1 - 39.99967MHz
      OSC2 - 16.00019MHz


      ROMs (EPR/MPR)
      ----
                     IC88  IC66  IC67  IC68  IC69  IC70  IC71  IC47  IC11  IC58  IC57  IC56  IC76  IC75  IC74  IC118  IC117  IC116  IC133  IC132  IC131
      -------------------------------------------------------------------------------------------------------------------------------------------------
      Out Run        10187 10193 10192 10191 10190 10189 10188 10186 10185 10329 10330 -     10327 10328 -     10382  10383  -      10380  10381  -
      Super Hang On  10649 10643 10644 10645 10646 -     -     10642 -     10790 10791 -     10792 10793 -     10884  10885  -      10886  10887  -
      -------------------------------------------------------------------------------------------------------------------------------------------------


Video Board: (Used only on Super Hang On)

171-5480
837-6279 SEGA 1987
Sticker: 837-6279-03
|-----------------------------------------------------------------------------------|
|10675.8 10676.7 10677.6 10678.5 10679.4 10680.3 10681.2 IC1  LED  CN6       CN5    |
|                                                                                   |
|                                                                               DSWB|
|                                                                                   |
|10682.16 10683.15 10684.14 10685.13 10686.12 10687.11 10688.10 IC9                 |
|JP3 JP1                                                                        DSWA|
|JP4 JP2                                                                            |
|                                                                TLP521-4           |
|315-5251           315-5213          TMM2018  TMM2018           TLP521-4           |
|      |-----------|                                             TLP521-4           |
|      | SEGA      |                                             TLP521-4           |
|      | 315-5196  |                                                                |
|      |           |                                                   ULN2003      |
|      |           |                                                     D4051      |
|      |-----------|                                                             CN4|
|                                                                                   |
|                                                                                   |
|                                                                                   |
|                                                                                   |
|                                                                                   |
|  TMM2018  TMM2018  TMM2018  TMM2018                                               |
|JP5                                                                                |
|JP6  10650.56 10651.55 10652.54                                                    |
|                                  D42832   TMM2115                           DAP601|
|                                                                             DAP601|
|25.1748MHz                                                                 ADC0804 |
|      |-----------|                                                                |
|   JP7| SEGA      |               D42832   TMM2115                                 |
|      | 315-5197  |                                                                |
|JP8   |           |                                               |----------|     |
|      |           |                                               | SEGA     |     |
|      |-----------|               TMM2115  TMM2115                | 315-5242 |     |
|                                                                  |----------|     |
|         A                         B                                 C             |
|-----------------------------------------------------------------------------------|
Notes:
      D42832   - NEC D42832 32kx8 SRAM (DIP28)
      TMM2018  - Toshiba TMM2018 2kx8 SRAM (NDIP24)
      TMM2115  - Toshiba TMM2115 2kx8 SRAM (NDIP24)
      TLP521-4 - Toshiba TLP521-4 Photocoupler Gallium Arsenide Infrared Diode & Photo???Transistor (DIP16)
      ADC0804  - National Semiconductor ADC0804 8-Bit Microprocessor Compatible A/D Converter (DIP20)
      ULN2003  - NEC uPA2003 7 High-Voltage, High-Current Darlington Transistor Arrays (DIP16)
      D4051    - NEC D4051 Single 8-Channel, Dual 4-Channel, Triple 2-Channel Analog Multiplexer/Demultiplexer with Logic Level Conversion (DIP16)
      DAP601   - Diotec Semiconductor DAP601 Small Signal Diode Array IC (SIP7)
      CN4      - 50 Pin Connector for Controls/Inputs/Outputs
      CN5      - 6 Pin Connector for Video Output (RGB/Sync/GND)
      CN6      - 10 Pin Connector for 5V Input and GND
      A/B/C    - 50 Pin Connectors (x3) for joining Video Board to CPU Board
      315-5196 - Sega Custom Sprite Generator (PGA package)
      315-5197 - Sega Custom Tilemap Generator (PGA package)
      315-5213 - MMI PAL16R6 (sprite-related) (DIP20)
      315-5242 - Sega Custom Color Encoder (wide custom DIP32 ceramic package with surface mount caps/resistors/transistors and a QFP44 IC)
      315-5251 - Signetics CK2605 (DIP20)
      JPx      - 0-Ohm Resistors for ROM Configuration
                 JP1: 512 populated
                 JP2: 1M  populated from lower hole to JP4 upper hole (i.e. diagonally across both JP2 and JP4)
                 JP3: 1M  not populated
                 JP4: 512, see JP2
                 JP5: 256 populated
                 JP6: 512 not populated
                 JP7: 512 not populated
                 JP8: 256 populated
      106xx.xx   ROMs, 1065x = 27C256, all other 106xx.xx are 27C512
      IC1/9    - DIP28 sockets not populated

      Measurements
      ------------
      VSync - 60.0543Hz
      HSync - 15.6740kHz
      OSC1  - 25.1747MHz


Video Board: (Used only on Out Run)

171-5377-01
837-6064 SEGA 1986
|-----------------------------------------------------------------------------------|
|HM65256 HM65256 HM65256 HM65256                              LED   K         H     |
|                HM65256 HM65256 HM65256 HM65256  TMM2063                           |
|                                                 TMM2063                       DSWB|
|                                                                                   |
|HM65256 HM65256 HM65256 HM65256                                                    |
|                HM65256 HM65256 HM65256 HM65256                                DSWA|
|                                                                                   |
|                                                                                   |
|                   25.1748MHz  TMM2018               TMM2015             2401      |
| |-----------|                 TMM2018               HM65256             2401      |
| | SEGA      |                                       TMM2015             2401      |
| | 315-5211  |                                       HM65256             2401      |
| |           |                                                                     |
| |           |                                                                     |
| |-----------|                                                                    G|
|               315-5227A                                                           |
|   R1    R2     R3                                                                 |
|                                                                                   |
|IC12 IC16 IC20 IC24 IC28 IC32 IC36 IC40 IC44                                       |
|                                                |-----------|       315-5228       |
|                                                | SEGA      |                      |
|                                                | 315-5197  |                      |
|IC11 IC15 IC19 IC23 IC27 IC31 IC35 IC39 IC43    |           |             UPD2003  |
|                                                |           |             UPD2003 F|
|                                                |-----------|             UPD2003  |
|                                                     R11 R12 R13                   |
|IC10 IC14 IC18 IC22 IC26 IC30 IC34 IC38 IC42  IC102 IC103 IC104                    |
|                                                                                   |
|                                                                      8255         |
|                                                                                   |
|IC9  IC13 IC17 IC21 IC25 IC29 IC33 IC37 IC41  IC99  IC100 IC101            DAP601  |
|                                                                           DAP601 D|
|                                                                           ADC0804 |
|         A                         B                                 C     4051    |
|-----------------------------------------------------------------------------------|
Notes:
      HM65256  - Hitachi HM65256 or uPD42832 32kx8 SRAM (DIP28)
      TMM2063  - Toshiba TMM2063 8kx8 SRAM (NDIP28)
      TMM2018  - Toshiba TMM2018 2kx8 SRAM (NDIP24)
      TMM2015  - Toshiba TMM2015 2kx8 SRAM (NDIP24)
      ADC0804  - National Semiconductor ADC0804 8-Bit Microprocessor Compatible A/D Converter (DIP20)
      ULN2003  - NEC uPA2003 7 High-Voltage, High-Current Darlington Transistor Arrays (DIP16)
      D4051    - NEC D4051 Single 8-Channel, Dual 4-Channel, Triple 2-Channel Analog Multiplexer/Demultiplexer with Logic Level Conversion (DIP16)
      DAP601   - Diotec Semiconductor DAP601 Small Signal Diode Array IC (SIP7)
      8255     - NEC D8255AC-2 Programmable Peripheral Interface Adapter (DIP40)
      2401     - NEC 2401 (or Sharp PC817) 4-Channel Type Photocoupler (DIP16)
      G        - 50 Pin Connector for Controls/Inputs/Outputs
      H        - 6 Pin Connector for Video Output (RGB/Sync/GND)
      K        - 10 Pin Connector for 5V Input and GND
      A/B/C    - 50 Pin Connectors (x3) for joining Video Board to CPU Board
      F        - 26 Pin Connector
      D        - 20 Pin Connector
      315-5197 - Sega Custom Tilemap Generator (PGA package)
      315-5211 - Sega Custom Sprite Generator (PGA package)
      315-5227A- Signetics CK2678 (DIP20)
      315-5228 - Signetics CK2605 (DIP20)
      R1/2/3   - 0-Ohm Resistors for ROM Configuration (for IC9 - IC44)
                 For 831000 mask ROMs: R1 populated, R2 & R3 not populated
                 For 27C256 EPROMs: R1 not populated, R2 & R3 populated
                 Other possible configurations unknown
      R11/12/13- 0-Ohm Resistors for ROM Configuration (for IC99 - IC104)
                 For 27C256 EPROMs: R11 & R13 populated, R12 not populated
                 Other possible configurations unknown
      ICxx     - DIP28 sockets

      Measurements
      ------------
      VSync - 60.0543Hz
      HSync - 15.6740kHz
      OSC1  - 25.1747MHz

***************************************************************************/


#include "emu.h"
#include "cpu/z80/z80.h"
#include "includes/segas16.h"
#include "machine/fd1089.h"
#include "machine/segaic16.h"
#include "machine/8255ppi.h"
#include "cpu/m68000/m68000.h"
#include "sound/2151intf.h"
#include "sound/segapcm.h"
#include "video/segaic16.h"

#include "outrun.lh"


#define MASTER_CLOCK		(XTAL_40MHz)
#define SOUND_CLOCK 		(XTAL_16MHz)
#define MASTER_CLOCK_25MHz	(XTAL_25_1748MHz)



/*************************************
 *
 *  Statics
 *
 *************************************/

static UINT16 *workram;
static UINT16 *cpu1ram, *cpu1rom;

/*************************************
 *
 *  Prototypes
 *
 *************************************/

static READ16_HANDLER( misc_io_r );
static WRITE16_HANDLER( misc_io_w );

static READ8_DEVICE_HANDLER( unknown_porta_r );
static READ8_DEVICE_HANDLER( unknown_portb_r );
static READ8_DEVICE_HANDLER( unknown_portc_r );
static WRITE8_DEVICE_HANDLER( unknown_porta_w );
static WRITE8_DEVICE_HANDLER( unknown_portb_w );
static WRITE8_DEVICE_HANDLER( video_control_w );



/*************************************
 *
 *  PPI interfaces
 *
 *************************************/

static const ppi8255_interface single_ppi_intf =
{
	DEVCB_HANDLER(unknown_porta_r),
	DEVCB_HANDLER(unknown_portb_r),
	DEVCB_HANDLER(unknown_portc_r),
	DEVCB_HANDLER(unknown_porta_w),
	DEVCB_HANDLER(unknown_portb_w),
	DEVCB_HANDLER(video_control_w)
};



/*************************************
 *
 *  Memory mapping tables
 *
 *************************************/

static const segaic16_memory_map_entry outrun_info[] =
{
	{ 0x35/2, 0x90000, 0x10000, 0xf00000,      ~0, segaic16_road_control_0_r, NULL,     segaic16_road_control_0_w, NULL,     NULL,                  "road control" },
	{ 0x35/2, 0x80000, 0x01000, 0xf0f000,      ~0, NULL,                      "bank10", NULL,                      "bank10", &segaic16_roadram_0,   "road RAM" },
	{ 0x35/2, 0x60000, 0x08000, 0xf18000,      ~0, NULL,                      "bank11", NULL,                      "bank11", &cpu1ram,              "CPU 1 RAM" },
	{ 0x35/2, 0x00000, 0x60000, 0xf00000,      ~0, NULL,                      "bank12", NULL,                      NULL,     &cpu1rom,              "CPU 1 ROM" },
	{ 0x31/2, 0x00000, 0x04000, 0xffc000,      ~0, misc_io_r,                 NULL,     misc_io_w,                 NULL,     NULL,                  "I/O space" },
	{ 0x2d/2, 0x00000, 0x01000, 0xfff000,      ~0, NULL,                      "bank13", NULL,                      "bank13", &segaic16_spriteram_0, "object RAM" },
	{ 0x29/2, 0x00000, 0x02000, 0xffe000,      ~0, NULL,                      "bank14", segaic16_paletteram_w,     NULL,     &segaic16_paletteram,  "color RAM" },
	{ 0x25/2, 0x00000, 0x10000, 0xfe0000,      ~0, NULL,                      "bank15", segaic16_tileram_0_w,      NULL,     &segaic16_tileram_0,   "tile RAM" },
	{ 0x25/2, 0x10000, 0x01000, 0xfef000,      ~0, NULL,                      "bank16", segaic16_textram_0_w,      NULL,     &segaic16_textram_0,   "text RAM" },
	{ 0x21/2, 0x60000, 0x08000, 0xf98000,      ~0, NULL,                      "bank17", NULL,                      "bank17", &workram,              "CPU 0 RAM" },
	{ 0x21/2, 0x00000, 0x60000, 0xf80000, 0x00000, NULL,                      "bank18", NULL,                      NULL,     NULL,                  "CPU 0 ROM" },
	{ 0 }
};



/*************************************
 *
 *  Configuration
 *
 *************************************/

static TIMER_CALLBACK( delayed_sound_data_w )
{
	segas1x_state *state = (segas1x_state *)machine->driver_data;
	const address_space *space = cpu_get_address_space(state->maincpu, ADDRESS_SPACE_PROGRAM);
	soundlatch_w(space, 0, param);
	cpu_set_input_line(state->soundcpu, INPUT_LINE_NMI, ASSERT_LINE);
}


static void sound_data_w(running_machine *machine, UINT8 data)
{
	timer_call_after_resynch(machine, NULL, data, delayed_sound_data_w);
}


static READ8_HANDLER( sound_data_r )
{
	segas1x_state *state = (segas1x_state *)space->machine->driver_data;

	cpu_set_input_line(state->soundcpu, INPUT_LINE_NMI, CLEAR_LINE);
	return soundlatch_r(space, offset);
}



static void outrun_generic_init(running_machine *machine)
{
	segas1x_state *state = (segas1x_state *)machine->driver_data;

	/* allocate memory for regions not automatically assigned */
	segaic16_spriteram_0 = auto_alloc_array(machine, UINT16, 0x01000/2);
	segaic16_paletteram  = auto_alloc_array(machine, UINT16, 0x02000/2);
	segaic16_tileram_0   = auto_alloc_array(machine, UINT16, 0x10000/2);
	segaic16_textram_0   = auto_alloc_array(machine, UINT16, 0x01000/2);
	workram              = auto_alloc_array(machine, UINT16, 0x08000/2);

	/* init the memory mapper */
	segaic16_memory_mapper_init(devtag_get_device(machine, "maincpu"), outrun_info, sound_data_w, NULL);

	/* init the FD1094 */
	fd1094_driver_init(machine, "maincpu", segaic16_memory_mapper_set_decrypted);

	/* reset the custom handlers and other pointers */
	state->custom_io_r = NULL;
	state->custom_io_w = NULL;
	state->custom_map = NULL;

	state->maincpu = devtag_get_device(machine, "maincpu");
	state->soundcpu = devtag_get_device(machine, "soundcpu");
	state->subcpu = devtag_get_device(machine, "sub");
	state->ppi8255 = devtag_get_device(machine, "ppi8255");

	state_save_register_global(machine, state->adc_select);
	state_save_register_global(machine, state->vblank_irq_state);
	state_save_register_global(machine, state->irq2_state);
	state_save_register_global_pointer(machine, segaic16_spriteram_0, 0x01000/2);
	state_save_register_global_pointer(machine, segaic16_paletteram,  0x02000/2);
	state_save_register_global_pointer(machine, segaic16_tileram_0,   0x10000/2);
	state_save_register_global_pointer(machine, segaic16_textram_0,   0x01000/2);
	state_save_register_global_pointer(machine, workram,              0x08000/2);
}



/*************************************
 *
 *  Initialization & interrupts
 *
 *************************************/

static void update_main_irqs(running_machine *machine)
{
	segas1x_state *state = (segas1x_state *)machine->driver_data;

	cpu_set_input_line(state->maincpu, 2, state->irq2_state ? ASSERT_LINE : CLEAR_LINE);
	cpu_set_input_line(state->maincpu, 4, state->vblank_irq_state ? ASSERT_LINE : CLEAR_LINE);
	cpu_set_input_line(state->maincpu, 6, state->vblank_irq_state && state->irq2_state ? ASSERT_LINE : CLEAR_LINE);

	if (state->vblank_irq_state || state->irq2_state)
		cpuexec_boost_interleave(machine, attotime_zero, ATTOTIME_IN_USEC(100));
}


static TIMER_CALLBACK( irq2_gen )
{
	segas1x_state *state = (segas1x_state *)machine->driver_data;

	/* set the IRQ2 line */
	state->irq2_state = 1;
	update_main_irqs(machine);
}


static TIMER_CALLBACK( scanline_callback )
{
	segas1x_state *state = (segas1x_state *)machine->driver_data;
	int scanline = param;
	int next_scanline = scanline;

	/* trigger IRQs on certain scanlines */
	switch (scanline)
	{
		/* IRQ2 triggers on HBLANK of scanlines 65, 129, 193 */
		case 65:
		case 129:
		case 193:
			timer_set(machine, video_screen_get_time_until_pos(machine->primary_screen, scanline, video_screen_get_visible_area(machine->primary_screen)->max_x + 1), NULL, 0, irq2_gen);
			next_scanline = scanline + 1;
			break;

		/* IRQ2 turns off at the start of scanlines 66, 130, 194 */
		case 66:
		case 130:
		case 194:
			state->irq2_state = 0;
			next_scanline = (scanline == 194) ? 223 : (scanline + 63);
			break;

		/* VBLANK triggers on scanline 223 */
		case 223:
			state->vblank_irq_state = 1;
			next_scanline = scanline + 1;
			cpu_set_input_line(state->subcpu, 4, ASSERT_LINE);
			break;

		/* VBLANK turns off at the start of scanline 224 */
		case 224:
			state->vblank_irq_state = 0;
			next_scanline = 65;
			cpu_set_input_line(state->subcpu, 4, CLEAR_LINE);
			break;
	}

	/* update IRQs on the main CPU */
	update_main_irqs(machine);

	/* come back at the next targeted scanline */
	timer_set(machine, video_screen_get_time_until_pos(machine->primary_screen, next_scanline, 0), NULL, next_scanline, scanline_callback);
}



/*************************************
 *
 *  Basic machine setup
 *
 *************************************/

static void outrun_reset(running_device *device)
{
	segas1x_state *state = (segas1x_state *)device->machine->driver_data;
	cpu_set_input_line(state->subcpu, INPUT_LINE_RESET, PULSE_LINE);
}


static MACHINE_RESET( outrun )
{
	segas1x_state *state = (segas1x_state *)machine->driver_data;
	fd1094_machine_init(devtag_get_device(machine, "maincpu"));

	/* reset misc components */
	segaic16_memory_mapper_reset(machine);
	if (state->custom_map)
		segaic16_memory_mapper_config(machine, state->custom_map);
	segaic16_tilemap_reset(machine, 0);

	/* hook the RESET line, which resets CPU #1 */
	m68k_set_reset_callback(devtag_get_device(machine, "maincpu"), outrun_reset);

	/* start timers to track interrupts */
	timer_set(machine, video_screen_get_time_until_pos(machine->primary_screen, 223, 0), NULL, 223, scanline_callback);
}



/*************************************
 *
 *  8255 handlers
 *
 *************************************/

static void log_unknown_ppi_read( running_machine *machine, unsigned port )
{
	segas1x_state *state = (segas1x_state *)machine->driver_data;
	static const char ports[] = "ABC";

	logerror("%06X:read from 8255 port %c\n", cpu_get_pc(state->maincpu), ports[port]);
}


static void log_unknown_ppi_write( running_machine *machine, unsigned port, UINT8 data )
{
	segas1x_state *state = (segas1x_state *)machine->driver_data;
	static const char ports[] = "ABC";

	logerror("%06X:write %02X to 8255 port %c\n", cpu_get_pc(state->maincpu), data, ports[port]);
}


static READ8_DEVICE_HANDLER( unknown_porta_r )
{
	log_unknown_ppi_read(device->machine, 0);
	return 0;
}


static READ8_DEVICE_HANDLER( unknown_portb_r )
{
	log_unknown_ppi_read(device->machine, 1);
	return 0;
}


static READ8_DEVICE_HANDLER( unknown_portc_r )
{
	log_unknown_ppi_read(device->machine, 2);
	return 0;
}


static WRITE8_DEVICE_HANDLER( unknown_porta_w )
{
	log_unknown_ppi_write(device->machine, 0, data);
}


static WRITE8_DEVICE_HANDLER( unknown_portb_w )
{
	log_unknown_ppi_write(device->machine, 1, data);
}


static WRITE8_DEVICE_HANDLER( video_control_w )
{
	segas1x_state *state = (segas1x_state *)device->machine->driver_data;

	/* Output port:
        D7: SG1 -- connects to sprite chip
        D6: SG0 -- connects to mixing
        D5: Screen display (1= blanked, 0= displayed)
        D4-D2: (ADC2-0)
        D1: (CONT) - affects sprite hardware
        D0: Sound section reset (1= normal operation, 0= reset)
    */
	segaic16_set_display_enable(device->machine, data & 0x20);
	state->adc_select = (data >> 2) & 7;
	cpu_set_input_line(state->soundcpu, INPUT_LINE_RESET, (data & 0x01) ? CLEAR_LINE : ASSERT_LINE);
}



/*************************************
 *
 *  I/O space
 *
 *************************************/

static READ16_HANDLER( misc_io_r )
{
	segas1x_state *state = (segas1x_state *)space->machine->driver_data;

	if (state->custom_io_r)
		return state->custom_io_r(space, offset, mem_mask);
	logerror("%06X:misc_io_r - unknown read access to address %04X\n", cpu_get_pc(space->cpu), offset * 2);
	return segaic16_open_bus_r(space, 0, mem_mask);
}


static WRITE16_HANDLER( misc_io_w )
{
	segas1x_state *state = (segas1x_state *)space->machine->driver_data;

	if (state->custom_io_w)
	{
		state->custom_io_w(space, offset, data, mem_mask);
		return;
	}
	logerror("%06X:misc_io_w - unknown write access to address %04X = %04X & %04X\n", cpu_get_pc(space->cpu), offset * 2, data, mem_mask);
}


static READ16_HANDLER( outrun_custom_io_r )
{
	segas1x_state *state = (segas1x_state *)space->machine->driver_data;

	offset &= 0x7f/2;
	switch (offset & 0x70/2)
	{
		case 0x00/2:
			return ppi8255_r(state->ppi8255, offset & 3);

		case 0x10/2:
		{
			static const char *const sysports[] = { "SERVICE", "UNKNOWN", "COINAGE", "DSW" };
			return input_port_read(space->machine, sysports[offset & 3]);
		}

		case 0x30/2:
		{
			static const char *const ports[] = { "ADC0", "ADC1", "ADC2", "ADC3", "ADC4", "ADC5", "ADC6", "ADC7" };
			return input_port_read_safe(space->machine, ports[state->adc_select], 0x0010);
		}

		case 0x60/2:
			return watchdog_reset_r(space,0);
	}

	logerror("%06X:outrun_custom_io_r - unknown read access to address %04X\n", cpu_get_pc(space->cpu), offset * 2);
	return segaic16_open_bus_r(space, 0, mem_mask);
}


static WRITE16_HANDLER( outrun_custom_io_w )
{
	segas1x_state *state = (segas1x_state *)space->machine->driver_data;

	offset &= 0x7f/2;
	switch (offset & 0x70/2)
	{
		case 0x00/2:
			if (ACCESSING_BITS_0_7)
				ppi8255_w(state->ppi8255, offset & 3, data);
			return;

		case 0x20/2:
			if (ACCESSING_BITS_0_7)
			{
				/* Output port:
                    D7: /MUTE
                    D6-D0: unknown
                */
				sound_global_enable(space->machine, data & 0x80);
			}
			return;

		case 0x30/2:
			/* ADC trigger */
			return;

		case 0x60/2:
			watchdog_reset_w(space,0,0);
			return;

		case 0x70/2:
			segaic16_sprites_draw_0_w(space, offset, data, mem_mask);
			return;
	}
	logerror("%06X:misc_io_w - unknown write access to address %04X = %04X & %04X\n", cpu_get_pc(space->cpu), offset * 2, data, mem_mask);
}


static READ16_HANDLER( shangon_custom_io_r )
{
	segas1x_state *state = (segas1x_state *)space->machine->driver_data;

	offset &= 0x303f/2;
	switch (offset)
	{
		case 0x1000/2:
		case 0x1002/2:
		case 0x1004/2:
		case 0x1006/2:
		{
			static const char *const sysports[] = { "SERVICE", "UNKNOWN", "COINAGE", "DSW" };
			return input_port_read(space->machine, sysports[offset & 3]);
		}

		case 0x3020/2:
		{
			static const char *const ports[] = { "ADC0", "ADC1", "ADC2", "ADC3" };
			return input_port_read_safe(space->machine, ports[state->adc_select], 0x0010);
		}
	}
	logerror("%06X:misc_io_r - unknown read access to address %04X\n", cpu_get_pc(space->cpu), offset * 2);
	return segaic16_open_bus_r(space,0,mem_mask);
}


static WRITE16_HANDLER( shangon_custom_io_w )
{
	segas1x_state *state = (segas1x_state *)space->machine->driver_data;

	offset &= 0x303f/2;
	switch (offset)
	{
		case 0x0000/2:
			/* Output port:
                D7-D6: (ADC1-0)
                D5: Screen display
            */
			state->adc_select = (data >> 6) & 3;
			segaic16_set_display_enable(space->machine, (data >> 5) & 1);
			return;

		case 0x0020/2:
			/* Output port:
                D0: Sound section reset (1= normal operation, 0= reset)
            */
			cpu_set_input_line(state->soundcpu, INPUT_LINE_RESET, (data & 1) ? CLEAR_LINE : ASSERT_LINE);
			return;

		case 0x3000/2:
			watchdog_reset_w(space, 0, 0);
			return;

		case 0x3020/2:
			/* ADC trigger */
			return;
	}
	logerror("%06X:misc_io_w - unknown write access to address %04X = %04X & %04X\n", cpu_get_pc(space->cpu), offset * 2, data, mem_mask);
}



/*************************************
 *
 *  Capacitor-backed RAM
 *
 *************************************/

static NVRAM_HANDLER( outrun )
{
	if (read_or_write)
		mame_fwrite(file, workram, 0x8000);
	else if (file)
		mame_fread(file, workram, 0x8000);
}



/*************************************
 *
 *  Main CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( outrun_map, ADDRESS_SPACE_PROGRAM, 16 )
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x000000, 0xffffff) AM_READWRITE(segaic16_memory_mapper_lsb_r, segaic16_memory_mapper_lsb_w)
ADDRESS_MAP_END



/*************************************
 *
 *  Second CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( sub_map, ADDRESS_SPACE_PROGRAM, 16 )
	ADDRESS_MAP_UNMAP_HIGH
	ADDRESS_MAP_GLOBAL_MASK(0xfffff)
	AM_RANGE(0x000000, 0x05ffff) AM_ROM AM_BASE(&cpu1rom)
	AM_RANGE(0x060000, 0x067fff) AM_MIRROR(0x018000) AM_RAM AM_BASE(&cpu1ram)
	AM_RANGE(0x080000, 0x080fff) AM_MIRROR(0x00f000) AM_RAM AM_BASE(&segaic16_roadram_0)
	AM_RANGE(0x090000, 0x09ffff) AM_READWRITE(segaic16_road_control_0_r, segaic16_road_control_0_w)
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

static INPUT_PORTS_START( outrun_generic )
	PORT_START("SERVICE")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_SERVICE_NO_TOGGLE( 0x02, IP_ACTIVE_LOW )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_NAME("Gear Shift") PORT_CODE(KEYCODE_SPACE) PORT_TOGGLE
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN2 )

	PORT_START("UNKNOWN")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNKNOWN )

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

	PORT_START("DSW")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) ) PORT_DIPLOCATION("SWB:1")
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) ) PORT_DIPLOCATION("SWB:2")
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Demo_Sounds ) )  PORT_DIPLOCATION("SWB:3")
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) ) PORT_DIPLOCATION("SWB:4")
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x30, 0x30, "Time Adj." )  PORT_DIPLOCATION("SWB:5,6")
	PORT_DIPSETTING(    0x20, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SWB:7,8") /* Number of Enemy Cars */
	PORT_DIPSETTING(    0x80, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )

	PORT_START("ADC0")	/* steering */
	PORT_BIT( 0xff, 0x80, IPT_PADDLE ) PORT_MINMAX(0x20,0xe0) PORT_SENSITIVITY(100) PORT_KEYDELTA(4)

	PORT_START("ADC1")	/* gas pedal */
	PORT_BIT( 0xff, 0x00, IPT_PEDAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(20)

	PORT_START("ADC2")	/* brake */
	PORT_BIT( 0xff, 0x00, IPT_PEDAL2 ) PORT_SENSITIVITY(100) PORT_KEYDELTA(40)
INPUT_PORTS_END



/*************************************
 *
 *  Game-specific port definitions
 *
 *************************************/

static INPUT_PORTS_START( outrun )
	PORT_INCLUDE( outrun_generic )

	PORT_MODIFY("DSW")
	PORT_DIPNAME( 0x03, 0x01, DEF_STR( Cabinet ) ) PORT_DIPLOCATION("SWB:1,2")
	PORT_DIPSETTING(    0x03, "Moving" )
	PORT_DIPSETTING(    0x02, "Up Cockpit" )
	PORT_DIPSETTING(    0x01, "Mini Up" )
//  PORT_DIPSETTING(    0x00, DEF_STR( Unused ) )
INPUT_PORTS_END


static INPUT_PORTS_START( outrundx )
	PORT_INCLUDE( outrun_generic )

	PORT_MODIFY("DSW")
	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Cabinet ) ) PORT_DIPLOCATION("SWB:1")
	PORT_DIPSETTING(    0x00, "Not Moving" )
	PORT_DIPSETTING(    0x01, "Moving" )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SWB:2")
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPUNUSED_DIPLOC( 0x04, IP_ACTIVE_LOW, "SWB:3" )
INPUT_PORTS_END


// This is used by the Outrun to Turbo Outrun conversion kit
// In service mode, all four settings for DSW-B 1&2 are valid
// In game, uses motors only if DSW-B 1&2 are both OFF ("Moving")
static INPUT_PORTS_START( toutrun )
	PORT_INCLUDE( outrun_generic )

	PORT_MODIFY("SERVICE")
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("Turbo") PORT_CODE(KEYCODE_LSHIFT)

	PORT_MODIFY("DSW")
	PORT_DIPNAME( 0x03, 0x01, DEF_STR( Cabinet ) ) PORT_DIPLOCATION("SWB:1,2")
	PORT_DIPSETTING(    0x03, "Moving" )
	PORT_DIPSETTING(    0x02, "Cockpit Conversion" )
	PORT_DIPSETTING(    0x01, "Mini Up" )
	PORT_DIPSETTING(    0x00, "Cockpit" )
	PORT_DIPNAME( 0x08, 0x08, "Turbo" ) PORT_DIPLOCATION("SWB:4")
	PORT_DIPSETTING(    0x00, "Use Start Button" )
	PORT_DIPSETTING(    0x08, "Use Turbo Shifter" )
	PORT_DIPNAME( 0x30, 0x10, "Credits" ) PORT_DIPLOCATION("SWB:5,6")
	PORT_DIPSETTING(    0x20, "3 to Start/2 to Continue" )
	PORT_DIPSETTING(    0x30, "2 to Start/1 to Continue" )
	PORT_DIPSETTING(    0x10, "1 to Start/1 to Continue" )
	PORT_DIPSETTING(    0x00, "2 to Start/2 to Continue" )
INPUT_PORTS_END


// This is used by the deluxe cockpit version
// In service mode, the only valid setting for DSW-B 1&2 is both OFF ("Moving")
// In game, uses motors regardless of DSW-B 1&2 setting
static INPUT_PORTS_START( toutrunm )
	PORT_INCLUDE( toutrun )

	PORT_MODIFY("DSW")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Cabinet ) ) PORT_DIPLOCATION("SWB:1,2")
	PORT_DIPSETTING(    0x03, "Moving" )
//  PORT_DIPSETTING(    0x02, DEF_STR( Unused ) )
//  PORT_DIPSETTING(    0x01, DEF_STR( Unused ) )
//  PORT_DIPSETTING(    0x00, DEF_STR( Unused ) )
INPUT_PORTS_END


// This is used by the sitdown/cockpit version
// In service mode, the only valid setting for DSW-B 1&2 is both ON ("Cockpit")
// In game, does not use motors regardless of DSW-B 1&2 setting
// In service mode, the only valid setting for DSW-B 4 is OFF
// In game, always uses Turbo Shifter button regardless of DSW-B 4 setting
static INPUT_PORTS_START( toutrunc )
	PORT_INCLUDE( toutrun )

	PORT_MODIFY("DSW")
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Cabinet ) ) PORT_DIPLOCATION("SWB:1,2")
//  PORT_DIPSETTING(    0x03, DEF_STR( Unused ) )
//  PORT_DIPSETTING(    0x02, DEF_STR( Unused ) )
//  PORT_DIPSETTING(    0x01, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x00, "Cockpit" )
	PORT_DIPNAME( 0x08, 0x08, "Turbo" ) PORT_DIPLOCATION("SWB:4")
//  PORT_DIPSETTING(    0x00, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x08, "Use Turbo Shifter" )
INPUT_PORTS_END


static INPUT_PORTS_START( shangon )
	PORT_INCLUDE( outrun_generic )

	PORT_MODIFY("SERVICE")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_MODIFY("UNKNOWN")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_SERVICE_NO_TOGGLE( 0x04, IP_ACTIVE_LOW )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("Supercharger") PORT_CODE(KEYCODE_LSHIFT)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_MODIFY("DSW")
	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SWB:1")
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x06, 0x06, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SWB:2,3") /* Other Bike's Appearance Frequency */
	PORT_DIPSETTING(    0x04, DEF_STR( Easy ) )      /* 30% Less then Normal */
	PORT_DIPSETTING(    0x06, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Hard ) )      /* 40% More then Normal */
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )   /* 80% More then Normal */
	PORT_DIPNAME( 0x18, 0x18, "Time Adj." ) PORT_DIPLOCATION("SWB:4,5")
	PORT_DIPSETTING(    0x10, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x18, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPUNUSED_DIPLOC( 0x20, IP_ACTIVE_LOW, "SWB:6" )
	PORT_DIPUNUSED_DIPLOC( 0x40, IP_ACTIVE_LOW, "SWB:7" )
	PORT_DIPUNUSED_DIPLOC( 0x80, IP_ACTIVE_LOW, "SWB:8" )

	PORT_MODIFY("ADC0")	/* steering */
	PORT_BIT( 0xff, 0x80, IPT_PADDLE ) PORT_MINMAX(0x20,0xe0) PORT_SENSITIVITY(100) PORT_KEYDELTA(4) PORT_REVERSE
INPUT_PORTS_END



/*************************************
 *
 *  Sound definitions
 *
 *************************************/

static const sega_pcm_interface segapcm_interface =
{
	BANK_512
};



/*************************************
 *
 *  Graphics definitions
 *
 *************************************/

static const gfx_layout charlayout =
{
	8,8,
	RGN_FRAC(1,3),
	3,
	{ RGN_FRAC(2,3), RGN_FRAC(1,3), RGN_FRAC(0,3) },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};


static GFXDECODE_START( segaorun )
	GFXDECODE_ENTRY( "gfx1", 0, charlayout,	0, 1024 )
GFXDECODE_END



/*************************************
 *
 *  Generic machine drivers
 *
 *************************************/

static MACHINE_DRIVER_START( outrun_base )

	/* driver data */
	MDRV_DRIVER_DATA(segas1x_state)

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", M68000, MASTER_CLOCK/4)
	MDRV_CPU_PROGRAM_MAP(outrun_map)

	MDRV_CPU_ADD("sub", M68000, MASTER_CLOCK/4)
	MDRV_CPU_PROGRAM_MAP(sub_map)

	MDRV_CPU_ADD("soundcpu", Z80, SOUND_CLOCK/4)
	MDRV_CPU_PROGRAM_MAP(sound_map)
	MDRV_CPU_IO_MAP(sound_portmap)

	MDRV_MACHINE_RESET(outrun)
	MDRV_QUANTUM_TIME(HZ(6000))

	MDRV_PPI8255_ADD( "ppi8255", single_ppi_intf )

	/* video hardware */
	MDRV_GFXDECODE(segaorun)
	MDRV_PALETTE_LENGTH(4096*3)

	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_RAW_PARAMS(MASTER_CLOCK_25MHz/4, 400, 0, 320, 262, 0, 224)

	MDRV_VIDEO_START(outrun)
	MDRV_VIDEO_UPDATE(outrun)


	/* sound hardware */
	MDRV_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")

	MDRV_SOUND_ADD("ymsnd", YM2151, SOUND_CLOCK/4)
	MDRV_SOUND_ROUTE(0, "lspeaker", 0.43)
	MDRV_SOUND_ROUTE(1, "rspeaker", 0.43)

	MDRV_SOUND_ADD("pcm", SEGAPCM, SOUND_CLOCK/4)
	MDRV_SOUND_CONFIG(segapcm_interface)
	MDRV_SOUND_ROUTE(0, "lspeaker", 1.0)
	MDRV_SOUND_ROUTE(1, "rspeaker", 1.0)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( outrundx )
	MDRV_IMPORT_FROM(outrun_base)
	MDRV_SEGA16SP_ADD_OUTRUN("segaspr1")
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( outrun )
	MDRV_IMPORT_FROM(outrun_base)
	MDRV_NVRAM_HANDLER(outrun)

	MDRV_SEGA16SP_ADD_OUTRUN("segaspr1")
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( shangon )
	MDRV_IMPORT_FROM(outrun_base)
	MDRV_NVRAM_HANDLER(outrun)

	MDRV_SCREEN_MODIFY("screen")
	MDRV_SCREEN_RAW_PARAMS(MASTER_CLOCK_25MHz/4, 400, 0, 321, 262, 0, 224)

	MDRV_VIDEO_START(shangon)
	MDRV_VIDEO_UPDATE(shangon)

	MDRV_SEGA16SP_ADD_16B("segaspr1")
MACHINE_DRIVER_END



/*************************************
 *
 *  ROM definition(s)
 *
 *************************************/

/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Outrun
    CPU: 68000
     GAME BD  834-6065-02 (or 834-6065-04)
     CPU BD   837-6063-02 (or 837-6095)
     VIDEO BD 837-6064-02 (or 837-6096)

Note: GAME BD 834-6065-01 (or 834-6065-03) & CPU BD  837-6063-01
      Use EPR-10331 to EPR-10334 for the Main 68000 code, it's unknown if they are the same... they are not currently dumped

Note: Manuals for Upright Standard and Sitdown Standard list the same Main & Sub CPU EPR codes.
      Dipswitches are used to determine the machine type.

*/
ROM_START( outrun )
	ROM_REGION( 0x60000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "epr-10380b.133", 0x000000, 0x10000, CRC(1f6cadad) SHA1(31e870f307f44eb4f293b607123b623beee2bc3c) )
	ROM_LOAD16_BYTE( "epr-10382b.118", 0x000001, 0x10000, CRC(c4c3fa1a) SHA1(69236cf9f27691dee290c79db1fc9b5e73ea77d7) )
	ROM_LOAD16_BYTE( "epr-10381b.132", 0x020000, 0x10000, CRC(be8c412b) SHA1(bf3ff05bbf81bdd44567f3b9bb4919ed4a499624) ) /* Same as the "A" version belown ??? */
	ROM_LOAD16_BYTE( "epr-10383b.117", 0x020001, 0x10000, CRC(10a2014a) SHA1(1970895145ad8b5735f66ed8c837d9d453ce9b23) )

	ROM_REGION( 0x60000, "sub", 0 ) /* second 68000 CPU */
	ROM_LOAD16_BYTE( "epr-10327a.76", 0x00000, 0x10000, CRC(e28a5baf) SHA1(f715bde96c73ed47035acf5a41630fdeb41bb2f9) )
	ROM_LOAD16_BYTE( "epr-10329a.58", 0x00001, 0x10000, CRC(da131c81) SHA1(57d5219bd0e2fd886217e37e8773fd76be9b40eb) )
	ROM_LOAD16_BYTE( "epr-10328a.75", 0x20000, 0x10000, CRC(d5ec5e5d) SHA1(a4e3cfca4d803e72bc4fcf91ab00e21bf3f8959f) )
	ROM_LOAD16_BYTE( "epr-10330a.57", 0x20001, 0x10000, CRC(ba9ec82a) SHA1(2136c9572e26b7ae6de402c0cd53174407cc6018) )

	ROM_REGION( 0x30000, "gfx1", 0 ) /* tiles */
	ROM_LOAD( "opr-10268.99",  0x00000, 0x08000, CRC(95344b04) SHA1(b3480714b11fc49b449660431f85d4ba92f799ba) )
	ROM_LOAD( "opr-10232.102", 0x08000, 0x08000, CRC(776ba1eb) SHA1(e3477961d19e694c97643066534a1f720e0c4327) )
	ROM_LOAD( "opr-10267.100", 0x10000, 0x08000, CRC(a85bb823) SHA1(a7e0143dee5a47e679fd5155e58e717813912692) )
	ROM_LOAD( "opr-10231.103", 0x18000, 0x08000, CRC(8908bcbf) SHA1(8e1237b640a6f26bdcbfd5e201dadb2687c4febb) )
	ROM_LOAD( "opr-10266.101", 0x20000, 0x08000, CRC(9f6f1a74) SHA1(09164e858ebeedcff4d389524ddf89e7c216dcae) )
	ROM_LOAD( "opr-10230.104", 0x28000, 0x08000, CRC(686f5e50) SHA1(03697b892f911177968aa40de6c5f464eb0258e7) )

	ROM_REGION32_LE( 0x100000, "gfx2", 0 ) /* sprites */
	/* VIDEO BD 837-6064-02 uses mask roms four times the size of those used on VIDEO BD 837-6064-01, same data */
	ROM_LOAD32_BYTE( "mpr-10371.9",  0x00000, 0x20000, CRC(7cc86208) SHA1(21320f945f7c8e990c97c9b1232a0f4b6bd00f8f) )
	ROM_LOAD32_BYTE( "mpr-10373.10", 0x00001, 0x20000, CRC(b0d26ac9) SHA1(3a9ce8547cd43b7b04abddf9a9ab5634e0bbfaba) )
	ROM_LOAD32_BYTE( "mpr-10375.11", 0x00002, 0x20000, CRC(59b60bd7) SHA1(e5d8c67e020608edd24ba87b7687b2ac2483ee7f) )
	ROM_LOAD32_BYTE( "mpr-10377.12", 0x00003, 0x20000, CRC(17a1b04a) SHA1(9f7210cb4153ac9029a785dcd4b45f4513a4b008) )
	ROM_LOAD32_BYTE( "mpr-10372.13", 0x80000, 0x20000, CRC(b557078c) SHA1(a3746a2da077a8df4932348f650a061f413e8430) )
	ROM_LOAD32_BYTE( "mpr-10374.14", 0x80001, 0x20000, CRC(8051e517) SHA1(9c8509fbed170b4ac74c169da573393e54774f49) )
	ROM_LOAD32_BYTE( "mpr-10376.15", 0x80002, 0x20000, CRC(f3b8f318) SHA1(a5f2532613f33a64441e0f75443c10ba78dccc6e) )
	ROM_LOAD32_BYTE( "mpr-10378.16", 0x80003, 0x20000, CRC(a1062984) SHA1(4399030a155caf71f2dec7f75c4b65531ab53576) )

	ROM_REGION( 0x10000, "gfx3", 0 ) /* road gfx (2 identical roms, 1 for each road) */
	ROM_LOAD( "opr-10186.47", 0x0000, 0x8000, CRC(22794426) SHA1(a554d4b68e71861a0d0da4d031b3b811b246f082) )
	ROM_LOAD( "opr-10185.11", 0x8000, 0x8000, CRC(22794426) SHA1(a554d4b68e71861a0d0da4d031b3b811b246f082) )

	ROM_REGION( 0x10000, "soundcpu", 0 ) /* sound CPU */
	ROM_LOAD( "epr-10187.88", 0x00000, 0x8000, CRC(a10abaa9) SHA1(01c8a819587a66d2ee4d255656e36fa0904377b0) )

	ROM_REGION( 0x80000, "pcm", ROMREGION_ERASEFF ) /* sound PCM data */
	ROM_LOAD( "opr-10193.66", 0x00000, 0x08000, CRC(bcd10dde) SHA1(417ce1d7242884640c5b14f4db8ee57cde7d085d) )
	ROM_RELOAD(               0x08000, 0x08000 )
	ROM_LOAD( "opr-10192.67", 0x10000, 0x08000, CRC(770f1270) SHA1(686bdf44d45c1d6002622f6658f037735382f3e0) )
	ROM_RELOAD(               0x18000, 0x08000 )
	ROM_LOAD( "opr-10191.68", 0x20000, 0x08000, CRC(20a284ab) SHA1(7c9027416d4122791ba53782fe2230cf02b7d506) )
	ROM_RELOAD(               0x28000, 0x08000 )
	ROM_LOAD( "opr-10190.69", 0x30000, 0x08000, CRC(7cab70e2) SHA1(a3c581d2b438630d0d4c39481dcfd85681c9f889) )
	ROM_RELOAD(               0x38000, 0x08000 )
	ROM_LOAD( "opr-10189.70", 0x40000, 0x08000, CRC(01366b54) SHA1(f467a6b807694d5832a985f5381c170d24aaee4e) )
	ROM_RELOAD(               0x48000, 0x08000 )
	ROM_LOAD( "opr-10188.71", 0x50000, 0x08000, CRC(bad30ad9) SHA1(f70dd3a6362c314adef313b064102f7a250401c8) )
	ROM_RELOAD(               0x58000, 0x08000 )
ROM_END

/**************************************************************************************************************************
    Outrun
    CPU: 68000
     GAME BD  834-6065-02 (or 834-6065-04)
     CPU BD   837-6063-02
     VIDEO BD 837-6064-02
*/
ROM_START( outrunra )
	ROM_REGION( 0x60000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "epr-10380a.133", 0x000000, 0x10000, CRC(434fadbc) SHA1(83c861d331e69ef4f2452c313ae4b5ea9d8b7948) )
	ROM_LOAD16_BYTE( "epr-10382a.118", 0x000001, 0x10000, CRC(1ddcc04e) SHA1(945d207d8d602d7fdb6d25f6b93c9c0b995e8d5a) )
	ROM_LOAD16_BYTE( "epr-10381a.132", 0x020000, 0x10000, CRC(be8c412b) SHA1(bf3ff05bbf81bdd44567f3b9bb4919ed4a499624) ) /* Same as the original version below, but labeled as rev A */
	ROM_LOAD16_BYTE( "epr-10383a.117", 0x020001, 0x10000, CRC(dcc586e7) SHA1(d6e1de6b562359574d94b88ce6101646c506e701) ) /* Same as the original version below, but labeled as rev A */

	ROM_REGION( 0x60000, "sub", 0 ) /* second 68000 CPU */
	ROM_LOAD16_BYTE( "epr-10327a.76", 0x00000, 0x10000, CRC(e28a5baf) SHA1(f715bde96c73ed47035acf5a41630fdeb41bb2f9) )
	ROM_LOAD16_BYTE( "epr-10329a.58", 0x00001, 0x10000, CRC(da131c81) SHA1(57d5219bd0e2fd886217e37e8773fd76be9b40eb) )
	ROM_LOAD16_BYTE( "epr-10328a.75", 0x20000, 0x10000, CRC(d5ec5e5d) SHA1(a4e3cfca4d803e72bc4fcf91ab00e21bf3f8959f) )
	ROM_LOAD16_BYTE( "epr-10330a.57", 0x20001, 0x10000, CRC(ba9ec82a) SHA1(2136c9572e26b7ae6de402c0cd53174407cc6018) )

	ROM_REGION( 0x30000, "gfx1", 0 ) /* tiles */
	ROM_LOAD( "opr-10268.99",  0x00000, 0x08000, CRC(95344b04) SHA1(b3480714b11fc49b449660431f85d4ba92f799ba) )
	ROM_LOAD( "opr-10232.102", 0x08000, 0x08000, CRC(776ba1eb) SHA1(e3477961d19e694c97643066534a1f720e0c4327) )
	ROM_LOAD( "opr-10267.100", 0x10000, 0x08000, CRC(a85bb823) SHA1(a7e0143dee5a47e679fd5155e58e717813912692) )
	ROM_LOAD( "opr-10231.103", 0x18000, 0x08000, CRC(8908bcbf) SHA1(8e1237b640a6f26bdcbfd5e201dadb2687c4febb) )
	ROM_LOAD( "opr-10266.101", 0x20000, 0x08000, CRC(9f6f1a74) SHA1(09164e858ebeedcff4d389524ddf89e7c216dcae) )
	ROM_LOAD( "opr-10230.104", 0x28000, 0x08000, CRC(686f5e50) SHA1(03697b892f911177968aa40de6c5f464eb0258e7) )

	ROM_REGION32_LE( 0x100000, "gfx2", 0 ) /* sprites */
	/* VIDEO BD 837-6064-02 uses mask roms four times the size of those used on VIDEO BD 837-6064-01, same data */
	ROM_LOAD32_BYTE( "mpr-10371.9",  0x00000, 0x20000, CRC(7cc86208) SHA1(21320f945f7c8e990c97c9b1232a0f4b6bd00f8f) )
	ROM_LOAD32_BYTE( "mpr-10373.10", 0x00001, 0x20000, CRC(b0d26ac9) SHA1(3a9ce8547cd43b7b04abddf9a9ab5634e0bbfaba) )
	ROM_LOAD32_BYTE( "mpr-10375.11", 0x00002, 0x20000, CRC(59b60bd7) SHA1(e5d8c67e020608edd24ba87b7687b2ac2483ee7f) )
	ROM_LOAD32_BYTE( "mpr-10377.12", 0x00003, 0x20000, CRC(17a1b04a) SHA1(9f7210cb4153ac9029a785dcd4b45f4513a4b008) )
	ROM_LOAD32_BYTE( "mpr-10372.13", 0x80000, 0x20000, CRC(b557078c) SHA1(a3746a2da077a8df4932348f650a061f413e8430) )
	ROM_LOAD32_BYTE( "mpr-10374.14", 0x80001, 0x20000, CRC(8051e517) SHA1(9c8509fbed170b4ac74c169da573393e54774f49) )
	ROM_LOAD32_BYTE( "mpr-10376.15", 0x80002, 0x20000, CRC(f3b8f318) SHA1(a5f2532613f33a64441e0f75443c10ba78dccc6e) )
	ROM_LOAD32_BYTE( "mpr-10378.16", 0x80003, 0x20000, CRC(a1062984) SHA1(4399030a155caf71f2dec7f75c4b65531ab53576) )

	ROM_REGION( 0x10000, "gfx3", 0 ) /* road gfx (2 identical roms, 1 for each road) */
	ROM_LOAD( "opr-10186.47", 0x0000, 0x8000, CRC(22794426) SHA1(a554d4b68e71861a0d0da4d031b3b811b246f082) )
	ROM_LOAD( "opr-10185.11", 0x8000, 0x8000, CRC(22794426) SHA1(a554d4b68e71861a0d0da4d031b3b811b246f082) )

	ROM_REGION( 0x10000, "soundcpu", 0 ) /* sound CPU */
	ROM_LOAD( "epr-10187.88", 0x00000, 0x8000, CRC(a10abaa9) SHA1(01c8a819587a66d2ee4d255656e36fa0904377b0) )

	ROM_REGION( 0x80000, "pcm", ROMREGION_ERASEFF ) /* sound PCM data */
	ROM_LOAD( "opr-10193.66", 0x00000, 0x08000, CRC(bcd10dde) SHA1(417ce1d7242884640c5b14f4db8ee57cde7d085d) )
	ROM_RELOAD(               0x08000, 0x08000 )
	ROM_LOAD( "opr-10192.67", 0x10000, 0x08000, CRC(770f1270) SHA1(686bdf44d45c1d6002622f6658f037735382f3e0) )
	ROM_RELOAD(               0x18000, 0x08000 )
	ROM_LOAD( "opr-10191.68", 0x20000, 0x08000, CRC(20a284ab) SHA1(7c9027416d4122791ba53782fe2230cf02b7d506) )
	ROM_RELOAD(               0x28000, 0x08000 )
	ROM_LOAD( "opr-10190.69", 0x30000, 0x08000, CRC(7cab70e2) SHA1(a3c581d2b438630d0d4c39481dcfd85681c9f889) )
	ROM_RELOAD(               0x38000, 0x08000 )
	ROM_LOAD( "opr-10189.70", 0x40000, 0x08000, CRC(01366b54) SHA1(f467a6b807694d5832a985f5381c170d24aaee4e) )
	ROM_RELOAD(               0x48000, 0x08000 )
	ROM_LOAD( "opr-10188.71", 0x50000, 0x08000, CRC(bad30ad9) SHA1(f70dd3a6362c314adef313b064102f7a250401c8) )
	ROM_RELOAD(               0x58000, 0x08000 )
ROM_END

/**************************************************************************************************************************
    Outrun
    CPU: 68000
     GAME BD  834-6065-??
     CPU BD   837-6063-??
     VIDEO BD 837-6064-01
*/

ROM_START( outruno )
	ROM_REGION( 0x60000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "epr-10380.133", 0x000000, 0x10000, CRC(e339e87a) SHA1(ac319cdafb156adcf6be29ae1b82d46d3048022e) )
	ROM_LOAD16_BYTE( "epr-10382.118", 0x000001, 0x10000, CRC(65248dd5) SHA1(4b75526df71bba0d588f47a65790a3d21b236302) )
	ROM_LOAD16_BYTE( "epr-10381.132", 0x020000, 0x10000, CRC(be8c412b) SHA1(bf3ff05bbf81bdd44567f3b9bb4919ed4a499624) )
	ROM_LOAD16_BYTE( "epr-10383.117", 0x020001, 0x10000, CRC(dcc586e7) SHA1(d6e1de6b562359574d94b88ce6101646c506e701) )

	ROM_REGION( 0x60000, "sub", 0 ) /* second 68000 CPU */
	ROM_LOAD16_BYTE( "epr-10327.76", 0x00000, 0x10000, CRC(da99d855) SHA1(77d18092e3c10a400e62eeba562f161398fe37a7) )
	ROM_LOAD16_BYTE( "epr-10329.58", 0x00001, 0x10000, CRC(fe0fa5e2) SHA1(e63fe5f7950af35131539836f18fa056767c2c80) )
	ROM_LOAD16_BYTE( "epr-10328.75", 0x20000, 0x10000, CRC(3c0e9a7f) SHA1(0e182fdac70423a85dc2b996c70bcb3954e75e10) )
	ROM_LOAD16_BYTE( "epr-10330.57", 0x20001, 0x10000, CRC(59786e99) SHA1(834bf361ca67cee3793c324bb26cf0ec82a72068) )

	ROM_REGION( 0x30000, "gfx1", 0 ) /* tiles */
	ROM_LOAD( "opr-10268.99",  0x00000, 0x08000, CRC(95344b04) SHA1(b3480714b11fc49b449660431f85d4ba92f799ba) )
	ROM_LOAD( "opr-10232.102", 0x08000, 0x08000, CRC(776ba1eb) SHA1(e3477961d19e694c97643066534a1f720e0c4327) )
	ROM_LOAD( "opr-10267.100", 0x10000, 0x08000, CRC(a85bb823) SHA1(a7e0143dee5a47e679fd5155e58e717813912692) )
	ROM_LOAD( "opr-10231.103", 0x18000, 0x08000, CRC(8908bcbf) SHA1(8e1237b640a6f26bdcbfd5e201dadb2687c4febb) )
	ROM_LOAD( "opr-10266.101", 0x20000, 0x08000, CRC(9f6f1a74) SHA1(09164e858ebeedcff4d389524ddf89e7c216dcae) )
	ROM_LOAD( "opr-10230.104", 0x28000, 0x08000, CRC(686f5e50) SHA1(03697b892f911177968aa40de6c5f464eb0258e7) )

	ROM_REGION32_LE( 0x100000, "gfx2", 0 ) /* sprites */
	/* VIDEO BD 837-6064-01 uses eproms a fourth of the size of those used on VIDEO BD 837-6064-02, same data */
	ROM_LOAD32_BYTE( "epr-10194.26", 0x00000, 0x08000, CRC(f0eda3bd) SHA1(173e10a10372d42da81e6eb48c3e23a117638c0c) )
	ROM_LOAD32_BYTE( "epr-10203.38", 0x00001, 0x08000, CRC(8445a622) SHA1(1187dee7db09a42446fc75872d49936310141eb8) )
	ROM_LOAD32_BYTE( "epr-10212.52", 0x00002, 0x08000, CRC(dee7e731) SHA1(f09d18f8d8405025b87dd01488ad2098e28410b0) )
	ROM_LOAD32_BYTE( "epr-10221.66", 0x00003, 0x08000, CRC(43431387) SHA1(a28896e888bc4d4f67babd49003d663c1ceabb71) )
	ROM_LOAD32_BYTE( "epr-10195.27", 0x20000, 0x08000, CRC(0de75cdd) SHA1(a97faea76aca663ccbbde327f3d1d8ae256649d3) )
	ROM_LOAD32_BYTE( "epr-10204.39", 0x20001, 0x08000, CRC(5f4b5abb) SHA1(f81637b2eb6a4bde76c43eedfad7e5375594c7bd) )
	ROM_LOAD32_BYTE( "epr-10213.53", 0x20002, 0x08000, CRC(1d1b22f0) SHA1(d3b1c36d08c4b7b08f9969a521e62eebd5b2238d) )
	ROM_LOAD32_BYTE( "epr-10222.67", 0x20003, 0x08000, CRC(a254c706) SHA1(e2801a0a7fd5546a48cd53ad7e4743d821d985ff) )
	ROM_LOAD32_BYTE( "epr-10196.28", 0x40000, 0x08000, CRC(8688bb59) SHA1(0aaa90c5101aa1db00db776a15a0a525587dfc43) )
	ROM_LOAD32_BYTE( "epr-10205.40", 0x40001, 0x08000, CRC(74bd93ca) SHA1(6a02ea3b977e56cfd61302afa2abf6c2dc766ba7) )
	ROM_LOAD32_BYTE( "epr-10214.54", 0x40002, 0x08000, CRC(57527e18) SHA1(4cc95c4b741f495e5b9c3b9d4d9ab9a6fded9aeb) )
	ROM_LOAD32_BYTE( "epr-10223.68", 0x40003, 0x08000, CRC(3850690e) SHA1(0f92743f848edc8deaeeef3afca5f662ceba61e7) )
	ROM_LOAD32_BYTE( "epr-10197.29", 0x60000, 0x08000, CRC(009165a6) SHA1(987b91e8c5c54bb7c4520b13a72f1f47c34278f4) )
	ROM_LOAD32_BYTE( "epr-10206.41", 0x60001, 0x08000, CRC(954542c5) SHA1(3c67e3568c04ba083f4aacad2e8857cdd16b3b2f) )
	ROM_LOAD32_BYTE( "epr-10215.55", 0x60002, 0x08000, CRC(69be5a6c) SHA1(2daac5877a71de04878f231f03361f697552431f) )
	ROM_LOAD32_BYTE( "epr-10224.69", 0x60003, 0x08000, CRC(5cffc346) SHA1(0481f864bb584c96cd92c260a62c0c1d4030bde8) )
	ROM_LOAD32_BYTE( "epr-10198.30", 0x80000, 0x08000, CRC(d894992e) SHA1(451469f743a0019b8797d16ba7b26a267d13fe06) )
	ROM_LOAD32_BYTE( "epr-10207.42", 0x80001, 0x08000, CRC(ca61cea4) SHA1(7c39e2863f5c7be290522acdaf046b1dab7a3542) )
	ROM_LOAD32_BYTE( "epr-10216.56", 0x80002, 0x08000, CRC(d394134d) SHA1(42f768a9c9eb9f556d197548c35b3a0cd5414734) )
	ROM_LOAD32_BYTE( "epr-10225.70", 0x80003, 0x08000, CRC(0a5d1f2b) SHA1(43d9c7539b6cebbac3395a4ba71a702300c9e644) )
	ROM_LOAD32_BYTE( "epr-10199.31", 0xa0000, 0x08000, CRC(86376af6) SHA1(971f4b0d9a01ca7ffb50cefbe1ab41b703a4a41a) )
	ROM_LOAD32_BYTE( "epr-10208.43", 0xa0001, 0x08000, CRC(6830b7fa) SHA1(3ece1971a4f025104ebd026da6751caea9aa8a64) )
	ROM_LOAD32_BYTE( "epr-10217.57", 0xa0002, 0x08000, CRC(bf2c9b76) SHA1(248e273255968115a60855b1fffcce1dbeacc3d4) )
	ROM_LOAD32_BYTE( "epr-10226.71", 0xa0003, 0x08000, CRC(5a452474) SHA1(6789a33b55a1693ec9cc196b3ebd220b14169e08) )
	ROM_LOAD32_BYTE( "epr-10200.32", 0xc0000, 0x08000, CRC(1e5d4f73) SHA1(79deddf4461dad5784441c2839894207b7d2ecac) )
	ROM_LOAD32_BYTE( "epr-10209.44", 0xc0001, 0x08000, CRC(5c15419e) SHA1(7b4e9c0cb430afae7f927c0224021add0a627251) )
	ROM_LOAD32_BYTE( "epr-10218.58", 0xc0002, 0x08000, CRC(db4bdb39) SHA1(b4661611b28e7ff1c721565175038cfd1e99d383) )
	ROM_LOAD32_BYTE( "epr-10227.72", 0xc0003, 0x08000, CRC(c7def392) SHA1(fa7d1245eefdc3abb9520118bbb0d025ca62901e) )
	ROM_LOAD32_BYTE( "epr-10201.33", 0xe0000, 0x08000, CRC(1d9d4b9c) SHA1(3264b66c87aa7de4c140450b96adbe3071231d4a) )
	ROM_LOAD32_BYTE( "epr-10210.45", 0xe0001, 0x08000, CRC(39422931) SHA1(8d8a3f4597945c92aebd20c0784180696b6c9c1c) )
	ROM_LOAD32_BYTE( "epr-10219.59", 0xe0002, 0x08000, CRC(e73b9224) SHA1(1904a71a0c18ab2a3a5929e72b1c215dbb0fa213) )
	ROM_LOAD32_BYTE( "epr-10228.73", 0xe0003, 0x08000, CRC(25803978) SHA1(1a18922aeb516e8deb026d52e3cdcc4e69385af5) )

	ROM_REGION( 0x10000, "gfx3", 0 ) /* road gfx (2 identical roms, 1 for each road) */
	ROM_LOAD( "opr-10186.47", 0x0000, 0x8000, CRC(22794426) SHA1(a554d4b68e71861a0d0da4d031b3b811b246f082) )
	ROM_LOAD( "opr-10185.11", 0x8000, 0x8000, CRC(22794426) SHA1(a554d4b68e71861a0d0da4d031b3b811b246f082) )

	ROM_REGION( 0x10000, "soundcpu", 0 ) /* sound CPU */
	ROM_LOAD( "epr-10187.88", 0x00000, 0x8000, CRC(a10abaa9) SHA1(01c8a819587a66d2ee4d255656e36fa0904377b0) )

	ROM_REGION( 0x80000, "pcm", ROMREGION_ERASEFF ) /* sound PCM data */
	ROM_LOAD( "opr-10193.66", 0x00000, 0x08000, CRC(bcd10dde) SHA1(417ce1d7242884640c5b14f4db8ee57cde7d085d) )
	ROM_RELOAD(               0x08000, 0x08000 )
	ROM_LOAD( "opr-10192.67", 0x10000, 0x08000, CRC(770f1270) SHA1(686bdf44d45c1d6002622f6658f037735382f3e0) )
	ROM_RELOAD(               0x18000, 0x08000 )
	ROM_LOAD( "opr-10191.68", 0x20000, 0x08000, CRC(20a284ab) SHA1(7c9027416d4122791ba53782fe2230cf02b7d506) )
	ROM_RELOAD(               0x28000, 0x08000 )
	ROM_LOAD( "opr-10190.69", 0x30000, 0x08000, CRC(7cab70e2) SHA1(a3c581d2b438630d0d4c39481dcfd85681c9f889) )
	ROM_RELOAD(               0x38000, 0x08000 )
	ROM_LOAD( "opr-10189.70", 0x40000, 0x08000, CRC(01366b54) SHA1(f467a6b807694d5832a985f5381c170d24aaee4e) )
	ROM_RELOAD(               0x48000, 0x08000 )
	ROM_LOAD( "opr-10188.71", 0x50000, 0x08000, CRC(bad30ad9) SHA1(f70dd3a6362c314adef313b064102f7a250401c8) )
	ROM_RELOAD(               0x58000, 0x08000 )
ROM_END

/**************************************************************************************************************************
    Outrun Deluxe
    CPU: 68000
     GAME BD  834-6065 Rev A
     CPU BD   837-6063
     VIDEO BD 837-6064-01
*/

ROM_START( outrundx )
	ROM_REGION( 0x60000, "maincpu", 0 ) /* 68000 code */
        /* Earlier version of CPU BD?? uses half size eproms compared to the above sets */
	ROM_LOAD16_BYTE( "epr-10183.115", 0x000000, 0x8000, CRC(3d992396) SHA1(8cef43799b71cfd36d3fea140afff7fe0bafcfc1) )
	ROM_LOAD16_BYTE( "epr-10261.130", 0x000001, 0x8000, CRC(1d034847) SHA1(664b24c13f7885403328906682213e38c1ad994e) )
	ROM_LOAD16_BYTE( "epr-10184.116", 0x010000, 0x8000, CRC(1a73dc46) SHA1(70f31619e80eb3d70747e7006e135c8bc0a31675) )
	ROM_LOAD16_BYTE( "epr-10262.131", 0x010001, 0x8000, CRC(5386b6b3) SHA1(a554ed1b4e07811c4accc59c063baa42949b6670) )
	ROM_LOAD16_BYTE( "epr-10258.117", 0x020000, 0x8000, CRC(39408e4f) SHA1(4f7f8b393dfb1e1935d595ae55a6913a27b02f80) )
	ROM_LOAD16_BYTE( "epr-10263.132", 0x020001, 0x8000, CRC(eda65fd6) SHA1(dd9c072856edffff3e73423f22ab40c5893bd26f) )
	ROM_LOAD16_BYTE( "epr-10259.118", 0x030000, 0x8000, CRC(95100b1a) SHA1(d2a5eb97623321b6c943bc435de26bf5d39ea312) )
	ROM_LOAD16_BYTE( "epr-10264.133", 0x030001, 0x8000, CRC(cc94b102) SHA1(29dc7e2a8509d0b5d30e2fb9404e0517b97f64e8) )

	ROM_REGION( 0x60000, "sub", 0 ) /* second 68000 CPU */
	ROM_LOAD16_BYTE( "epr-10173.66", 0x000000, 0x8000, CRC(6c2775c0) SHA1(2dd3a4e7f7b8808da74fbd53423a83775afff5d5) )
	ROM_LOAD16_BYTE( "epr-10178.86", 0x000001, 0x8000, CRC(6d36be05) SHA1(02527701451bbdfa14280ef4db6f4d540e6ee470) )
	ROM_LOAD16_BYTE( "epr-10174.67", 0x010000, 0x8000, CRC(aae7efad) SHA1(bbc68daafc8bb61d0b065baa3a3583e95de4d9ad) )
	ROM_LOAD16_BYTE( "epr-10179.87", 0x010001, 0x8000, CRC(180fd041) SHA1(87f1566cff1bded7642e260b8337a278052727bb) )
	ROM_LOAD16_BYTE( "epr-10175.68", 0x020000, 0x8000, CRC(31c76063) SHA1(a3069c5443e7f87c38a69530f00ccc6e9a8eac42) )
	ROM_LOAD16_BYTE( "epr-10180.88", 0x020001, 0x8000, CRC(4713b264) SHA1(ab498b5232520657bae841927ee74994a6fb1c4e) )
	ROM_LOAD16_BYTE( "epr-10176.69", 0x030000, 0x8000, CRC(a7811f90) SHA1(a2ac49f0947ddddbbdaa90ebdefd232fdbf27c35) )
	ROM_LOAD16_BYTE( "epr-10181.89", 0x030001, 0x8000, CRC(e009a04d) SHA1(f3253a0feb6acd08238e025e7ab8b5cb175d1c67) )

	ROM_REGION( 0x30000, "gfx1", 0 ) /* tiles */
	ROM_LOAD( "opr-10268.99",  0x00000, 0x08000, CRC(95344b04) SHA1(b3480714b11fc49b449660431f85d4ba92f799ba) )
	ROM_LOAD( "opr-10232.102", 0x08000, 0x08000, CRC(776ba1eb) SHA1(e3477961d19e694c97643066534a1f720e0c4327) )
	ROM_LOAD( "opr-10267.100", 0x10000, 0x08000, CRC(a85bb823) SHA1(a7e0143dee5a47e679fd5155e58e717813912692) )
	ROM_LOAD( "opr-10231.103", 0x18000, 0x08000, CRC(8908bcbf) SHA1(8e1237b640a6f26bdcbfd5e201dadb2687c4febb) )
	ROM_LOAD( "opr-10266.101", 0x20000, 0x08000, CRC(9f6f1a74) SHA1(09164e858ebeedcff4d389524ddf89e7c216dcae) )
	ROM_LOAD( "opr-10230.104", 0x28000, 0x08000, CRC(686f5e50) SHA1(03697b892f911177968aa40de6c5f464eb0258e7) )

	ROM_REGION32_LE( 0x100000, "gfx2", 0 ) /* sprites */
	/* VIDEO BD 837-6064-01 uses eproms a fourth of the size of those used on VIDEO BD 837-6064-02, same data */
	ROM_LOAD32_BYTE( "epr-10194.26", 0x00000, 0x08000, CRC(f0eda3bd) SHA1(173e10a10372d42da81e6eb48c3e23a117638c0c) )
	ROM_LOAD32_BYTE( "epr-10203.38", 0x00001, 0x08000, CRC(8445a622) SHA1(1187dee7db09a42446fc75872d49936310141eb8) )
	ROM_LOAD32_BYTE( "epr-10212.52", 0x00002, 0x08000, CRC(dee7e731) SHA1(f09d18f8d8405025b87dd01488ad2098e28410b0) )
	ROM_LOAD32_BYTE( "epr-10221.66", 0x00003, 0x08000, CRC(43431387) SHA1(a28896e888bc4d4f67babd49003d663c1ceabb71) )
	ROM_LOAD32_BYTE( "epr-10195.27", 0x20000, 0x08000, CRC(0de75cdd) SHA1(a97faea76aca663ccbbde327f3d1d8ae256649d3) )
	ROM_LOAD32_BYTE( "epr-10204.39", 0x20001, 0x08000, CRC(5f4b5abb) SHA1(f81637b2eb6a4bde76c43eedfad7e5375594c7bd) )
	ROM_LOAD32_BYTE( "epr-10213.53", 0x20002, 0x08000, CRC(1d1b22f0) SHA1(d3b1c36d08c4b7b08f9969a521e62eebd5b2238d) )
	ROM_LOAD32_BYTE( "epr-10222.67", 0x20003, 0x08000, CRC(a254c706) SHA1(e2801a0a7fd5546a48cd53ad7e4743d821d985ff) )
	ROM_LOAD32_BYTE( "epr-10196.28", 0x40000, 0x08000, CRC(8688bb59) SHA1(0aaa90c5101aa1db00db776a15a0a525587dfc43) )
	ROM_LOAD32_BYTE( "epr-10205.40", 0x40001, 0x08000, CRC(74bd93ca) SHA1(6a02ea3b977e56cfd61302afa2abf6c2dc766ba7) )
	ROM_LOAD32_BYTE( "epr-10214.54", 0x40002, 0x08000, CRC(57527e18) SHA1(4cc95c4b741f495e5b9c3b9d4d9ab9a6fded9aeb) )
	ROM_LOAD32_BYTE( "epr-10223.68", 0x40003, 0x08000, CRC(3850690e) SHA1(0f92743f848edc8deaeeef3afca5f662ceba61e7) )
	ROM_LOAD32_BYTE( "epr-10197.29", 0x60000, 0x08000, CRC(009165a6) SHA1(987b91e8c5c54bb7c4520b13a72f1f47c34278f4) )
	ROM_LOAD32_BYTE( "epr-10206.41", 0x60001, 0x08000, CRC(954542c5) SHA1(3c67e3568c04ba083f4aacad2e8857cdd16b3b2f) )
	ROM_LOAD32_BYTE( "epr-10215.55", 0x60002, 0x08000, CRC(69be5a6c) SHA1(2daac5877a71de04878f231f03361f697552431f) )
	ROM_LOAD32_BYTE( "epr-10224.69", 0x60003, 0x08000, CRC(5cffc346) SHA1(0481f864bb584c96cd92c260a62c0c1d4030bde8) )
	ROM_LOAD32_BYTE( "epr-10198.30", 0x80000, 0x08000, CRC(d894992e) SHA1(451469f743a0019b8797d16ba7b26a267d13fe06) )
	ROM_LOAD32_BYTE( "epr-10207.42", 0x80001, 0x08000, CRC(ca61cea4) SHA1(7c39e2863f5c7be290522acdaf046b1dab7a3542) )
	ROM_LOAD32_BYTE( "epr-10216.56", 0x80002, 0x08000, CRC(d394134d) SHA1(42f768a9c9eb9f556d197548c35b3a0cd5414734) )
	ROM_LOAD32_BYTE( "epr-10225.70", 0x80003, 0x08000, CRC(0a5d1f2b) SHA1(43d9c7539b6cebbac3395a4ba71a702300c9e644) )
	ROM_LOAD32_BYTE( "epr-10199.31", 0xa0000, 0x08000, CRC(86376af6) SHA1(971f4b0d9a01ca7ffb50cefbe1ab41b703a4a41a) )
	ROM_LOAD32_BYTE( "epr-10208.43", 0xa0001, 0x08000, CRC(6830b7fa) SHA1(3ece1971a4f025104ebd026da6751caea9aa8a64) )
	ROM_LOAD32_BYTE( "epr-10217.57", 0xa0002, 0x08000, CRC(bf2c9b76) SHA1(248e273255968115a60855b1fffcce1dbeacc3d4) )
	ROM_LOAD32_BYTE( "epr-10226.71", 0xa0003, 0x08000, CRC(5a452474) SHA1(6789a33b55a1693ec9cc196b3ebd220b14169e08) )
	ROM_LOAD32_BYTE( "epr-10200.32", 0xc0000, 0x08000, CRC(1e5d4f73) SHA1(79deddf4461dad5784441c2839894207b7d2ecac) )
	ROM_LOAD32_BYTE( "epr-10209.44", 0xc0001, 0x08000, CRC(5c15419e) SHA1(7b4e9c0cb430afae7f927c0224021add0a627251) )
	ROM_LOAD32_BYTE( "epr-10218.58", 0xc0002, 0x08000, CRC(db4bdb39) SHA1(b4661611b28e7ff1c721565175038cfd1e99d383) )
	ROM_LOAD32_BYTE( "epr-10227.72", 0xc0003, 0x08000, CRC(c7def392) SHA1(fa7d1245eefdc3abb9520118bbb0d025ca62901e) )
	ROM_LOAD32_BYTE( "epr-10201.33", 0xe0000, 0x08000, CRC(1d9d4b9c) SHA1(3264b66c87aa7de4c140450b96adbe3071231d4a) )
	ROM_LOAD32_BYTE( "epr-10210.45", 0xe0001, 0x08000, CRC(39422931) SHA1(8d8a3f4597945c92aebd20c0784180696b6c9c1c) )
	ROM_LOAD32_BYTE( "epr-10219.59", 0xe0002, 0x08000, CRC(e73b9224) SHA1(1904a71a0c18ab2a3a5929e72b1c215dbb0fa213) )
	ROM_LOAD32_BYTE( "epr-10228.73", 0xe0003, 0x08000, CRC(25803978) SHA1(1a18922aeb516e8deb026d52e3cdcc4e69385af5) )

	ROM_REGION( 0x10000, "gfx3", 0 ) /* road gfx (2 identical roms, 1 for each road) */
	ROM_LOAD( "opr-10186.47", 0x0000, 0x8000, CRC(22794426) SHA1(a554d4b68e71861a0d0da4d031b3b811b246f082) )
	ROM_LOAD( "opr-10185.11", 0x8000, 0x8000, CRC(22794426) SHA1(a554d4b68e71861a0d0da4d031b3b811b246f082) )

	ROM_REGION( 0x10000, "soundcpu", 0 ) /* sound CPU */
	ROM_LOAD( "epr-10187.88", 0x00000, 0x8000, CRC(a10abaa9) SHA1(01c8a819587a66d2ee4d255656e36fa0904377b0) )

	ROM_REGION( 0x80000, "pcm", ROMREGION_ERASEFF ) /* sound PCM data */
	ROM_LOAD( "opr-10193.66", 0x00000, 0x08000, CRC(bcd10dde) SHA1(417ce1d7242884640c5b14f4db8ee57cde7d085d) )
	ROM_RELOAD(               0x08000, 0x08000 )
	ROM_LOAD( "opr-10192.67", 0x10000, 0x08000, CRC(770f1270) SHA1(686bdf44d45c1d6002622f6658f037735382f3e0) )
	ROM_RELOAD(               0x18000, 0x08000 )
	ROM_LOAD( "opr-10191.68", 0x20000, 0x08000, CRC(20a284ab) SHA1(7c9027416d4122791ba53782fe2230cf02b7d506) )
	ROM_RELOAD(               0x28000, 0x08000 )
	ROM_LOAD( "opr-10190.69", 0x30000, 0x08000, CRC(7cab70e2) SHA1(a3c581d2b438630d0d4c39481dcfd85681c9f889) )
	ROM_RELOAD(               0x38000, 0x08000 )
	ROM_LOAD( "opr-10189.70", 0x40000, 0x08000, CRC(01366b54) SHA1(f467a6b807694d5832a985f5381c170d24aaee4e) )
	ROM_RELOAD(               0x48000, 0x08000 )
	ROM_LOAD( "opr-10188.71", 0x50000, 0x08000, CRC(bad30ad9) SHA1(f70dd3a6362c314adef313b064102f7a250401c8) )
	ROM_RELOAD(               0x58000, 0x08000 )
ROM_END

/**************************************************************************************************************************
    Outrun (bootleg)

    Outrun bootleg made by PHILKO

    It is composed of 3 boards.

    The upper board contains:
    2x 68000 cpus
    1x z80 cpu
    1x ym2151
    1x 20mhz osc (near 68k)
    1x 16mhz osc (near z80)
    2x pots
    1x PHILKO custom chip quad package soldered (gfx chip?). it's marked "Philko PK8702 8717".
    eproms from a-1 to a-14

    Mid board contains:
    many TTLs and rams
    1x NEC D8255AC-2 (I/O chip?)
    5x Philko custom chips dip package (little as a ttl). They are all marked "Philko PK8701 8720"
    eproms a-15, a-16 and a-17

    Lower board contains:
    lots of rams
    1x custom Philko chip the size of a z80. it's marked "Philko PK8703"
    1x custom Philko chip the size of a z80. it's marked "Philko PK8704"
    1x custom Philko chip the size of a z80. it's marked "Philko PK8705"
    eproms from a-18 to a-33

    Dumped by Corrado Tomaselli
*/
ROM_START( outrunb )
	ROM_REGION( 0x60000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "a-10.bin", 0x000000, 0x10000, CRC(cddceea2) SHA1(34cb4ca61c941e96e585f3cd2aed79bdde67f8eb) )
	ROM_LOAD16_BYTE( "a-9.bin",  0x000001, 0x10000, CRC(14e97a67) SHA1(a86ccb719ad695ed814bedfe02dbafa435fc65da) )
	ROM_LOAD16_BYTE( "a-14.bin", 0x020000, 0x10000, CRC(3092d857) SHA1(8ebfeab9217b80a7983a4f8eb7bb7d3387d791b3) )
	ROM_LOAD16_BYTE( "a-13.bin", 0x020001, 0x10000, CRC(30a1c496) SHA1(734c82930197e6e8cd2bea145aedda6b3c1145d0) )

	ROM_REGION( 0x60000, "sub", 0 ) /* second 68000 CPU */
	ROM_LOAD16_BYTE( "a-8.bin",  0x00000, 0x10000, CRC(d7f5aae0) SHA1(0f9b693f078cdbbfeade5a373a94a20110d586ca) )
	ROM_LOAD16_BYTE( "a-7.bin",  0x00001, 0x10000, CRC(88c2e78f) SHA1(198cab9133345e4529f7fb52c29974c9a1a84933) )
	ROM_LOAD16_BYTE( "a-12.bin", 0x20000, 0x10000, CRC(d5ec5e5d) SHA1(a4e3cfca4d803e72bc4fcf91ab00e21bf3f8959f) )
	ROM_LOAD16_BYTE( "a-11.bin", 0x20001, 0x10000, CRC(74c5fbec) SHA1(a44f1477d830fdb4d6c29351da94776843e5d3e1) )

	ROM_REGION( 0x30000, "gfx1", 0 ) /* tiles */
	ROM_LOAD( "a-15.bin",	0x00000, 0x10000, CRC(4c489133) SHA1(db97de9d84ca5916e69972ee19ccb4c15fa98bf9) )
	ROM_LOAD( "a-17.bin",	0x10000, 0x10000, CRC(899c781d) SHA1(4f759c316a57a1e42838375525290425d25b53e1) )
	ROM_LOAD( "a-16.bin",	0x20000, 0x10000, CRC(98dd4d15) SHA1(914ebcb330455ab35968b4add4d94be123a185a5) )

	ROM_REGION32_LE( 0x100000, "gfx2", 0 ) /* sprites */
	ROM_LOAD32_BYTE( "a-18.bin",	0x00000, 0x10000, CRC(77377e00) SHA1(4f376b05692f33d529f4c058dac989136c808ca1) )
	ROM_LOAD32_BYTE( "a-20.bin",	0x00001, 0x10000, CRC(69ecc975) SHA1(3560e9a31fc71e263a6ff61224b8db2b17836075) )
	ROM_LOAD32_BYTE( "a-22.bin",	0x00002, 0x10000, CRC(b6a8d0e2) SHA1(6184700dbe2c8c9c91f220e246501b7a865e4a05) )
	ROM_LOAD32_BYTE( "a-24.bin",	0x00003, 0x10000, CRC(d632d8a2) SHA1(27ca6faaa073bd01b2be959dba0359f93e8c1ec1) )
	ROM_LOAD32_BYTE( "a-26.bin",	0x40000, 0x10000, CRC(4f784236) SHA1(1fb610fd29d3ddd8c5d4892ae215386b18552e6f) )
	ROM_LOAD32_BYTE( "a-28.bin",	0x40001, 0x10000, CRC(ee4f7154) SHA1(3a84c1b19d9dfcd5310e9cee90c0d4562a4a7786) )
	ROM_LOAD32_BYTE( "a-30.bin",	0x40002, 0x10000, CRC(e9880aa3) SHA1(cc47f631e758bd856bbc6d010fe230f9b1ed29de) )
	ROM_LOAD32_BYTE( "a-32.bin",	0x40003, 0x10000, CRC(dc286dc2) SHA1(eaa245b81f8a324988f617467fc3134a39b59c65) )
	ROM_LOAD32_BYTE( "a-19.bin",	0x80000, 0x10000, CRC(2c0e7277) SHA1(cf14d1ca1fba2e2687998c04ad2ab8c629917412) )
	ROM_LOAD32_BYTE( "a-21.bin",	0x80001, 0x10000, CRC(54761e57) SHA1(dc0fc645eb998675ab9fe683d63d4ee57ae23693) )
	ROM_LOAD32_BYTE( "a-23.bin",	0x80002, 0x10000, CRC(a00d0676) SHA1(c2ab29a7489c6f774ce26ef023758215ea3f7050) )
	ROM_LOAD32_BYTE( "a-25.bin",	0x80003, 0x10000, CRC(da398368) SHA1(115b2881d2d5ddeda2ce82bb209a2c0b4acfcae4) )
	ROM_LOAD32_BYTE( "a-27.bin",	0xc0000, 0x10000, CRC(8d459356) SHA1(143914b1ac074708fed1d89072f915424aeb841e) )
	ROM_LOAD32_BYTE( "a-29.bin",	0xc0001, 0x10000, CRC(a8245727) SHA1(13c1d417078d91b8c97e35d632e1ac4bc9bd64e3) )
	ROM_LOAD32_BYTE( "a-31.bin",	0xc0002, 0x10000, CRC(ef7d06fe) SHA1(541b5ba45f4140e2cc29a9da2592b476d414af5d) )
	ROM_LOAD32_BYTE( "a-33.bin",	0xc0003, 0x10000, CRC(1222af9f) SHA1(2364bd54cbe21dd688efff32e93bf154546c93d6) )

	ROM_REGION( 0x10000, "gfx3", 0 ) /* road gfx - correct order unknown (identical after bitswapping) */
	ROM_LOAD( "a-2.bin", 0x0000, 0x8000, CRC(ed5bda9c) SHA1(f09a34caf1f9f6b119700a00635ab8fa8244362d) )
	ROM_LOAD( "a-3.bin", 0x8000, 0x8000, CRC(666fe754) SHA1(606090db53d658d7b04dca4748014a411e12f259) )

	ROM_REGION( 0x10000, "soundcpu", 0 ) /* sound CPU */
	ROM_LOAD( "a-1.bin", 0x00000, 0x8000, CRC(209bb53a) SHA1(4ec9ca7532354f05f06295a01c4fa4982268e1d5) )

	ROM_REGION( 0x80000, "pcm", ROMREGION_ERASEFF ) /* sound PCM data */
	ROM_LOAD( "a-6.bin", 0x00000, 0x08000, CRC(191f98f4) SHA1(a6ac6feeeeed8e08a19bfa280c5f5f8bc69833e2) )
	ROM_CONTINUE(        0x10000, 0x08000 )
	ROM_LOAD( "a-5.bin", 0x20000, 0x08000, CRC(374466d0) SHA1(c648bcb17ed0501bb3e94994716b4a7b81ec75e4) )
	ROM_CONTINUE(        0x30000, 0x08000 )
	ROM_LOAD( "a-4.bin", 0x40000, 0x08000, CRC(2a27d0b0) SHA1(018db0e80af37c22c4eb57747093ac3b9faf8931) )
	ROM_CONTINUE(        0x50000, 0x08000 )
ROM_END


/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Turbo Outrun (Out Run upgrade set)
    CPU: FD1094 (317-0118)
    GAME BD   834-6919-12
     CPU BD   837-6905-12
     VIDEO BD 837-6906-02
*/
ROM_START( toutrun )
	ROM_REGION( 0x60000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "epr-12513.133", 0x000000, 0x10000, CRC(ae8835a5) SHA1(09573964d4f42ac0f08be3682b73e3420df27c6d) )
	ROM_LOAD16_BYTE( "epr-12512.118", 0x000001, 0x10000, CRC(f90372ad) SHA1(b42dd8c580421b4d7ffacf8d3baa7b9fc9e392ef) )
	ROM_LOAD16_BYTE( "epr-12515.132", 0x020000, 0x10000, CRC(1f047df4) SHA1(c1c67847f1390e671c19f0b90c3cbfbc237d960b) )
	ROM_LOAD16_BYTE( "epr-12514.117", 0x020001, 0x10000, CRC(5539e9c3) SHA1(01046e3b836f66ba6d5c4be1611de48197aca67f) )
	ROM_LOAD16_BYTE( "epr-12293.131", 0x040000, 0x10000, CRC(f4321eea) SHA1(64334acc82c14bb58b7d51719f34fd81cfb9fc6b) )
	ROM_LOAD16_BYTE( "epr-12292.116", 0x040001, 0x10000, CRC(51d98af0) SHA1(6e7115706bfafb687faa23d55d4a8c8e498a4df2) )

	ROM_REGION( 0x2000, "user1", 0 )	/* decryption key */
	ROM_LOAD( "317-0118.key", 0x0000, 0x2000, CRC(083d7d56) SHA1(3153e44479986859f60a26fe9264ecea07e6e469) )

	ROM_REGION( 0x60000, "sub", 0 ) /* second 68000 CPU */
	ROM_LOAD16_BYTE( "opr-12295.76", 0x000000, 0x10000, CRC(d43a3a84) SHA1(362c98f62c205b6b40b7e8a4ba107745b547b984) )
	ROM_LOAD16_BYTE( "opr-12294.58", 0x000001, 0x10000, CRC(27cdcfd3) SHA1(4fe57db95b109ab1bb1326789e06a3d3aac311cc) )
	ROM_LOAD16_BYTE( "opr-12297.75", 0x020000, 0x10000, CRC(1d9b5677) SHA1(fb6e33acc43fbc7a8d7ac44045439ecdf794fdeb) )
	ROM_LOAD16_BYTE( "opr-12296.57", 0x020001, 0x10000, CRC(0a513671) SHA1(4c13ca3a6f0aa9d06ed80798b466cca0c966a265) )

	ROM_REGION( 0x30000, "gfx1", 0 ) /* tiles */
	ROM_LOAD( "opr-12323.102", 0x00000, 0x10000, CRC(4de43a6f) SHA1(68909338e1f192ac2699c8a8d24c3f46502dd019) )
	ROM_LOAD( "opr-12324.103", 0x10000, 0x10000, CRC(24607a55) SHA1(69033f2281cd42e88233c23d809b73607fe54853) )
	ROM_LOAD( "opr-12325.104", 0x20000, 0x10000, CRC(1405137a) SHA1(367db88d36852e35c5e839f692be5ea8c8e072d2) )

	ROM_REGION32_LE( 0x100000, "gfx2", 0 ) /* sprites */
	/* Video BD 837-6906-2, Mask Roms twice the size as EPR/OPR counterparts */
	ROM_LOAD32_BYTE( "mpr-12336.9",   0x00000, 0x20000, CRC(dda465c7) SHA1(83acc12a387b004986f084f25964c15a9f88a41a) )
	ROM_LOAD32_BYTE( "mpr-12337.10",  0x00001, 0x20000, CRC(828233d1) SHA1(d73a200af4245d590e1fd3ac436723f99cc50452) )
	ROM_LOAD32_BYTE( "mpr-12338.11",  0x00002, 0x20000, CRC(46b4b5f4) SHA1(afeb2e5ac6792edafe7328993fe8dfcd4bce1924) )
	ROM_LOAD32_BYTE( "mpr-12339.12",  0x00003, 0x20000, CRC(0d7e3bab) SHA1(fdb603df55785ded593daf591ddd90f8f24e0d47) )
	ROM_LOAD32_BYTE( "mpr-12364.13",  0x80000, 0x20000, CRC(a4b83e65) SHA1(966d8c163cef0842abff54e1dba3f15248e73f68) )
	ROM_LOAD32_BYTE( "mpr-12365.14",  0x80001, 0x20000, CRC(4a80b2a9) SHA1(14b4fe71e102622a73c8dc0dbd0013cbbe6fcf9d) )
	ROM_LOAD32_BYTE( "mpr-12366.15",  0x80002, 0x20000, CRC(385cb3ab) SHA1(fec6d80d488bfe26524fa3a48b195a45a073e481) )
	ROM_LOAD32_BYTE( "mpr-12367.16",  0x80003, 0x20000, CRC(4930254a) SHA1(00f24be3bf02b143fa554f4d32e283bdac79af6a) )

	ROM_REGION( 0x10000, "gfx3", 0 ) /* road gfx (2 identical roms, 1 for each road) */
	ROM_LOAD( "epr-12299.47", 0x0000, 0x8000, CRC(fc9bc41b) SHA1(9af73e096253cf2c4f283f227530110a4b37fcee) ) /* Manual shows both as EPR-12298 */
	ROM_LOAD( "epr-12298.11", 0x8000, 0x8000, CRC(fc9bc41b) SHA1(9af73e096253cf2c4f283f227530110a4b37fcee) )

	ROM_REGION( 0x10000, "soundcpu", 0 ) /* sound CPU */
	ROM_LOAD( "epr-12300.88", 0x00000, 0x10000, CRC(e8ff7011) SHA1(6eaf3aea507007ea31d507ed7825d905f4b8e7ab) )

	ROM_REGION( 0x80000, "pcm", ROMREGION_ERASEFF ) /* sound PCM data */
	ROM_LOAD( "opr-12301.66", 0x00000, 0x10000, CRC(6e78ad15) SHA1(c31ddf434b459cd1a381d2a028beabddd4ed10d2) )
	ROM_LOAD( "opr-12302.67", 0x10000, 0x10000, CRC(e72928af) SHA1(40e0b178958cfe97c097fe9d82b5de54bc27a29f) )
	ROM_LOAD( "opr-12303.68", 0x20000, 0x10000, CRC(8384205c) SHA1(c1f9d52bc587eab5a97867198e9aa7c19e973429) )
	ROM_LOAD( "opr-12304.69", 0x30000, 0x10000, CRC(e1762ac3) SHA1(855f06c082a17d90857e6efa3cf95b0eda0e634d) )
	ROM_LOAD( "opr-12305.70", 0x40000, 0x10000, CRC(ba9ce677) SHA1(056781f92450c902e1d279a02bda28337815cba9) )
	ROM_LOAD( "opr-12306.71", 0x50000, 0x10000, CRC(e49249fd) SHA1(ff36e4dba4e9d3d354e3dd528edeb50ad9c18ee4) )
ROM_END

/**************************************************************************************************************************
    Turbo Outrun (Upright version?? According to service mode dipswitch screen it's a Cockpit)
    CPU: FD1094 (317-????)
    GAME BD   834-6919
     CPU BD   837-6905
     VIDEO BD 837-6906

    Must set Cabinet Type dipswitch (DSW-B 1&2 to both ON) to Cockpit
    Must set Turbo Switch dipswitch (DSW-B 4 to OFF) to Use Turbo Shifter
*/
ROM_START( toutrun3 )
	ROM_REGION( 0x60000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "epr-12410.133", 0x000000, 0x10000, CRC(aa74f3e9) SHA1(2daf6b17317542063c0a40beea5b45c797192591) )
	ROM_LOAD16_BYTE( "epr-12409.118", 0x000001, 0x10000, CRC(c11c8ef7) SHA1(4c1c5100d7fd728642d58e4bf45fba48695841e3) )
	ROM_LOAD16_BYTE( "epr-12412.132", 0x020000, 0x10000, CRC(b0534647) SHA1(40f2260ff0d0ac662d118cc7280bb26006ee75e9) )
	ROM_LOAD16_BYTE( "epr-12411.117", 0x020001, 0x10000, CRC(12bb0d83) SHA1(4aa1b724b2a7258fff7aa1971582950b3163c0db) )
	ROM_LOAD16_BYTE( "epr-12293.131", 0x040000, 0x10000, CRC(f4321eea) SHA1(64334acc82c14bb58b7d51719f34fd81cfb9fc6b) )
	ROM_LOAD16_BYTE( "epr-12292.116", 0x040001, 0x10000, CRC(51d98af0) SHA1(6e7115706bfafb687faa23d55d4a8c8e498a4df2) )

	ROM_REGION( 0x2000, "user1", 0 )	/* decryption key */
	ROM_LOAD( "317-unknown.key", 0x0000, 0x2000, CRC(33e632ae) SHA1(9fd8bd11d0a87ec4dfc4dc386012ab7992cb2bd7) )

	ROM_REGION( 0x60000, "sub", 0 ) /* second 68000 CPU */
	ROM_LOAD16_BYTE( "opr-12295.76", 0x000000, 0x10000, CRC(d43a3a84) SHA1(362c98f62c205b6b40b7e8a4ba107745b547b984) )
	ROM_LOAD16_BYTE( "opr-12294.58", 0x000001, 0x10000, CRC(27cdcfd3) SHA1(4fe57db95b109ab1bb1326789e06a3d3aac311cc) )
	ROM_LOAD16_BYTE( "opr-12297.75", 0x020000, 0x10000, CRC(1d9b5677) SHA1(fb6e33acc43fbc7a8d7ac44045439ecdf794fdeb) )
	ROM_LOAD16_BYTE( "opr-12296.57", 0x020001, 0x10000, CRC(0a513671) SHA1(4c13ca3a6f0aa9d06ed80798b466cca0c966a265) )

	ROM_REGION( 0x30000, "gfx1", 0 ) /* tiles */
	ROM_LOAD( "opr-12323.102", 0x00000, 0x10000, CRC(4de43a6f) SHA1(68909338e1f192ac2699c8a8d24c3f46502dd019) )
	ROM_LOAD( "opr-12324.103", 0x10000, 0x10000, CRC(24607a55) SHA1(69033f2281cd42e88233c23d809b73607fe54853) )
	ROM_LOAD( "opr-12325.104", 0x20000, 0x10000, CRC(1405137a) SHA1(367db88d36852e35c5e839f692be5ea8c8e072d2) )

	ROM_REGION32_LE( 0x100000, "gfx2", 0 ) /* sprites */
	ROM_LOAD32_BYTE( "opr-12307.9",  0x00000, 0x10000, CRC(437dcf09) SHA1(0022ee4d1c3698f77271e570cef98a8a1e5c5d6a) )
	ROM_LOAD32_BYTE( "opr-12308.10", 0x00001, 0x10000, CRC(0de70cc2) SHA1(c03f8f8cda72daf64af2878bf254840ac6dd17eb) )
	ROM_LOAD32_BYTE( "opr-12309.11", 0x00002, 0x10000, CRC(deb8c242) SHA1(c05d8ced4eafae52c4795fb1471cd66f5903d1aa) )
	ROM_LOAD32_BYTE( "opr-12310.12", 0x00003, 0x10000, CRC(45cf157e) SHA1(5d0be2a374a53ea1fe0ba2bf9b2173e96de1eb51) )
	ROM_LOAD32_BYTE( "opr-12311.13", 0x40000, 0x10000, CRC(ae2bd639) SHA1(64bb60ae7e3f87fbbce00106ba65c4e6fc1af0e4) )
	ROM_LOAD32_BYTE( "opr-12312.14", 0x40001, 0x10000, CRC(626000e7) SHA1(4a7f9e76dd76a3dc56b8257149bc94be3f4f2e87) )
	ROM_LOAD32_BYTE( "opr-12313.15", 0x40002, 0x10000, CRC(52870c37) SHA1(3a6836a46d94c0f9115800d206410252a1134c57) )
	ROM_LOAD32_BYTE( "opr-12314.16", 0x40003, 0x10000, CRC(40c461ea) SHA1(7bed8f24112dc3c827fd087138fcf2700092aa59) )
	ROM_LOAD32_BYTE( "opr-12315.17", 0x80000, 0x10000, CRC(3ff9a3a3) SHA1(0d90fe2669d03bd07a0d3b05934201778e28d54c) )
	ROM_LOAD32_BYTE( "opr-12316.18", 0x80001, 0x10000, CRC(8a1e6dc8) SHA1(32f09ec504c2b6772815bad7380a2f738f11746a) )
	ROM_LOAD32_BYTE( "opr-12317.19", 0x80002, 0x10000, CRC(77e382d4) SHA1(5b7912069a46043b7be989d82436add85497d318) )
	ROM_LOAD32_BYTE( "opr-12318.20", 0x80003, 0x10000, CRC(d1afdea9) SHA1(813eccc88d5046992be5b5a0618d32127d16e30b) )
	ROM_LOAD32_BYTE( "opr-12319.25", 0xc0000, 0x10000, CRC(df23baf9) SHA1(f9611391bb3b3b92203fa9f6dd461e3a6e863622) )
	ROM_LOAD32_BYTE( "opr-12320.22", 0xc0001, 0x10000, CRC(7931e446) SHA1(9f2161a689ebad61f6653942e23d9c2bc6170d4a) )
	ROM_LOAD32_BYTE( "opr-12321.23", 0xc0002, 0x10000, CRC(830bacd4) SHA1(5a4816969437ee1edca5845006c0b8e9ba365491) )
	ROM_LOAD32_BYTE( "opr-12322.24", 0xc0003, 0x10000, CRC(8b812492) SHA1(bf1f9e059c093c0991c7caf1b01c739ed54b8357) )

	ROM_REGION( 0x10000, "gfx3", 0 ) /* road gfx (2 identical roms, 1 for each road) */
	ROM_LOAD( "epr-12299.47", 0x0000, 0x8000, CRC(fc9bc41b) SHA1(9af73e096253cf2c4f283f227530110a4b37fcee) ) /* Manual shows both as EPR-12298 */
	ROM_LOAD( "epr-12298.11", 0x8000, 0x8000, CRC(fc9bc41b) SHA1(9af73e096253cf2c4f283f227530110a4b37fcee) )

	ROM_REGION( 0x10000, "soundcpu", 0 ) /* sound CPU */
	ROM_LOAD( "epr-12300.88", 0x00000, 0x10000, CRC(e8ff7011) SHA1(6eaf3aea507007ea31d507ed7825d905f4b8e7ab) )

	ROM_REGION( 0x80000, "pcm", ROMREGION_ERASEFF ) /* sound PCM data */
	ROM_LOAD( "opr-12301.66", 0x00000, 0x10000, CRC(6e78ad15) SHA1(c31ddf434b459cd1a381d2a028beabddd4ed10d2) )
	ROM_LOAD( "opr-12302.67", 0x10000, 0x10000, CRC(e72928af) SHA1(40e0b178958cfe97c097fe9d82b5de54bc27a29f) )
	ROM_LOAD( "opr-12303.68", 0x20000, 0x10000, CRC(8384205c) SHA1(c1f9d52bc587eab5a97867198e9aa7c19e973429) )
	ROM_LOAD( "opr-12304.69", 0x30000, 0x10000, CRC(e1762ac3) SHA1(855f06c082a17d90857e6efa3cf95b0eda0e634d) )
	ROM_LOAD( "opr-12305.70", 0x40000, 0x10000, CRC(ba9ce677) SHA1(056781f92450c902e1d279a02bda28337815cba9) )
	ROM_LOAD( "opr-12306.71", 0x50000, 0x10000, CRC(e49249fd) SHA1(ff36e4dba4e9d3d354e3dd528edeb50ad9c18ee4) )
ROM_END

/**************************************************************************************************************************
    Turbo Outrun (Cockpit version)
    CPU: FD1094 (317-????) - Manual doesn't state the FD1094 317 number??
    GAME BD   834-6919
     CPU BD   837-6905
     VIDEO BD 837-6906

    Must set Cabinet Type dipswitch (DSW-B 1&2 to both ON) to Cockpit
    Must set Turbo Switch dipswitch (DSW-B 4 to OFF) to Use Turbo Shifter
*/
ROM_START( toutrun2 )
	ROM_REGION( 0x60000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "epr-12397.133", 0x000000, 0x10000, CRC(e4b57d7d) SHA1(62be55356c82b38ebebcc87a5458e23300019339) )
	ROM_LOAD16_BYTE( "epr-12396.118", 0x000001, 0x10000, CRC(5e7115cb) SHA1(02c9ec91d9afb424e5045671ab0b5499181728c9) )
	ROM_LOAD16_BYTE( "epr-12399.132", 0x020000, 0x10000, CRC(62c77b1b) SHA1(004803c68cb1b3e414296ffbf50dc3b33b9ffb9a) )
	ROM_LOAD16_BYTE( "epr-12398.117", 0x020001, 0x10000, CRC(18e34520) SHA1(3f10ecb809106b82fd44fd6244d8d8e7f1c8e08d) )
	ROM_LOAD16_BYTE( "epr-12293.131", 0x040000, 0x10000, CRC(f4321eea) SHA1(64334acc82c14bb58b7d51719f34fd81cfb9fc6b) )
	ROM_LOAD16_BYTE( "epr-12292.116", 0x040001, 0x10000, CRC(51d98af0) SHA1(6e7115706bfafb687faa23d55d4a8c8e498a4df2) )

	ROM_REGION( 0x2000, "user1", 0 )	/* decryption key */
	ROM_LOAD( "317-unknown-toutrun1.key", 0x0000, 0x2000, NO_DUMP )	/* FD1094 number unknown and not dumped */

	ROM_REGION( 0x60000, "sub", 0 ) /* second 68000 CPU */
	ROM_LOAD16_BYTE( "opr-12295.76", 0x000000, 0x10000, CRC(d43a3a84) SHA1(362c98f62c205b6b40b7e8a4ba107745b547b984) )
	ROM_LOAD16_BYTE( "opr-12294.58", 0x000001, 0x10000, CRC(27cdcfd3) SHA1(4fe57db95b109ab1bb1326789e06a3d3aac311cc) )
	ROM_LOAD16_BYTE( "opr-12297.75", 0x020000, 0x10000, CRC(1d9b5677) SHA1(fb6e33acc43fbc7a8d7ac44045439ecdf794fdeb) )
	ROM_LOAD16_BYTE( "opr-12296.57", 0x020001, 0x10000, CRC(0a513671) SHA1(4c13ca3a6f0aa9d06ed80798b466cca0c966a265) )

	ROM_REGION( 0x30000, "gfx1", 0 ) /* tiles */
	ROM_LOAD( "opr-12323.102", 0x00000, 0x10000, CRC(4de43a6f) SHA1(68909338e1f192ac2699c8a8d24c3f46502dd019) )
	ROM_LOAD( "opr-12324.103", 0x10000, 0x10000, CRC(24607a55) SHA1(69033f2281cd42e88233c23d809b73607fe54853) )
	ROM_LOAD( "opr-12325.104", 0x20000, 0x10000, CRC(1405137a) SHA1(367db88d36852e35c5e839f692be5ea8c8e072d2) )

	ROM_REGION32_LE( 0x100000, "gfx2", 0 ) /* sprites */
	ROM_LOAD32_BYTE( "opr-12307.9",  0x00000, 0x10000, CRC(437dcf09) SHA1(0022ee4d1c3698f77271e570cef98a8a1e5c5d6a) )
	ROM_LOAD32_BYTE( "opr-12308.10", 0x00001, 0x10000, CRC(0de70cc2) SHA1(c03f8f8cda72daf64af2878bf254840ac6dd17eb) )
	ROM_LOAD32_BYTE( "opr-12309.11", 0x00002, 0x10000, CRC(deb8c242) SHA1(c05d8ced4eafae52c4795fb1471cd66f5903d1aa) )
	ROM_LOAD32_BYTE( "opr-12310.12", 0x00003, 0x10000, CRC(45cf157e) SHA1(5d0be2a374a53ea1fe0ba2bf9b2173e96de1eb51) )
	ROM_LOAD32_BYTE( "opr-12311.13", 0x40000, 0x10000, CRC(ae2bd639) SHA1(64bb60ae7e3f87fbbce00106ba65c4e6fc1af0e4) )
	ROM_LOAD32_BYTE( "opr-12312.14", 0x40001, 0x10000, CRC(626000e7) SHA1(4a7f9e76dd76a3dc56b8257149bc94be3f4f2e87) )
	ROM_LOAD32_BYTE( "opr-12313.15", 0x40002, 0x10000, CRC(52870c37) SHA1(3a6836a46d94c0f9115800d206410252a1134c57) )
	ROM_LOAD32_BYTE( "opr-12314.16", 0x40003, 0x10000, CRC(40c461ea) SHA1(7bed8f24112dc3c827fd087138fcf2700092aa59) )
	ROM_LOAD32_BYTE( "opr-12315.17", 0x80000, 0x10000, CRC(3ff9a3a3) SHA1(0d90fe2669d03bd07a0d3b05934201778e28d54c) )
	ROM_LOAD32_BYTE( "opr-12316.18", 0x80001, 0x10000, CRC(8a1e6dc8) SHA1(32f09ec504c2b6772815bad7380a2f738f11746a) )
	ROM_LOAD32_BYTE( "opr-12317.19", 0x80002, 0x10000, CRC(77e382d4) SHA1(5b7912069a46043b7be989d82436add85497d318) )
	ROM_LOAD32_BYTE( "opr-12318.20", 0x80003, 0x10000, CRC(d1afdea9) SHA1(813eccc88d5046992be5b5a0618d32127d16e30b) )
	ROM_LOAD32_BYTE( "opr-12319.25", 0xc0000, 0x10000, CRC(df23baf9) SHA1(f9611391bb3b3b92203fa9f6dd461e3a6e863622) )
	ROM_LOAD32_BYTE( "opr-12320.22", 0xc0001, 0x10000, CRC(7931e446) SHA1(9f2161a689ebad61f6653942e23d9c2bc6170d4a) )
	ROM_LOAD32_BYTE( "opr-12321.23", 0xc0002, 0x10000, CRC(830bacd4) SHA1(5a4816969437ee1edca5845006c0b8e9ba365491) )
	ROM_LOAD32_BYTE( "opr-12322.24", 0xc0003, 0x10000, CRC(8b812492) SHA1(bf1f9e059c093c0991c7caf1b01c739ed54b8357) )

	ROM_REGION( 0x10000, "gfx3", 0 ) /* road gfx (2 identical roms, 1 for each road) */
	ROM_LOAD( "epr-12299.47", 0x0000, 0x8000, CRC(fc9bc41b) SHA1(9af73e096253cf2c4f283f227530110a4b37fcee) ) /* Manual shows both as EPR-12298 */
	ROM_LOAD( "epr-12298.11", 0x8000, 0x8000, CRC(fc9bc41b) SHA1(9af73e096253cf2c4f283f227530110a4b37fcee) )

	ROM_REGION( 0x10000, "soundcpu", 0 ) /* sound CPU */
	ROM_LOAD( "epr-12300.88", 0x00000, 0x10000, CRC(e8ff7011) SHA1(6eaf3aea507007ea31d507ed7825d905f4b8e7ab) )

	ROM_REGION( 0x80000, "pcm", ROMREGION_ERASEFF ) /* sound PCM data */
	ROM_LOAD( "opr-12301.66", 0x00000, 0x10000, CRC(6e78ad15) SHA1(c31ddf434b459cd1a381d2a028beabddd4ed10d2) )
	ROM_LOAD( "opr-12302.67", 0x10000, 0x10000, CRC(e72928af) SHA1(40e0b178958cfe97c097fe9d82b5de54bc27a29f) )
	ROM_LOAD( "opr-12303.68", 0x20000, 0x10000, CRC(8384205c) SHA1(c1f9d52bc587eab5a97867198e9aa7c19e973429) )
	ROM_LOAD( "opr-12304.69", 0x30000, 0x10000, CRC(e1762ac3) SHA1(855f06c082a17d90857e6efa3cf95b0eda0e634d) )
	ROM_LOAD( "opr-12305.70", 0x40000, 0x10000, CRC(ba9ce677) SHA1(056781f92450c902e1d279a02bda28337815cba9) )
	ROM_LOAD( "opr-12306.71", 0x50000, 0x10000, CRC(e49249fd) SHA1(ff36e4dba4e9d3d354e3dd528edeb50ad9c18ee4) )
ROM_END

/**************************************************************************************************************************
    Turbo Outrun (White cockpit/sitdown Deluxe version)
    CPU: FD1094 (317-0109)
*/
ROM_START( toutrun1 )
	ROM_REGION( 0x60000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "epr-12289.133", 0x000000, 0x10000, CRC(812fd035) SHA1(7bea9ba611333dfb86cfc2e2be8cff5f700b6f71) )
	ROM_LOAD16_BYTE( "epr-12288.118", 0x000001, 0x10000, CRC(2f1151bb) SHA1(e91600d4f4e5d3d5a67cafb1ff34006f281434f1) )
	ROM_LOAD16_BYTE( "epr-12291.132", 0x020000, 0x10000, CRC(8ca284d2) SHA1(93f71ec554ab000294aaa4de9ece0eecfcfe3c46) )
	ROM_LOAD16_BYTE( "epr-12290.117", 0x020001, 0x10000, CRC(44dbf3cb) SHA1(ad867a66d97e9f5b9e14a8d05049581214a077bf) )
	ROM_LOAD16_BYTE( "epr-12293.131", 0x040000, 0x10000, CRC(f4321eea) SHA1(64334acc82c14bb58b7d51719f34fd81cfb9fc6b) )
	ROM_LOAD16_BYTE( "epr-12292.116", 0x040001, 0x10000, CRC(51d98af0) SHA1(6e7115706bfafb687faa23d55d4a8c8e498a4df2) )

	ROM_REGION( 0x2000, "user1", 0 )	/* decryption key */
	ROM_LOAD( "317-0109.key", 0x0000, 0x2000, CRC(e12a6e78) SHA1(358325490fc93bb979e9a9a296ce639d331e8b52) )

	ROM_REGION( 0x60000, "sub", 0 ) /* second 68000 CPU */
	ROM_LOAD16_BYTE( "opr-12295.76", 0x000000, 0x10000, CRC(d43a3a84) SHA1(362c98f62c205b6b40b7e8a4ba107745b547b984) )
	ROM_LOAD16_BYTE( "opr-12294.58", 0x000001, 0x10000, CRC(27cdcfd3) SHA1(4fe57db95b109ab1bb1326789e06a3d3aac311cc) )
	ROM_LOAD16_BYTE( "opr-12297.75", 0x020000, 0x10000, CRC(1d9b5677) SHA1(fb6e33acc43fbc7a8d7ac44045439ecdf794fdeb) )
	ROM_LOAD16_BYTE( "opr-12296.57", 0x020001, 0x10000, CRC(0a513671) SHA1(4c13ca3a6f0aa9d06ed80798b466cca0c966a265) )

	ROM_REGION( 0x30000, "gfx1", 0 ) /* tiles */
	ROM_LOAD( "opr-12323.102", 0x00000, 0x10000, CRC(4de43a6f) SHA1(68909338e1f192ac2699c8a8d24c3f46502dd019) )
	ROM_LOAD( "opr-12324.103", 0x10000, 0x10000, CRC(24607a55) SHA1(69033f2281cd42e88233c23d809b73607fe54853) )
	ROM_LOAD( "opr-12325.104", 0x20000, 0x10000, CRC(1405137a) SHA1(367db88d36852e35c5e839f692be5ea8c8e072d2) )

	ROM_REGION32_LE( 0x100000, "gfx2", 0 ) /* sprites */
	ROM_LOAD32_BYTE( "opr-12307.9",  0x00000, 0x10000, CRC(437dcf09) SHA1(0022ee4d1c3698f77271e570cef98a8a1e5c5d6a) )
	ROM_LOAD32_BYTE( "opr-12308.10", 0x00001, 0x10000, CRC(0de70cc2) SHA1(c03f8f8cda72daf64af2878bf254840ac6dd17eb) )
	ROM_LOAD32_BYTE( "opr-12309.11", 0x00002, 0x10000, CRC(deb8c242) SHA1(c05d8ced4eafae52c4795fb1471cd66f5903d1aa) )
	ROM_LOAD32_BYTE( "opr-12310.12", 0x00003, 0x10000, CRC(45cf157e) SHA1(5d0be2a374a53ea1fe0ba2bf9b2173e96de1eb51) )
	ROM_LOAD32_BYTE( "opr-12311.13", 0x40000, 0x10000, CRC(ae2bd639) SHA1(64bb60ae7e3f87fbbce00106ba65c4e6fc1af0e4) )
	ROM_LOAD32_BYTE( "opr-12312.14", 0x40001, 0x10000, CRC(626000e7) SHA1(4a7f9e76dd76a3dc56b8257149bc94be3f4f2e87) )
	ROM_LOAD32_BYTE( "opr-12313.15", 0x40002, 0x10000, CRC(52870c37) SHA1(3a6836a46d94c0f9115800d206410252a1134c57) )
	ROM_LOAD32_BYTE( "opr-12314.16", 0x40003, 0x10000, CRC(40c461ea) SHA1(7bed8f24112dc3c827fd087138fcf2700092aa59) )
	ROM_LOAD32_BYTE( "opr-12315.17", 0x80000, 0x10000, CRC(3ff9a3a3) SHA1(0d90fe2669d03bd07a0d3b05934201778e28d54c) )
	ROM_LOAD32_BYTE( "opr-12316.18", 0x80001, 0x10000, CRC(8a1e6dc8) SHA1(32f09ec504c2b6772815bad7380a2f738f11746a) )
	ROM_LOAD32_BYTE( "opr-12317.19", 0x80002, 0x10000, CRC(77e382d4) SHA1(5b7912069a46043b7be989d82436add85497d318) )
	ROM_LOAD32_BYTE( "opr-12318.20", 0x80003, 0x10000, CRC(d1afdea9) SHA1(813eccc88d5046992be5b5a0618d32127d16e30b) )
	ROM_LOAD32_BYTE( "opr-12319.25", 0xc0000, 0x10000, CRC(df23baf9) SHA1(f9611391bb3b3b92203fa9f6dd461e3a6e863622) )
	ROM_LOAD32_BYTE( "opr-12320.22", 0xc0001, 0x10000, CRC(7931e446) SHA1(9f2161a689ebad61f6653942e23d9c2bc6170d4a) )
	ROM_LOAD32_BYTE( "opr-12321.23", 0xc0002, 0x10000, CRC(830bacd4) SHA1(5a4816969437ee1edca5845006c0b8e9ba365491) )
	ROM_LOAD32_BYTE( "opr-12322.24", 0xc0003, 0x10000, CRC(8b812492) SHA1(bf1f9e059c093c0991c7caf1b01c739ed54b8357) )

	ROM_REGION( 0x10000, "gfx3", 0 ) /* road gfx (2 identical roms, 1 for each road) */
	ROM_LOAD( "epr-12299.47", 0x0000, 0x8000, CRC(fc9bc41b) SHA1(9af73e096253cf2c4f283f227530110a4b37fcee) ) /* Manual shows both as EPR-12298 */
	ROM_LOAD( "epr-12298.11", 0x8000, 0x8000, CRC(fc9bc41b) SHA1(9af73e096253cf2c4f283f227530110a4b37fcee) )

	ROM_REGION( 0x10000, "soundcpu", 0 ) /* sound CPU */
	ROM_LOAD( "epr-12300.88", 0x00000, 0x10000, CRC(e8ff7011) SHA1(6eaf3aea507007ea31d507ed7825d905f4b8e7ab) )

	ROM_REGION( 0x80000, "pcm", ROMREGION_ERASEFF ) /* sound PCM data */
	ROM_LOAD( "opr-12301.66", 0x00000, 0x10000, CRC(6e78ad15) SHA1(c31ddf434b459cd1a381d2a028beabddd4ed10d2) )
	ROM_LOAD( "opr-12302.67", 0x10000, 0x10000, CRC(e72928af) SHA1(40e0b178958cfe97c097fe9d82b5de54bc27a29f) )
	ROM_LOAD( "opr-12303.68", 0x20000, 0x10000, CRC(8384205c) SHA1(c1f9d52bc587eab5a97867198e9aa7c19e973429) )
	ROM_LOAD( "opr-12304.69", 0x30000, 0x10000, CRC(e1762ac3) SHA1(855f06c082a17d90857e6efa3cf95b0eda0e634d) )
	ROM_LOAD( "opr-12305.70", 0x40000, 0x10000, CRC(ba9ce677) SHA1(056781f92450c902e1d279a02bda28337815cba9) )
	ROM_LOAD( "opr-12306.71", 0x50000, 0x10000, CRC(e49249fd) SHA1(ff36e4dba4e9d3d354e3dd528edeb50ad9c18ee4) )
ROM_END


/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Super Hangon
    CPU: 68000 (317-????)
     CPU BD SUPER HANG-ON   837-6278-01 (or 837-6278-03)
     VIDEO BD SUPER HANG-ON 837-6279-01
*/
ROM_START( shangon )
	ROM_REGION( 0x60000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "epr-10886.133", 0x000000, 0x10000, CRC(8be3cd36) SHA1(de96481807e782ca441d51e99f1a221bdee7d170) )
	ROM_LOAD16_BYTE( "epr-10884.118", 0x000001, 0x10000, CRC(cb06150d) SHA1(840dada0cdeec444b554e6c1f2bdacc1047be567) )
	ROM_LOAD16_BYTE( "epr-10887.132", 0x020000, 0x10000, CRC(8d248bb0) SHA1(7d8ed61609fd0df203255e7d046d9d30983f1dcd) )
	ROM_LOAD16_BYTE( "epr-10884.117", 0x020001, 0x10000, CRC(70795f26) SHA1(332921b0a6534c4cbfe76ff957c721cc80d341b0) )

	ROM_REGION( 0x60000, "sub", 0 ) /* second 68000 CPU */
	ROM_LOAD16_BYTE( "epr-10792.76", 0x00000, 0x10000, CRC(16299d25) SHA1(b14d5feef3e6889320d51ffca36801f4c9c4d5f8) )
	ROM_LOAD16_BYTE( "epr-10790.58", 0x00001, 0x10000, CRC(2246cbc1) SHA1(c192b1ddf4c848adb564c7c87d5413d62ed650d7) )
	ROM_LOAD16_BYTE( "epr-10793.75", 0x20000, 0x10000, CRC(d9525427) SHA1(cdb24db9f7a293f20fd8becc4afe84fd6abd678a) )
	ROM_LOAD16_BYTE( "epr-10791.57", 0x20001, 0x10000, CRC(5faf4cbe) SHA1(41659a961e6469d9233849c3c587cd5a0a141344) )

	ROM_REGION( 0x18000, "gfx1", 0 ) /* tiles */
	ROM_LOAD( "epr-10652.54", 0x00000, 0x08000, CRC(260286f9) SHA1(dc7c8d2c6ef924a937328685eed19bda1c8b1819) )
	ROM_LOAD( "epr-10651.55", 0x08000, 0x08000, CRC(c609ee7b) SHA1(c6dacf81cbfe7e5df1f9a967cf571be1dcf1c429) )
	ROM_LOAD( "epr-10650.56", 0x10000, 0x08000, CRC(b236a403) SHA1(af02b8122794c083a66f2ab35d2c73b84b2df0be) )

	ROM_REGION16_BE( 0x100000, "gfx2", 0 ) /* sprites */
	/* Super Hang-On Video board 837-6279-01 (mask rom type), same data but mask roms twice the size as "EPR" counterparts */
	ROM_LOAD16_BYTE( "mpr-10794.8",	 0x000001, 0x020000, CRC(7c958e63) SHA1(ef79614e94280607a6cdf6e13db051accfd2add0) )
	ROM_LOAD16_BYTE( "mpr-10798.16", 0x000000, 0x020000, CRC(7d58f807) SHA1(783c9929d27a0270b3f7d5eb799cee6b2e5b7ae5) )
	ROM_LOAD16_BYTE( "mpr-10795.6",  0x040001, 0x020000, CRC(d9d31f8c) SHA1(3ce07b83e3aa2d8834c1a449fa31e003df5486a3) )
	ROM_LOAD16_BYTE( "mpr-10799.14", 0x040000, 0x020000, CRC(96d90d3d) SHA1(6572cbce8f98a1a7a8e59b0c502ab274f78d2819) )
	ROM_LOAD16_BYTE( "mpr-10796.4",  0x080001, 0x020000, CRC(fb48957c) SHA1(86a66bcf38686be5537a1361d390ecbbccdddc11) )
	ROM_LOAD16_BYTE( "mpr-10800.12", 0x080000, 0x020000, CRC(feaff98e) SHA1(20e38f9039079f64919d750a2e1382503d437463) )
	ROM_LOAD16_BYTE( "mpr-10797.2",  0x0c0001, 0x020000, CRC(27f2870d) SHA1(40a34e4555885bf3c6a42e472b80d11c3bd4dcba) )
	ROM_LOAD16_BYTE( "mpr-10801.10", 0x0c0000, 0x020000, CRC(12781795) SHA1(44bf6f657f32b9fab119557eb73c2fbf78700204) )

	ROM_REGION( 0x10000, "gfx3", 0 ) /* road gfx */
	ROM_LOAD( "epr-10642.47", 0x0000, 0x8000, CRC(7836bcc3) SHA1(26f308bf96224311ddf685799d7aa29aac42dd2f) )
	/* socket IC11 not populated */

	ROM_REGION( 0x10000, "soundcpu", 0 ) /* sound CPU */
	ROM_LOAD( "epr-10649c.88", 0x0000, 0x08000, CRC(f6c1ce71) SHA1(12299f7e5378a56be3a31cce3b8b74e48744f33a) )

	ROM_REGION( 0x40000, "pcm", ROMREGION_ERASEFF ) /* sound PCM data */
	ROM_LOAD( "epr-10643.66", 0x00000, 0x08000, CRC(06f55364) SHA1(fd685795e12541e3d0059d383fab293b3980d247) )
	ROM_RELOAD(               0x08000, 0x08000 )
	ROM_LOAD( "epr-10644.67", 0x10000, 0x08000, CRC(b41d541d) SHA1(28bbfa5edaa4a5901c74074354ba6f14d8f42ff6) )
	ROM_RELOAD(               0x18000, 0x08000 )
	ROM_LOAD( "epr-10645.68", 0x20000, 0x08000, CRC(a60dabff) SHA1(bbef0fb0d7837cc7efc866226bfa2bd7fab06459) )
	ROM_RELOAD(               0x28000, 0x08000 )
	ROM_LOAD( "epr-10646.69", 0x30000, 0x08000, CRC(473cc411) SHA1(04ca2d047eb59581cd5d76e0ac6eca8b19eef497) )
	ROM_RELOAD(               0x38000, 0x08000 )
ROM_END

/**************************************************************************************************************************
    Super Hangon
    CPU: FD1089B (317-0034)
     CPU BD SUPER HANG-ON   837-6278-01 (or 837-6278-03)
     VIDEO BD SUPER HANG-ON 837-6279 (or 837-6279-02, roms would be "OPR")
*/
ROM_START( shangon3 )
	ROM_REGION( 0x60000, "maincpu", 0 ) /* 68000 code - protected */
	ROM_LOAD16_BYTE( "epr-10789.133",  0x000000, 0x10000, CRC(6092c5ce) SHA1(dc010ab6d4dbbcb2f38de9f4f80674e9e1502dea) )
	ROM_LOAD16_BYTE( "epr-10788.118",  0x000001, 0x10000, CRC(c3d8a1ea) SHA1(b7f5de5e9ab9e5fb59937c11acd960f8e4a9bc2f) )
	ROM_LOAD16_BYTE( "epr-10637a.132", 0x020000, 0x10000, CRC(ad6c1308) SHA1(ee63168205bcb8b2c3dcbc3d7ba8a7f8f8a85952) )
	ROM_LOAD16_BYTE( "epr-10635a.117", 0x020001, 0x10000, CRC(a2415595) SHA1(2a8b960ea70066bf43c7b3772a0ed53d7c737b2c) )

	ROM_REGION( 0x60000, "sub", 0 ) /* second 68000 CPU */
	ROM_LOAD16_BYTE( "epr-10792.76", 0x00000, 0x10000, CRC(16299d25) SHA1(b14d5feef3e6889320d51ffca36801f4c9c4d5f8) )
	ROM_LOAD16_BYTE( "epr-10790.58", 0x00001, 0x10000, CRC(2246cbc1) SHA1(c192b1ddf4c848adb564c7c87d5413d62ed650d7) )
	ROM_LOAD16_BYTE( "epr-10793.75", 0x20000, 0x10000, CRC(d9525427) SHA1(cdb24db9f7a293f20fd8becc4afe84fd6abd678a) )
	ROM_LOAD16_BYTE( "epr-10791.57", 0x20001, 0x10000, CRC(5faf4cbe) SHA1(41659a961e6469d9233849c3c587cd5a0a141344) )

	ROM_REGION( 0x18000, "gfx1", 0 ) /* tiles */
	ROM_LOAD( "epr-10652.54", 0x00000, 0x08000, CRC(260286f9) SHA1(dc7c8d2c6ef924a937328685eed19bda1c8b1819) )
	ROM_LOAD( "epr-10651.55", 0x08000, 0x08000, CRC(c609ee7b) SHA1(c6dacf81cbfe7e5df1f9a967cf571be1dcf1c429) )
	ROM_LOAD( "epr-10650.56", 0x10000, 0x08000, CRC(b236a403) SHA1(af02b8122794c083a66f2ab35d2c73b84b2df0be) )

	ROM_REGION16_BE( 0x0e0000, "gfx2", 0 ) /* sprites */
	ROM_LOAD16_BYTE( "epr-10675.8",	 0x000001, 0x010000, CRC(d6ac012b) SHA1(305023b1a0a9d84cfc081ffc2ad7578b53d562f2) )
	ROM_LOAD16_BYTE( "epr-10682.16", 0x000000, 0x010000, CRC(d9d83250) SHA1(f8ca3197edcdf53643a5b335c3c044ddc1310cd4) )
	ROM_LOAD16_BYTE( "epr-10676.7",  0x020001, 0x010000, CRC(25ebf2c5) SHA1(abcf673ae4e280417dd9f46d18c0ec7c0e4802ae) )
	ROM_LOAD16_BYTE( "epr-10683.15", 0x020000, 0x010000, CRC(6365d2e9) SHA1(688e2ba194e859f86cd3486c2575ebae257e975a) )
	ROM_LOAD16_BYTE( "epr-10677.6",  0x040001, 0x010000, CRC(8a57b8d6) SHA1(df1a31559dd2d1e7c2c9d800bf97526bdf3e84e6) )
	ROM_LOAD16_BYTE( "epr-10684.14", 0x040000, 0x010000, CRC(3aff8910) SHA1(4b41a49a7f02363424e814b37edce9a7a44a112e) )
	ROM_LOAD16_BYTE( "epr-10678.5",  0x060001, 0x010000, CRC(af473098) SHA1(a2afaba1cbf672949dc50e407b46d7e9ae183774) )
	ROM_LOAD16_BYTE( "epr-10685.13", 0x060000, 0x010000, CRC(80bafeef) SHA1(f01bcf65485e60f34e533295a896fca0b92e5b14) )
	ROM_LOAD16_BYTE( "epr-10679.4",  0x080001, 0x010000, CRC(03bc4878) SHA1(548fc58bcc620204e30fa12fa4c4f0a3f6a1e4c0) )
	ROM_LOAD16_BYTE( "epr-10686.12", 0x080000, 0x010000, CRC(274b734e) SHA1(906fa528659bc17c9b4744cec52f7096711adce8) )
	ROM_LOAD16_BYTE( "epr-10680.3",  0x0a0001, 0x010000, CRC(9f0677ed) SHA1(5964642b70bfad418da44f2d91476f887b021f74) )
	ROM_LOAD16_BYTE( "epr-10687.11", 0x0a0000, 0x010000, CRC(508a4701) SHA1(d17aea2aadc2e2cd65d81bf91feb3ef6923d5c0b) )
	ROM_LOAD16_BYTE( "epr-10681.2",  0x0c0001, 0x010000, CRC(b176ea72) SHA1(7ec0eb0f13398d014c2e235773ded00351edb3e2) )
	ROM_LOAD16_BYTE( "epr-10688.10", 0x0c0000, 0x010000, CRC(42fcd51d) SHA1(0eacb3527dc21746e5b901fcac83f2764a0f9e2c) )

	ROM_REGION( 0x10000, "gfx3", 0 ) /* road gfx */
	ROM_LOAD( "epr-10642.47", 0x0000, 0x8000, CRC(7836bcc3) SHA1(26f308bf96224311ddf685799d7aa29aac42dd2f) )
	/* socket IC11 not populated */

	ROM_REGION( 0x10000, "soundcpu", 0 ) /* sound CPU */
	ROM_LOAD( "epr-10649a.88", 0x0000, 0x08000, CRC(bf38330f) SHA1(3d825bb02ef5a9f5c4fcaa71b3735e7f8e47f178) )

	ROM_REGION( 0x40000, "pcm", ROMREGION_ERASEFF ) /* sound PCM data */
	ROM_LOAD( "epr-10643.66", 0x00000, 0x08000, CRC(06f55364) SHA1(fd685795e12541e3d0059d383fab293b3980d247) )
	ROM_RELOAD(               0x08000, 0x08000 )
	ROM_LOAD( "epr-10644.67", 0x10000, 0x08000, CRC(b41d541d) SHA1(28bbfa5edaa4a5901c74074354ba6f14d8f42ff6) )
	ROM_RELOAD(               0x18000, 0x08000 )
	ROM_LOAD( "epr-10645.68", 0x20000, 0x08000, CRC(a60dabff) SHA1(bbef0fb0d7837cc7efc866226bfa2bd7fab06459) )
	ROM_RELOAD(               0x28000, 0x08000 )
	ROM_LOAD( "epr-10646.69", 0x30000, 0x08000, CRC(473cc411) SHA1(04ca2d047eb59581cd5d76e0ac6eca8b19eef497) )
	ROM_RELOAD(               0x38000, 0x08000 )

	ROM_REGION( 0x2000, "fd1089b", 0 ) /* decryption key */
	ROM_LOAD( "317-0034.key", 0x0000, 0x2000, CRC(263ca773) SHA1(8e80d69d61cf54fd02b0ca59dd397fa60c713f3d) )
ROM_END

/**************************************************************************************************************************
    Super Hangon
    CPU: FD1089B (317-0034)
     CPU BD SUPER HANG-ON   837-6278-01 (or 837-6278-03)
     VIDEO BD SUPER HANG-ON 837-6279 (or 837-6279-02, roms would be "OPR")
*/
ROM_START( shangon2 )
	ROM_REGION( 0x60000, "maincpu", 0 ) /* 68000 code - protected */
	ROM_LOAD16_BYTE( "epr-10636a.133", 0x000000, 0x10000, CRC(74a64f4f) SHA1(3266a9a3c68e147bc8626de7ec45b59fd28f9d1d) )
	ROM_LOAD16_BYTE( "epr-10634a.118", 0x000001, 0x10000, CRC(1608cb4a) SHA1(56b0a6a0a4951f15a269d94d18821809ac0d3d53) )
	ROM_LOAD16_BYTE( "epr-10637a.132", 0x020000, 0x10000, CRC(ad6c1308) SHA1(ee63168205bcb8b2c3dcbc3d7ba8a7f8f8a85952) )
	ROM_LOAD16_BYTE( "epr-10635a.117", 0x020001, 0x10000, CRC(a2415595) SHA1(2a8b960ea70066bf43c7b3772a0ed53d7c737b2c) )

	ROM_REGION( 0x60000, "sub", 0 ) /* second 68000 CPU */
	ROM_LOAD16_BYTE( "epr-10640.76", 0x00000, 0x10000, CRC(02be68db) SHA1(8c9f98ee49db54ee53b721ecf53f91737ae6cd73) )
	ROM_LOAD16_BYTE( "epr-10638.58", 0x00001, 0x10000, CRC(f13e8bee) SHA1(1c16c018f58f1fb49e240314a7e97a947087fad9) )
	ROM_LOAD16_BYTE( "epr-10641.75", 0x20000, 0x10000, CRC(38c3f808) SHA1(36fae99b56980ef33853170afe10b363cd41c053) )
	ROM_LOAD16_BYTE( "epr-10639.57", 0x20001, 0x10000, CRC(8cdbcde8) SHA1(0bcb4df96ee16db3dd4ce52fccd939f48a4bc1a0) )

	ROM_REGION( 0x18000, "gfx1", 0 ) /* tiles */
	ROM_LOAD( "epr-10652.54", 0x00000, 0x08000, CRC(260286f9) SHA1(dc7c8d2c6ef924a937328685eed19bda1c8b1819) )
	ROM_LOAD( "epr-10651.55", 0x08000, 0x08000, CRC(c609ee7b) SHA1(c6dacf81cbfe7e5df1f9a967cf571be1dcf1c429) )
	ROM_LOAD( "epr-10650.56", 0x10000, 0x08000, CRC(b236a403) SHA1(af02b8122794c083a66f2ab35d2c73b84b2df0be) )

	ROM_REGION16_BE( 0x0e0000, "gfx2", 0 ) /* sprites */
	ROM_LOAD16_BYTE( "epr-10675.8",	 0x000001, 0x010000, CRC(d6ac012b) SHA1(305023b1a0a9d84cfc081ffc2ad7578b53d562f2) )
	ROM_LOAD16_BYTE( "epr-10682.16", 0x000000, 0x010000, CRC(d9d83250) SHA1(f8ca3197edcdf53643a5b335c3c044ddc1310cd4) )
	ROM_LOAD16_BYTE( "epr-10676.7",  0x020001, 0x010000, CRC(25ebf2c5) SHA1(abcf673ae4e280417dd9f46d18c0ec7c0e4802ae) )
	ROM_LOAD16_BYTE( "epr-10683.15", 0x020000, 0x010000, CRC(6365d2e9) SHA1(688e2ba194e859f86cd3486c2575ebae257e975a) )
	ROM_LOAD16_BYTE( "epr-10677.6",  0x040001, 0x010000, CRC(8a57b8d6) SHA1(df1a31559dd2d1e7c2c9d800bf97526bdf3e84e6) )
	ROM_LOAD16_BYTE( "epr-10684.14", 0x040000, 0x010000, CRC(3aff8910) SHA1(4b41a49a7f02363424e814b37edce9a7a44a112e) )
	ROM_LOAD16_BYTE( "epr-10678.5",  0x060001, 0x010000, CRC(af473098) SHA1(a2afaba1cbf672949dc50e407b46d7e9ae183774) )
	ROM_LOAD16_BYTE( "epr-10685.13", 0x060000, 0x010000, CRC(80bafeef) SHA1(f01bcf65485e60f34e533295a896fca0b92e5b14) )
	ROM_LOAD16_BYTE( "epr-10679.4",  0x080001, 0x010000, CRC(03bc4878) SHA1(548fc58bcc620204e30fa12fa4c4f0a3f6a1e4c0) )
	ROM_LOAD16_BYTE( "epr-10686.12", 0x080000, 0x010000, CRC(274b734e) SHA1(906fa528659bc17c9b4744cec52f7096711adce8) )
	ROM_LOAD16_BYTE( "epr-10680.3",  0x0a0001, 0x010000, CRC(9f0677ed) SHA1(5964642b70bfad418da44f2d91476f887b021f74) )
	ROM_LOAD16_BYTE( "epr-10687.11", 0x0a0000, 0x010000, CRC(508a4701) SHA1(d17aea2aadc2e2cd65d81bf91feb3ef6923d5c0b) )
	ROM_LOAD16_BYTE( "epr-10681.2",  0x0c0001, 0x010000, CRC(b176ea72) SHA1(7ec0eb0f13398d014c2e235773ded00351edb3e2) )
	ROM_LOAD16_BYTE( "epr-10688.10", 0x0c0000, 0x010000, CRC(42fcd51d) SHA1(0eacb3527dc21746e5b901fcac83f2764a0f9e2c) )

	ROM_REGION( 0x10000, "gfx3", 0 ) /* road gfx */
	ROM_LOAD( "epr-10642.47", 0x0000, 0x8000, CRC(7836bcc3) SHA1(26f308bf96224311ddf685799d7aa29aac42dd2f) )
	/* socket IC11 not populated */

	ROM_REGION( 0x10000, "soundcpu", 0 ) /* sound CPU */
	ROM_LOAD( "ic88", 0x0000, 0x08000, CRC(1254efa6) SHA1(997770ccdd776de6e335a6d8b1e15d200cbd4410) ) /* EPR-10649 or EPR-10649B ??? */

	ROM_REGION( 0x40000, "pcm", ROMREGION_ERASEFF ) /* sound PCM data */
	ROM_LOAD( "epr-10643.66", 0x00000, 0x08000, CRC(06f55364) SHA1(fd685795e12541e3d0059d383fab293b3980d247) )
	ROM_RELOAD(               0x08000, 0x08000 )
	ROM_LOAD( "epr-10644.67", 0x10000, 0x08000, CRC(b41d541d) SHA1(28bbfa5edaa4a5901c74074354ba6f14d8f42ff6) )
	ROM_RELOAD(               0x18000, 0x08000 )
	ROM_LOAD( "epr-10645.68", 0x20000, 0x08000, CRC(a60dabff) SHA1(bbef0fb0d7837cc7efc866226bfa2bd7fab06459) )
	ROM_RELOAD(               0x28000, 0x08000 )
	ROM_LOAD( "epr-10646.69", 0x30000, 0x08000, CRC(473cc411) SHA1(04ca2d047eb59581cd5d76e0ac6eca8b19eef497) )
	ROM_RELOAD(               0x38000, 0x08000 )

	ROM_REGION( 0x2000, "fd1089b", 0 ) /* decryption key */
	ROM_LOAD( "317-0034.key", 0x0000, 0x2000, CRC(263ca773) SHA1(8e80d69d61cf54fd02b0ca59dd397fa60c713f3d) )
ROM_END

/**************************************************************************************************************************
    Super Hangon
    CPU: FD1089B (317-0034)
     CPU BD SUPER HANG-ON   837-6278-01 (or 837-6278-03)
     VIDEO BD SUPER HANG-ON 837-6279 (or 837-6279-02, roms would be "OPR")
*/
ROM_START( shangon1 )
	ROM_REGION( 0x60000, "maincpu", 0 ) /* 68000 code - protected */
	ROM_LOAD16_BYTE( "ic133", 0x000000, 0x10000, CRC(e52721fe) SHA1(21f0aa14d0cbda3d762bca86efe089646031aef5) ) /* All EPR numbers for this main CPU version are unknown */
	ROM_LOAD16_BYTE( "ic118", 0x000001, 0x10000, BAD_DUMP CRC(5fee09f6) SHA1(b97a08ba75d8c617aceff2b03c1f2bfcb16181ef) )
	ROM_LOAD16_BYTE( "ic132", 0x020000, 0x10000, CRC(5d55d65f) SHA1(d02d76b98d74746b078b0f49f0320b8be48e4c47) )
	ROM_LOAD16_BYTE( "ic117", 0x020001, 0x10000, CRC(b967e8c3) SHA1(00224b337b162daff03bbfabdcf8541025220d46) )

	ROM_REGION( 0x60000, "sub", 0 ) /* second 68000 CPU */
	ROM_LOAD16_BYTE( "epr-10640.76", 0x00000, 0x10000, CRC(02be68db) SHA1(8c9f98ee49db54ee53b721ecf53f91737ae6cd73) )
	ROM_LOAD16_BYTE( "epr-10638.58", 0x00001, 0x10000, CRC(f13e8bee) SHA1(1c16c018f58f1fb49e240314a7e97a947087fad9) )
	ROM_LOAD16_BYTE( "epr-10641.75", 0x20000, 0x10000, CRC(38c3f808) SHA1(36fae99b56980ef33853170afe10b363cd41c053) )
	ROM_LOAD16_BYTE( "epr-10639.57", 0x20001, 0x10000, CRC(8cdbcde8) SHA1(0bcb4df96ee16db3dd4ce52fccd939f48a4bc1a0) )

	ROM_REGION( 0x18000, "gfx1", 0 ) /* tiles */
	ROM_LOAD( "epr-10652.54", 0x00000, 0x08000, CRC(260286f9) SHA1(dc7c8d2c6ef924a937328685eed19bda1c8b1819) )
	ROM_LOAD( "epr-10651.55", 0x08000, 0x08000, CRC(c609ee7b) SHA1(c6dacf81cbfe7e5df1f9a967cf571be1dcf1c429) )
	ROM_LOAD( "epr-10650.56", 0x10000, 0x08000, CRC(b236a403) SHA1(af02b8122794c083a66f2ab35d2c73b84b2df0be) )

	ROM_REGION16_BE( 0x0e0000, "gfx2", 0 ) /* sprites */
	ROM_LOAD16_BYTE( "epr-10675.8",	 0x000001, 0x010000, CRC(d6ac012b) SHA1(305023b1a0a9d84cfc081ffc2ad7578b53d562f2) )
	ROM_LOAD16_BYTE( "epr-10682.16", 0x000000, 0x010000, CRC(d9d83250) SHA1(f8ca3197edcdf53643a5b335c3c044ddc1310cd4) )
	ROM_LOAD16_BYTE( "epr-10676.7",  0x020001, 0x010000, CRC(25ebf2c5) SHA1(abcf673ae4e280417dd9f46d18c0ec7c0e4802ae) )
	ROM_LOAD16_BYTE( "epr-10683.15", 0x020000, 0x010000, CRC(6365d2e9) SHA1(688e2ba194e859f86cd3486c2575ebae257e975a) )
	ROM_LOAD16_BYTE( "epr-10677.6",  0x040001, 0x010000, CRC(8a57b8d6) SHA1(df1a31559dd2d1e7c2c9d800bf97526bdf3e84e6) )
	ROM_LOAD16_BYTE( "epr-10684.14", 0x040000, 0x010000, CRC(3aff8910) SHA1(4b41a49a7f02363424e814b37edce9a7a44a112e) )
	ROM_LOAD16_BYTE( "epr-10678.5",  0x060001, 0x010000, CRC(af473098) SHA1(a2afaba1cbf672949dc50e407b46d7e9ae183774) )
	ROM_LOAD16_BYTE( "epr-10685.13", 0x060000, 0x010000, CRC(80bafeef) SHA1(f01bcf65485e60f34e533295a896fca0b92e5b14) )
	ROM_LOAD16_BYTE( "epr-10679.4",  0x080001, 0x010000, CRC(03bc4878) SHA1(548fc58bcc620204e30fa12fa4c4f0a3f6a1e4c0) )
	ROM_LOAD16_BYTE( "epr-10686.12", 0x080000, 0x010000, CRC(274b734e) SHA1(906fa528659bc17c9b4744cec52f7096711adce8) )
	ROM_LOAD16_BYTE( "epr-10680.3",  0x0a0001, 0x010000, CRC(9f0677ed) SHA1(5964642b70bfad418da44f2d91476f887b021f74) )
	ROM_LOAD16_BYTE( "epr-10687.11", 0x0a0000, 0x010000, CRC(508a4701) SHA1(d17aea2aadc2e2cd65d81bf91feb3ef6923d5c0b) )
	ROM_LOAD16_BYTE( "epr-10681.2",  0x0c0001, 0x010000, CRC(b176ea72) SHA1(7ec0eb0f13398d014c2e235773ded00351edb3e2) )
	ROM_LOAD16_BYTE( "epr-10688.10", 0x0c0000, 0x010000, CRC(42fcd51d) SHA1(0eacb3527dc21746e5b901fcac83f2764a0f9e2c) )

	ROM_REGION( 0x10000, "gfx3", 0 ) /* road gfx */
	ROM_LOAD( "epr-10642.47", 0x0000, 0x8000, CRC(7836bcc3) SHA1(26f308bf96224311ddf685799d7aa29aac42dd2f) )
	/* socket IC11 not populated */

	ROM_REGION( 0x10000, "soundcpu", 0 ) /* sound CPU */
	ROM_LOAD( "ic88", 0x0000, 0x08000, CRC(1254efa6) SHA1(997770ccdd776de6e335a6d8b1e15d200cbd4410) ) /* EPR-10649 or EPR-10649B ??? */

	ROM_REGION( 0x40000, "pcm", ROMREGION_ERASEFF ) /* sound PCM data */
	ROM_LOAD( "epr-10643.66", 0x00000, 0x08000, CRC(06f55364) SHA1(fd685795e12541e3d0059d383fab293b3980d247) )
	ROM_RELOAD(               0x08000, 0x08000 )
	ROM_LOAD( "epr-10644.67", 0x10000, 0x08000, CRC(b41d541d) SHA1(28bbfa5edaa4a5901c74074354ba6f14d8f42ff6) )
	ROM_RELOAD(               0x18000, 0x08000 )
	ROM_LOAD( "epr-10645.68", 0x20000, 0x08000, CRC(a60dabff) SHA1(bbef0fb0d7837cc7efc866226bfa2bd7fab06459) )
	ROM_RELOAD(               0x28000, 0x08000 )
	ROM_LOAD( "epr-10646.69", 0x30000, 0x08000, CRC(473cc411) SHA1(04ca2d047eb59581cd5d76e0ac6eca8b19eef497) )
	ROM_RELOAD(               0x38000, 0x08000 )

	ROM_REGION( 0x2000, "fd1089b", 0 ) /* decryption key */
	ROM_LOAD( "317-0034.key", 0x0000, 0x2000, CRC(263ca773) SHA1(8e80d69d61cf54fd02b0ca59dd397fa60c713f3d) )
ROM_END

/**************************************************************************************************************************
    Limited Edition Hangon
    CPU: 68000 (317-????)
     CPU BD SUPER HANG-ON   837-6278-01 (or 837-6278-03)
     VIDEO BD SUPER HANG-ON 837-6279 (or 837-6279-02, rom would be "OPR")
*/
ROM_START( shangonle )
	ROM_REGION( 0x60000, "maincpu", 0 ) /* 68000 code  */
	ROM_LOAD16_BYTE( "epr-13944.133", 0x000000, 0x10000, CRC(989a80db) SHA1(5026e5cf52d4fd85a0bab6c4ea7a34cf266b2a3b) )
	ROM_LOAD16_BYTE( "epr-13943.118", 0x000001, 0x10000, CRC(426e3050) SHA1(f332ea76285b4e1361d818cbe5aab0640b4185c3) )
	ROM_LOAD16_BYTE( "epr-10899.132", 0x020000, 0x10000, CRC(bb3faa37) SHA1(ccf3352255503fd6619e6e116d187a8cd1ff75e6) )
	ROM_LOAD16_BYTE( "epr-10897.117", 0x020001, 0x10000, CRC(5f087eb1) SHA1(bdfcc39e92087057acc4e91741a03e7ba57824c1) )

	ROM_REGION( 0x60000, "sub", 0 ) /* second 68000 CPU */
	ROM_LOAD16_BYTE( "epr-10640.76", 0x00000, 0x10000, CRC(02be68db) SHA1(8c9f98ee49db54ee53b721ecf53f91737ae6cd73) )
	ROM_LOAD16_BYTE( "epr-10638.58", 0x00001, 0x10000, CRC(f13e8bee) SHA1(1c16c018f58f1fb49e240314a7e97a947087fad9) )
	ROM_LOAD16_BYTE( "epr-10641.75", 0x20000, 0x10000, CRC(38c3f808) SHA1(36fae99b56980ef33853170afe10b363cd41c053) )
	ROM_LOAD16_BYTE( "epr-10639.57", 0x20001, 0x10000, CRC(8cdbcde8) SHA1(0bcb4df96ee16db3dd4ce52fccd939f48a4bc1a0) )

	ROM_REGION( 0x18000, "gfx1", 0 ) /* tiles */
	ROM_LOAD( "epr-10652.54", 0x00000, 0x08000, CRC(260286f9) SHA1(dc7c8d2c6ef924a937328685eed19bda1c8b1819) )
	ROM_LOAD( "epr-10651.55", 0x08000, 0x08000, CRC(c609ee7b) SHA1(c6dacf81cbfe7e5df1f9a967cf571be1dcf1c429) )
	ROM_LOAD( "epr-10650.56", 0x10000, 0x08000, CRC(b236a403) SHA1(af02b8122794c083a66f2ab35d2c73b84b2df0be) )

	ROM_REGION16_BE( 0x0e0000, "gfx2", 0 ) /* sprites */
	ROM_LOAD16_BYTE( "epr-10675.8",	 0x000001, 0x010000, CRC(d6ac012b) SHA1(305023b1a0a9d84cfc081ffc2ad7578b53d562f2) )
	ROM_LOAD16_BYTE( "epr-10682.16", 0x000000, 0x010000, CRC(d9d83250) SHA1(f8ca3197edcdf53643a5b335c3c044ddc1310cd4) )
	ROM_LOAD16_BYTE( "epr-13945.7",  0x020001, 0x010000, CRC(fbb1eef9) SHA1(2798df2f25706e0d3be01d945274f478d7e5a2ae) )
	ROM_LOAD16_BYTE( "epr-13946.15", 0x020000, 0x010000, CRC(03144930) SHA1(c20f4883ee2de35cd0b67949de0e41464f2c5fae) )
	ROM_LOAD16_BYTE( "epr-10677.6",  0x040001, 0x010000, CRC(8a57b8d6) SHA1(df1a31559dd2d1e7c2c9d800bf97526bdf3e84e6) )
	ROM_LOAD16_BYTE( "epr-10684.14", 0x040000, 0x010000, CRC(3aff8910) SHA1(4b41a49a7f02363424e814b37edce9a7a44a112e) )
	ROM_LOAD16_BYTE( "epr-10678.5",  0x060001, 0x010000, CRC(af473098) SHA1(a2afaba1cbf672949dc50e407b46d7e9ae183774) )
	ROM_LOAD16_BYTE( "epr-10685.13", 0x060000, 0x010000, CRC(80bafeef) SHA1(f01bcf65485e60f34e533295a896fca0b92e5b14) )
	ROM_LOAD16_BYTE( "epr-10679.4",  0x080001, 0x010000, CRC(03bc4878) SHA1(548fc58bcc620204e30fa12fa4c4f0a3f6a1e4c0) )
	ROM_LOAD16_BYTE( "epr-10686.12", 0x080000, 0x010000, CRC(274b734e) SHA1(906fa528659bc17c9b4744cec52f7096711adce8) )
	ROM_LOAD16_BYTE( "epr-10680.3",  0x0a0001, 0x010000, CRC(9f0677ed) SHA1(5964642b70bfad418da44f2d91476f887b021f74) )
	ROM_LOAD16_BYTE( "epr-10687.11", 0x0a0000, 0x010000, CRC(508a4701) SHA1(d17aea2aadc2e2cd65d81bf91feb3ef6923d5c0b) )
	ROM_LOAD16_BYTE( "epr-10681.2",  0x0c0001, 0x010000, CRC(b176ea72) SHA1(7ec0eb0f13398d014c2e235773ded00351edb3e2) )
	ROM_LOAD16_BYTE( "epr-10688.10", 0x0c0000, 0x010000, CRC(42fcd51d) SHA1(0eacb3527dc21746e5b901fcac83f2764a0f9e2c) )

	ROM_REGION( 0x10000, "gfx3", 0 ) /* Road Graphics */
	ROM_LOAD( "epr-10642.47", 0x0000, 0x8000, CRC(7836bcc3) SHA1(26f308bf96224311ddf685799d7aa29aac42dd2f) )
	/* socket IC11 not populated */

	ROM_REGION( 0x10000, "soundcpu", 0 ) /* sound CPU */
	ROM_LOAD( "epr-10649c.88", 0x0000, 0x08000, CRC(f6c1ce71) SHA1(12299f7e5378a56be3a31cce3b8b74e48744f33a) )

	ROM_REGION( 0x40000, "pcm", ROMREGION_ERASEFF ) /* sound PCM data */
	ROM_LOAD( "epr-10643.66", 0x00000, 0x08000, CRC(06f55364) SHA1(fd685795e12541e3d0059d383fab293b3980d247) )
	ROM_RELOAD(               0x08000, 0x08000 )
	ROM_LOAD( "epr-10644.67", 0x10000, 0x08000, CRC(b41d541d) SHA1(28bbfa5edaa4a5901c74074354ba6f14d8f42ff6) )
	ROM_RELOAD(               0x18000, 0x08000 )
	ROM_LOAD( "epr-10645.68", 0x20000, 0x08000, CRC(a60dabff) SHA1(bbef0fb0d7837cc7efc866226bfa2bd7fab06459) )
	ROM_RELOAD(               0x28000, 0x08000 )
	ROM_LOAD( "epr-10646.69", 0x30000, 0x08000, CRC(473cc411) SHA1(04ca2d047eb59581cd5d76e0ac6eca8b19eef497) )
	ROM_RELOAD(               0x38000, 0x08000 )
ROM_END



/*************************************
 *
 *  Generic driver initialization
 *
 *************************************/

static DRIVER_INIT( outrun )
{
	segas1x_state *state = (segas1x_state *)machine->driver_data;

	outrun_generic_init(machine);
	state->custom_io_r = outrun_custom_io_r;
	state->custom_io_w = outrun_custom_io_w;
}


static DRIVER_INIT( outrunb )
{
	segas1x_state *state = (segas1x_state *)machine->driver_data;
	static const UINT8 memory_map[] = { 0x02,0x00,0x0d,0x10,0x00,0x12,0x0c,0x13,0x08,0x14,0x0f,0x20,0x00,0x00,0x00,0x00 };
	UINT16 *word;
	UINT8 *byte;
	int i, length;

	outrun_generic_init(machine);
	state->custom_map = memory_map;
	state->custom_io_r = outrun_custom_io_r;
	state->custom_io_w = outrun_custom_io_w;

	/* main CPU: swap bits 11,12 and 6,7 */
	word = (UINT16 *)memory_region(machine, "maincpu");
	length = memory_region_length(machine, "maincpu") / 2;
	for (i = 0; i < length; i++)
		word[i] = BITSWAP16(word[i], 15,14,11,12,13,10,9,8,6,7,5,4,3,2,1,0);

	/* sub CPU: swap bits 14,15 and 2,3 */
	word = (UINT16 *)memory_region(machine, "sub");
	length = memory_region_length(machine, "sub") / 2;
	for (i = 0; i < length; i++)
		word[i] = BITSWAP16(word[i], 14,15,13,12,11,10,9,8,7,6,5,4,2,3,1,0);

	/* road gfx */
	/* rom a-2.bin: swap bits 6,7 */
	/* rom a-3.bin: swap bits 5,6 */
	byte = memory_region(machine, "gfx3");
	length = memory_region_length(machine, "gfx3") / 2;
	for (i = 0; i < length; i++)
	{
		byte[i]        = BITSWAP8(byte[i],        6,7,5,4,3,2,1,0);
		byte[i+length] = BITSWAP8(byte[i+length], 7,5,6,4,3,2,1,0);
	}

	/* Z80 code: swap bits 5,6 */
	byte = memory_region(machine, "soundcpu");
	length = memory_region_length(machine, "soundcpu");
	for (i = 0; i < length; i++)
		byte[i] = BITSWAP8(byte[i], 7,5,6,4,3,2,1,0);
}


static DRIVER_INIT( shangon )
{
	segas1x_state *state = (segas1x_state *)machine->driver_data;

	outrun_generic_init(machine);
	state->custom_io_r = shangon_custom_io_r;
	state->custom_io_w = shangon_custom_io_w;
}


static DRIVER_INIT( shangon3 )
{
	segas1x_state *state = (segas1x_state *)machine->driver_data;

	outrun_generic_init(machine);
	fd1089b_decrypt(machine);
	state->custom_io_r = shangon_custom_io_r;
	state->custom_io_w = shangon_custom_io_w;
}



/*************************************
 *
 *  Game driver(s)
 *
 *************************************/

GAMEL(1986, outrun,   0,        outrun,   outrun,   outrun,   ROT0, "Sega",    "Out Run (sitdown/upright, Rev B)", 0, layout_outrun ) /* Upright/Sitdown determined by dipswitch settings */
GAMEL(1986, outrunra, outrun,   outrun,   outrun,   outrun,   ROT0, "Sega",    "Out Run (sitdown/upright, Rev A)", 0, layout_outrun ) /* Upright/Sitdown determined by dipswitch settings */
GAMEL(1986, outruno,  outrun,   outrun,   outrun,   outrun,   ROT0, "Sega",    "Out Run (sitdown/upright)", 0, layout_outrun ) /* Upright/Sitdown determined by dipswitch settings */
GAMEL(1986, outrundx, outrun,   outrundx, outrundx, outrun,   ROT0, "Sega",    "Out Run (deluxe sitdown)", 0, layout_outrun )
GAMEL(1986, outrunb,  outrun,   outrun,   outrun,   outrunb,  ROT0, "bootleg", "Out Run (bootleg)", 0, layout_outrun )
GAME( 1987, shangon,  0,        shangon,  shangon,  shangon,  ROT0, "Sega",    "Super Hang-On (sitdown/upright, unprotected)", 0 )
GAME( 1987, shangon3, shangon,  shangon,  shangon,  shangon3, ROT0, "Sega",    "Super Hang-On (sitdown/upright, FD1089B 317-0034)", 0 )
GAME( 1987, shangon2, shangon,  shangon,  shangon,  shangon3, ROT0, "Sega",    "Super Hang-On (mini ride-on, Rev A, FD1089B 317-0034)", 0 )
GAME( 1987, shangon1, shangon,  shangon,  shangon,  shangon3, ROT0, "Sega",    "Super Hang-On (mini ride-on?, FD1089B 317-0034)", GAME_NOT_WORKING ) /* bad program rom */
GAME( 1991, shangonle,shangon,  shangon,  shangon,  shangon,  ROT0, "Sega",    "Limited Edition Hang-On", 0 )
GAMEL(1989, toutrun,  0,        outrun,   toutrun,  outrun,   ROT0, "Sega",    "Turbo Out Run (Out Run upgrade, FD1094 317-0118)" , 0, layout_outrun ) /* Cabinet determined by dipswitch settings */
GAMEL(1989, toutrun3, toutrun,  outrun,   toutrunc, outrun,   ROT0, "Sega",    "Turbo Out Run (upright, FD1094 317-unknown)", 0, layout_outrun )
GAMEL(1989, toutrun2, toutrun,  outrun,   toutrun,  outrun,   ROT0, "Sega",    "Turbo Out Run (cockpit, FD1094 317-unknown)", GAME_NOT_WORKING, layout_outrun ) /* FD1094 CPU not decrypted */
GAMEL(1989, toutrun1, toutrun,  outrun,   toutrunm, outrun,   ROT0, "Sega",    "Turbo Out Run (deluxe cockpit, FD1094 317-0109)", 0, layout_outrun )
