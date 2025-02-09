/***************************************************************************

  ay8910.c

  Emulation of the AY-3-8910 / YM2149 sound chip.

  Based on various code snippets by Ville Hallik, Michael Cuddy,
  Tatsuyuki Satoh, Fabrice Frances, Nicola Salmoria.

  Mostly rewritten by couriersud in 2008

  Games using ADSR: gyruss

  A list with more games using ADSR can be found here:
        http://mametesters.org/view.php?id=3043

  TODO:
  * The AY8930 has an extended mode which is currently
    not emulated.
  * The YMZ284 only has one 1 output channel (mixed chan A,B,C).
    This should be forced.
  * YM2610 & YM2608 will need a separate flag in their config structures
    to distinguish between legacy and discrete mode.

  The rewrite also introduces a generic model for the DAC. This model is
  not perfect, but allows channel mixing based on a parametrized approach.
  This model also allows to factor in different loads on individual channels.
  If a better model is developped in the future or better measurements are
  available, the driver should be easy to change. The model is described
  later.

  In order to not break hundreds of existing drivers by default the flag
  AY8910_LEGACY_OUTPUT is used by drivers not changed to take into account the
  new model. All outputs are normalized to the old output range (i.e. 0 .. 7ffff).
  In the case of channel mixing, output range is 0...3 * 7fff.

  The main difference between the AY-3-8910 and the YM2149 is, that the
  AY-3-8910 datasheet mentions, that fixed volume level 0, which is set by
  registers 8 to 10 is "channel off". The YM2149 mentions, that the generated
  signal has a 2V DC component. This is confirmed by measurements. The approach
  taken here is to assume the 2V DC offset for all outputs for the YM2149.
  For the AY-3-8910, an offset is used if envelope is active for a channel.
  This is backed by oscilloscope pictures from the datasheet. If a fixed volume
  is set, i.e. enveloppe is disabled, the output voltage is set to 0V. Recordings
  I found on the web for gyruss indicate, that the AY-3-8910 offset should
  be around 0.2V. This will also make sound levels more compatible with
  user observations for scramble.

  The Model:
                     5V     5V
                      |      |
                      /      |
  Volume Level x >---|       Z
                      >      Z Pullup Resistor RU
                       |     Z
                       Z     |
                    Rx Z     |
                       Z     |
                       |     |
                       '-----+-------->  >---+----> Output signal
                             |               |
                             Z               Z
               Pulldown RD   Z               Z Load RL
                             Z               Z
                             |               |
                            GND             GND

Each Volume level x will select a different resistor Rx. Measurements from fpgaarcade.com
where used to calibrate channel mixing for the YM2149. This was done using
a least square approach using a fixed RL of 1K Ohm.

For the AY measurements cited in e.g. openmsx as "Hacker Kay" for a single
channel were taken. These were normalized to 0 ... 65535 and consequently
adapted to an offset of 0.2V and a VPP of 1.3V. These measurements are in
line e.g. with the formula used by pcmenc for the volume: vol(i) = exp(i/2-7.5).

The following is documentation from the code moved here and amended to reflect
the changes done:

Careful studies of the chip output prove that the chip counts up from 0
until the counter becomes greater or equal to the period. This is an
important difference when the program is rapidly changing the period to
modulate the sound. This is worthwhile noting, since the datasheets
say, that the chip counts down.
Also, note that period = 0 is the same as period = 1. This is mentioned
in the YM2203 data sheets. However, this does NOT apply to the Envelope
period. In that case, period = 0 is half as period = 1.

Envelope shapes:
    C AtAlH
    0 0 x x  \___
    0 1 x x  /___
    1 0 0 0  \\\\
    1 0 0 1  \___
    1 0 1 0  \/\/
    1 0 1 1  \```
    1 1 0 0  ////
    1 1 0 1  /```
    1 1 1 0  /\/\
    1 1 1 1  /___

The envelope counter on the AY-3-8910 has 16 steps. On the YM2149 it
has twice the steps, happening twice as fast.

***************************************************************************/

#include "emu.h"
#include "streams.h"
#include "ay8910.h"

/*************************************
 *
 *  Defines
 *
 *************************************/

#define ENABLE_REGISTER_TEST		(0)		/* Enable preprogrammed registers */

#define MAX_OUTPUT 0x7fff
#define NUM_CHANNELS 3

/* register id's */
#define AY_AFINE	(0)
#define AY_ACOARSE	(1)
#define AY_BFINE	(2)
#define AY_BCOARSE	(3)
#define AY_CFINE	(4)
#define AY_CCOARSE	(5)
#define AY_NOISEPER	(6)
#define AY_ENABLE	(7)
#define AY_AVOL		(8)
#define AY_BVOL		(9)
#define AY_CVOL		(10)
#define AY_EFINE	(11)
#define AY_ECOARSE	(12)
#define AY_ESHAPE	(13)

#define AY_PORTA	(14)
#define AY_PORTB	(15)

#define NOISE_ENABLEQ(_psg, _chan)	(((_psg)->regs[AY_ENABLE] >> (3 + _chan)) & 1)
#define TONE_ENABLEQ(_psg, _chan)	(((_psg)->regs[AY_ENABLE] >> (_chan)) & 1)
#define TONE_PERIOD(_psg, _chan)	( (_psg)->regs[(_chan) << 1] | (((_psg)->regs[((_chan) << 1) | 1] & 0x0f) << 8) )
#define NOISE_PERIOD(_psg)			( (_psg)->regs[AY_NOISEPER] & 0x1f)
#define TONE_VOLUME(_psg, _chan)	( (_psg)->regs[AY_AVOL + (_chan)] & 0x0f)
#define TONE_ENVELOPE(_psg, _chan)	(((_psg)->regs[AY_AVOL + (_chan)] >> 4) & 1)
#define ENVELOPE_PERIOD(_psg)		(((_psg)->regs[AY_EFINE] | ((_psg)->regs[AY_ECOARSE]<<8)))

/*************************************
 *
 *  Type definitions
 *
 *************************************/

typedef struct _ay_ym_param ay_ym_param;
struct _ay_ym_param
{
	double r_up;
	double r_down;
	int    res_count;
	double res[32];
};

typedef struct _ay8910_context ay8910_context;
struct _ay8910_context
{
	running_device *device;
	int streams;
	int ready;
	sound_stream *channel;
	const ay8910_interface *intf;
	INT32 register_latch;
	UINT8 regs[16];
	INT32 last_enable;
	INT32 count[NUM_CHANNELS];
	UINT8 output[NUM_CHANNELS];
	UINT8 output_noise;
	INT32 count_noise;
	INT32 count_env;
	INT8 env_step;
	UINT32 env_volume;
	UINT8 hold,alternate,attack,holding;
	INT32 rng;
	UINT8 env_step_mask;
	/* init parameters ... */
	int step;
	int zero_is_off;
	UINT8 vol_enabled[NUM_CHANNELS];
	const ay_ym_param *par;
	const ay_ym_param *par_env;
	INT32 vol_table[NUM_CHANNELS][16];
	INT32 env_table[NUM_CHANNELS][32];
	INT32 vol3d_table[8*32*32*32];
	devcb_resolved_read8 portAread;
	devcb_resolved_read8 portBread;
	devcb_resolved_write8 portAwrite;
	devcb_resolved_write8 portBwrite;
};

INLINE ay8910_context *get_safe_token(running_device *device)
{
	assert(device != NULL);
	assert(device->token != NULL);
	assert(device->type == SOUND);
	assert(sound_get_type(device) == SOUND_AY8910 ||
		   sound_get_type(device) == SOUND_AY8912 ||
		   sound_get_type(device) == SOUND_AY8913 ||
		   sound_get_type(device) == SOUND_AY8930 ||
		   sound_get_type(device) == SOUND_YM2149 ||
		   sound_get_type(device) == SOUND_YM3439 ||
		   sound_get_type(device) == SOUND_YMZ284 ||
		   sound_get_type(device) == SOUND_YMZ294);
	return (ay8910_context *)device->token;
}


/*************************************
 *
 *  Static
 *
 *************************************/

static const ay_ym_param ym2149_param =
{
	630, 801,
	16,
	{ 73770, 37586, 27458, 21451, 15864, 12371, 8922,  6796,
	   4763,  3521,  2403,  1737,  1123,   762,  438,   251 },
};

static const ay_ym_param ym2149_param_env =
{
	630, 801,
	32,
	{ 103350, 73770, 52657, 37586, 32125, 27458, 24269, 21451,
	   18447, 15864, 14009, 12371, 10506,  8922,  7787,  6796,
	    5689,  4763,  4095,  3521,  2909,  2403,  2043,  1737,
	    1397,  1123,   925,   762,   578,   438,   332,   251 },
};

#if 0
/* RL = 1000, Hacker Kay normalized, 2.1V to 3.2V */
static const ay_ym_param ay8910_param =
{
	664, 913,
	16,
	{ 85785, 34227, 26986, 20398, 14886, 10588,  7810,  4856,
	   4120,  2512,  1737,  1335,  1005,   747,   586,    451 },
};

/*
 * RL = 3000, Hacker Kay normalized pattern, 1.5V to 2.8V
 * These values correspond with guesses based on Gyruss schematics
 * They work well with scramble as well.
 */
static const ay_ym_param ay8910_param =
{
	930, 454,
	16,
	{ 85066, 34179, 27027, 20603, 15046, 10724, 7922, 4935,
	   4189,  2557,  1772,  1363,  1028,  766,   602,  464 },
};

/*
 * RL = 1000, Hacker Kay normalized pattern, 0.75V to 2.05V
 * These values correspond with guesses based on Gyruss schematics
 * They work well with scramble as well.
 */
static const ay_ym_param ay8910_param =
{
	1371, 313,
	16,
	{ 93399, 33289, 25808, 19285, 13940, 9846,  7237,  4493,
	   3814,  2337,  1629,  1263,   962,  727,   580,   458 },
};

/*
 * RL = 1000, Hacker Kay normalized pattern, 0.2V to 1.5V
 */
static const ay_ym_param ay8910_param =
{
	5806, 300,
	16,
	{ 118996, 42698, 33105, 24770, 17925, 12678,  9331,  5807,
        4936,  3038,  2129,  1658,  1271,   969,   781,   623 }
};
#endif

/*
 * RL = 2000, Based on Matthew Westcott's measurements from Dec 2001.
 * -------------------------------------------------------------------
 *
 * http://groups.google.com/group/comp.sys.sinclair/browse_thread/thread/fb3091da4c4caf26/d5959a800cda0b5e?lnk=gst&q=Matthew+Westcott#d5959a800cda0b5e
 * After what Russell mentioned a couple of weeks back about the lack of
 * publicised measurements of AY chip volumes - I've finally got round to
 * making these readings, and I'm placing them in the public domain - so
 * anyone's welcome to use them in emulators or anything else.

 * To make the readings, I set up the chip to produce a constant voltage on
 * channel C (setting bits 2 and 5 of register 6), and varied the amplitude
 * (the low 4 bits of register 10). The voltages were measured between the
 * channel C output (pin 1) and ground (pin 6).
 *
 * Level  Voltage
 *  0     1.147
 *  1     1.162
 *  2     1.169
 *  3     1.178
 *  4     1.192
 *  5     1.213
 *  6     1.238
 *  7     1.299
 *  8     1.336
 *  9     1.457
 * 10     1.573
 * 11     1.707
 * 12     1.882
 * 13     2.06
 * 14     2.32
 * 15     2.58
 * -------------------------------------------------------------------
 *
 * The ZX spectrum output circuit was modelled in SwitcherCAD and
 * the resistor values below create the voltage levels above.
 * RD was measured on a real chip to be 8m Ohm, RU was 0.8m Ohm.
 */

static const ay_ym_param ay8910_param =
{
	800000, 8000000,
	16,
	{ 15950, 15350, 15090, 14760, 14275, 13620, 12890, 11370,
      10600,  8590,  7190,  5985,  4820,  3945,  3017,  2345 }
};

/*************************************
 *
 *  Inline
 *
 *************************************/

INLINE void build_3D_table(double rl, const ay_ym_param *par, const ay_ym_param *par_env, int normalize, double factor, int zero_is_off, INT32 *tab)
{
	int j, j1, j2, j3, e, indx;
	double rt, rw, n;
	double min = 10.0,  max = 0.0;
	double *temp;

	temp = global_alloc_array(double, 8*32*32*32);

	for (e=0; e < 8; e++)
		for (j1=0; j1 < 32; j1++)
			for (j2=0; j2 < 32; j2++)
				for (j3=0; j3 < 32; j3++)
				{
					if (zero_is_off)
					{
						n  = (j1 != 0 || (e & 0x01)) ? 1 : 0;
						n += (j2 != 0 || (e & 0x02)) ? 1 : 0;
						n += (j3 != 0 || (e & 0x04)) ? 1 : 0;
					}
					else
						n = 3.0;

					rt = n / par->r_up + 3.0 / par->r_down + 1.0 / rl;
					rw = n / par->r_up;

					rw += 1.0 / ( (e & 0x01) ? par_env->res[j1] : par->res[j1]);
					rt += 1.0 / ( (e & 0x01) ? par_env->res[j1] : par->res[j1]);
					rw += 1.0 / ( (e & 0x02) ? par_env->res[j2] : par->res[j2]);
					rt += 1.0 / ( (e & 0x02) ? par_env->res[j2] : par->res[j2]);
					rw += 1.0 / ( (e & 0x04) ? par_env->res[j3] : par->res[j3]);
					rt += 1.0 / ( (e & 0x04) ? par_env->res[j3] : par->res[j3]);

					indx = (e << 15) | (j3<<10) | (j2<<5) | j1;
					temp[indx] = rw / rt;
					if (temp[indx] < min)
						min = temp[indx];
					if (temp[indx] > max)
						max = temp[indx];
				}

	if (normalize)
	{
		for (j=0; j < 32*32*32*8; j++)
			tab[j] = MAX_OUTPUT * (((temp[j] - min)/(max-min))) * factor;
	}
	else
	{
		for (j=0; j < 32*32*32*8; j++)
			tab[j] = MAX_OUTPUT * temp[j];
	}

	/* for (e=0;e<16;e++) printf("%d %d\n",e<<10, tab[e<<10]); */

	global_free(temp);
}

INLINE void build_single_table(double rl, const ay_ym_param *par, int normalize, INT32 *tab, int zero_is_off)
{
	int j;
	double rt, rw = 0;
	double temp[32], min=10.0, max=0.0;

	for (j=0; j < par->res_count; j++)
	{
		rt = 1.0 / par->r_down + 1.0 / rl;

		rw = 1.0 / par->res[j];
		rt += 1.0 / par->res[j];

		if (!(zero_is_off && j == 0))
		{
			rw += 1.0 / par->r_up;
			rt += 1.0 / par->r_up;
		}

		temp[j] = rw / rt;
		if (temp[j] < min)
			min = temp[j];
		if (temp[j] > max)
			max = temp[j];
	}
	if (normalize)
	{
		for (j=0; j < par->res_count; j++)
			tab[j] = MAX_OUTPUT * (((temp[j] - min)/(max-min)) - 0.25) * 0.5;
	}
	else
	{
		for (j=0; j < par->res_count; j++)
			tab[j] = MAX_OUTPUT * temp[j];
	}

}

INLINE UINT16 mix_3D(ay8910_context *psg)
{
	int indx = 0, chan;

	for (chan = 0; chan < NUM_CHANNELS; chan++)
		if (TONE_ENVELOPE(psg, chan))
		{
			indx |= (1 << (chan+15)) | ( psg->vol_enabled[chan] ? psg->env_volume << (chan*5) : 0);
		}
		else
		{
			indx |= (psg->vol_enabled[chan] ? TONE_VOLUME(psg, chan) << (chan*5) : 0);
		}
	return psg->vol3d_table[indx];
}

/*************************************
 *
 * Static functions
 *
 *************************************/

static void ay8910_write_reg(ay8910_context *psg, int r, int v)
{
	//if (r >= 11 && r <= 13 ) printf("%d %x %02x\n", PSG->index, r, v);
	psg->regs[r] = v;

	switch( r )
	{
		case AY_AFINE:
		case AY_ACOARSE:
		case AY_BFINE:
		case AY_BCOARSE:
		case AY_CFINE:
		case AY_CCOARSE:
		case AY_NOISEPER:
		case AY_AVOL:
		case AY_BVOL:
		case AY_CVOL:
		case AY_EFINE:
			/* No action required */
			break;
		case AY_ECOARSE:
			#ifdef MAME_DEBUG
			if ( (v & 0x0f) > 0)
				popmessage("Write to ECoarse register detected - please inform www.mametesters.org");
			#endif
			/* No action required */
			break;
		case AY_ENABLE:
			if ((psg->last_enable == -1) ||
			    ((psg->last_enable & 0x40) != (psg->regs[AY_ENABLE] & 0x40)))
			{
				/* write out 0xff if port set to input */
				devcb_call_write8(&psg->portAwrite, 0, (psg->regs[AY_ENABLE] & 0x40) ? psg->regs[AY_PORTA] : 0xff);
			}

			if ((psg->last_enable == -1) ||
			    ((psg->last_enable & 0x80) != (psg->regs[AY_ENABLE] & 0x80)))
			{
				/* write out 0xff if port set to input */
				devcb_call_write8(&psg->portBwrite, 0, (psg->regs[AY_ENABLE] & 0x80) ? psg->regs[AY_PORTB] : 0xff);
			}

			psg->last_enable = psg->regs[AY_ENABLE];
			break;
		case AY_ESHAPE:
			#ifdef MAME_DEBUG
			if ( (v & 0x0f) > 0)
				popmessage("Write to EShape register detected - please inform www.mametesters.org");
			#endif
			psg->attack = (psg->regs[AY_ESHAPE] & 0x04) ? psg->env_step_mask : 0x00;
			if ((psg->regs[AY_ESHAPE] & 0x08) == 0)
			{
				/* if Continue = 0, map the shape to the equivalent one which has Continue = 1 */
				psg->hold = 1;
				psg->alternate = psg->attack;
			}
			else
			{
				psg->hold = psg->regs[AY_ESHAPE] & 0x01;
				psg->alternate = psg->regs[AY_ESHAPE] & 0x02;
			}
			psg->env_step = psg->env_step_mask;
			psg->holding = 0;
			psg->env_volume = (psg->env_step ^ psg->attack);
			break;
		case AY_PORTA:
			if (psg->regs[AY_ENABLE] & 0x40)
			{
				if (psg->portAwrite.write)
					devcb_call_write8(&psg->portAwrite, 0, psg->regs[AY_PORTA]);
				else
					logerror("warning - write %02x to 8910 '%s' Port A\n",psg->regs[AY_PORTA],psg->device->tag());
			}
			else
			{
				logerror("warning: write to 8910 '%s' Port A set as input - ignored\n",psg->device->tag());
			}
			break;
		case AY_PORTB:
			if (psg->regs[AY_ENABLE] & 0x80)
			{
				if (psg->portBwrite.write)
					devcb_call_write8(&psg->portBwrite, 0, psg->regs[AY_PORTB]);
				else
					logerror("warning - write %02x to 8910 '%s' Port B\n",psg->regs[AY_PORTB],psg->device->tag());
			}
			else
			{
				logerror("warning: write to 8910 '%s' Port B set as input - ignored\n",psg->device->tag());
			}
			break;
	}
}

static STREAM_UPDATE( ay8910_update )
{
	ay8910_context *psg = (ay8910_context *)param;
	stream_sample_t *buf[NUM_CHANNELS];
	int chan;

	buf[0] = outputs[0];
	buf[1] = NULL;
	buf[2] = NULL;
	if (psg->streams == NUM_CHANNELS)
	{
		buf[1] = outputs[1];
		buf[2] = outputs[2];
	}

	/* hack to prevent us from hanging when starting filtered outputs */
	if (!psg->ready)
	{
		for (chan = 0; chan < NUM_CHANNELS; chan++)
			if (buf[chan] != NULL)
				memset(buf[chan], 0, samples * sizeof(*buf[chan]));
	}

	/* The 8910 has three outputs, each output is the mix of one of the three */
	/* tone generators and of the (single) noise generator. The two are mixed */
	/* BEFORE going into the DAC. The formula to mix each channel is: */
	/* (ToneOn | ToneDisable) & (NoiseOn | NoiseDisable). */
	/* Note that this means that if both tone and noise are disabled, the output */
	/* is 1, not 0, and can be modulated changing the volume. */

	/* buffering loop */
	while (samples)
	{
		for (chan = 0; chan < NUM_CHANNELS; chan++)
		{
			psg->count[chan]++;
			if (psg->count[chan] >= TONE_PERIOD(psg, chan))
			{
				psg->output[chan] ^= 1;
				psg->count[chan] = 0;;
			}
		}

		psg->count_noise++;
		if (psg->count_noise >= NOISE_PERIOD(psg))
		{
			/* Is noise output going to change? */
			if ((psg->rng + 1) & 2)	/* (bit0^bit1)? */
			{
				psg->output_noise ^= 1;
			}

			/* The Random Number Generator of the 8910 is a 17-bit shift */
			/* register. The input to the shift register is bit0 XOR bit3 */
			/* (bit0 is the output). This was verified on AY-3-8910 and YM2149 chips. */

			/* The following is a fast way to compute bit17 = bit0^bit3. */
			/* Instead of doing all the logic operations, we only check */
			/* bit0, relying on the fact that after three shifts of the */
			/* register, what now is bit3 will become bit0, and will */
			/* invert, if necessary, bit14, which previously was bit17. */
			if (psg->rng & 1)
				psg->rng ^= 0x24000; /* This version is called the "Galois configuration". */
			psg->rng >>= 1;
			psg->count_noise = 0;
		}

		for (chan = 0; chan < NUM_CHANNELS; chan++)
		{
			psg->vol_enabled[chan] = (psg->output[chan] | TONE_ENABLEQ(psg, chan)) & (psg->output_noise | NOISE_ENABLEQ(psg, chan));
		}

		/* update envelope */
		if (psg->holding == 0)
		{
			psg->count_env++;
			if (psg->count_env >= ENVELOPE_PERIOD(psg) * psg->step )
			{
				psg->count_env = 0;
				psg->env_step--;

				/* check envelope current position */
				if (psg->env_step < 0)
				{
					if (psg->hold)
					{
						if (psg->alternate)
							psg->attack ^= psg->env_step_mask;
						psg->holding = 1;
						psg->env_step = 0;
					}
					else
					{
						/* if CountEnv has looped an odd number of times (usually 1), */
						/* invert the output. */
						if (psg->alternate && (psg->env_step & (psg->env_step_mask + 1)))
							psg->attack ^= psg->env_step_mask;

						psg->env_step &= psg->env_step_mask;
					}
				}

			}
		}
		psg->env_volume = (psg->env_step ^ psg->attack);

		if (psg->streams == 3)
		{
			for (chan = 0; chan < NUM_CHANNELS; chan++)
				if (TONE_ENVELOPE(psg,chan))
				{
					/* Envolope has no "off" state */
					*(buf[chan]++) = psg->env_table[chan][psg->vol_enabled[chan] ? psg->env_volume : 0];
				}
				else
				{
					*(buf[chan]++) = psg->vol_table[chan][psg->vol_enabled[chan] ? TONE_VOLUME(psg, chan) : 0];
				}
		}
		else
		{
			*(buf[0]++) = mix_3D(psg);
#if 0
			*(buf[0]) = (  vol_enabled[0] * psg->vol_table[psg->Vol[0]]
			             + vol_enabled[1] * psg->vol_table[psg->Vol[1]]
			             + vol_enabled[2] * psg->vol_table[psg->Vol[2]]) / psg->step;
#endif
		}
		samples--;
	}
}

static void build_mixer_table(ay8910_context *psg)
{
	int	normalize = 0;
	int	chan;

	if ((psg->intf->flags & AY8910_LEGACY_OUTPUT) != 0)
	{
		logerror("AY-3-8910/YM2149 using legacy output levels!\n");
		normalize = 1;
	}

	for (chan=0; chan < NUM_CHANNELS; chan++)
	{
		build_single_table(psg->intf->res_load[chan], psg->par, normalize, psg->vol_table[chan], psg->zero_is_off);
		build_single_table(psg->intf->res_load[chan], psg->par_env, normalize, psg->env_table[chan], 0);
	}
	/*
     * The previous implementation added all three channels up instead of averaging them.
     * The factor of 3 will force the same levels if normalizing is used.
     */
	build_3D_table(psg->intf->res_load[0], psg->par, psg->par_env, normalize, 3, psg->zero_is_off, psg->vol3d_table);
}

static void ay8910_statesave(ay8910_context *psg, running_device *device)
{
	state_save_register_device_item(device, 0, psg->register_latch);
	state_save_register_device_item_array(device, 0, psg->regs);
	state_save_register_device_item(device, 0, psg->last_enable);

	state_save_register_device_item_array(device, 0, psg->count);
	state_save_register_device_item(device, 0, psg->count_noise);
	state_save_register_device_item(device, 0, psg->count_env);

	state_save_register_device_item(device, 0, psg->env_volume);

	state_save_register_device_item_array(device, 0, psg->output);
	state_save_register_device_item(device, 0, psg->output_noise);

	state_save_register_device_item(device, 0, psg->env_step);
	state_save_register_device_item(device, 0, psg->hold);
	state_save_register_device_item(device, 0, psg->alternate);
	state_save_register_device_item(device, 0, psg->attack);
	state_save_register_device_item(device, 0, psg->holding);
	state_save_register_device_item(device, 0, psg->rng);
}

/*************************************
 *
 * Public functions
 *
 *   used by e.g. YM2203, YM2210 ...
 *
 *************************************/

void *ay8910_start_ym(void *infoptr, sound_type chip_type, running_device *device, int clock, const ay8910_interface *intf)
{
	ay8910_context *info = (ay8910_context *)infoptr;

	if (info == NULL)
		info = auto_alloc_clear(device->machine, ay8910_context);

	info->device = device;
	info->intf = intf;
	devcb_resolve_read8(&info->portAread, &intf->portAread, device);
	devcb_resolve_read8(&info->portBread, &intf->portBread, device);
	devcb_resolve_write8(&info->portAwrite, &intf->portAwrite, device);
	devcb_resolve_write8(&info->portBwrite, &intf->portBwrite, device);
	if ((info->intf->flags & AY8910_SINGLE_OUTPUT) != 0)
	{
		logerror("AY-3-8910/YM2149 using single output!\n");
		info->streams = 1;
	}
	else
		info->streams = 3;

	if (chip_type == SOUND_AY8910 || chip_type == SOUND_AY8930)
	{
		info->step = 2;
		info->par = &ay8910_param;
		info->par_env = &ay8910_param;
		info->zero_is_off = 0;		/* FIXME: Remove after verification that off=vol(0) */
		info->env_step_mask = 0x0F;
	}
	else
	{
		info->step = 1;
		info->par = &ym2149_param;
		info->par_env = &ym2149_param_env;
		info->zero_is_off = 0;
		info->env_step_mask = 0x1F;
	}

	build_mixer_table(info);

	/* The envelope is pacing twice as fast for the YM2149 as for the AY-3-8910,    */
	/* This handled by the step parameter. Consequently we use a divider of 8 here. */
	info->channel = stream_create(device, 0, info->streams, device->clock / 8, info, ay8910_update);

	ay8910_set_clock_ym(info,device->clock);
	ay8910_statesave(info, device);

	return info;
}

void ay8910_stop_ym(void *chip)
{
}

void ay8910_reset_ym(void *chip)
{
	ay8910_context *psg = (ay8910_context *)chip;
	int i;

	psg->register_latch = 0;
	psg->rng = 1;
	psg->output[0] = 0;
	psg->output[1] = 0;
	psg->output[2] = 0;
	psg->count[0] = 0;
	psg->count[1] = 0;
	psg->count[2] = 0;
	psg->count_noise = 0;
	psg->count_env = 0;
	psg->output_noise = 0x01;
	psg->last_enable = -1;	/* force a write */
	for (i = 0;i < AY_PORTA;i++)
		ay8910_write_reg(psg,i,0);
	psg->ready = 1;
#if ENABLE_REGISTER_TEST
	ay8910_write_reg(psg, AY_AFINE, 0);
	ay8910_write_reg(psg, AY_ACOARSE, 1);
	ay8910_write_reg(psg, AY_BFINE, 0);
	ay8910_write_reg(psg, AY_BCOARSE, 2);
	ay8910_write_reg(psg, AY_CFINE, 0);
	ay8910_write_reg(psg, AY_CCOARSE, 4);
	//#define AY_NOISEPER   (6)
	ay8910_write_reg(psg, AY_ENABLE, ~7);
	ay8910_write_reg(psg, AY_AVOL, 10);
	ay8910_write_reg(psg, AY_BVOL, 10);
	ay8910_write_reg(psg, AY_CVOL, 10);
	//#define AY_EFINE  (11)
	//#define AY_ECOARSE    (12)
	//#define AY_ESHAPE (13)
#endif
}

void ay8910_set_volume(running_device *device,int channel,int volume)
{
	ay8910_context *psg = get_safe_token(device);
	int ch;

	for (ch = 0; ch < psg->streams; ch++)
		if (channel == ch || psg->streams == 1 || channel == ALL_8910_CHANNELS)
			stream_set_output_gain(psg->channel, ch, volume / 100.0);
}

void ay8910_set_clock_ym(void *chip, int clock)
{
	ay8910_context *psg = (ay8910_context *)chip;
	stream_set_sample_rate(psg->channel, clock / 8 );
}

void ay8910_write_ym(void *chip, int addr, int data)
{
	ay8910_context *psg = (ay8910_context *)chip;

	if (addr & 1)
	{	/* Data port */
		int r = psg->register_latch;

		if (r > 15) return;
		if (r == AY_ESHAPE || psg->regs[r] != data)
		{
			/* update the output buffer before changing the register */
			stream_update(psg->channel);
		}

		ay8910_write_reg(psg,r,data);
	}
	else
	{	/* Register port */
		psg->register_latch = data & 0x0f;
	}
}

int ay8910_read_ym(void *chip)
{
	ay8910_context *psg = (ay8910_context *)chip;
	int r = psg->register_latch;

	if (r > 15) return 0;

	/* There are no state dependent register in the AY8910! */
	/* stream_update(psg->channel); */

	switch (r)
	{
	case AY_PORTA:
		if ((psg->regs[AY_ENABLE] & 0x40) != 0)
			logerror("warning: read from 8910 '%s' Port A set as output\n",psg->device->tag());
		/*
           even if the port is set as output, we still need to return the external
           data. Some games, like kidniki, need this to work.
         */
		if (psg->portAread.read)
			psg->regs[AY_PORTA] = devcb_call_read8(&psg->portAread, 0);
		else
			logerror("%s: warning - read 8910 '%s' Port A\n",cpuexec_describe_context(psg->device->machine),psg->device->tag());
		break;
	case AY_PORTB:
		if ((psg->regs[AY_ENABLE] & 0x80) != 0)
			logerror("warning: read from 8910 '%s' Port B set as output\n",psg->device->tag());
		if (psg->portBread.read)
			psg->regs[AY_PORTB] = devcb_call_read8(&psg->portBread, 0);
		else
			logerror("%s: warning - read 8910 '%s' Port B\n",cpuexec_describe_context(psg->device->machine),psg->device->tag());
		break;
	}
	return psg->regs[r];
}

/*************************************
 *
 * Sound Interface
 *
 *************************************/

static DEVICE_START( ay8910 )
{
	static const ay8910_interface generic_ay8910 =
	{
		AY8910_LEGACY_OUTPUT,
		AY8910_DEFAULT_LOADS,
		DEVCB_NULL, DEVCB_NULL, DEVCB_NULL, DEVCB_NULL
	};
	const ay8910_interface *intf = (device->baseconfig().static_config ? (const ay8910_interface *)device->baseconfig().static_config : &generic_ay8910);
	ay8910_start_ym(get_safe_token(device), SOUND_AY8910, device, device->clock, intf);
}

static DEVICE_START( ym2149 )
{
	static const ay8910_interface generic_ay8910 =
	{
		AY8910_LEGACY_OUTPUT,
		AY8910_DEFAULT_LOADS,
		DEVCB_NULL, DEVCB_NULL, DEVCB_NULL, DEVCB_NULL
	};
	const ay8910_interface *intf = (device->baseconfig().static_config ? (const ay8910_interface *)device->baseconfig().static_config : &generic_ay8910);
	ay8910_start_ym(get_safe_token(device), SOUND_YM2149, device, device->clock, intf);
}

static DEVICE_STOP( ay8910 )
{
	ay8910_stop_ym(get_safe_token(device));
}

static DEVICE_RESET( ay8910 )
{
	ay8910_reset_ym(get_safe_token(device));
}

DEVICE_GET_INFO( ay8910 )
{
	switch (state)
	{
		/* --- the following bits of info are returned as 64-bit signed integers --- */
		case DEVINFO_INT_TOKEN_BYTES:					info->i = sizeof(ay8910_context);				break;

		/* --- the following bits of info are returned as pointers to data or functions --- */
		case DEVINFO_FCT_START:							info->start = DEVICE_START_NAME( ay8910 );			break;
		case DEVINFO_FCT_STOP:							info->stop = DEVICE_STOP_NAME( ay8910 );			break;
		case DEVINFO_FCT_RESET:							info->reset = DEVICE_RESET_NAME( ay8910 );			break;

		/* --- the following bits of info are returned as NULL-terminated strings --- */
		case DEVINFO_STR_NAME:							strcpy(info->s, "AY-3-8910A");					break;
		case DEVINFO_STR_FAMILY:					strcpy(info->s, "PSG");							break;
		case DEVINFO_STR_VERSION:					strcpy(info->s, "1.0");							break;
		case DEVINFO_STR_SOURCE_FILE:						strcpy(info->s, __FILE__);						break;
		case DEVINFO_STR_CREDITS:					strcpy(info->s, "Copyright Nicola Salmoria and the MAME Team"); break;
	}
}

DEVICE_GET_INFO( ay8912 )
{
	switch (state)
	{
		case DEVINFO_FCT_START:							info->start = DEVICE_START_NAME( ay8910 );			break;
		case DEVINFO_STR_NAME:							strcpy(info->s, "AY-3-8912A");					break;
		default:										DEVICE_GET_INFO_CALL(ay8910);						break;
	}
}

DEVICE_GET_INFO( ay8913 )
{
	switch (state)
	{
		case DEVINFO_FCT_START:							info->start = DEVICE_START_NAME( ay8910 );			break;
		case DEVINFO_STR_NAME:							strcpy(info->s, "AY-3-8913A");					break;
		default:										DEVICE_GET_INFO_CALL(ay8910);						break;
	}
}

DEVICE_GET_INFO( ay8930 )
{
	switch (state)
	{
		case DEVINFO_FCT_START:							info->start = DEVICE_START_NAME( ay8910 );			break;
		case DEVINFO_STR_NAME:							strcpy(info->s, "AY8930");						break;
		default:										DEVICE_GET_INFO_CALL(ay8910);						break;
	}
}

DEVICE_GET_INFO( ym2149 )
{
	switch (state)
	{
		case DEVINFO_FCT_START:							info->start = DEVICE_START_NAME( ym2149 );			break;
		case DEVINFO_STR_NAME:							strcpy(info->s, "YM2149");						break;
		default:										DEVICE_GET_INFO_CALL(ay8910);						break;
	}
}

DEVICE_GET_INFO( ym3439 )
{
	switch (state)
	{
		case DEVINFO_FCT_START:							info->start = DEVICE_START_NAME( ym2149 );			break;
		case DEVINFO_STR_NAME:							strcpy(info->s, "YM3439");						break;
		default:										DEVICE_GET_INFO_CALL(ay8910);						break;
	}
}

DEVICE_GET_INFO( ymz284 )
{
	switch (state)
	{
		case DEVINFO_FCT_START:							info->start = DEVICE_START_NAME( ym2149 );			break;
		case DEVINFO_STR_NAME:							strcpy(info->s, "YMZ284");						break;
		default:										DEVICE_GET_INFO_CALL(ay8910);						break;
	}
}

DEVICE_GET_INFO( ymz294 )
{
	switch (state)
	{
		case DEVINFO_FCT_START:							info->start = DEVICE_START_NAME( ym2149 );			break;
		case DEVINFO_STR_NAME:							strcpy(info->s, "YMZ294");						break;
		default:										DEVICE_GET_INFO_CALL(ay8910);						break;
	}
}

/*************************************
 *
 * Read/Write Handlers
 *
 *************************************/

READ8_DEVICE_HANDLER( ay8910_r )
{
	return ay8910_read_ym(get_safe_token(device));
}

WRITE8_DEVICE_HANDLER( ay8910_data_address_w )
{
	/* note that directly connecting BC1 to A0 puts data on 0 and address on 1 */
	ay8910_write_ym(get_safe_token(device), ~offset & 1, data);
}

WRITE8_DEVICE_HANDLER( ay8910_address_data_w )
{
	ay8910_write_ym(get_safe_token(device), offset & 1, data);
}

WRITE8_DEVICE_HANDLER( ay8910_address_w )
{
#if ENABLE_REGISTER_TEST
	return;
#else
	ay8910_data_address_w(device, 1, data);
#endif
}

WRITE8_DEVICE_HANDLER( ay8910_data_w )
{
#if ENABLE_REGISTER_TEST
	return;
#else
	ay8910_data_address_w(device, 0, data);
#endif
}
