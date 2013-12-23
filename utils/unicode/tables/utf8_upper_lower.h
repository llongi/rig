#include <stdint.h>

const struct {
	const uint16_t key;
	const uint8_t val[2];
} UTF8_UPPER_LOWER_2B[] = {
	{ 0xC380, { 0xC3, 0xA0 } },
	{ 0xC381, { 0xC3, 0xA1 } },
	{ 0xC382, { 0xC3, 0xA2 } },
	{ 0xC383, { 0xC3, 0xA3 } },
	{ 0xC384, { 0xC3, 0xA4 } },
	{ 0xC385, { 0xC3, 0xA5 } },
	{ 0xC386, { 0xC3, 0xA6 } },
	{ 0xC387, { 0xC3, 0xA7 } },
	{ 0xC388, { 0xC3, 0xA8 } },
	{ 0xC389, { 0xC3, 0xA9 } },
	{ 0xC38A, { 0xC3, 0xAA } },
	{ 0xC38B, { 0xC3, 0xAB } },
	{ 0xC38C, { 0xC3, 0xAC } },
	{ 0xC38D, { 0xC3, 0xAD } },
	{ 0xC38E, { 0xC3, 0xAE } },
	{ 0xC38F, { 0xC3, 0xAF } },
	{ 0xC390, { 0xC3, 0xB0 } },
	{ 0xC391, { 0xC3, 0xB1 } },
	{ 0xC392, { 0xC3, 0xB2 } },
	{ 0xC393, { 0xC3, 0xB3 } },
	{ 0xC394, { 0xC3, 0xB4 } },
	{ 0xC395, { 0xC3, 0xB5 } },
	{ 0xC396, { 0xC3, 0xB6 } },
	{ 0xC398, { 0xC3, 0xB8 } },
	{ 0xC399, { 0xC3, 0xB9 } },
	{ 0xC39A, { 0xC3, 0xBA } },
	{ 0xC39B, { 0xC3, 0xBB } },
	{ 0xC39C, { 0xC3, 0xBC } },
	{ 0xC39D, { 0xC3, 0xBD } },
	{ 0xC39E, { 0xC3, 0xBE } },
	{ 0xC480, { 0xC4, 0x81 } },
	{ 0xC482, { 0xC4, 0x83 } },
	{ 0xC484, { 0xC4, 0x85 } },
	{ 0xC486, { 0xC4, 0x87 } },
	{ 0xC488, { 0xC4, 0x89 } },
	{ 0xC48A, { 0xC4, 0x8B } },
	{ 0xC48C, { 0xC4, 0x8D } },
	{ 0xC48E, { 0xC4, 0x8F } },
	{ 0xC490, { 0xC4, 0x91 } },
	{ 0xC492, { 0xC4, 0x93 } },
	{ 0xC494, { 0xC4, 0x95 } },
	{ 0xC496, { 0xC4, 0x97 } },
	{ 0xC498, { 0xC4, 0x99 } },
	{ 0xC49A, { 0xC4, 0x9B } },
	{ 0xC49C, { 0xC4, 0x9D } },
	{ 0xC49E, { 0xC4, 0x9F } },
	{ 0xC4A0, { 0xC4, 0xA1 } },
	{ 0xC4A2, { 0xC4, 0xA3 } },
	{ 0xC4A4, { 0xC4, 0xA5 } },
	{ 0xC4A6, { 0xC4, 0xA7 } },
	{ 0xC4A8, { 0xC4, 0xA9 } },
	{ 0xC4AA, { 0xC4, 0xAB } },
	{ 0xC4AC, { 0xC4, 0xAD } },
	{ 0xC4AE, { 0xC4, 0xAF } },
	{ 0xC4B2, { 0xC4, 0xB3 } },
	{ 0xC4B4, { 0xC4, 0xB5 } },
	{ 0xC4B6, { 0xC4, 0xB7 } },
	{ 0xC4B9, { 0xC4, 0xBA } },
	{ 0xC4BB, { 0xC4, 0xBC } },
	{ 0xC4BD, { 0xC4, 0xBE } },
	{ 0xC4BF, { 0xC5, 0x80 } },
	{ 0xC581, { 0xC5, 0x82 } },
	{ 0xC583, { 0xC5, 0x84 } },
	{ 0xC585, { 0xC5, 0x86 } },
	{ 0xC587, { 0xC5, 0x88 } },
	{ 0xC58A, { 0xC5, 0x8B } },
	{ 0xC58C, { 0xC5, 0x8D } },
	{ 0xC58E, { 0xC5, 0x8F } },
	{ 0xC590, { 0xC5, 0x91 } },
	{ 0xC592, { 0xC5, 0x93 } },
	{ 0xC594, { 0xC5, 0x95 } },
	{ 0xC596, { 0xC5, 0x97 } },
	{ 0xC598, { 0xC5, 0x99 } },
	{ 0xC59A, { 0xC5, 0x9B } },
	{ 0xC59C, { 0xC5, 0x9D } },
	{ 0xC59E, { 0xC5, 0x9F } },
	{ 0xC5A0, { 0xC5, 0xA1 } },
	{ 0xC5A2, { 0xC5, 0xA3 } },
	{ 0xC5A4, { 0xC5, 0xA5 } },
	{ 0xC5A6, { 0xC5, 0xA7 } },
	{ 0xC5A8, { 0xC5, 0xA9 } },
	{ 0xC5AA, { 0xC5, 0xAB } },
	{ 0xC5AC, { 0xC5, 0xAD } },
	{ 0xC5AE, { 0xC5, 0xAF } },
	{ 0xC5B0, { 0xC5, 0xB1 } },
	{ 0xC5B2, { 0xC5, 0xB3 } },
	{ 0xC5B4, { 0xC5, 0xB5 } },
	{ 0xC5B6, { 0xC5, 0xB7 } },
	{ 0xC5B8, { 0xC3, 0xBF } },
	{ 0xC5B9, { 0xC5, 0xBA } },
	{ 0xC5BB, { 0xC5, 0xBC } },
	{ 0xC5BD, { 0xC5, 0xBE } },
	{ 0xC681, { 0xC9, 0x93 } },
	{ 0xC682, { 0xC6, 0x83 } },
	{ 0xC684, { 0xC6, 0x85 } },
	{ 0xC686, { 0xC9, 0x94 } },
	{ 0xC687, { 0xC6, 0x88 } },
	{ 0xC689, { 0xC9, 0x96 } },
	{ 0xC68A, { 0xC9, 0x97 } },
	{ 0xC68B, { 0xC6, 0x8C } },
	{ 0xC68E, { 0xC7, 0x9D } },
	{ 0xC68F, { 0xC9, 0x99 } },
	{ 0xC690, { 0xC9, 0x9B } },
	{ 0xC691, { 0xC6, 0x92 } },
	{ 0xC693, { 0xC9, 0xA0 } },
	{ 0xC694, { 0xC9, 0xA3 } },
	{ 0xC696, { 0xC9, 0xA9 } },
	{ 0xC697, { 0xC9, 0xA8 } },
	{ 0xC698, { 0xC6, 0x99 } },
	{ 0xC69C, { 0xC9, 0xAF } },
	{ 0xC69D, { 0xC9, 0xB2 } },
	{ 0xC69F, { 0xC9, 0xB5 } },
	{ 0xC6A0, { 0xC6, 0xA1 } },
	{ 0xC6A2, { 0xC6, 0xA3 } },
	{ 0xC6A4, { 0xC6, 0xA5 } },
	{ 0xC6A6, { 0xCA, 0x80 } },
	{ 0xC6A7, { 0xC6, 0xA8 } },
	{ 0xC6A9, { 0xCA, 0x83 } },
	{ 0xC6AC, { 0xC6, 0xAD } },
	{ 0xC6AE, { 0xCA, 0x88 } },
	{ 0xC6AF, { 0xC6, 0xB0 } },
	{ 0xC6B1, { 0xCA, 0x8A } },
	{ 0xC6B2, { 0xCA, 0x8B } },
	{ 0xC6B3, { 0xC6, 0xB4 } },
	{ 0xC6B5, { 0xC6, 0xB6 } },
	{ 0xC6B7, { 0xCA, 0x92 } },
	{ 0xC6B8, { 0xC6, 0xB9 } },
	{ 0xC6BC, { 0xC6, 0xBD } },
	{ 0xC784, { 0xC7, 0x86 } },
	{ 0xC785, { 0xC7, 0x86 } },
	{ 0xC787, { 0xC7, 0x89 } },
	{ 0xC788, { 0xC7, 0x89 } },
	{ 0xC78A, { 0xC7, 0x8C } },
	{ 0xC78B, { 0xC7, 0x8C } },
	{ 0xC78D, { 0xC7, 0x8E } },
	{ 0xC78F, { 0xC7, 0x90 } },
	{ 0xC791, { 0xC7, 0x92 } },
	{ 0xC793, { 0xC7, 0x94 } },
	{ 0xC795, { 0xC7, 0x96 } },
	{ 0xC797, { 0xC7, 0x98 } },
	{ 0xC799, { 0xC7, 0x9A } },
	{ 0xC79B, { 0xC7, 0x9C } },
	{ 0xC79E, { 0xC7, 0x9F } },
	{ 0xC7A0, { 0xC7, 0xA1 } },
	{ 0xC7A2, { 0xC7, 0xA3 } },
	{ 0xC7A4, { 0xC7, 0xA5 } },
	{ 0xC7A6, { 0xC7, 0xA7 } },
	{ 0xC7A8, { 0xC7, 0xA9 } },
	{ 0xC7AA, { 0xC7, 0xAB } },
	{ 0xC7AC, { 0xC7, 0xAD } },
	{ 0xC7AE, { 0xC7, 0xAF } },
	{ 0xC7B1, { 0xC7, 0xB3 } },
	{ 0xC7B2, { 0xC7, 0xB3 } },
	{ 0xC7B4, { 0xC7, 0xB5 } },
	{ 0xC7B6, { 0xC6, 0x95 } },
	{ 0xC7B7, { 0xC6, 0xBF } },
	{ 0xC7B8, { 0xC7, 0xB9 } },
	{ 0xC7BA, { 0xC7, 0xBB } },
	{ 0xC7BC, { 0xC7, 0xBD } },
	{ 0xC7BE, { 0xC7, 0xBF } },
	{ 0xC880, { 0xC8, 0x81 } },
	{ 0xC882, { 0xC8, 0x83 } },
	{ 0xC884, { 0xC8, 0x85 } },
	{ 0xC886, { 0xC8, 0x87 } },
	{ 0xC888, { 0xC8, 0x89 } },
	{ 0xC88A, { 0xC8, 0x8B } },
	{ 0xC88C, { 0xC8, 0x8D } },
	{ 0xC88E, { 0xC8, 0x8F } },
	{ 0xC890, { 0xC8, 0x91 } },
	{ 0xC892, { 0xC8, 0x93 } },
	{ 0xC894, { 0xC8, 0x95 } },
	{ 0xC896, { 0xC8, 0x97 } },
	{ 0xC898, { 0xC8, 0x99 } },
	{ 0xC89A, { 0xC8, 0x9B } },
	{ 0xC89C, { 0xC8, 0x9D } },
	{ 0xC89E, { 0xC8, 0x9F } },
	{ 0xC8A0, { 0xC6, 0x9E } },
	{ 0xC8A2, { 0xC8, 0xA3 } },
	{ 0xC8A4, { 0xC8, 0xA5 } },
	{ 0xC8A6, { 0xC8, 0xA7 } },
	{ 0xC8A8, { 0xC8, 0xA9 } },
	{ 0xC8AA, { 0xC8, 0xAB } },
	{ 0xC8AC, { 0xC8, 0xAD } },
	{ 0xC8AE, { 0xC8, 0xAF } },
	{ 0xC8B0, { 0xC8, 0xB1 } },
	{ 0xC8B2, { 0xC8, 0xB3 } },
	{ 0xC8BB, { 0xC8, 0xBC } },
	{ 0xC8BD, { 0xC6, 0x9A } },
	{ 0xC981, { 0xC9, 0x82 } },
	{ 0xC983, { 0xC6, 0x80 } },
	{ 0xC984, { 0xCA, 0x89 } },
	{ 0xC985, { 0xCA, 0x8C } },
	{ 0xC986, { 0xC9, 0x87 } },
	{ 0xC988, { 0xC9, 0x89 } },
	{ 0xC98A, { 0xC9, 0x8B } },
	{ 0xC98C, { 0xC9, 0x8D } },
	{ 0xC98E, { 0xC9, 0x8F } },
	{ 0xCDB0, { 0xCD, 0xB1 } },
	{ 0xCDB2, { 0xCD, 0xB3 } },
	{ 0xCDB6, { 0xCD, 0xB7 } },
	{ 0xCE86, { 0xCE, 0xAC } },
	{ 0xCE88, { 0xCE, 0xAD } },
	{ 0xCE89, { 0xCE, 0xAE } },
	{ 0xCE8A, { 0xCE, 0xAF } },
	{ 0xCE8C, { 0xCF, 0x8C } },
	{ 0xCE8E, { 0xCF, 0x8D } },
	{ 0xCE8F, { 0xCF, 0x8E } },
	{ 0xCE91, { 0xCE, 0xB1 } },
	{ 0xCE92, { 0xCE, 0xB2 } },
	{ 0xCE93, { 0xCE, 0xB3 } },
	{ 0xCE94, { 0xCE, 0xB4 } },
	{ 0xCE95, { 0xCE, 0xB5 } },
	{ 0xCE96, { 0xCE, 0xB6 } },
	{ 0xCE97, { 0xCE, 0xB7 } },
	{ 0xCE98, { 0xCE, 0xB8 } },
	{ 0xCE99, { 0xCE, 0xB9 } },
	{ 0xCE9A, { 0xCE, 0xBA } },
	{ 0xCE9B, { 0xCE, 0xBB } },
	{ 0xCE9C, { 0xCE, 0xBC } },
	{ 0xCE9D, { 0xCE, 0xBD } },
	{ 0xCE9E, { 0xCE, 0xBE } },
	{ 0xCE9F, { 0xCE, 0xBF } },
	{ 0xCEA0, { 0xCF, 0x80 } },
	{ 0xCEA1, { 0xCF, 0x81 } },
	{ 0xCEA3, { 0xCF, 0x83 } },
	{ 0xCEA4, { 0xCF, 0x84 } },
	{ 0xCEA5, { 0xCF, 0x85 } },
	{ 0xCEA6, { 0xCF, 0x86 } },
	{ 0xCEA7, { 0xCF, 0x87 } },
	{ 0xCEA8, { 0xCF, 0x88 } },
	{ 0xCEA9, { 0xCF, 0x89 } },
	{ 0xCEAA, { 0xCF, 0x8A } },
	{ 0xCEAB, { 0xCF, 0x8B } },
	{ 0xCF8F, { 0xCF, 0x97 } },
	{ 0xCF98, { 0xCF, 0x99 } },
	{ 0xCF9A, { 0xCF, 0x9B } },
	{ 0xCF9C, { 0xCF, 0x9D } },
	{ 0xCF9E, { 0xCF, 0x9F } },
	{ 0xCFA0, { 0xCF, 0xA1 } },
	{ 0xCFA2, { 0xCF, 0xA3 } },
	{ 0xCFA4, { 0xCF, 0xA5 } },
	{ 0xCFA6, { 0xCF, 0xA7 } },
	{ 0xCFA8, { 0xCF, 0xA9 } },
	{ 0xCFAA, { 0xCF, 0xAB } },
	{ 0xCFAC, { 0xCF, 0xAD } },
	{ 0xCFAE, { 0xCF, 0xAF } },
	{ 0xCFB4, { 0xCE, 0xB8 } },
	{ 0xCFB7, { 0xCF, 0xB8 } },
	{ 0xCFB9, { 0xCF, 0xB2 } },
	{ 0xCFBA, { 0xCF, 0xBB } },
	{ 0xCFBD, { 0xCD, 0xBB } },
	{ 0xCFBE, { 0xCD, 0xBC } },
	{ 0xCFBF, { 0xCD, 0xBD } },
	{ 0xD080, { 0xD1, 0x90 } },
	{ 0xD081, { 0xD1, 0x91 } },
	{ 0xD082, { 0xD1, 0x92 } },
	{ 0xD083, { 0xD1, 0x93 } },
	{ 0xD084, { 0xD1, 0x94 } },
	{ 0xD085, { 0xD1, 0x95 } },
	{ 0xD086, { 0xD1, 0x96 } },
	{ 0xD087, { 0xD1, 0x97 } },
	{ 0xD088, { 0xD1, 0x98 } },
	{ 0xD089, { 0xD1, 0x99 } },
	{ 0xD08A, { 0xD1, 0x9A } },
	{ 0xD08B, { 0xD1, 0x9B } },
	{ 0xD08C, { 0xD1, 0x9C } },
	{ 0xD08D, { 0xD1, 0x9D } },
	{ 0xD08E, { 0xD1, 0x9E } },
	{ 0xD08F, { 0xD1, 0x9F } },
	{ 0xD090, { 0xD0, 0xB0 } },
	{ 0xD091, { 0xD0, 0xB1 } },
	{ 0xD092, { 0xD0, 0xB2 } },
	{ 0xD093, { 0xD0, 0xB3 } },
	{ 0xD094, { 0xD0, 0xB4 } },
	{ 0xD095, { 0xD0, 0xB5 } },
	{ 0xD096, { 0xD0, 0xB6 } },
	{ 0xD097, { 0xD0, 0xB7 } },
	{ 0xD098, { 0xD0, 0xB8 } },
	{ 0xD099, { 0xD0, 0xB9 } },
	{ 0xD09A, { 0xD0, 0xBA } },
	{ 0xD09B, { 0xD0, 0xBB } },
	{ 0xD09C, { 0xD0, 0xBC } },
	{ 0xD09D, { 0xD0, 0xBD } },
	{ 0xD09E, { 0xD0, 0xBE } },
	{ 0xD09F, { 0xD0, 0xBF } },
	{ 0xD0A0, { 0xD1, 0x80 } },
	{ 0xD0A1, { 0xD1, 0x81 } },
	{ 0xD0A2, { 0xD1, 0x82 } },
	{ 0xD0A3, { 0xD1, 0x83 } },
	{ 0xD0A4, { 0xD1, 0x84 } },
	{ 0xD0A5, { 0xD1, 0x85 } },
	{ 0xD0A6, { 0xD1, 0x86 } },
	{ 0xD0A7, { 0xD1, 0x87 } },
	{ 0xD0A8, { 0xD1, 0x88 } },
	{ 0xD0A9, { 0xD1, 0x89 } },
	{ 0xD0AA, { 0xD1, 0x8A } },
	{ 0xD0AB, { 0xD1, 0x8B } },
	{ 0xD0AC, { 0xD1, 0x8C } },
	{ 0xD0AD, { 0xD1, 0x8D } },
	{ 0xD0AE, { 0xD1, 0x8E } },
	{ 0xD0AF, { 0xD1, 0x8F } },
	{ 0xD1A0, { 0xD1, 0xA1 } },
	{ 0xD1A2, { 0xD1, 0xA3 } },
	{ 0xD1A4, { 0xD1, 0xA5 } },
	{ 0xD1A6, { 0xD1, 0xA7 } },
	{ 0xD1A8, { 0xD1, 0xA9 } },
	{ 0xD1AA, { 0xD1, 0xAB } },
	{ 0xD1AC, { 0xD1, 0xAD } },
	{ 0xD1AE, { 0xD1, 0xAF } },
	{ 0xD1B0, { 0xD1, 0xB1 } },
	{ 0xD1B2, { 0xD1, 0xB3 } },
	{ 0xD1B4, { 0xD1, 0xB5 } },
	{ 0xD1B6, { 0xD1, 0xB7 } },
	{ 0xD1B8, { 0xD1, 0xB9 } },
	{ 0xD1BA, { 0xD1, 0xBB } },
	{ 0xD1BC, { 0xD1, 0xBD } },
	{ 0xD1BE, { 0xD1, 0xBF } },
	{ 0xD280, { 0xD2, 0x81 } },
	{ 0xD28A, { 0xD2, 0x8B } },
	{ 0xD28C, { 0xD2, 0x8D } },
	{ 0xD28E, { 0xD2, 0x8F } },
	{ 0xD290, { 0xD2, 0x91 } },
	{ 0xD292, { 0xD2, 0x93 } },
	{ 0xD294, { 0xD2, 0x95 } },
	{ 0xD296, { 0xD2, 0x97 } },
	{ 0xD298, { 0xD2, 0x99 } },
	{ 0xD29A, { 0xD2, 0x9B } },
	{ 0xD29C, { 0xD2, 0x9D } },
	{ 0xD29E, { 0xD2, 0x9F } },
	{ 0xD2A0, { 0xD2, 0xA1 } },
	{ 0xD2A2, { 0xD2, 0xA3 } },
	{ 0xD2A4, { 0xD2, 0xA5 } },
	{ 0xD2A6, { 0xD2, 0xA7 } },
	{ 0xD2A8, { 0xD2, 0xA9 } },
	{ 0xD2AA, { 0xD2, 0xAB } },
	{ 0xD2AC, { 0xD2, 0xAD } },
	{ 0xD2AE, { 0xD2, 0xAF } },
	{ 0xD2B0, { 0xD2, 0xB1 } },
	{ 0xD2B2, { 0xD2, 0xB3 } },
	{ 0xD2B4, { 0xD2, 0xB5 } },
	{ 0xD2B6, { 0xD2, 0xB7 } },
	{ 0xD2B8, { 0xD2, 0xB9 } },
	{ 0xD2BA, { 0xD2, 0xBB } },
	{ 0xD2BC, { 0xD2, 0xBD } },
	{ 0xD2BE, { 0xD2, 0xBF } },
	{ 0xD380, { 0xD3, 0x8F } },
	{ 0xD381, { 0xD3, 0x82 } },
	{ 0xD383, { 0xD3, 0x84 } },
	{ 0xD385, { 0xD3, 0x86 } },
	{ 0xD387, { 0xD3, 0x88 } },
	{ 0xD389, { 0xD3, 0x8A } },
	{ 0xD38B, { 0xD3, 0x8C } },
	{ 0xD38D, { 0xD3, 0x8E } },
	{ 0xD390, { 0xD3, 0x91 } },
	{ 0xD392, { 0xD3, 0x93 } },
	{ 0xD394, { 0xD3, 0x95 } },
	{ 0xD396, { 0xD3, 0x97 } },
	{ 0xD398, { 0xD3, 0x99 } },
	{ 0xD39A, { 0xD3, 0x9B } },
	{ 0xD39C, { 0xD3, 0x9D } },
	{ 0xD39E, { 0xD3, 0x9F } },
	{ 0xD3A0, { 0xD3, 0xA1 } },
	{ 0xD3A2, { 0xD3, 0xA3 } },
	{ 0xD3A4, { 0xD3, 0xA5 } },
	{ 0xD3A6, { 0xD3, 0xA7 } },
	{ 0xD3A8, { 0xD3, 0xA9 } },
	{ 0xD3AA, { 0xD3, 0xAB } },
	{ 0xD3AC, { 0xD3, 0xAD } },
	{ 0xD3AE, { 0xD3, 0xAF } },
	{ 0xD3B0, { 0xD3, 0xB1 } },
	{ 0xD3B2, { 0xD3, 0xB3 } },
	{ 0xD3B4, { 0xD3, 0xB5 } },
	{ 0xD3B6, { 0xD3, 0xB7 } },
	{ 0xD3B8, { 0xD3, 0xB9 } },
	{ 0xD3BA, { 0xD3, 0xBB } },
	{ 0xD3BC, { 0xD3, 0xBD } },
	{ 0xD3BE, { 0xD3, 0xBF } },
	{ 0xD480, { 0xD4, 0x81 } },
	{ 0xD482, { 0xD4, 0x83 } },
	{ 0xD484, { 0xD4, 0x85 } },
	{ 0xD486, { 0xD4, 0x87 } },
	{ 0xD488, { 0xD4, 0x89 } },
	{ 0xD48A, { 0xD4, 0x8B } },
	{ 0xD48C, { 0xD4, 0x8D } },
	{ 0xD48E, { 0xD4, 0x8F } },
	{ 0xD490, { 0xD4, 0x91 } },
	{ 0xD492, { 0xD4, 0x93 } },
	{ 0xD494, { 0xD4, 0x95 } },
	{ 0xD496, { 0xD4, 0x97 } },
	{ 0xD498, { 0xD4, 0x99 } },
	{ 0xD49A, { 0xD4, 0x9B } },
	{ 0xD49C, { 0xD4, 0x9D } },
	{ 0xD49E, { 0xD4, 0x9F } },
	{ 0xD4A0, { 0xD4, 0xA1 } },
	{ 0xD4A2, { 0xD4, 0xA3 } },
	{ 0xD4A4, { 0xD4, 0xA5 } },
	{ 0xD4B1, { 0xD5, 0xA1 } },
	{ 0xD4B2, { 0xD5, 0xA2 } },
	{ 0xD4B3, { 0xD5, 0xA3 } },
	{ 0xD4B4, { 0xD5, 0xA4 } },
	{ 0xD4B5, { 0xD5, 0xA5 } },
	{ 0xD4B6, { 0xD5, 0xA6 } },
	{ 0xD4B7, { 0xD5, 0xA7 } },
	{ 0xD4B8, { 0xD5, 0xA8 } },
	{ 0xD4B9, { 0xD5, 0xA9 } },
	{ 0xD4BA, { 0xD5, 0xAA } },
	{ 0xD4BB, { 0xD5, 0xAB } },
	{ 0xD4BC, { 0xD5, 0xAC } },
	{ 0xD4BD, { 0xD5, 0xAD } },
	{ 0xD4BE, { 0xD5, 0xAE } },
	{ 0xD4BF, { 0xD5, 0xAF } },
	{ 0xD580, { 0xD5, 0xB0 } },
	{ 0xD581, { 0xD5, 0xB1 } },
	{ 0xD582, { 0xD5, 0xB2 } },
	{ 0xD583, { 0xD5, 0xB3 } },
	{ 0xD584, { 0xD5, 0xB4 } },
	{ 0xD585, { 0xD5, 0xB5 } },
	{ 0xD586, { 0xD5, 0xB6 } },
	{ 0xD587, { 0xD5, 0xB7 } },
	{ 0xD588, { 0xD5, 0xB8 } },
	{ 0xD589, { 0xD5, 0xB9 } },
	{ 0xD58A, { 0xD5, 0xBA } },
	{ 0xD58B, { 0xD5, 0xBB } },
	{ 0xD58C, { 0xD5, 0xBC } },
	{ 0xD58D, { 0xD5, 0xBD } },
	{ 0xD58E, { 0xD5, 0xBE } },
	{ 0xD58F, { 0xD5, 0xBF } },
	{ 0xD590, { 0xD6, 0x80 } },
	{ 0xD591, { 0xD6, 0x81 } },
	{ 0xD592, { 0xD6, 0x82 } },
	{ 0xD593, { 0xD6, 0x83 } },
	{ 0xD594, { 0xD6, 0x84 } },
	{ 0xD595, { 0xD6, 0x85 } },
	{ 0xD596, { 0xD6, 0x86 } },
};

const struct {
	const uint16_t key;
	const uint8_t val_len;
	const uint8_t val[3];
} UTF8_UPPER_LOWER_2B_DBL[] = {
	{ 0xC4B0, 3, { 0x69, 0xCC, 0x87 } },
	{ 0xC8BA, 3, { 0xE2, 0xB1, 0xA5 } },
	{ 0xC8BE, 3, { 0xE2, 0xB1, 0xA6 } },
};

const struct {
	const uint32_t key;
	const uint8_t val[3];
} UTF8_UPPER_LOWER_3B[] = {
	{ 0xE182A0, { 0xE2, 0xB4, 0x80 } },
	{ 0xE182A1, { 0xE2, 0xB4, 0x81 } },
	{ 0xE182A2, { 0xE2, 0xB4, 0x82 } },
	{ 0xE182A3, { 0xE2, 0xB4, 0x83 } },
	{ 0xE182A4, { 0xE2, 0xB4, 0x84 } },
	{ 0xE182A5, { 0xE2, 0xB4, 0x85 } },
	{ 0xE182A6, { 0xE2, 0xB4, 0x86 } },
	{ 0xE182A7, { 0xE2, 0xB4, 0x87 } },
	{ 0xE182A8, { 0xE2, 0xB4, 0x88 } },
	{ 0xE182A9, { 0xE2, 0xB4, 0x89 } },
	{ 0xE182AA, { 0xE2, 0xB4, 0x8A } },
	{ 0xE182AB, { 0xE2, 0xB4, 0x8B } },
	{ 0xE182AC, { 0xE2, 0xB4, 0x8C } },
	{ 0xE182AD, { 0xE2, 0xB4, 0x8D } },
	{ 0xE182AE, { 0xE2, 0xB4, 0x8E } },
	{ 0xE182AF, { 0xE2, 0xB4, 0x8F } },
	{ 0xE182B0, { 0xE2, 0xB4, 0x90 } },
	{ 0xE182B1, { 0xE2, 0xB4, 0x91 } },
	{ 0xE182B2, { 0xE2, 0xB4, 0x92 } },
	{ 0xE182B3, { 0xE2, 0xB4, 0x93 } },
	{ 0xE182B4, { 0xE2, 0xB4, 0x94 } },
	{ 0xE182B5, { 0xE2, 0xB4, 0x95 } },
	{ 0xE182B6, { 0xE2, 0xB4, 0x96 } },
	{ 0xE182B7, { 0xE2, 0xB4, 0x97 } },
	{ 0xE182B8, { 0xE2, 0xB4, 0x98 } },
	{ 0xE182B9, { 0xE2, 0xB4, 0x99 } },
	{ 0xE182BA, { 0xE2, 0xB4, 0x9A } },
	{ 0xE182BB, { 0xE2, 0xB4, 0x9B } },
	{ 0xE182BC, { 0xE2, 0xB4, 0x9C } },
	{ 0xE182BD, { 0xE2, 0xB4, 0x9D } },
	{ 0xE182BE, { 0xE2, 0xB4, 0x9E } },
	{ 0xE182BF, { 0xE2, 0xB4, 0x9F } },
	{ 0xE18380, { 0xE2, 0xB4, 0xA0 } },
	{ 0xE18381, { 0xE2, 0xB4, 0xA1 } },
	{ 0xE18382, { 0xE2, 0xB4, 0xA2 } },
	{ 0xE18383, { 0xE2, 0xB4, 0xA3 } },
	{ 0xE18384, { 0xE2, 0xB4, 0xA4 } },
	{ 0xE18385, { 0xE2, 0xB4, 0xA5 } },
	{ 0xE1B880, { 0xE1, 0xB8, 0x81 } },
	{ 0xE1B882, { 0xE1, 0xB8, 0x83 } },
	{ 0xE1B884, { 0xE1, 0xB8, 0x85 } },
	{ 0xE1B886, { 0xE1, 0xB8, 0x87 } },
	{ 0xE1B888, { 0xE1, 0xB8, 0x89 } },
	{ 0xE1B88A, { 0xE1, 0xB8, 0x8B } },
	{ 0xE1B88C, { 0xE1, 0xB8, 0x8D } },
	{ 0xE1B88E, { 0xE1, 0xB8, 0x8F } },
	{ 0xE1B890, { 0xE1, 0xB8, 0x91 } },
	{ 0xE1B892, { 0xE1, 0xB8, 0x93 } },
	{ 0xE1B894, { 0xE1, 0xB8, 0x95 } },
	{ 0xE1B896, { 0xE1, 0xB8, 0x97 } },
	{ 0xE1B898, { 0xE1, 0xB8, 0x99 } },
	{ 0xE1B89A, { 0xE1, 0xB8, 0x9B } },
	{ 0xE1B89C, { 0xE1, 0xB8, 0x9D } },
	{ 0xE1B89E, { 0xE1, 0xB8, 0x9F } },
	{ 0xE1B8A0, { 0xE1, 0xB8, 0xA1 } },
	{ 0xE1B8A2, { 0xE1, 0xB8, 0xA3 } },
	{ 0xE1B8A4, { 0xE1, 0xB8, 0xA5 } },
	{ 0xE1B8A6, { 0xE1, 0xB8, 0xA7 } },
	{ 0xE1B8A8, { 0xE1, 0xB8, 0xA9 } },
	{ 0xE1B8AA, { 0xE1, 0xB8, 0xAB } },
	{ 0xE1B8AC, { 0xE1, 0xB8, 0xAD } },
	{ 0xE1B8AE, { 0xE1, 0xB8, 0xAF } },
	{ 0xE1B8B0, { 0xE1, 0xB8, 0xB1 } },
	{ 0xE1B8B2, { 0xE1, 0xB8, 0xB3 } },
	{ 0xE1B8B4, { 0xE1, 0xB8, 0xB5 } },
	{ 0xE1B8B6, { 0xE1, 0xB8, 0xB7 } },
	{ 0xE1B8B8, { 0xE1, 0xB8, 0xB9 } },
	{ 0xE1B8BA, { 0xE1, 0xB8, 0xBB } },
	{ 0xE1B8BC, { 0xE1, 0xB8, 0xBD } },
	{ 0xE1B8BE, { 0xE1, 0xB8, 0xBF } },
	{ 0xE1B980, { 0xE1, 0xB9, 0x81 } },
	{ 0xE1B982, { 0xE1, 0xB9, 0x83 } },
	{ 0xE1B984, { 0xE1, 0xB9, 0x85 } },
	{ 0xE1B986, { 0xE1, 0xB9, 0x87 } },
	{ 0xE1B988, { 0xE1, 0xB9, 0x89 } },
	{ 0xE1B98A, { 0xE1, 0xB9, 0x8B } },
	{ 0xE1B98C, { 0xE1, 0xB9, 0x8D } },
	{ 0xE1B98E, { 0xE1, 0xB9, 0x8F } },
	{ 0xE1B990, { 0xE1, 0xB9, 0x91 } },
	{ 0xE1B992, { 0xE1, 0xB9, 0x93 } },
	{ 0xE1B994, { 0xE1, 0xB9, 0x95 } },
	{ 0xE1B996, { 0xE1, 0xB9, 0x97 } },
	{ 0xE1B998, { 0xE1, 0xB9, 0x99 } },
	{ 0xE1B99A, { 0xE1, 0xB9, 0x9B } },
	{ 0xE1B99C, { 0xE1, 0xB9, 0x9D } },
	{ 0xE1B99E, { 0xE1, 0xB9, 0x9F } },
	{ 0xE1B9A0, { 0xE1, 0xB9, 0xA1 } },
	{ 0xE1B9A2, { 0xE1, 0xB9, 0xA3 } },
	{ 0xE1B9A4, { 0xE1, 0xB9, 0xA5 } },
	{ 0xE1B9A6, { 0xE1, 0xB9, 0xA7 } },
	{ 0xE1B9A8, { 0xE1, 0xB9, 0xA9 } },
	{ 0xE1B9AA, { 0xE1, 0xB9, 0xAB } },
	{ 0xE1B9AC, { 0xE1, 0xB9, 0xAD } },
	{ 0xE1B9AE, { 0xE1, 0xB9, 0xAF } },
	{ 0xE1B9B0, { 0xE1, 0xB9, 0xB1 } },
	{ 0xE1B9B2, { 0xE1, 0xB9, 0xB3 } },
	{ 0xE1B9B4, { 0xE1, 0xB9, 0xB5 } },
	{ 0xE1B9B6, { 0xE1, 0xB9, 0xB7 } },
	{ 0xE1B9B8, { 0xE1, 0xB9, 0xB9 } },
	{ 0xE1B9BA, { 0xE1, 0xB9, 0xBB } },
	{ 0xE1B9BC, { 0xE1, 0xB9, 0xBD } },
	{ 0xE1B9BE, { 0xE1, 0xB9, 0xBF } },
	{ 0xE1BA80, { 0xE1, 0xBA, 0x81 } },
	{ 0xE1BA82, { 0xE1, 0xBA, 0x83 } },
	{ 0xE1BA84, { 0xE1, 0xBA, 0x85 } },
	{ 0xE1BA86, { 0xE1, 0xBA, 0x87 } },
	{ 0xE1BA88, { 0xE1, 0xBA, 0x89 } },
	{ 0xE1BA8A, { 0xE1, 0xBA, 0x8B } },
	{ 0xE1BA8C, { 0xE1, 0xBA, 0x8D } },
	{ 0xE1BA8E, { 0xE1, 0xBA, 0x8F } },
	{ 0xE1BA90, { 0xE1, 0xBA, 0x91 } },
	{ 0xE1BA92, { 0xE1, 0xBA, 0x93 } },
	{ 0xE1BA94, { 0xE1, 0xBA, 0x95 } },
	{ 0xE1BAA0, { 0xE1, 0xBA, 0xA1 } },
	{ 0xE1BAA2, { 0xE1, 0xBA, 0xA3 } },
	{ 0xE1BAA4, { 0xE1, 0xBA, 0xA5 } },
	{ 0xE1BAA6, { 0xE1, 0xBA, 0xA7 } },
	{ 0xE1BAA8, { 0xE1, 0xBA, 0xA9 } },
	{ 0xE1BAAA, { 0xE1, 0xBA, 0xAB } },
	{ 0xE1BAAC, { 0xE1, 0xBA, 0xAD } },
	{ 0xE1BAAE, { 0xE1, 0xBA, 0xAF } },
	{ 0xE1BAB0, { 0xE1, 0xBA, 0xB1 } },
	{ 0xE1BAB2, { 0xE1, 0xBA, 0xB3 } },
	{ 0xE1BAB4, { 0xE1, 0xBA, 0xB5 } },
	{ 0xE1BAB6, { 0xE1, 0xBA, 0xB7 } },
	{ 0xE1BAB8, { 0xE1, 0xBA, 0xB9 } },
	{ 0xE1BABA, { 0xE1, 0xBA, 0xBB } },
	{ 0xE1BABC, { 0xE1, 0xBA, 0xBD } },
	{ 0xE1BABE, { 0xE1, 0xBA, 0xBF } },
	{ 0xE1BB80, { 0xE1, 0xBB, 0x81 } },
	{ 0xE1BB82, { 0xE1, 0xBB, 0x83 } },
	{ 0xE1BB84, { 0xE1, 0xBB, 0x85 } },
	{ 0xE1BB86, { 0xE1, 0xBB, 0x87 } },
	{ 0xE1BB88, { 0xE1, 0xBB, 0x89 } },
	{ 0xE1BB8A, { 0xE1, 0xBB, 0x8B } },
	{ 0xE1BB8C, { 0xE1, 0xBB, 0x8D } },
	{ 0xE1BB8E, { 0xE1, 0xBB, 0x8F } },
	{ 0xE1BB90, { 0xE1, 0xBB, 0x91 } },
	{ 0xE1BB92, { 0xE1, 0xBB, 0x93 } },
	{ 0xE1BB94, { 0xE1, 0xBB, 0x95 } },
	{ 0xE1BB96, { 0xE1, 0xBB, 0x97 } },
	{ 0xE1BB98, { 0xE1, 0xBB, 0x99 } },
	{ 0xE1BB9A, { 0xE1, 0xBB, 0x9B } },
	{ 0xE1BB9C, { 0xE1, 0xBB, 0x9D } },
	{ 0xE1BB9E, { 0xE1, 0xBB, 0x9F } },
	{ 0xE1BBA0, { 0xE1, 0xBB, 0xA1 } },
	{ 0xE1BBA2, { 0xE1, 0xBB, 0xA3 } },
	{ 0xE1BBA4, { 0xE1, 0xBB, 0xA5 } },
	{ 0xE1BBA6, { 0xE1, 0xBB, 0xA7 } },
	{ 0xE1BBA8, { 0xE1, 0xBB, 0xA9 } },
	{ 0xE1BBAA, { 0xE1, 0xBB, 0xAB } },
	{ 0xE1BBAC, { 0xE1, 0xBB, 0xAD } },
	{ 0xE1BBAE, { 0xE1, 0xBB, 0xAF } },
	{ 0xE1BBB0, { 0xE1, 0xBB, 0xB1 } },
	{ 0xE1BBB2, { 0xE1, 0xBB, 0xB3 } },
	{ 0xE1BBB4, { 0xE1, 0xBB, 0xB5 } },
	{ 0xE1BBB6, { 0xE1, 0xBB, 0xB7 } },
	{ 0xE1BBB8, { 0xE1, 0xBB, 0xB9 } },
	{ 0xE1BBBA, { 0xE1, 0xBB, 0xBB } },
	{ 0xE1BBBC, { 0xE1, 0xBB, 0xBD } },
	{ 0xE1BBBE, { 0xE1, 0xBB, 0xBF } },
	{ 0xE1BC88, { 0xE1, 0xBC, 0x80 } },
	{ 0xE1BC89, { 0xE1, 0xBC, 0x81 } },
	{ 0xE1BC8A, { 0xE1, 0xBC, 0x82 } },
	{ 0xE1BC8B, { 0xE1, 0xBC, 0x83 } },
	{ 0xE1BC8C, { 0xE1, 0xBC, 0x84 } },
	{ 0xE1BC8D, { 0xE1, 0xBC, 0x85 } },
	{ 0xE1BC8E, { 0xE1, 0xBC, 0x86 } },
	{ 0xE1BC8F, { 0xE1, 0xBC, 0x87 } },
	{ 0xE1BC98, { 0xE1, 0xBC, 0x90 } },
	{ 0xE1BC99, { 0xE1, 0xBC, 0x91 } },
	{ 0xE1BC9A, { 0xE1, 0xBC, 0x92 } },
	{ 0xE1BC9B, { 0xE1, 0xBC, 0x93 } },
	{ 0xE1BC9C, { 0xE1, 0xBC, 0x94 } },
	{ 0xE1BC9D, { 0xE1, 0xBC, 0x95 } },
	{ 0xE1BCA8, { 0xE1, 0xBC, 0xA0 } },
	{ 0xE1BCA9, { 0xE1, 0xBC, 0xA1 } },
	{ 0xE1BCAA, { 0xE1, 0xBC, 0xA2 } },
	{ 0xE1BCAB, { 0xE1, 0xBC, 0xA3 } },
	{ 0xE1BCAC, { 0xE1, 0xBC, 0xA4 } },
	{ 0xE1BCAD, { 0xE1, 0xBC, 0xA5 } },
	{ 0xE1BCAE, { 0xE1, 0xBC, 0xA6 } },
	{ 0xE1BCAF, { 0xE1, 0xBC, 0xA7 } },
	{ 0xE1BCB8, { 0xE1, 0xBC, 0xB0 } },
	{ 0xE1BCB9, { 0xE1, 0xBC, 0xB1 } },
	{ 0xE1BCBA, { 0xE1, 0xBC, 0xB2 } },
	{ 0xE1BCBB, { 0xE1, 0xBC, 0xB3 } },
	{ 0xE1BCBC, { 0xE1, 0xBC, 0xB4 } },
	{ 0xE1BCBD, { 0xE1, 0xBC, 0xB5 } },
	{ 0xE1BCBE, { 0xE1, 0xBC, 0xB6 } },
	{ 0xE1BCBF, { 0xE1, 0xBC, 0xB7 } },
	{ 0xE1BD88, { 0xE1, 0xBD, 0x80 } },
	{ 0xE1BD89, { 0xE1, 0xBD, 0x81 } },
	{ 0xE1BD8A, { 0xE1, 0xBD, 0x82 } },
	{ 0xE1BD8B, { 0xE1, 0xBD, 0x83 } },
	{ 0xE1BD8C, { 0xE1, 0xBD, 0x84 } },
	{ 0xE1BD8D, { 0xE1, 0xBD, 0x85 } },
	{ 0xE1BD99, { 0xE1, 0xBD, 0x91 } },
	{ 0xE1BD9B, { 0xE1, 0xBD, 0x93 } },
	{ 0xE1BD9D, { 0xE1, 0xBD, 0x95 } },
	{ 0xE1BD9F, { 0xE1, 0xBD, 0x97 } },
	{ 0xE1BDA8, { 0xE1, 0xBD, 0xA0 } },
	{ 0xE1BDA9, { 0xE1, 0xBD, 0xA1 } },
	{ 0xE1BDAA, { 0xE1, 0xBD, 0xA2 } },
	{ 0xE1BDAB, { 0xE1, 0xBD, 0xA3 } },
	{ 0xE1BDAC, { 0xE1, 0xBD, 0xA4 } },
	{ 0xE1BDAD, { 0xE1, 0xBD, 0xA5 } },
	{ 0xE1BDAE, { 0xE1, 0xBD, 0xA6 } },
	{ 0xE1BDAF, { 0xE1, 0xBD, 0xA7 } },
	{ 0xE1BE88, { 0xE1, 0xBE, 0x80 } },
	{ 0xE1BE89, { 0xE1, 0xBE, 0x81 } },
	{ 0xE1BE8A, { 0xE1, 0xBE, 0x82 } },
	{ 0xE1BE8B, { 0xE1, 0xBE, 0x83 } },
	{ 0xE1BE8C, { 0xE1, 0xBE, 0x84 } },
	{ 0xE1BE8D, { 0xE1, 0xBE, 0x85 } },
	{ 0xE1BE8E, { 0xE1, 0xBE, 0x86 } },
	{ 0xE1BE8F, { 0xE1, 0xBE, 0x87 } },
	{ 0xE1BE98, { 0xE1, 0xBE, 0x90 } },
	{ 0xE1BE99, { 0xE1, 0xBE, 0x91 } },
	{ 0xE1BE9A, { 0xE1, 0xBE, 0x92 } },
	{ 0xE1BE9B, { 0xE1, 0xBE, 0x93 } },
	{ 0xE1BE9C, { 0xE1, 0xBE, 0x94 } },
	{ 0xE1BE9D, { 0xE1, 0xBE, 0x95 } },
	{ 0xE1BE9E, { 0xE1, 0xBE, 0x96 } },
	{ 0xE1BE9F, { 0xE1, 0xBE, 0x97 } },
	{ 0xE1BEA8, { 0xE1, 0xBE, 0xA0 } },
	{ 0xE1BEA9, { 0xE1, 0xBE, 0xA1 } },
	{ 0xE1BEAA, { 0xE1, 0xBE, 0xA2 } },
	{ 0xE1BEAB, { 0xE1, 0xBE, 0xA3 } },
	{ 0xE1BEAC, { 0xE1, 0xBE, 0xA4 } },
	{ 0xE1BEAD, { 0xE1, 0xBE, 0xA5 } },
	{ 0xE1BEAE, { 0xE1, 0xBE, 0xA6 } },
	{ 0xE1BEAF, { 0xE1, 0xBE, 0xA7 } },
	{ 0xE1BEB8, { 0xE1, 0xBE, 0xB0 } },
	{ 0xE1BEB9, { 0xE1, 0xBE, 0xB1 } },
	{ 0xE1BEBA, { 0xE1, 0xBD, 0xB0 } },
	{ 0xE1BEBB, { 0xE1, 0xBD, 0xB1 } },
	{ 0xE1BEBC, { 0xE1, 0xBE, 0xB3 } },
	{ 0xE1BF88, { 0xE1, 0xBD, 0xB2 } },
	{ 0xE1BF89, { 0xE1, 0xBD, 0xB3 } },
	{ 0xE1BF8A, { 0xE1, 0xBD, 0xB4 } },
	{ 0xE1BF8B, { 0xE1, 0xBD, 0xB5 } },
	{ 0xE1BF8C, { 0xE1, 0xBF, 0x83 } },
	{ 0xE1BF98, { 0xE1, 0xBF, 0x90 } },
	{ 0xE1BF99, { 0xE1, 0xBF, 0x91 } },
	{ 0xE1BF9A, { 0xE1, 0xBD, 0xB6 } },
	{ 0xE1BF9B, { 0xE1, 0xBD, 0xB7 } },
	{ 0xE1BFA8, { 0xE1, 0xBF, 0xA0 } },
	{ 0xE1BFA9, { 0xE1, 0xBF, 0xA1 } },
	{ 0xE1BFAA, { 0xE1, 0xBD, 0xBA } },
	{ 0xE1BFAB, { 0xE1, 0xBD, 0xBB } },
	{ 0xE1BFAC, { 0xE1, 0xBF, 0xA5 } },
	{ 0xE1BFB8, { 0xE1, 0xBD, 0xB8 } },
	{ 0xE1BFB9, { 0xE1, 0xBD, 0xB9 } },
	{ 0xE1BFBA, { 0xE1, 0xBD, 0xBC } },
	{ 0xE1BFBB, { 0xE1, 0xBD, 0xBD } },
	{ 0xE1BFBC, { 0xE1, 0xBF, 0xB3 } },
	{ 0xE284B2, { 0xE2, 0x85, 0x8E } },
	{ 0xE285A0, { 0xE2, 0x85, 0xB0 } },
	{ 0xE285A1, { 0xE2, 0x85, 0xB1 } },
	{ 0xE285A2, { 0xE2, 0x85, 0xB2 } },
	{ 0xE285A3, { 0xE2, 0x85, 0xB3 } },
	{ 0xE285A4, { 0xE2, 0x85, 0xB4 } },
	{ 0xE285A5, { 0xE2, 0x85, 0xB5 } },
	{ 0xE285A6, { 0xE2, 0x85, 0xB6 } },
	{ 0xE285A7, { 0xE2, 0x85, 0xB7 } },
	{ 0xE285A8, { 0xE2, 0x85, 0xB8 } },
	{ 0xE285A9, { 0xE2, 0x85, 0xB9 } },
	{ 0xE285AA, { 0xE2, 0x85, 0xBA } },
	{ 0xE285AB, { 0xE2, 0x85, 0xBB } },
	{ 0xE285AC, { 0xE2, 0x85, 0xBC } },
	{ 0xE285AD, { 0xE2, 0x85, 0xBD } },
	{ 0xE285AE, { 0xE2, 0x85, 0xBE } },
	{ 0xE285AF, { 0xE2, 0x85, 0xBF } },
	{ 0xE28683, { 0xE2, 0x86, 0x84 } },
	{ 0xE292B6, { 0xE2, 0x93, 0x90 } },
	{ 0xE292B7, { 0xE2, 0x93, 0x91 } },
	{ 0xE292B8, { 0xE2, 0x93, 0x92 } },
	{ 0xE292B9, { 0xE2, 0x93, 0x93 } },
	{ 0xE292BA, { 0xE2, 0x93, 0x94 } },
	{ 0xE292BB, { 0xE2, 0x93, 0x95 } },
	{ 0xE292BC, { 0xE2, 0x93, 0x96 } },
	{ 0xE292BD, { 0xE2, 0x93, 0x97 } },
	{ 0xE292BE, { 0xE2, 0x93, 0x98 } },
	{ 0xE292BF, { 0xE2, 0x93, 0x99 } },
	{ 0xE29380, { 0xE2, 0x93, 0x9A } },
	{ 0xE29381, { 0xE2, 0x93, 0x9B } },
	{ 0xE29382, { 0xE2, 0x93, 0x9C } },
	{ 0xE29383, { 0xE2, 0x93, 0x9D } },
	{ 0xE29384, { 0xE2, 0x93, 0x9E } },
	{ 0xE29385, { 0xE2, 0x93, 0x9F } },
	{ 0xE29386, { 0xE2, 0x93, 0xA0 } },
	{ 0xE29387, { 0xE2, 0x93, 0xA1 } },
	{ 0xE29388, { 0xE2, 0x93, 0xA2 } },
	{ 0xE29389, { 0xE2, 0x93, 0xA3 } },
	{ 0xE2938A, { 0xE2, 0x93, 0xA4 } },
	{ 0xE2938B, { 0xE2, 0x93, 0xA5 } },
	{ 0xE2938C, { 0xE2, 0x93, 0xA6 } },
	{ 0xE2938D, { 0xE2, 0x93, 0xA7 } },
	{ 0xE2938E, { 0xE2, 0x93, 0xA8 } },
	{ 0xE2938F, { 0xE2, 0x93, 0xA9 } },
	{ 0xE2B080, { 0xE2, 0xB0, 0xB0 } },
	{ 0xE2B081, { 0xE2, 0xB0, 0xB1 } },
	{ 0xE2B082, { 0xE2, 0xB0, 0xB2 } },
	{ 0xE2B083, { 0xE2, 0xB0, 0xB3 } },
	{ 0xE2B084, { 0xE2, 0xB0, 0xB4 } },
	{ 0xE2B085, { 0xE2, 0xB0, 0xB5 } },
	{ 0xE2B086, { 0xE2, 0xB0, 0xB6 } },
	{ 0xE2B087, { 0xE2, 0xB0, 0xB7 } },
	{ 0xE2B088, { 0xE2, 0xB0, 0xB8 } },
	{ 0xE2B089, { 0xE2, 0xB0, 0xB9 } },
	{ 0xE2B08A, { 0xE2, 0xB0, 0xBA } },
	{ 0xE2B08B, { 0xE2, 0xB0, 0xBB } },
	{ 0xE2B08C, { 0xE2, 0xB0, 0xBC } },
	{ 0xE2B08D, { 0xE2, 0xB0, 0xBD } },
	{ 0xE2B08E, { 0xE2, 0xB0, 0xBE } },
	{ 0xE2B08F, { 0xE2, 0xB0, 0xBF } },
	{ 0xE2B090, { 0xE2, 0xB1, 0x80 } },
	{ 0xE2B091, { 0xE2, 0xB1, 0x81 } },
	{ 0xE2B092, { 0xE2, 0xB1, 0x82 } },
	{ 0xE2B093, { 0xE2, 0xB1, 0x83 } },
	{ 0xE2B094, { 0xE2, 0xB1, 0x84 } },
	{ 0xE2B095, { 0xE2, 0xB1, 0x85 } },
	{ 0xE2B096, { 0xE2, 0xB1, 0x86 } },
	{ 0xE2B097, { 0xE2, 0xB1, 0x87 } },
	{ 0xE2B098, { 0xE2, 0xB1, 0x88 } },
	{ 0xE2B099, { 0xE2, 0xB1, 0x89 } },
	{ 0xE2B09A, { 0xE2, 0xB1, 0x8A } },
	{ 0xE2B09B, { 0xE2, 0xB1, 0x8B } },
	{ 0xE2B09C, { 0xE2, 0xB1, 0x8C } },
	{ 0xE2B09D, { 0xE2, 0xB1, 0x8D } },
	{ 0xE2B09E, { 0xE2, 0xB1, 0x8E } },
	{ 0xE2B09F, { 0xE2, 0xB1, 0x8F } },
	{ 0xE2B0A0, { 0xE2, 0xB1, 0x90 } },
	{ 0xE2B0A1, { 0xE2, 0xB1, 0x91 } },
	{ 0xE2B0A2, { 0xE2, 0xB1, 0x92 } },
	{ 0xE2B0A3, { 0xE2, 0xB1, 0x93 } },
	{ 0xE2B0A4, { 0xE2, 0xB1, 0x94 } },
	{ 0xE2B0A5, { 0xE2, 0xB1, 0x95 } },
	{ 0xE2B0A6, { 0xE2, 0xB1, 0x96 } },
	{ 0xE2B0A7, { 0xE2, 0xB1, 0x97 } },
	{ 0xE2B0A8, { 0xE2, 0xB1, 0x98 } },
	{ 0xE2B0A9, { 0xE2, 0xB1, 0x99 } },
	{ 0xE2B0AA, { 0xE2, 0xB1, 0x9A } },
	{ 0xE2B0AB, { 0xE2, 0xB1, 0x9B } },
	{ 0xE2B0AC, { 0xE2, 0xB1, 0x9C } },
	{ 0xE2B0AD, { 0xE2, 0xB1, 0x9D } },
	{ 0xE2B0AE, { 0xE2, 0xB1, 0x9E } },
	{ 0xE2B1A0, { 0xE2, 0xB1, 0xA1 } },
	{ 0xE2B1A3, { 0xE1, 0xB5, 0xBD } },
	{ 0xE2B1A7, { 0xE2, 0xB1, 0xA8 } },
	{ 0xE2B1A9, { 0xE2, 0xB1, 0xAA } },
	{ 0xE2B1AB, { 0xE2, 0xB1, 0xAC } },
	{ 0xE2B1B2, { 0xE2, 0xB1, 0xB3 } },
	{ 0xE2B1B5, { 0xE2, 0xB1, 0xB6 } },
	{ 0xE2B280, { 0xE2, 0xB2, 0x81 } },
	{ 0xE2B282, { 0xE2, 0xB2, 0x83 } },
	{ 0xE2B284, { 0xE2, 0xB2, 0x85 } },
	{ 0xE2B286, { 0xE2, 0xB2, 0x87 } },
	{ 0xE2B288, { 0xE2, 0xB2, 0x89 } },
	{ 0xE2B28A, { 0xE2, 0xB2, 0x8B } },
	{ 0xE2B28C, { 0xE2, 0xB2, 0x8D } },
	{ 0xE2B28E, { 0xE2, 0xB2, 0x8F } },
	{ 0xE2B290, { 0xE2, 0xB2, 0x91 } },
	{ 0xE2B292, { 0xE2, 0xB2, 0x93 } },
	{ 0xE2B294, { 0xE2, 0xB2, 0x95 } },
	{ 0xE2B296, { 0xE2, 0xB2, 0x97 } },
	{ 0xE2B298, { 0xE2, 0xB2, 0x99 } },
	{ 0xE2B29A, { 0xE2, 0xB2, 0x9B } },
	{ 0xE2B29C, { 0xE2, 0xB2, 0x9D } },
	{ 0xE2B29E, { 0xE2, 0xB2, 0x9F } },
	{ 0xE2B2A0, { 0xE2, 0xB2, 0xA1 } },
	{ 0xE2B2A2, { 0xE2, 0xB2, 0xA3 } },
	{ 0xE2B2A4, { 0xE2, 0xB2, 0xA5 } },
	{ 0xE2B2A6, { 0xE2, 0xB2, 0xA7 } },
	{ 0xE2B2A8, { 0xE2, 0xB2, 0xA9 } },
	{ 0xE2B2AA, { 0xE2, 0xB2, 0xAB } },
	{ 0xE2B2AC, { 0xE2, 0xB2, 0xAD } },
	{ 0xE2B2AE, { 0xE2, 0xB2, 0xAF } },
	{ 0xE2B2B0, { 0xE2, 0xB2, 0xB1 } },
	{ 0xE2B2B2, { 0xE2, 0xB2, 0xB3 } },
	{ 0xE2B2B4, { 0xE2, 0xB2, 0xB5 } },
	{ 0xE2B2B6, { 0xE2, 0xB2, 0xB7 } },
	{ 0xE2B2B8, { 0xE2, 0xB2, 0xB9 } },
	{ 0xE2B2BA, { 0xE2, 0xB2, 0xBB } },
	{ 0xE2B2BC, { 0xE2, 0xB2, 0xBD } },
	{ 0xE2B2BE, { 0xE2, 0xB2, 0xBF } },
	{ 0xE2B380, { 0xE2, 0xB3, 0x81 } },
	{ 0xE2B382, { 0xE2, 0xB3, 0x83 } },
	{ 0xE2B384, { 0xE2, 0xB3, 0x85 } },
	{ 0xE2B386, { 0xE2, 0xB3, 0x87 } },
	{ 0xE2B388, { 0xE2, 0xB3, 0x89 } },
	{ 0xE2B38A, { 0xE2, 0xB3, 0x8B } },
	{ 0xE2B38C, { 0xE2, 0xB3, 0x8D } },
	{ 0xE2B38E, { 0xE2, 0xB3, 0x8F } },
	{ 0xE2B390, { 0xE2, 0xB3, 0x91 } },
	{ 0xE2B392, { 0xE2, 0xB3, 0x93 } },
	{ 0xE2B394, { 0xE2, 0xB3, 0x95 } },
	{ 0xE2B396, { 0xE2, 0xB3, 0x97 } },
	{ 0xE2B398, { 0xE2, 0xB3, 0x99 } },
	{ 0xE2B39A, { 0xE2, 0xB3, 0x9B } },
	{ 0xE2B39C, { 0xE2, 0xB3, 0x9D } },
	{ 0xE2B39E, { 0xE2, 0xB3, 0x9F } },
	{ 0xE2B3A0, { 0xE2, 0xB3, 0xA1 } },
	{ 0xE2B3A2, { 0xE2, 0xB3, 0xA3 } },
	{ 0xE2B3AB, { 0xE2, 0xB3, 0xAC } },
	{ 0xE2B3AD, { 0xE2, 0xB3, 0xAE } },
	{ 0xEA9980, { 0xEA, 0x99, 0x81 } },
	{ 0xEA9982, { 0xEA, 0x99, 0x83 } },
	{ 0xEA9984, { 0xEA, 0x99, 0x85 } },
	{ 0xEA9986, { 0xEA, 0x99, 0x87 } },
	{ 0xEA9988, { 0xEA, 0x99, 0x89 } },
	{ 0xEA998A, { 0xEA, 0x99, 0x8B } },
	{ 0xEA998C, { 0xEA, 0x99, 0x8D } },
	{ 0xEA998E, { 0xEA, 0x99, 0x8F } },
	{ 0xEA9990, { 0xEA, 0x99, 0x91 } },
	{ 0xEA9992, { 0xEA, 0x99, 0x93 } },
	{ 0xEA9994, { 0xEA, 0x99, 0x95 } },
	{ 0xEA9996, { 0xEA, 0x99, 0x97 } },
	{ 0xEA9998, { 0xEA, 0x99, 0x99 } },
	{ 0xEA999A, { 0xEA, 0x99, 0x9B } },
	{ 0xEA999C, { 0xEA, 0x99, 0x9D } },
	{ 0xEA999E, { 0xEA, 0x99, 0x9F } },
	{ 0xEA99A2, { 0xEA, 0x99, 0xA3 } },
	{ 0xEA99A4, { 0xEA, 0x99, 0xA5 } },
	{ 0xEA99A6, { 0xEA, 0x99, 0xA7 } },
	{ 0xEA99A8, { 0xEA, 0x99, 0xA9 } },
	{ 0xEA99AA, { 0xEA, 0x99, 0xAB } },
	{ 0xEA99AC, { 0xEA, 0x99, 0xAD } },
	{ 0xEA9A80, { 0xEA, 0x9A, 0x81 } },
	{ 0xEA9A82, { 0xEA, 0x9A, 0x83 } },
	{ 0xEA9A84, { 0xEA, 0x9A, 0x85 } },
	{ 0xEA9A86, { 0xEA, 0x9A, 0x87 } },
	{ 0xEA9A88, { 0xEA, 0x9A, 0x89 } },
	{ 0xEA9A8A, { 0xEA, 0x9A, 0x8B } },
	{ 0xEA9A8C, { 0xEA, 0x9A, 0x8D } },
	{ 0xEA9A8E, { 0xEA, 0x9A, 0x8F } },
	{ 0xEA9A90, { 0xEA, 0x9A, 0x91 } },
	{ 0xEA9A92, { 0xEA, 0x9A, 0x93 } },
	{ 0xEA9A94, { 0xEA, 0x9A, 0x95 } },
	{ 0xEA9A96, { 0xEA, 0x9A, 0x97 } },
	{ 0xEA9CA2, { 0xEA, 0x9C, 0xA3 } },
	{ 0xEA9CA4, { 0xEA, 0x9C, 0xA5 } },
	{ 0xEA9CA6, { 0xEA, 0x9C, 0xA7 } },
	{ 0xEA9CA8, { 0xEA, 0x9C, 0xA9 } },
	{ 0xEA9CAA, { 0xEA, 0x9C, 0xAB } },
	{ 0xEA9CAC, { 0xEA, 0x9C, 0xAD } },
	{ 0xEA9CAE, { 0xEA, 0x9C, 0xAF } },
	{ 0xEA9CB2, { 0xEA, 0x9C, 0xB3 } },
	{ 0xEA9CB4, { 0xEA, 0x9C, 0xB5 } },
	{ 0xEA9CB6, { 0xEA, 0x9C, 0xB7 } },
	{ 0xEA9CB8, { 0xEA, 0x9C, 0xB9 } },
	{ 0xEA9CBA, { 0xEA, 0x9C, 0xBB } },
	{ 0xEA9CBC, { 0xEA, 0x9C, 0xBD } },
	{ 0xEA9CBE, { 0xEA, 0x9C, 0xBF } },
	{ 0xEA9D80, { 0xEA, 0x9D, 0x81 } },
	{ 0xEA9D82, { 0xEA, 0x9D, 0x83 } },
	{ 0xEA9D84, { 0xEA, 0x9D, 0x85 } },
	{ 0xEA9D86, { 0xEA, 0x9D, 0x87 } },
	{ 0xEA9D88, { 0xEA, 0x9D, 0x89 } },
	{ 0xEA9D8A, { 0xEA, 0x9D, 0x8B } },
	{ 0xEA9D8C, { 0xEA, 0x9D, 0x8D } },
	{ 0xEA9D8E, { 0xEA, 0x9D, 0x8F } },
	{ 0xEA9D90, { 0xEA, 0x9D, 0x91 } },
	{ 0xEA9D92, { 0xEA, 0x9D, 0x93 } },
	{ 0xEA9D94, { 0xEA, 0x9D, 0x95 } },
	{ 0xEA9D96, { 0xEA, 0x9D, 0x97 } },
	{ 0xEA9D98, { 0xEA, 0x9D, 0x99 } },
	{ 0xEA9D9A, { 0xEA, 0x9D, 0x9B } },
	{ 0xEA9D9C, { 0xEA, 0x9D, 0x9D } },
	{ 0xEA9D9E, { 0xEA, 0x9D, 0x9F } },
	{ 0xEA9DA0, { 0xEA, 0x9D, 0xA1 } },
	{ 0xEA9DA2, { 0xEA, 0x9D, 0xA3 } },
	{ 0xEA9DA4, { 0xEA, 0x9D, 0xA5 } },
	{ 0xEA9DA6, { 0xEA, 0x9D, 0xA7 } },
	{ 0xEA9DA8, { 0xEA, 0x9D, 0xA9 } },
	{ 0xEA9DAA, { 0xEA, 0x9D, 0xAB } },
	{ 0xEA9DAC, { 0xEA, 0x9D, 0xAD } },
	{ 0xEA9DAE, { 0xEA, 0x9D, 0xAF } },
	{ 0xEA9DB9, { 0xEA, 0x9D, 0xBA } },
	{ 0xEA9DBB, { 0xEA, 0x9D, 0xBC } },
	{ 0xEA9DBD, { 0xE1, 0xB5, 0xB9 } },
	{ 0xEA9DBE, { 0xEA, 0x9D, 0xBF } },
	{ 0xEA9E80, { 0xEA, 0x9E, 0x81 } },
	{ 0xEA9E82, { 0xEA, 0x9E, 0x83 } },
	{ 0xEA9E84, { 0xEA, 0x9E, 0x85 } },
	{ 0xEA9E86, { 0xEA, 0x9E, 0x87 } },
	{ 0xEA9E8B, { 0xEA, 0x9E, 0x8C } },
	{ 0xEFBCA1, { 0xEF, 0xBD, 0x81 } },
	{ 0xEFBCA2, { 0xEF, 0xBD, 0x82 } },
	{ 0xEFBCA3, { 0xEF, 0xBD, 0x83 } },
	{ 0xEFBCA4, { 0xEF, 0xBD, 0x84 } },
	{ 0xEFBCA5, { 0xEF, 0xBD, 0x85 } },
	{ 0xEFBCA6, { 0xEF, 0xBD, 0x86 } },
	{ 0xEFBCA7, { 0xEF, 0xBD, 0x87 } },
	{ 0xEFBCA8, { 0xEF, 0xBD, 0x88 } },
	{ 0xEFBCA9, { 0xEF, 0xBD, 0x89 } },
	{ 0xEFBCAA, { 0xEF, 0xBD, 0x8A } },
	{ 0xEFBCAB, { 0xEF, 0xBD, 0x8B } },
	{ 0xEFBCAC, { 0xEF, 0xBD, 0x8C } },
	{ 0xEFBCAD, { 0xEF, 0xBD, 0x8D } },
	{ 0xEFBCAE, { 0xEF, 0xBD, 0x8E } },
	{ 0xEFBCAF, { 0xEF, 0xBD, 0x8F } },
	{ 0xEFBCB0, { 0xEF, 0xBD, 0x90 } },
	{ 0xEFBCB1, { 0xEF, 0xBD, 0x91 } },
	{ 0xEFBCB2, { 0xEF, 0xBD, 0x92 } },
	{ 0xEFBCB3, { 0xEF, 0xBD, 0x93 } },
	{ 0xEFBCB4, { 0xEF, 0xBD, 0x94 } },
	{ 0xEFBCB5, { 0xEF, 0xBD, 0x95 } },
	{ 0xEFBCB6, { 0xEF, 0xBD, 0x96 } },
	{ 0xEFBCB7, { 0xEF, 0xBD, 0x97 } },
	{ 0xEFBCB8, { 0xEF, 0xBD, 0x98 } },
	{ 0xEFBCB9, { 0xEF, 0xBD, 0x99 } },
	{ 0xEFBCBA, { 0xEF, 0xBD, 0x9A } },
};

const struct {
	const uint32_t key;
	const uint8_t val_len;
	const uint8_t val[2];
} UTF8_UPPER_LOWER_3B_DBL[] = {
	{ 0xE1BA9E, 2, { 0xC3, 0x9F } },
	{ 0xE284A6, 2, { 0xCF, 0x89 } },
	{ 0xE284AA, 1, { 0x6B } },
	{ 0xE284AB, 2, { 0xC3, 0xA5 } },
	{ 0xE2B1A2, 2, { 0xC9, 0xAB } },
	{ 0xE2B1A4, 2, { 0xC9, 0xBD } },
	{ 0xE2B1AD, 2, { 0xC9, 0x91 } },
	{ 0xE2B1AE, 2, { 0xC9, 0xB1 } },
	{ 0xE2B1AF, 2, { 0xC9, 0x90 } },
	{ 0xE2B1B0, 2, { 0xC9, 0x92 } },
	{ 0xE2B1BE, 2, { 0xC8, 0xBF } },
	{ 0xE2B1BF, 2, { 0xC9, 0x80 } },
};

const struct {
	const uint32_t key;
	const uint8_t val[4];
} UTF8_UPPER_LOWER_4B[] = {
	{ 0xF0909080, { 0xF0, 0x90, 0x90, 0xA8 } },
	{ 0xF0909081, { 0xF0, 0x90, 0x90, 0xA9 } },
	{ 0xF0909082, { 0xF0, 0x90, 0x90, 0xAA } },
	{ 0xF0909083, { 0xF0, 0x90, 0x90, 0xAB } },
	{ 0xF0909084, { 0xF0, 0x90, 0x90, 0xAC } },
	{ 0xF0909085, { 0xF0, 0x90, 0x90, 0xAD } },
	{ 0xF0909086, { 0xF0, 0x90, 0x90, 0xAE } },
	{ 0xF0909087, { 0xF0, 0x90, 0x90, 0xAF } },
	{ 0xF0909088, { 0xF0, 0x90, 0x90, 0xB0 } },
	{ 0xF0909089, { 0xF0, 0x90, 0x90, 0xB1 } },
	{ 0xF090908A, { 0xF0, 0x90, 0x90, 0xB2 } },
	{ 0xF090908B, { 0xF0, 0x90, 0x90, 0xB3 } },
	{ 0xF090908C, { 0xF0, 0x90, 0x90, 0xB4 } },
	{ 0xF090908D, { 0xF0, 0x90, 0x90, 0xB5 } },
	{ 0xF090908E, { 0xF0, 0x90, 0x90, 0xB6 } },
	{ 0xF090908F, { 0xF0, 0x90, 0x90, 0xB7 } },
	{ 0xF0909090, { 0xF0, 0x90, 0x90, 0xB8 } },
	{ 0xF0909091, { 0xF0, 0x90, 0x90, 0xB9 } },
	{ 0xF0909092, { 0xF0, 0x90, 0x90, 0xBA } },
	{ 0xF0909093, { 0xF0, 0x90, 0x90, 0xBB } },
	{ 0xF0909094, { 0xF0, 0x90, 0x90, 0xBC } },
	{ 0xF0909095, { 0xF0, 0x90, 0x90, 0xBD } },
	{ 0xF0909096, { 0xF0, 0x90, 0x90, 0xBE } },
	{ 0xF0909097, { 0xF0, 0x90, 0x90, 0xBF } },
	{ 0xF0909098, { 0xF0, 0x90, 0x91, 0x80 } },
	{ 0xF0909099, { 0xF0, 0x90, 0x91, 0x81 } },
	{ 0xF090909A, { 0xF0, 0x90, 0x91, 0x82 } },
	{ 0xF090909B, { 0xF0, 0x90, 0x91, 0x83 } },
	{ 0xF090909C, { 0xF0, 0x90, 0x91, 0x84 } },
	{ 0xF090909D, { 0xF0, 0x90, 0x91, 0x85 } },
	{ 0xF090909E, { 0xF0, 0x90, 0x91, 0x86 } },
	{ 0xF090909F, { 0xF0, 0x90, 0x91, 0x87 } },
	{ 0xF09090A0, { 0xF0, 0x90, 0x91, 0x88 } },
	{ 0xF09090A1, { 0xF0, 0x90, 0x91, 0x89 } },
	{ 0xF09090A2, { 0xF0, 0x90, 0x91, 0x8A } },
	{ 0xF09090A3, { 0xF0, 0x90, 0x91, 0x8B } },
	{ 0xF09090A4, { 0xF0, 0x90, 0x91, 0x8C } },
	{ 0xF09090A5, { 0xF0, 0x90, 0x91, 0x8D } },
	{ 0xF09090A6, { 0xF0, 0x90, 0x91, 0x8E } },
	{ 0xF09090A7, { 0xF0, 0x90, 0x91, 0x8F } },
};
