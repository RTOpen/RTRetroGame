#include <stdint.h>

/********************* LCD *********************/

static const uint16_t colorization_palettes[48][3][4] = {
// 0x00 - 0x1F: Game-specific colorization palettes extracted from GBC's BIOS
	{{0x7FFF, 0x01DF, 0x0112, 0x0000}, {0x7FFF, 0x7EEB, 0x001F, 0x7C00}, {0x7FFF, 0x42B5, 0x3DC8, 0x0000}},
	{{0x231F, 0x035F, 0x00F2, 0x0009}, {0x7FFF, 0x421F, 0x1CF2, 0x0000}, {0x4FFF, 0x7ED2, 0x3A4C, 0x1CE0}},
	{{0x7FFF, 0x7FFF, 0x7E8C, 0x7C00}, {0x7FFF, 0x32BF, 0x00D0, 0x0000}, {0x03ED, 0x7FFF, 0x255F, 0x0000}},
	{{0x7FFF, 0x7FFF, 0x7E8C, 0x7C00}, {0x7FFF, 0x421F, 0x1CF2, 0x0000}, {0x036A, 0x021F, 0x03FF, 0x7FFF}},
	{{0x7FFF, 0x421F, 0x1CF2, 0x0000}, {0x7FFF, 0x421F, 0x1CF2, 0x0000}, {0x7FFF, 0x03EF, 0x01D6, 0x0000}},
	{{0x7FFF, 0x421F, 0x1CF2, 0x0000}, {0x7FFF, 0x7EEB, 0x001F, 0x7C00}, {0x7FFF, 0x03EA, 0x011F, 0x0000}},
	{{0x7FFF, 0x421F, 0x1CF2, 0x0000}, {0x7FFF, 0x7EEB, 0x001F, 0x7C00}, {0x7FFF, 0x027F, 0x001F, 0x0000}},
	{{0x7FFF, 0x7E8C, 0x7C00, 0x0000}, {0x7FFF, 0x7EEB, 0x001F, 0x7C00}, {0x7FFF, 0x03FF, 0x001F, 0x0000}},
	{{0x299F, 0x001A, 0x000C, 0x0000}, {0x7C00, 0x7FFF, 0x3FFF, 0x7E00}, {0x7E74, 0x03FF, 0x0180, 0x0000}},
	{{0x7FFF, 0x01DF, 0x0112, 0x0000}, {0x7FFF, 0x7E8C, 0x7C00, 0x0000}, {0x67FF, 0x77AC, 0x1A13, 0x2D6B}},
	{{0x0000, 0x7FFF, 0x421F, 0x1CF2}, {0x0000, 0x7FFF, 0x421F, 0x1CF2}, {0x7ED6, 0x4BFF, 0x2175, 0x0000}},
	{{0x7FFF, 0x421F, 0x1CF2, 0x0000}, {0x7FFF, 0x3FFF, 0x7E00, 0x001F}, {0x7FFF, 0x7E8C, 0x7C00, 0x0000}},
	{{0x231F, 0x035F, 0x00F2, 0x0009}, {0x7FFF, 0x7EEB, 0x001F, 0x7C00}, {0x7FFF, 0x6E31, 0x454A, 0x0000}},
	{{0x7FFF, 0x421F, 0x1CF2, 0x0000}, {0x7FFF, 0x32BF, 0x00D0, 0x0000}, {0x7FFF, 0x6E31, 0x454A, 0x0000}},
	{{0x7FFF, 0x421F, 0x1CF2, 0x0000}, {0x7FFF, 0x7E8C, 0x7C00, 0x0000}, {0x7FFF, 0x1BEF, 0x0200, 0x0000}},
	{{0x7FFF, 0x7E8C, 0x7C00, 0x0000}, {0x7FFF, 0x1BEF, 0x0200, 0x0000}, {0x7FFF, 0x32BF, 0x00D0, 0x0000}},
	{{0x7FFF, 0x1BEF, 0x0200, 0x0000}, {0x7FFF, 0x7E8C, 0x7C00, 0x0000}, {0x7FFF, 0x421F, 0x1CF2, 0x0000}},
	{{0x7FFF, 0x03E0, 0x0206, 0x0120}, {0x7FFF, 0x7E8C, 0x7C00, 0x0000}, {0x7FFF, 0x421F, 0x1CF2, 0x0000}},
	{{0x7FFF, 0x1BEF, 0x0200, 0x0000}, {0x7FFF, 0x7E8C, 0x7C00, 0x0000}, {0x7FFF, 0x32BF, 0x00D0, 0x0000}},
	{{0x7FFF, 0x421F, 0x1CF2, 0x0000}, {0x7FFF, 0x1BEF, 0x0200, 0x0000}, {0x0000, 0x4200, 0x037F, 0x7FFF}},
	{{0x03FF, 0x001F, 0x000C, 0x0000}, {0x7FFF, 0x1BEF, 0x0200, 0x0000}, {0x7FFF, 0x7E8C, 0x7C00, 0x0000}},
	{{0x7FFF, 0x32BF, 0x00D0, 0x0000}, {0x7FFF, 0x7E8C, 0x7C00, 0x0000}, {0x7FFF, 0x42B5, 0x3DC8, 0x0000}},
	{{0x7FFF, 0x5294, 0x294A, 0x0000}, {0x7FFF, 0x5294, 0x294A, 0x0000}, {0x7FFF, 0x5294, 0x294A, 0x0000}},
	{{0x7FFF, 0x1BEF, 0x0200, 0x0000}, {0x7FFF, 0x7E8C, 0x7C00, 0x0000}, {0x53FF, 0x4A5F, 0x7E52, 0x0000}},
	{{0x7FFF, 0x421F, 0x1CF2, 0x0000}, {0x7FFF, 0x1BEF, 0x0200, 0x0000}, {0x7FFF, 0x7E8C, 0x7C00, 0x0000}},
	{{0x7FFF, 0x32BF, 0x00D0, 0x0000}, {0x7FFF, 0x32BF, 0x00D0, 0x0000}, {0x639F, 0x4279, 0x15B0, 0x04CB}},
	{{0x7FFF, 0x7E8C, 0x7C00, 0x0000}, {0x7FFF, 0x1BEF, 0x0200, 0x0000}, {0x7FFF, 0x03FF, 0x012F, 0x0000}},
	{{0x7FFF, 0x033F, 0x0193, 0x0000}, {0x7FFF, 0x033F, 0x0193, 0x0000}, {0x7FFF, 0x033F, 0x0193, 0x0000}},
	{{0x7FFF, 0x421F, 0x1CF2, 0x0000}, {0x7FFF, 0x7E8C, 0x7C00, 0x0000}, {0x7FFF, 0x1BEF, 0x6180, 0x0000}},
	{{0x2120, 0x8022, 0x8281, 0x1110}, {0xFF7F, 0xDF7F, 0x1201, 0x0001}, {0xFF00, 0xFF7F, 0x1F03, 0x0000}},
	{{0x7FFF, 0x32BF, 0x00D0, 0x0000}, {0x7FFF, 0x32BF, 0x00D0, 0x0000}, {0x7FFF, 0x32BF, 0x00D0, 0x0000}},
	{{0x7FFF, 0x32BF, 0x00D0, 0x0000}, {0x7FFF, 0x32BF, 0x00D0, 0x0000}, {0x7FFF, 0x32BF, 0x00D0, 0x0000}},

// 0x20 - 0x24 : Console-based colorization palettes
	{{0x0272, 0x0DCA, 0x0D45, 0x0102}, {0x0272, 0x0DCA, 0x0D45, 0x0102}, {0x0272, 0x0DCA, 0x0D45, 0x0102}}, // GB_PALETTE_DMG
	{{0x7FFF, 0x5AD6, 0x318C, 0x0000}, {0x7FFF, 0x5AD6, 0x318C, 0x0000}, {0x7FFF, 0x5AD6, 0x318C, 0x0000}}, // GB_PALETTE_MGB0
	{{0x36D5, 0x260E, 0x1D47, 0x18C4}, {0x36D5, 0x260E, 0x1D47, 0x18C4}, {0x36D5, 0x260E, 0x1D47, 0x18C4}}, // GB_PALETTE_MGB1
	{{0x6BDD, 0x3ED4, 0x1D86, 0x0860}, {0x6BDD, 0x3ED4, 0x1D86, 0x0860}, {0x6BDD, 0x3ED4, 0x1D86, 0x0860}}, // GB_PALETTE_CGB (real colors calculated in code)
	{{0x6BDD, 0x3ED4, 0x1D86, 0x0860}, {0x6BDD, 0x3ED4, 0x1D86, 0x0860}, {0x6BDD, 0x3ED4, 0x1D86, 0x0860}}, // GB_PALETTE_SGB (real colors calculated in code)

// 0x25 - 0x2F : Other misc palettes
	{{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // Bleh
};



/********************* SOUND *********************/

static const uint8_t dmgwave[16] =
{
	0xac, 0xdd, 0xda, 0x48,
	0x36, 0x02, 0xcf, 0x16,
	0x2c, 0x04, 0xe5, 0x2c,
	0xac, 0xdd, 0xda, 0x48
};

static const uint8_t cgbwave[16] =
{
	0x00, 0xff, 0x00, 0xff,
	0x00, 0xff, 0x00, 0xff,
	0x00, 0xff, 0x00, 0xff,
	0x00, 0xff, 0x00, 0xff,
};

static const uint8_t sqwave[4][8] =
{
	{  0, 0,-1, 0, 0, 0, 0, 0 },
	{  0,-1,-1, 0, 0, 0, 0, 0 },
	{ -1,-1,-1,-1, 0, 0, 0, 0 },
	{ -1, 0, 0,-1,-1,-1,-1,-1 }
};

static const int freqtab[8] =
{
	(1<<14)*2,
	(1<<14),
	(1<<14)/2,
	(1<<14)/3,
	(1<<14)/4,
	(1<<14)/5,
	(1<<14)/6,
	(1<<14)/7
};

static const uint8_t noise7[] =
{
0xfb,0xe7,0xae,0x1b,0xa6,0x2b,0x05,0xe3,
0xb6,0x4a,0x42,0x72,0xd1,0x19,0xaa,0x03,
};

static const uint8_t noise15[] =
{
0xff,0xfb,0xff,0xe7,0xff,0xaf,0xfe,0x1f,
0xfb,0xbf,0xe6,0x7f,0xaa,0xfe,0x01,0xfb,
0xfb,0xe7,0xe7,0xaf,0xae,0x1e,0x1b,0xbb,
0xa6,0x66,0x2a,0xab,0x00,0x05,0xff,0xe3,
0xff,0xb7,0xfe,0x4f,0xfa,0x5f,0xe2,0x3f,
0xb3,0x7e,0x54,0xfa,0x05,0xe3,0xe3,0xb7,
0xb6,0x4e,0x4a,0x5a,0x42,0x22,0x73,0x32,
0xd5,0x51,0x00,0x19,0xff,0xab,0xfe,0x07,
0xfb,0xef,0xe7,0x9f,0xae,0xbe,0x18,0x7b,
0xae,0xe6,0x19,0xab,0xaa,0x06,0x03,0xeb,
0xf7,0x87,0xce,0xef,0x59,0x9c,0x2a,0xb7,
0x00,0x4d,0xfe,0x53,0xfa,0x17,0xe3,0x8f,
0xb6,0xde,0x49,0x3a,0x49,0x62,0x48,0xb2,
0x4c,0x52,0x56,0x12,0x0b,0x93,0xc6,0x97,
0x68,0x8c,0x8c,0xd4,0xd5,0x05,0x01,0xe1,
0xfb,0xbb,0xe6,0x67,0xaa,0xae,0x00,0x1b,
0xff,0xa7,0xfe,0x2f,0xfb,0x1f,0xe5,0xbf,
0xa2,0x7e,0x32,0xfb,0x51,0xe4,0x1b,0xa7,
0xa6,0x2e,0x2b,0x1b,0x05,0xa5,0xe2,0x23,
0xb3,0x36,0x55,0x4a,0x00,0x43,0xfe,0x77,
0xfa,0xcf,0xe1,0x5f,0xb8,0x3e,0x6f,0x7a,
0x9c,0xe0,0xb5,0xbc,0x42,0x76,0x72,0xca,
0xd1,0x41,0x18,0x79,0xae,0xea,0x19,0x83,
0xaa,0xf6,0x01,0xcb,0xfb,0x47,0xe4,0x6f,
0xa6,0x9e,0x28,0xbb,0x0c,0x65,0xd6,0xa3,
0x08,0x35,0xcf,0x43,0x5c,0x74,0x36,0xc7,
0x49,0x6c,0x48,0x96,0x4c,0x8a,0x54,0xc2,
0x05,0x73,0xe0,0xd7,0xbd,0x0e,0x71,0xda,
0xdb,0x21,0x25,0x39,0x21,0x69,0x38,0x89,
0x6c,0xc8,0x95,0x4c,0x80,0x54,0xfe,0x05,
0xfb,0xe3,0xe7,0xb7,0xae,0x4e,0x1a,0x5b,
0xa2,0x26,0x33,0x2b,0x55,0x04,0x01,0xe7,
0xfb,0xaf,0xe6,0x1f,0xab,0xbe,0x06,0x7b,
0xea,0xe7,0x81,0xae,0xfa,0x19,0xe3,0xab,
0xb6,0x06,0x4b,0xea,0x47,0x82,0x6e,0xf2,
0x99,0xd0,0xab,0x1c,0x05,0xb7,0xe2,0x4f,
0xb2,0x5e,0x52,0x3a,0x13,0x63,0x94,0xb6,
0x84,0x48,0xe6,0x4d,0xaa,0x52,0x02,0x13,
0xf3,0x97,0xd6,0x8f,0x08,0xdd,0xcd,0x33,
0x51,0x54,0x18,0x07,0xaf,0xee,0x1f,0x9b,
0xbe,0xa6,0x78,0x2a,0xef,0x01,0x9d,0xfa,
0xb3,0xe0,0x57,0xbe,0x0e,0x7b,0xda,0xe7,
0x21,0xad,0x3a,0x11,0x63,0x98,0xb6,0xac,
0x48,0x16,0x4f,0x8a,0x5e,0xc2,0x39,0x73,
0x68,0xd4,0x8d,0x04,0xd1,0xe5,0x1b,0xa1,
0xa6,0x3a,0x2b,0x63,0x04,0xb5,0xe4,0x43,
0xa6,0x76,0x2a,0xcb,0x01,0x45,0xf8,0x63,
0xee,0xb7,0x98,0x4e,0xae,0x58,0x1a,0x2f,
0xa3,0x1e,0x35,0xbb,0x42,0x64,0x72,0xa6,
0xd0,0x29,0x1f,0x09,0xbd,0xca,0x73,0x42,
0xd4,0x71,0x06,0xd9,0xe9,0x2b,0x89,0x06,
0xc9,0xe9,0x4b,0x88,0x46,0xce,0x69,0x5a,
0x88,0x20,0xcf,0x3d,0x5d,0x70,0x30,0xdf,
0x5d,0x3c,0x31,0x77,0x58,0xcc,0x2d,0x57,
0x10,0x0d,0x9f,0xd2,0xbf,0x10,0x7d,0x9e,
0xf2,0xb9,0xd0,0x6b,0x1e,0x85,0xb8,0xe2,
0x6d,0xb2,0x92,0x50,0x92,0x1c,0x93,0xb4,
0x96,0x44,0x8a,0x64,0xc2,0xa5,0x70,0x20,
0xdf,0x3d,0x3d,0x71,0x70,0xd8,0xdd,0x2d,
0x31,0x11,0x59,0x98,0x2a,0xaf,0x00,0x1d,
0xff,0xb3,0xfe,0x57,0xfa,0x0f,0xe3,0xdf,
0xb7,0x3e,0x4d,0x7a,0x50,0xe2,0x1d,0xb3,
0xb2,0x56,0x52,0x0a,0x13,0xc3,0x97,0x76,
0x8c,0xc8,0xd5,0x4d,0x00,0x51,0xfe,0x1b,
0xfb,0xa7,0xe6,0x2f,0xab,0x1e,0x05,0xbb,
0xe2,0x67,0xb2,0xae,0x50,0x1a,0x1f,0xa3,
0xbe,0x36,0x7b,0x4a,0xe4,0x41,0xa6,0x7a,
0x2a,0xe3,0x01,0xb5,0xfa,0x43,0xe2,0x77,
0xb2,0xce,0x51,0x5a,0x18,0x23,0xaf,0x36,
0x1d,0x4b,0xb0,0x46,0x5e,0x6a,0x3a,0x83,
0x60,0xf4,0xbd,0xc4,0x73,0x66,0xd4,0xa9,
0x04,0x09,0xe7,0xcb,0xaf,0x46,0x1c,0x6b,
0xb6,0x86,0x48,0xea,0x4d,0x82,0x52,0xf2,
0x11,0xd3,0x9b,0x16,0xa5,0x88,0x22,0xcf,
0x31,0x5d,0x58,0x30,0x2f,0x5f,0x1c,0x3d,
0xb7,0x72,0x4c,0xd2,0x55,0x12,0x01,0x93,
0xfa,0x97,0xe0,0x8f,0xbc,0xde,0x75,0x3a,
0xc1,0x61,0x78,0xb8,0xec,0x6d,0x96,0x92,
0x88,0x90,0xcc,0x9d,0x54,0xb0,0x04,0x5f,
0xe6,0x3f,0xab,0x7e,0x04,0xfb,0xe5,0xe7,
0xa3,0xae,0x36,0x1b,0x4b,0xa4,0x46,0x26,
0x6b,0x2a,0x85,0x00,0xe1,0xfd,0xbb,0xf2,
0x67,0xd2,0xaf,0x10,0x1d,0x9f,0xb2,0xbe,
0x50,0x7a,0x1e,0xe3,0xb9,0xb6,0x6a,0x4a,
0x82,0x40,0xf2,0x7d,0xd2,0xf3,0x11,0xd5,
0x9b,0x02,0xa5,0xf0,0x23,0xdf,0x37,0x3d,
0x4d,0x70,0x50,0xde,0x1d,0x3b,0xb1,0x66,
0x58,0xaa,0x2c,0x03,0x17,0xf5,0x8f,0xc2,
0xdf,0x71,0x3c,0xd9,0x75,0x28,0xc1,0x0d,
0x79,0xd0,0xeb,0x1d,0x85,0xb2,0xe2,0x51,
0xb2,0x1a,0x53,0xa2,0x16,0x33,0x8b,0x56,
0xc4,0x09,0x67,0xc8,0xaf,0x4c,0x1c,0x57,
0xb6,0x0e,0x4b,0xda,0x47,0x22,0x6d,0x32,
0x91,0x50,0x98,0x1c,0xaf,0xb4,0x1e,0x47,
0xba,0x6e,0x62,0x9a,0xb0,0xa0,0x5c,0x3e,
0x37,0x7b,0x4c,0xe4,0x55,0xa6,0x02,0x2b,
0xf3,0x07,0xd5,0xef,0x03,0x9d,0xf6,0xb3,
0xc8,0x57,0x4e,0x0c,0x5b,0xd6,0x27,0x0b,
0x2d,0xc5,0x13,0x61,0x94,0xba,0x84,0x60,
0xe6,0xbd,0xa8,0x72,0x0e,0xd3,0xd9,0x17,
0x29,0x8d,0x0a,0xd1,0xc1,0x1b,0x79,0xa4,
0xea,0x25,0x83,0x22,0xf5,0x31,0xc1,0x5b,
0x78,0x24,0xef,0x25,0x9d,0x22,0xb1,0x30,
0x59,0x5e,0x28,0x3b,0x0f,0x65,0xdc,0xa3,
0x34,0x35,0x47,0x40,0x6c,0x7e,0x96,0xf8,
0x89,0xec,0xcb,0x95,0x46,0x80,0x68,0xfe,
0x8d,0xf8,0xd3,0xed,0x17,0x91,0x8e,0x9a,
0xd8,0xa1,0x2c,0x39,0x17,0x69,0x8c,0x8a,
0xd4,0xc1,0x05,0x79,0xe0,0xeb,0xbd,0x86,
0x72,0xea,0xd1,0x81,0x1a,0xf9,0xa1,0xea,
0x3b,0x83,0x66,0xf4,0xa9,0xc4,0x0b,0x67,
0xc4,0xaf,0x64,0x1c,0xa7,0xb4,0x2e,0x47,
0x1a,0x6d,0xa2,0x92,0x30,0x93,0x5c,0x94,
0x34,0x87,0x44,0xec,0x65,0x96,0xa2,0x88,
0x30,0xcf,0x5d,0x5c,0x30,0x37,0x5f,0x4c,
0x3c,0x57,0x76,0x0c,0xcb,0xd5,0x47,0x00,
0x6d,0xfe,0x93,0xf8,0x97,0xec,0x8f,0x94,
0xde,0x85,0x38,0xe1,0x6d,0xb8,0x92,0x6c,
0x92,0x94,0x90,0x84,0x9c,0xe4,0xb5,0xa4,
0x42,0x26,0x73,0x2a,0xd5,0x01,0x01,0xf9,
0xfb,0xeb,0xe7,0x87,0xae,0xee,0x19,0x9b,
0xaa,0xa6,0x00,0x2b,0xff,0x07,0xfd,0xef,
0xf3,0x9f,0xd6,0xbf,0x08,0x7d,0xce,0xf3,
0x59,0xd4,0x2b,0x07,0x05,0xed,0xe3,0x93,
0xb6,0x96,0x48,0x8a,0x4c,0xc2,0x55,0x72,
0x00,0xd3,0xfd,0x17,0xf1,0x8f,0xda,0xdf,
0x21,0x3d,0x39,0x71,0x68,0xd8,0x8d,0x2c,
0xd1,0x15,0x19,0x81,0xaa,0xfa,0x01,0xe3,
0xfb,0xb7,0xe6,0x4f,0xaa,0x5e,0x02,0x3b,
0xf3,0x67,0xd4,0xaf,0x04,0x1d,0xe7,0xb3,
0xae,0x56,0x1a,0x0b,0xa3,0xc6,0x37,0x6b,
0x4c,0x84,0x54,0xe6,0x05,0xab,0xe2,0x07,
0xb3,0xee,0x57,0x9a,0x0e,0xa3,0xd8,0x37,
0x2f,0x4d,0x1c,0x51,0xb6,0x1a,0x4b,0xa2,
0x46,0x32,0x6b,0x52,0x84,0x10,0xe7,0x9d,
0xae,0xb2,0x18,0x53,0xae,0x16,0x1b,0x8b,
0xa6,0xc6,0x29,0x6b,0x08,0x85,0xcc,0xe3,
0x55,0xb4,0x02,0x47,0xf2,0x6f,0xd2,0x9f,
0x10,0xbd,0x9c,0x72,0xb6,0xd0,0x49,0x1e,
0x49,0xba,0x4a,0x62,0x42,0xb2,0x70,0x52,
0xde,0x11,0x3b,0x99,0x66,0xa8,0xa8,0x0c,
0x0f,0xd7,0xdf,0x0f,0x3d,0xdd,0x73,0x30,
0xd5,0x5d,0x00,0x31,0xff,0x5b,0xfc,0x27,
0xf7,0x2f,0xcd,0x1f,0x51,0xbc,0x1a,0x77,
0xa2,0xce,0x31,0x5b,0x58,0x24,0x2f,0x27,
0x1d,0x2d,0xb1,0x12,0x59,0x92,0x2a,0x93,
0x00,0x95,0xfc,0x83,0xf4,0xf7,0xc5,0xcf,
0x63,0x5c,0xb4,0x34,0x47,0x46,0x6c,0x6a,
0x96,0x80,0x88,0xfc,0xcd,0xf5,0x53,0xc0,
0x17,0x7f,0x8c,0xfe,0xd5,0xf9,0x03,0xe9,
0xf7,0x8b,0xce,0xc7,0x59,0x6c,0x28,0x97,
0x0c,0x8d,0xd4,0xd3,0x05,0x15,0xe1,0x83,
0xba,0xf6,0x61,0xca,0xbb,0x40,0x64,0x7e,
0xa6,0xf8,0x29,0xef,0x0b,0x9d,0xc6,0xb3,
0x68,0x54,0x8e,0x04,0xdb,0xe5,0x27,0xa1,
0x2e,0x39,0x1b,0x69,0xa4,0x8a,0x24,0xc3,
0x25,0x75,0x20,0xc1,0x3d,0x79,0x70,0xe8,
0xdd,0x8d,0x32,0xd1,0x51,0x18,0x19,0xaf,
0xaa,0x1e,0x03,0xbb,0xf6,0x67,0xca,0xaf,
0x40,0x1c,0x7f,0xb6,0xfe,0x49,0xfa,0x4b,
0xe2,0x47,0xb2,0x6e,0x52,0x9a,0x10,0xa3,
0x9c,0x36,0xb7,0x48,0x4c,0x4e,0x56,0x5a,
0x0a,0x23,0xc3,0x37,0x75,0x4c,0xc0,0x55,
0x7e,0x00,0xfb,0xfd,0xe7,0xf3,0xaf,0xd6,
0x1f,0x0b,0xbd,0xc6,0x73,0x6a,0xd4,0x81,
0x04,0xf9,0xe5,0xeb,0xa3,0x86,0x36,0xeb,
0x49,0x84,0x4a,0xe6,0x41,0xaa,0x7a,0x02,
0xe3,0xf1,0xb7,0xda,0x4f,0x22,0x5d,0x32,
0x31,0x53,0x58,0x14,0x2f,0x87,0x1e,0xed,
0xb9,0x92,0x6a,0x92,0x80,0x90,0xfc,0x9d,
0xf4,0xb3,0xc4,0x57,0x66,0x0c,0xab,0xd4,
0x07,0x07,0xed,0xef,0x93,0x9e,0x96,0xb8,
0x88,0x6c,0xce,0x95,0x58,0x80,0x2c,0xff,
0x15,0xfd,0x83,0xf2,0xf7,0xd1,0xcf,0x1b,
0x5d,0xa4,0x32,0x27,0x53,0x2c,0x15,0x17,
0x81,0x8e,0xfa,0xd9,0xe1,0x2b,0xb9,0x06,
0x69,0xea,0x8b,0x80,0xc6,0xfd,0x69,0xf0,
0x8b,0xdc,0xc7,0x35,0x6d,0x40,0x90,0x7c,
0x9e,0xf4,0xb9,0xc4,0x6b,0x66,0x84,0xa8,
0xe4,0x0d,0xa7,0xd2,0x2f,0x13,0x1d,0x95,
0xb2,0x82,0x50,0xf2,0x1d,0xd3,0xb3,0x16,
0x55,0x8a,0x02,0xc3,0xf1,0x77,0xd8,0xcf,
0x2d,0x5d,0x10,0x31,0x9f,0x5a,0xbc,0x20,
0x77,0x3e,0xcd,0x79,0x50,0xe8,0x1d,0x8f,
0xb2,0xde,0x51,0x3a,0x19,0x63,0xa8,0xb6,
0x0c,0x4b,0xd6,0x47,0x0a,0x6d,0xc2,0x93,
0x70,0x94,0xdc,0x85,0x34,0xe1,0x45,0xb8,
0x62,0x6e,0xb2,0x98,0x50,0xae,0x1c,0x1b,
0xb7,0xa6,0x4e,0x2a,0x5b,0x02,0x25,0xf3,
0x23,0xd5,0x37,0x01,0x4d,0xf8,0x53,0xee,
0x17,0x9b,0x8e,0xa6,0xd8,0x29,0x2f,0x09,
0x1d,0xc9,0xb3,0x4a,0x54,0x42,0x06,0x73,
0xea,0xd7,0x81,0x0e,0xf9,0xd9,0xeb,0x2b,
0x85,0x06,0xe1,0xe9,0xbb,0x8a,0x66,0xc2,
0xa9,0x70,0x08,0xdf,0xcd,0x3f,0x51,0x7c,
0x18,0xf7,0xad,0xce,0x13,0x5b,0x94,0x26,
0x87,0x28,0xed,0x0d,0x91,0xd2,0x9b,0x10,
0xa5,0x9c,0x22,0xb7,0x30,0x4d,0x5e,0x50,
0x3a,0x1f,0x63,0xbc,0xb6,0x74,0x4a,0xc6,
0x41,0x6a,0x78,0x82,0xec,0xf1,0x95,0xda,
0x83,0x20,0xf5,0x3d,0xc1,0x73,0x78,0xd4,
0xed,0x05,0x91,0xe2,0x9b,0xb0,0xa6,0x5c,
0x2a,0x37,0x03,0x4d,0xf4,0x53,0xc6,0x17,
0x6b,0x8c,0x86,0xd4,0xe9,0x05,0x89,0xe2,
0xcb,0xb1,0x46,0x58,0x6a,0x2e,0x83,0x18,
0xf5,0xad,0xc2,0x13,0x73,0x94,0xd6,0x85,
0x08,0xe1,0xcd,0xbb,0x52,0x64,0x12,0xa7,
0x90,0x2e,0x9f,0x18,0xbd,0xac,0x72,0x16,
0xd3,0x89,0x16,0xc9,0x89,0x4a,0xc8,0x41,
0x4e,0x78,0x5a,0xee,0x21,0x9b,0x3a,0xa5,
0x60,0x20,0xbf,0x3c,0x7d,0x76,0xf0,0xc9,
0xdd,0x4b,0x30,0x45,0x5e,0x60,0x3a,0xbf,
0x60,0x7c,0xbe,0xf4,0x79,0xc6,0xeb,0x69,
0x84,0x8a,0xe4,0xc1,0xa5,0x7a,0x20,0xe3,
0x3d,0xb5,0x72,0x40,0xd2,0x7d,0x12,0xf1,
0x91,0xda,0x9b,0x20,0xa5,0x3c,0x21,0x77,
0x38,0xcd,0x6d,0x50,0x90,0x1c,0x9f,0xb4,
0xbe,0x44,0x7a,0x66,0xe2,0xa9,0xb0,0x0a,
0x5f,0xc2,0x3f,0x73,0x7c,0xd4,0xf5,0x05,
0xc1,0xe3,0x7b,0xb4,0xe6,0x45,0xaa,0x62,
0x02,0xb3,0xf0,0x57,0xde,0x0f,0x3b,0xdd,
0x67,0x30,0xad,0x5c,0x10,0x37,0x9f,0x4e,
0xbc,0x58,0x76,0x2e,0xcb,0x19,0x45,0xa8,
0x62,0x0e,0xb3,0xd8,0x57,0x2e,0x0d,0x1b,
0xd1,0xa7,0x1a,0x2d,0xa3,0x12,0x35,0x93,
0x42,0x94,0x70,0x86,0xdc,0xe9,0x35,0x89,
0x42,0xc8,0x71,0x4e,0xd8,0x59,0x2e,0x29,
0x1b,0x09,0xa5,0xca,0x23,0x43,0x34,0x75,
0x46,0xc0,0x69,0x7e,0x88,0xf8,0xcd,0xed,
0x53,0x90,0x16,0x9f,0x88,0xbe,0xcc,0x79,
0x56,0xe8,0x09,0x8f,0xca,0xdf,0x41,0x3c,
0x79,0x76,0xe8,0xc9,0x8d,0x4a,0xd0,0x41,
0x1e,0x79,0xba,0xea,0x61,0x82,0xba,0xf0,
0x61,0xde,0xbb,0x38,0x65,0x6e,0xa0,0x98,
0x3c,0xaf,0x74,0x1c,0xc7,0xb5,0x6e,0x40,
0x9a,0x7c,0xa2,0xf4,0x31,0xc7,0x5b,0x6c,
0x24,0x97,0x24,0x8d,0x24,0xd1,0x25,0x19,
0x21,0xa9,0x3a,0x09,0x63,0xc8,0xb7,0x4c,
0x4c,0x56,0x56,0x0a,0x0b,0xc3,0xc7,0x77,
0x6c,0xcc,0x95,0x54,0x80,0x04,0xff,0xe5,
0xff,0xa3,0xfe,0x37,0xfb,0x4f,0xe4,0x5f,
0xa6,0x3e,0x2b,0x7b,0x04,0xe5,0xe5,0xa3,
0xa2,0x36,0x33,0x4b,0x54,0x44,0x06,0x67,
0xea,0xaf,0x80,0x1e,0xff,0xb9,0xfe,0x6b,
0xfa,0x87,0xe0,0xef,0xbd,0x9e,0x72,0xba,
0xd0,0x61,0x1e,0xb9,0xb8,0x6a,0x6e,0x82,
0x98,0xf0,0xad,0xdc,0x13,0x37,0x95,0x4e,
0x80,0x58,0xfe,0x2d,0xfb,0x13,0xe5,0x97,
0xa2,0x8e,0x30,0xdb,0x5d,0x24,0x31,0x27,
0x59,0x2c,0x29,0x17,0x09,0x8d,0xca,0xd3,
0x41,0x14,0x79,0x86,0xea,0xe9,0x81,0x8a,
0xfa,0xc1,0xe1,0x7b,0xb8,0xe6,0x6d,0xaa,
0x92,0x00,0x93,0xfc,0x97,0xf4,0x8f,0xc4,
0xdf,0x65,0x3c,0xa1,0x74,0x38,0xc7,0x6d,
0x6c,0x90,0x94,0x9c,0x84,0xb4,0xe4,0x45,
0xa6,0x62,0x2a,0xb3,0x00,0x55,0xfe,0x03,
0xfb,0xf7,0xe7,0xcf,0xaf,0x5e,0x1c,0x3b,
0xb7,0x66,0x4c,0xaa,0x54,0x02,0x07,0xf3,
0xef,0xd7,0x9f,0x0e,0xbd,0xd8,0x73,0x2e,
0xd5,0x19,0x01,0xa9,0xfa,0x0b,0xe3,0xc7,
0xb7,0x6e,0x4c,0x9a,0x54,0xa2,0x04,0x33,
0xe7,0x57,0xac,0x0e,0x17,0xdb,0x8f,0x26,
0xdd,0x29,0x31,0x09,0x59,0xc8,0x2b,0x4f,
0x04,0x5d,0xe6,0x33,0xab,0x56,0x04,0x0b,
0xe7,0xc7,0xaf,0x6e,0x1c,0x9b,0xb4,0xa6,
0x44,0x2a,0x67,0x02,0xad,0xf0,0x13,0xdf,
0x97,0x3e,0x8d,0x78,0xd0,0xed,0x1d,0x91,
0xb2,0x9a,0x50,0xa2,0x1c,0x33,0xb7,0x56,
0x4c,0x0a,0x57,0xc2,0x0f,0x73,0xdc,0xd7,
0x35,0x0d,0x41,0xd0,0x7b,0x1e,0xe5,0xb9,
0xa2,0x6a,0x32,0x83,0x50,0xf4,0x1d,0xc7,
0xb3,0x6e,0x54,0x9a,0x04,0xa3,0xe4,0x37,
0xa7,0x4e,0x2c,0x5b,0x16,0x25,0x8b,0x22,
0xc5,0x31,0x61,0x58,0xb8,0x2c,0x6f,0x16,
0x9d,0x88,0xb2,0xcc,0x51,0x56,0x18,0x0b,
0xaf,0xc6,0x1f,0x6b,0xbc,0x86,0x74,0xea,
0xc5,0x81,0x62,0xf8,0xb1,0xec,0x5b,0x96,
0x26,0x8b,0x28,0xc5,0x0d,0x61,0xd0,0xbb,
0x1c,0x65,0xb6,0xa2,0x48,0x32,0x4f,0x52,
0x5c,0x12,0x37,0x93,0x4e,0x94,0x58,0x86,
0x2c,0xeb,0x15,0x85,0x82,0xe2,0xf1,0xb1,
0xda,0x5b,0x22,0x25,0x33,0x21,0x55,0x38,
0x01,0x6f,0xf8,0x9f,0xec,0xbf,0x94,0x7e,
0x86,0xf8,0xe9,0xed,0x8b,0x92,0xc6,0x91,
0x68,0x98,0x8c,0xac,0xd4,0x15,0x07,0x81,
0xee,0xfb,0x99,0xe6,0xab,0xa8,0x06,0x0f,
0xeb,0xdf,0x87,0x3e,0xed,0x79,0x90,0xea,
0x9d,0x80,0xb2,0xfc,0x51,0xf6,0x1b,0xcb,
0xa7,0x46,0x2c,0x6b,0x16,0x85,0x88,0xe2,
0xcd,0xb1,0x52,0x58,0x12,0x2f,0x93,0x1e,
0x95,0xb8,0x82,0x6c,0xf2,0x95,0xd0,0x83,
0x1c,0xf5,0xb5,0xc2,0x43,0x72,0x74,0xd2,
0xc5,0x11,0x61,0x98,0xba,0xac,0x60,0x16,
0xbf,0x88,0x7e,0xce,0xf9,0x59,0xe8,0x2b,
0x8f,0x06,0xdd,0xe9,0x33,0x89,0x56,0xc8,
0x09,0x4f,0xc8,0x5f,0x4e,0x3c,0x5b,0x76,
0x24,0xcb,0x25,0x45,0x20,0x61,0x3e,0xb9,
0x78,0x68,0xee,0x8d,0x98,0xd2,0xad,0x10,
0x11,0x9f,0x9a,0xbe,0xa0,0x78,0x3e,0xef,
0x79,0x9c,0xea,0xb5,0x80,0x42,0xfe,0x71,
0xfa,0xdb,0xe1,0x27,0xb9,0x2e,0x69,0x1a,
0x89,0xa0,0xca,0x3d,0x43,0x70,0x74,0xde,
0xc5,0x39,0x61,0x68,0xb8,0x8c,0x6c,0xd6,
0x95,0x08,0x81,0xcc,0xfb,0x55,0xe4,0x03,
0xa7,0xf6,0x2f,0xcb,0x1f,0x45,0xbc,0x62,
0x76,0xb2,0xc8,0x51,0x4e,0x18,0x5b,0xae,
0x26,0x1b,0x2b,0xa5,0x06,0x21,0xeb,0x3b,
0x85,0x66,0xe0,0xa9,0xbc,0x0a,0x77,0xc2,
0xcf,0x71,0x5c,0xd8,0x35,0x2f,0x41,0x1c,
0x79,0xb6,0xea,0x49,0x82,0x4a,0xf2,0x41,
0xd2,0x7b,0x12,0xe5,0x91,0xa2,0x9a,0x30,
0xa3,0x5c,0x34,0x37,0x47,0x4c,0x6c,0x56,
0x96,0x08,0x8b,0xcc,0xc7,0x55,0x6c,0x00,
0x97,0xfc,0x8f,0xf4,0xdf,0xc5,0x3f,0x61,
0x7c,0xb8,0xf4,0x6d,0xc6,0x93,0x68,0x94,
0x8c,0x84,0xd4,0xe5,0x05,0xa1,0xe2,0x3b,
0xb3,0x66,0x54,0xaa,0x04,0x03,0xe7,0xf7,
0xaf,0xce,0x1f,0x5b,0xbc,0x26,0x77,0x2a,
0xcd,0x01,0x51,0xf8,0x1b,0xef,0xa7,0x9e,
0x2e,0xbb,0x18,0x65,0xae,0xa2,0x18,0x33,
0xaf,0x56,0x1c,0x0b,0xb7,0xc6,0x4f,0x6a,
0x5c,0x82,0x34,0xf3,0x45,0xd4,0x63,0x06,
0xb5,0xe8,0x43,0x8e,0x76,0xda,0xc9,0x21,
0x49,0x38,0x49,0x6e,0x48,0x9a,0x4c,0xa2,
0x54,0x32,0x07,0x53,0xec,0x17,0x97,0x8e,
0x8e,0xd8,0xd9,0x2d,0x29,0x11,0x09,0x99,
0xca,0xab,0x40,0x04,0x7f,0xe6,0xff,0xa9,
0xfe,0x0b,0xfb,0xc7,0xe7,0x6f,0xac,0x9e,
0x14,0xbb,0x84,0x66,0xe6,0xa9,0xa8,0x0a,
0x0f,0xc3,0xdf,0x77,0x3c,0xcd,0x75,0x50,
0xc0,0x1d,0x7f,0xb0,0xfe,0x5d,0xfa,0x33,
0xe3,0x57,0xb4,0x0e,0x47,0xda,0x6f,0x22,
0x9d,0x30,0xb1,0x5c,0x58,0x36,0x2f,0x4b,
0x1c,0x45,0xb6,0x62,0x4a,0xb2,0x40,0x52,
0x7e,0x12,0xfb,0x91,0xe6,0x9b,0xa8,0xa6,
0x0c,0x2b,0xd7,0x07,0x0d,0xed,0xd3,0x93,
0x16,0x95,0x88,0x82,0xcc,0xf1,0x55,0xd8,
0x03,0x2f,0xf5,0x1f,0xc1,0xbf,0x7a,0x7c,
0xe2,0xf5,0xb1,0xc2,0x5b,0x72,0x24,0xd3,
0x25,0x15,0x21,0x81,0x3a,0xf9,0x61,0xe8,
0xbb,0x8c,0x66,0xd6,0xa9,0x08,0x09,0xcf,
0xcb,0x5f,0x44,0x3c,0x67,0x76,0xac,0xc8,
0x15,0x4f,0x80,0x5e,0xfe,0x39,0xfb,0x6b,
0xe4,0x87,0xa4,0xee,0x25,0x9b,0x22,0xa5,
0x30,0x21,0x5f,0x38,0x3d,0x6f,0x70,0x9c,
0xdc,0xb5,0x34,0x41,0x46,0x78,0x6a,0xee,
0x81,0x98,0xfa,0xad,0xe0,0x13,0xbf,0x96,
0x7e,0x8a,0xf8,0xc1,0xed,0x7b,0x90,0xe6,
0x9d,0xa8,0xb2,0x0c,0x53,0xd6,0x17,0x0b,
0x8d,0xc6,0xd3,0x69,0x14,0x89,0x84,0xca,
0xe5,0x41,0xa0,0x7a,0x3e,0xe3,0x79,0xb4,
0xea,0x45,0x82,0x62,0xf2,0xb1,0xd0,0x5b,
0x1e,0x25,0xbb,0x22,0x65,0x32,0xa1,0x50,
0x38,0x1f,0x6f,0xbc,0x9e,0x74,0xba,0xc4,
0x61,0x66,0xb8,0xa8,0x6c,0x0e,0x97,0xd8,
0x8f,0x2c,0xdd,0x15,0x31,0x81,0x5a,0xf8,
0x21,0xef,0x3b,0x9d,0x66,0xb0,0xa8,0x5c,
0x0e,0x37,0xdb,0x4f,0x24,0x5d,0x26,0x31,
0x2b,0x59,0x04,0x29,0xe7,0x0b,0xad,0xc6,
0x13,0x6b,0x94,0x86,0x84,0xe8,0xe5,0x8d,
0xa2,0xd2,0x31,0x13,0x59,0x94,0x2a,0x87,
0x00,0xed,0xfd,0x93,0xf2,0x97,0xd0,0x8f,
0x1c,0xdd,0xb5,0x32,0x41,0x52,0x78,0x12,
0xef,0x91,0x9e,0x9a,0xb8,0xa0,0x6c,0x3e,
0x97,0x78,0x8c,0xec,0xd5,0x95,0x02,0x81,
0xf0,0xfb,0xdd,0xe7,0x33,0xad,0x56,0x10,
0x0b,0x9f,0xc6,0xbf,0x68,0x7c,0x8e,0xf4,
0xd9,0xc5,0x2b,0x61,0x04,0xb9,0xe4,0x6b,
0xa6,0x86,0x28,0xeb,0x0d,0x85,0xd2,0xe3,
0x11,0xb5,0x9a,0x42,0xa2,0x70,0x32,0xdf,
0x51,0x3c,0x19,0x77,0xa8,0xce,0x0d,0x5b,
0xd0,0x27,0x1f,0x2d,0xbd,0x12,0x71,0x92,
0xda,0x91,0x20,0x99,0x3c,0xa9,0x74,0x08,
0xc7,0xcd,0x6f,0x50,0x9c,0x1c,0xb7,0xb4,
0x4e,0x46,0x5a,0x6a,0x22,0x83,0x30,0xf5,
0x5d,0xc0,0x33,0x7f,0x54,0xfc,0x05,0xf7,
0xe3,0xcf,0xb7,0x5e,0x4c,0x3a,0x57,0x62,
0x0c,0xb3,0xd4,0x57,0x06,0x0d,0xeb,0xd3,
0x87,0x16,0xed,0x89,0x92,0xca,0x91,0x40,
0x98,0x7c,0xae,0xf4,0x19,0xc7,0xab,0x6e,
0x04,0x9b,0xe4,0xa7,0xa4,0x2e,0x27,0x1b,
0x2d,0xa5,0x12,0x21,0x93,0x3a,0x95,0x60,
0x80,0xbc,0xfc,0x75,0xf6,0xc3,0xc9,0x77,
0x48,0xcc,0x4d,0x56,0x50,0x0a,0x1f,0xc3,
0xbf,0x76,0x7c,0xca,0xf5,0x41,0xc0,0x7b,
0x7e,0xe4,0xf9,0xa5,0xea,0x23,0x83,0x36,
0xf5,0x49,0xc0,0x4b,0x7e,0x44,0xfa,0x65,
0xe2,0xa3,0xb0,0x36,0x5f,0x4a,0x3c,0x43,
0x76,0x74,0xca,0xc5,0x41,0x60,0x78,0xbe,
0xec,0x79,0x96,0xea,0x89,0x80,0xca,0xfd,
0x41,0xf0,0x7b,0xde,0xe7,0x39,0xad,0x6a,
0x10,0x83,0x9c,0xf6,0xb5,0xc8,0x43,0x4e,
0x74,0x5a,0xc6,0x21,0x6b,0x38,0x85,0x6c,
0xe0,0x95,0xbc,0x82,0x74,0xf2,0xc5,0xd1,
0x63,0x18,0xb5,0xac,0x42,0x16,0x73,0x8a,
0xd6,0xc1,0x09,0x79,0xc8,0xeb,0x4d,0x84,
0x52,0xe6,0x11,0xab,0x9a,0x06,0xa3,0xe8,
0x37,0x8f,0x4e,0xdc,0x59,0x36,0x29,0x4b,
0x08,0x45,0xce,0x63,0x5a,0xb4,0x20,0x47,
0x3e,0x6d,0x7a,0x90,0xe0,0x9d,0xbc,0xb2,
0x74,0x52,0xc6,0x11,0x6b,0x98,0x86,0xac,
0xe8,0x15,0x8f,0x82,0xde,0xf1,0x39,0xd9,
0x6b,0x28,0x85,0x0c,0xe1,0xd5,0xbb,0x02,
0x65,0xf2,0xa3,0xd0,0x37,0x1f,0x4d,0xbc,
0x52,0x76,0x12,0xcb,0x91,0x46,0x98,0x68,
0xae,0x8c,0x18,0xd7,0xad,0x0e,0x11,0xdb,
0x9b,0x26,0xa5,0x28,0x21,0x0f,0x39,0xdd,
0x6b,0x30,0x85,0x5c,0xe0,0x35,0xbf,0x42,
0x7c,0x72,0xf6,0xd1,0xc9,0x1b,0x49,0xa4,
0x4a,0x26,0x43,0x2a,0x75,0x02,0xc1,0xf1,
0x7b,0xd8,0xe7,0x2d,0xad,0x12,0x11,0x93,
0x9a,0x96,0xa0,0x88,0x3c,0xcf,0x75,0x5c,
0xc0,0x35,0x7f,0x40,0xfc,0x7d,0xf6,0xf3,
0xc9,0xd7,0x4b,0x0c,0x45,0xd6,0x63,0x0a,
0xb5,0xc0,0x43,0x7e,0x74,0xfa,0xc5,0xe1,
0x63,0xb8,0xb6,0x6c,0x4a,0x96,0x40,0x8a,
0x7c,0xc2,0xf5,0x71,0xc0,0xdb,0x7d,0x24,
0xf1,0x25,0xd9,0x23,0x29,0x35,0x09,0x41,
0xc8,0x7b,0x4e,0xe4,0x59,0xa6,0x2a,0x2b,
0x03,0x05,0xf5,0xe3,0xc3,0xb7,0x76,0x4c,
0xca,0x55,0x42,0x00,0x73,0xfe,0xd7,0xf9,
0x0f,0xe9,0xdf,0x8b,0x3e,0xc5,0x79,0x60,
0xe8,0xbd,0x8c,0x72,0xd6,0xd1,0x09,0x19,
0xc9,0xab,0x4a,0x04,0x43,0xe6,0x77,0xaa,
0xce,0x01,0x5b,0xf8,0x27,0xef,0x2f,0x9d,
0x1e,0xb1,0xb8,0x5a,0x6e,0x22,0x9b,0x30,
0xa5,0x5c,0x20,0x37,0x3f,0x4d,0x7c,0x50,
0xf6,0x1d,0xcb,0xb3,0x46,0x54,0x6a,0x06,
0x83,0xe8,0xf7,0x8d,0xce,0xd3,0x59,0x14,
0x29,0x87,0x0a,0xed,0xc1,0x93,0x7a,0x94,
0xe0,0x85,0xbc,0xe2,0x75,0xb2,0xc2,0x51,
0x72,0x18,0xd3,0xad,0x16,0x11,0x8b,0x9a,
0xc6,0xa1,0x68,0x38,0x8f,0x6c,0xdc,0x95,
0x34,0x81,0x44,0xf8,0x65,0xee,0xa3,0x98,
0x36,0xaf,0x48,0x1c,0x4f,0xb6,0x5e,0x4a,
0x3a,0x43,0x62,0x74,0xb2,0xc4,0x51,0x66,
0x18,0xab,0xac,0x06,0x17,0xeb,0x8f,0x86,
0xde,0xe9,0x39,0x89,0x6a,0xc8,0x81,0x4c,
0xf8,0x55,0xee,0x03,0x9b,0xf6,0xa7,0xc8,
0x2f,0x4f,0x1c,0x5d,0xb6,0x32,0x4b,0x52,
0x44,0x12,0x67,0x92,0xae,0x90,0x18,0x9f,
0xac,0xbe,0x14,0x7b,0x86,0xe6,0xe9,0xa9,
0x8a,0x0a,0xc3,0xc1,0x77,0x78,0xcc,0xed,
0x55,0x90,0x02,0x9f,0xf0,0xbf,0xdc,0x7f,
0x36,0xfd,0x49,0xf0,0x4b,0xde,0x47,0x3a,
0x6d,0x62,0x90,0xb0,0x9c,0x5c,0xb6,0x34,
0x4b,0x46,0x44,0x6a,0x66,0x82,0xa8,0xf0,
0x0d,0xdf,0xd3,0x3f,0x15,0x7d,0x80,0xf2,
0xfd,0xd1,0xf3,0x1b,0xd5,0xa7,0x02,0x2d,
0xf3,0x13,0xd5,0x97,0x02,0x8d,0xf0,0xd3,
0xdd,0x17,0x31,0x8d,0x5a,0xd0,0x21,0x1f,
0x39,0xbd,0x6a,0x70,0x82,0xdc,0xf1,0x35,
0xd9,0x43,0x28,0x75,0x0e,0xc1,0xd9,0x7b,
0x28,0xe5,0x0d,0xa1,0xd2,0x3b,0x13,0x65,
0x94,0xa2,0x84,0x30,0xe7,0x5d,0xac,0x32,
0x17,0x53,0x8c,0x16,0xd7,0x89,0x0e,0xc9,
0xd9,0x4b,0x28,0x45,0x0e,0x61,0xda,0xbb,
0x20,0x65,0x3e,0xa1,0x78,0x38,0xef,0x6d,
0x9c,0x92,0xb4,0x90,0x44,0x9e,0x64,0xba,
0xa4,0x60,0x26,0xbf,0x28,0x7d,0x0e,0xf1,
0xd9,0xdb,0x2b,0x25,0x05,0x21,0xe1,0x3b,
0xb9,0x66,0x68,0xaa,0x8c,0x00,0xd7,0xfd,
0x0f,0xf1,0xdf,0xdb,0x3f,0x25,0x7d,0x20,
0xf1,0x3d,0xd9,0x73,0x28,0xd5,0x0d,0x01,
0xd1,0xfb,0x1b,0xe5,0xa7,0xa2,0x2e,0x33,
0x1b,0x55,0xa4,0x02,0x27,0xf3,0x2f,0xd5,
0x1f,0x01,0xbd,0xfa,0x73,0xe2,0xd7,0xb1,
0x0e,0x59,0xda,0x2b,0x23,0x05,0x35,0xe1,
0x43,0xb8,0x76,0x6e,0xca,0x99,0x40,0xa8,
0x7c,0x0e,0xf7,0xd9,0xcf,0x2b,0x5d,0x04,
0x31,0xe7,0x5b,0xac,0x26,0x17,0x2b,0x8d,
0x06,0xd1,0xe9,0x1b,0x89,0xa6,0xca,0x29,
0x43,0x08,0x75,0xce,0xc3,0x59,0x74,0x28,
0xc7,0x0d,0x6d,0xd0,0x93,0x1c,0x95,0xb4,
0x82,0x44,0xf2,0x65,0xd2,0xa3,0x10,0x35,
0x9f,0x42,0xbc,0x70,0x76,0xde,0xc9,0x39,
0x49,0x68,0x48,0x8e,0x4c,0xda,0x55,0x22,
0x01,0x33,0xf9,0x57,0xe8,0x0f,0x8f,0xde,
0xdf,0x39,0x3d,0x69,0x70,0x88,0xdc,0xcd,
0x35,0x51,0x40,0x18,0x7f,0xae,0xfe,0x19,
0xfb,0xab,0xe6,0x07,0xab,0xee,0x07,0x9b,
0xee,0xa7,0x98,0x2e,0xaf,0x18,0x1d,0xaf,
0xb2,0x1e,0x53,0xba,0x16,0x63,0x8a,0xb6,
0xc0,0x49,0x7e,0x48,0xfa,0x4d,0xe2,0x53,
0xb2,0x16,0x53,0x8a,0x16,0xc3,0x89,0x76,
0xc8,0xc9,0x4d,0x48,0x50,0x4e,0x1e,0x5b,
0xba,0x26,0x63,0x2a,0xb5,0x00,0x41,0xfe,
0x7b,0xfa,0xe7,0xe1,0xaf,0xba,0x1e,0x63,
0xba,0xb6,0x60,0x4a,0xbe,0x40,0x7a,0x7e,
0xe2,0xf9,0xb1,0xea,0x5b,0x82,0x26,0xf3,
0x29,0xd5,0x0b,0x01,0xc5,0xfb,0x63,0xe4,
0xb7,0xa4,0x4e,0x26,0x5b,0x2a,0x25,0x03,
0x21,0xf5,0x3b,0xc1,0x67,0x78,0xac,0xec,
0x15,0x97,0x82,0x8e,0xf0,0xd9,0xdd,0x2b,
0x31,0x05,0x59,0xe0,0x2b,0xbf,0x06,0x7d,
0xea,0xf3,0x81,0xd6,0xfb,0x09,0xe5,0xcb,
0xa3,0x46,0x34,0x6b,0x46,0x84,0x68,0xe6,
0x8d,0xa8,0xd2,0x0d,0x13,0xd1,0x97,0x1a,
0x8d,0xa0,0xd2,0x3d,0x13,0x71,0x94,0xda,
0x85,0x20,0xe1,0x3d,0xb9,0x72,0x68,0xd2,
0x8d,0x10,0xd1,0x9d,0x1a,0xb1,0xa0,0x5a,
0x3e,0x23,0x7b,0x34,0xe5,0x45,0xa0,0x62,
0x3e,0xb3,0x78,0x54,0xee,0x05,0x9b,0xe2,
0xa7,0xb0,0x2e,0x5f,0x1a,0x3d,0xa3,0x72,
0x34,0xd3,0x45,0x14,0x61,0x86,0xba,0xe8,
0x61,0x8e,0xba,0xd8,0x61,0x2e,0xb9,0x18,
0x69,0xae,0x8a,0x18,0xc3,0xad,0x76,0x10,
0xcb,0x9d,0x46,0xb0,0x68,0x5e,0x8e,0x38,
0xdb,0x6d,0x24,0x91,0x24,0x99,0x24,0xa9,
0x24,0x09,0x27,0xc9,0x2f,0x49,0x1c,0x49,
0xb6,0x4a,0x4a,0x42,0x42,0x72,0x72,0xd2,
0xd1,0x11,0x19,0x99,0xaa,0xaa,0x00,0x03,
};
