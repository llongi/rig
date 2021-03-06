#include <stdint.h>

const struct {
	const uint16_t key;
	const uint8_t val[2];
} UTF8_LOWER_UPPER_2B[] = {
	{ 0xC2B5, { 0xCE, 0x9C } },
	{ 0xC39F, { 0x53, 0x53 } },
	{ 0xC3A0, { 0xC3, 0x80 } },
	{ 0xC3A1, { 0xC3, 0x81 } },
	{ 0xC3A2, { 0xC3, 0x82 } },
	{ 0xC3A3, { 0xC3, 0x83 } },
	{ 0xC3A4, { 0xC3, 0x84 } },
	{ 0xC3A5, { 0xC3, 0x85 } },
	{ 0xC3A6, { 0xC3, 0x86 } },
	{ 0xC3A7, { 0xC3, 0x87 } },
	{ 0xC3A8, { 0xC3, 0x88 } },
	{ 0xC3A9, { 0xC3, 0x89 } },
	{ 0xC3AA, { 0xC3, 0x8A } },
	{ 0xC3AB, { 0xC3, 0x8B } },
	{ 0xC3AC, { 0xC3, 0x8C } },
	{ 0xC3AD, { 0xC3, 0x8D } },
	{ 0xC3AE, { 0xC3, 0x8E } },
	{ 0xC3AF, { 0xC3, 0x8F } },
	{ 0xC3B0, { 0xC3, 0x90 } },
	{ 0xC3B1, { 0xC3, 0x91 } },
	{ 0xC3B2, { 0xC3, 0x92 } },
	{ 0xC3B3, { 0xC3, 0x93 } },
	{ 0xC3B4, { 0xC3, 0x94 } },
	{ 0xC3B5, { 0xC3, 0x95 } },
	{ 0xC3B6, { 0xC3, 0x96 } },
	{ 0xC3B8, { 0xC3, 0x98 } },
	{ 0xC3B9, { 0xC3, 0x99 } },
	{ 0xC3BA, { 0xC3, 0x9A } },
	{ 0xC3BB, { 0xC3, 0x9B } },
	{ 0xC3BC, { 0xC3, 0x9C } },
	{ 0xC3BD, { 0xC3, 0x9D } },
	{ 0xC3BE, { 0xC3, 0x9E } },
	{ 0xC3BF, { 0xC5, 0xB8 } },
	{ 0xC481, { 0xC4, 0x80 } },
	{ 0xC483, { 0xC4, 0x82 } },
	{ 0xC485, { 0xC4, 0x84 } },
	{ 0xC487, { 0xC4, 0x86 } },
	{ 0xC489, { 0xC4, 0x88 } },
	{ 0xC48B, { 0xC4, 0x8A } },
	{ 0xC48D, { 0xC4, 0x8C } },
	{ 0xC48F, { 0xC4, 0x8E } },
	{ 0xC491, { 0xC4, 0x90 } },
	{ 0xC493, { 0xC4, 0x92 } },
	{ 0xC495, { 0xC4, 0x94 } },
	{ 0xC497, { 0xC4, 0x96 } },
	{ 0xC499, { 0xC4, 0x98 } },
	{ 0xC49B, { 0xC4, 0x9A } },
	{ 0xC49D, { 0xC4, 0x9C } },
	{ 0xC49F, { 0xC4, 0x9E } },
	{ 0xC4A1, { 0xC4, 0xA0 } },
	{ 0xC4A3, { 0xC4, 0xA2 } },
	{ 0xC4A5, { 0xC4, 0xA4 } },
	{ 0xC4A7, { 0xC4, 0xA6 } },
	{ 0xC4A9, { 0xC4, 0xA8 } },
	{ 0xC4AB, { 0xC4, 0xAA } },
	{ 0xC4AD, { 0xC4, 0xAC } },
	{ 0xC4AF, { 0xC4, 0xAE } },
	{ 0xC4B3, { 0xC4, 0xB2 } },
	{ 0xC4B5, { 0xC4, 0xB4 } },
	{ 0xC4B7, { 0xC4, 0xB6 } },
	{ 0xC4BA, { 0xC4, 0xB9 } },
	{ 0xC4BC, { 0xC4, 0xBB } },
	{ 0xC4BE, { 0xC4, 0xBD } },
	{ 0xC580, { 0xC4, 0xBF } },
	{ 0xC582, { 0xC5, 0x81 } },
	{ 0xC584, { 0xC5, 0x83 } },
	{ 0xC586, { 0xC5, 0x85 } },
	{ 0xC588, { 0xC5, 0x87 } },
	{ 0xC58B, { 0xC5, 0x8A } },
	{ 0xC58D, { 0xC5, 0x8C } },
	{ 0xC58F, { 0xC5, 0x8E } },
	{ 0xC591, { 0xC5, 0x90 } },
	{ 0xC593, { 0xC5, 0x92 } },
	{ 0xC595, { 0xC5, 0x94 } },
	{ 0xC597, { 0xC5, 0x96 } },
	{ 0xC599, { 0xC5, 0x98 } },
	{ 0xC59B, { 0xC5, 0x9A } },
	{ 0xC59D, { 0xC5, 0x9C } },
	{ 0xC59F, { 0xC5, 0x9E } },
	{ 0xC5A1, { 0xC5, 0xA0 } },
	{ 0xC5A3, { 0xC5, 0xA2 } },
	{ 0xC5A5, { 0xC5, 0xA4 } },
	{ 0xC5A7, { 0xC5, 0xA6 } },
	{ 0xC5A9, { 0xC5, 0xA8 } },
	{ 0xC5AB, { 0xC5, 0xAA } },
	{ 0xC5AD, { 0xC5, 0xAC } },
	{ 0xC5AF, { 0xC5, 0xAE } },
	{ 0xC5B1, { 0xC5, 0xB0 } },
	{ 0xC5B3, { 0xC5, 0xB2 } },
	{ 0xC5B5, { 0xC5, 0xB4 } },
	{ 0xC5B7, { 0xC5, 0xB6 } },
	{ 0xC5BA, { 0xC5, 0xB9 } },
	{ 0xC5BC, { 0xC5, 0xBB } },
	{ 0xC5BE, { 0xC5, 0xBD } },
	{ 0xC680, { 0xC9, 0x83 } },
	{ 0xC683, { 0xC6, 0x82 } },
	{ 0xC685, { 0xC6, 0x84 } },
	{ 0xC688, { 0xC6, 0x87 } },
	{ 0xC68C, { 0xC6, 0x8B } },
	{ 0xC692, { 0xC6, 0x91 } },
	{ 0xC695, { 0xC7, 0xB6 } },
	{ 0xC699, { 0xC6, 0x98 } },
	{ 0xC69A, { 0xC8, 0xBD } },
	{ 0xC69E, { 0xC8, 0xA0 } },
	{ 0xC6A1, { 0xC6, 0xA0 } },
	{ 0xC6A3, { 0xC6, 0xA2 } },
	{ 0xC6A5, { 0xC6, 0xA4 } },
	{ 0xC6A8, { 0xC6, 0xA7 } },
	{ 0xC6AD, { 0xC6, 0xAC } },
	{ 0xC6B0, { 0xC6, 0xAF } },
	{ 0xC6B4, { 0xC6, 0xB3 } },
	{ 0xC6B6, { 0xC6, 0xB5 } },
	{ 0xC6B9, { 0xC6, 0xB8 } },
	{ 0xC6BD, { 0xC6, 0xBC } },
	{ 0xC6BF, { 0xC7, 0xB7 } },
	{ 0xC785, { 0xC7, 0x84 } },
	{ 0xC786, { 0xC7, 0x84 } },
	{ 0xC788, { 0xC7, 0x87 } },
	{ 0xC789, { 0xC7, 0x87 } },
	{ 0xC78B, { 0xC7, 0x8A } },
	{ 0xC78C, { 0xC7, 0x8A } },
	{ 0xC78E, { 0xC7, 0x8D } },
	{ 0xC790, { 0xC7, 0x8F } },
	{ 0xC792, { 0xC7, 0x91 } },
	{ 0xC794, { 0xC7, 0x93 } },
	{ 0xC796, { 0xC7, 0x95 } },
	{ 0xC798, { 0xC7, 0x97 } },
	{ 0xC79A, { 0xC7, 0x99 } },
	{ 0xC79C, { 0xC7, 0x9B } },
	{ 0xC79D, { 0xC6, 0x8E } },
	{ 0xC79F, { 0xC7, 0x9E } },
	{ 0xC7A1, { 0xC7, 0xA0 } },
	{ 0xC7A3, { 0xC7, 0xA2 } },
	{ 0xC7A5, { 0xC7, 0xA4 } },
	{ 0xC7A7, { 0xC7, 0xA6 } },
	{ 0xC7A9, { 0xC7, 0xA8 } },
	{ 0xC7AB, { 0xC7, 0xAA } },
	{ 0xC7AD, { 0xC7, 0xAC } },
	{ 0xC7AF, { 0xC7, 0xAE } },
	{ 0xC7B2, { 0xC7, 0xB1 } },
	{ 0xC7B3, { 0xC7, 0xB1 } },
	{ 0xC7B5, { 0xC7, 0xB4 } },
	{ 0xC7B9, { 0xC7, 0xB8 } },
	{ 0xC7BB, { 0xC7, 0xBA } },
	{ 0xC7BD, { 0xC7, 0xBC } },
	{ 0xC7BF, { 0xC7, 0xBE } },
	{ 0xC881, { 0xC8, 0x80 } },
	{ 0xC883, { 0xC8, 0x82 } },
	{ 0xC885, { 0xC8, 0x84 } },
	{ 0xC887, { 0xC8, 0x86 } },
	{ 0xC889, { 0xC8, 0x88 } },
	{ 0xC88B, { 0xC8, 0x8A } },
	{ 0xC88D, { 0xC8, 0x8C } },
	{ 0xC88F, { 0xC8, 0x8E } },
	{ 0xC891, { 0xC8, 0x90 } },
	{ 0xC893, { 0xC8, 0x92 } },
	{ 0xC895, { 0xC8, 0x94 } },
	{ 0xC897, { 0xC8, 0x96 } },
	{ 0xC899, { 0xC8, 0x98 } },
	{ 0xC89B, { 0xC8, 0x9A } },
	{ 0xC89D, { 0xC8, 0x9C } },
	{ 0xC89F, { 0xC8, 0x9E } },
	{ 0xC8A3, { 0xC8, 0xA2 } },
	{ 0xC8A5, { 0xC8, 0xA4 } },
	{ 0xC8A7, { 0xC8, 0xA6 } },
	{ 0xC8A9, { 0xC8, 0xA8 } },
	{ 0xC8AB, { 0xC8, 0xAA } },
	{ 0xC8AD, { 0xC8, 0xAC } },
	{ 0xC8AF, { 0xC8, 0xAE } },
	{ 0xC8B1, { 0xC8, 0xB0 } },
	{ 0xC8B3, { 0xC8, 0xB2 } },
	{ 0xC8BC, { 0xC8, 0xBB } },
	{ 0xC982, { 0xC9, 0x81 } },
	{ 0xC987, { 0xC9, 0x86 } },
	{ 0xC989, { 0xC9, 0x88 } },
	{ 0xC98B, { 0xC9, 0x8A } },
	{ 0xC98D, { 0xC9, 0x8C } },
	{ 0xC98F, { 0xC9, 0x8E } },
	{ 0xC993, { 0xC6, 0x81 } },
	{ 0xC994, { 0xC6, 0x86 } },
	{ 0xC996, { 0xC6, 0x89 } },
	{ 0xC997, { 0xC6, 0x8A } },
	{ 0xC999, { 0xC6, 0x8F } },
	{ 0xC99B, { 0xC6, 0x90 } },
	{ 0xC9A0, { 0xC6, 0x93 } },
	{ 0xC9A3, { 0xC6, 0x94 } },
	{ 0xC9A8, { 0xC6, 0x97 } },
	{ 0xC9A9, { 0xC6, 0x96 } },
	{ 0xC9AF, { 0xC6, 0x9C } },
	{ 0xC9B2, { 0xC6, 0x9D } },
	{ 0xC9B5, { 0xC6, 0x9F } },
	{ 0xCA80, { 0xC6, 0xA6 } },
	{ 0xCA83, { 0xC6, 0xA9 } },
	{ 0xCA88, { 0xC6, 0xAE } },
	{ 0xCA89, { 0xC9, 0x84 } },
	{ 0xCA8A, { 0xC6, 0xB1 } },
	{ 0xCA8B, { 0xC6, 0xB2 } },
	{ 0xCA8C, { 0xC9, 0x85 } },
	{ 0xCA92, { 0xC6, 0xB7 } },
	{ 0xCD85, { 0xCE, 0x99 } },
	{ 0xCDB1, { 0xCD, 0xB0 } },
	{ 0xCDB3, { 0xCD, 0xB2 } },
	{ 0xCDB7, { 0xCD, 0xB6 } },
	{ 0xCDBB, { 0xCF, 0xBD } },
	{ 0xCDBC, { 0xCF, 0xBE } },
	{ 0xCDBD, { 0xCF, 0xBF } },
	{ 0xCEAC, { 0xCE, 0x86 } },
	{ 0xCEAD, { 0xCE, 0x88 } },
	{ 0xCEAE, { 0xCE, 0x89 } },
	{ 0xCEAF, { 0xCE, 0x8A } },
	{ 0xCEB1, { 0xCE, 0x91 } },
	{ 0xCEB2, { 0xCE, 0x92 } },
	{ 0xCEB3, { 0xCE, 0x93 } },
	{ 0xCEB4, { 0xCE, 0x94 } },
	{ 0xCEB5, { 0xCE, 0x95 } },
	{ 0xCEB6, { 0xCE, 0x96 } },
	{ 0xCEB7, { 0xCE, 0x97 } },
	{ 0xCEB8, { 0xCE, 0x98 } },
	{ 0xCEB9, { 0xCE, 0x99 } },
	{ 0xCEBA, { 0xCE, 0x9A } },
	{ 0xCEBB, { 0xCE, 0x9B } },
	{ 0xCEBC, { 0xCE, 0x9C } },
	{ 0xCEBD, { 0xCE, 0x9D } },
	{ 0xCEBE, { 0xCE, 0x9E } },
	{ 0xCEBF, { 0xCE, 0x9F } },
	{ 0xCF80, { 0xCE, 0xA0 } },
	{ 0xCF81, { 0xCE, 0xA1 } },
	{ 0xCF82, { 0xCE, 0xA3 } },
	{ 0xCF83, { 0xCE, 0xA3 } },
	{ 0xCF84, { 0xCE, 0xA4 } },
	{ 0xCF85, { 0xCE, 0xA5 } },
	{ 0xCF86, { 0xCE, 0xA6 } },
	{ 0xCF87, { 0xCE, 0xA7 } },
	{ 0xCF88, { 0xCE, 0xA8 } },
	{ 0xCF89, { 0xCE, 0xA9 } },
	{ 0xCF8A, { 0xCE, 0xAA } },
	{ 0xCF8B, { 0xCE, 0xAB } },
	{ 0xCF8C, { 0xCE, 0x8C } },
	{ 0xCF8D, { 0xCE, 0x8E } },
	{ 0xCF8E, { 0xCE, 0x8F } },
	{ 0xCF90, { 0xCE, 0x92 } },
	{ 0xCF91, { 0xCE, 0x98 } },
	{ 0xCF95, { 0xCE, 0xA6 } },
	{ 0xCF96, { 0xCE, 0xA0 } },
	{ 0xCF97, { 0xCF, 0x8F } },
	{ 0xCF99, { 0xCF, 0x98 } },
	{ 0xCF9B, { 0xCF, 0x9A } },
	{ 0xCF9D, { 0xCF, 0x9C } },
	{ 0xCF9F, { 0xCF, 0x9E } },
	{ 0xCFA1, { 0xCF, 0xA0 } },
	{ 0xCFA3, { 0xCF, 0xA2 } },
	{ 0xCFA5, { 0xCF, 0xA4 } },
	{ 0xCFA7, { 0xCF, 0xA6 } },
	{ 0xCFA9, { 0xCF, 0xA8 } },
	{ 0xCFAB, { 0xCF, 0xAA } },
	{ 0xCFAD, { 0xCF, 0xAC } },
	{ 0xCFAF, { 0xCF, 0xAE } },
	{ 0xCFB0, { 0xCE, 0x9A } },
	{ 0xCFB1, { 0xCE, 0xA1 } },
	{ 0xCFB2, { 0xCF, 0xB9 } },
	{ 0xCFB5, { 0xCE, 0x95 } },
	{ 0xCFB8, { 0xCF, 0xB7 } },
	{ 0xCFBB, { 0xCF, 0xBA } },
	{ 0xD0B0, { 0xD0, 0x90 } },
	{ 0xD0B1, { 0xD0, 0x91 } },
	{ 0xD0B2, { 0xD0, 0x92 } },
	{ 0xD0B3, { 0xD0, 0x93 } },
	{ 0xD0B4, { 0xD0, 0x94 } },
	{ 0xD0B5, { 0xD0, 0x95 } },
	{ 0xD0B6, { 0xD0, 0x96 } },
	{ 0xD0B7, { 0xD0, 0x97 } },
	{ 0xD0B8, { 0xD0, 0x98 } },
	{ 0xD0B9, { 0xD0, 0x99 } },
	{ 0xD0BA, { 0xD0, 0x9A } },
	{ 0xD0BB, { 0xD0, 0x9B } },
	{ 0xD0BC, { 0xD0, 0x9C } },
	{ 0xD0BD, { 0xD0, 0x9D } },
	{ 0xD0BE, { 0xD0, 0x9E } },
	{ 0xD0BF, { 0xD0, 0x9F } },
	{ 0xD180, { 0xD0, 0xA0 } },
	{ 0xD181, { 0xD0, 0xA1 } },
	{ 0xD182, { 0xD0, 0xA2 } },
	{ 0xD183, { 0xD0, 0xA3 } },
	{ 0xD184, { 0xD0, 0xA4 } },
	{ 0xD185, { 0xD0, 0xA5 } },
	{ 0xD186, { 0xD0, 0xA6 } },
	{ 0xD187, { 0xD0, 0xA7 } },
	{ 0xD188, { 0xD0, 0xA8 } },
	{ 0xD189, { 0xD0, 0xA9 } },
	{ 0xD18A, { 0xD0, 0xAA } },
	{ 0xD18B, { 0xD0, 0xAB } },
	{ 0xD18C, { 0xD0, 0xAC } },
	{ 0xD18D, { 0xD0, 0xAD } },
	{ 0xD18E, { 0xD0, 0xAE } },
	{ 0xD18F, { 0xD0, 0xAF } },
	{ 0xD190, { 0xD0, 0x80 } },
	{ 0xD191, { 0xD0, 0x81 } },
	{ 0xD192, { 0xD0, 0x82 } },
	{ 0xD193, { 0xD0, 0x83 } },
	{ 0xD194, { 0xD0, 0x84 } },
	{ 0xD195, { 0xD0, 0x85 } },
	{ 0xD196, { 0xD0, 0x86 } },
	{ 0xD197, { 0xD0, 0x87 } },
	{ 0xD198, { 0xD0, 0x88 } },
	{ 0xD199, { 0xD0, 0x89 } },
	{ 0xD19A, { 0xD0, 0x8A } },
	{ 0xD19B, { 0xD0, 0x8B } },
	{ 0xD19C, { 0xD0, 0x8C } },
	{ 0xD19D, { 0xD0, 0x8D } },
	{ 0xD19E, { 0xD0, 0x8E } },
	{ 0xD19F, { 0xD0, 0x8F } },
	{ 0xD1A1, { 0xD1, 0xA0 } },
	{ 0xD1A3, { 0xD1, 0xA2 } },
	{ 0xD1A5, { 0xD1, 0xA4 } },
	{ 0xD1A7, { 0xD1, 0xA6 } },
	{ 0xD1A9, { 0xD1, 0xA8 } },
	{ 0xD1AB, { 0xD1, 0xAA } },
	{ 0xD1AD, { 0xD1, 0xAC } },
	{ 0xD1AF, { 0xD1, 0xAE } },
	{ 0xD1B1, { 0xD1, 0xB0 } },
	{ 0xD1B3, { 0xD1, 0xB2 } },
	{ 0xD1B5, { 0xD1, 0xB4 } },
	{ 0xD1B7, { 0xD1, 0xB6 } },
	{ 0xD1B9, { 0xD1, 0xB8 } },
	{ 0xD1BB, { 0xD1, 0xBA } },
	{ 0xD1BD, { 0xD1, 0xBC } },
	{ 0xD1BF, { 0xD1, 0xBE } },
	{ 0xD281, { 0xD2, 0x80 } },
	{ 0xD28B, { 0xD2, 0x8A } },
	{ 0xD28D, { 0xD2, 0x8C } },
	{ 0xD28F, { 0xD2, 0x8E } },
	{ 0xD291, { 0xD2, 0x90 } },
	{ 0xD293, { 0xD2, 0x92 } },
	{ 0xD295, { 0xD2, 0x94 } },
	{ 0xD297, { 0xD2, 0x96 } },
	{ 0xD299, { 0xD2, 0x98 } },
	{ 0xD29B, { 0xD2, 0x9A } },
	{ 0xD29D, { 0xD2, 0x9C } },
	{ 0xD29F, { 0xD2, 0x9E } },
	{ 0xD2A1, { 0xD2, 0xA0 } },
	{ 0xD2A3, { 0xD2, 0xA2 } },
	{ 0xD2A5, { 0xD2, 0xA4 } },
	{ 0xD2A7, { 0xD2, 0xA6 } },
	{ 0xD2A9, { 0xD2, 0xA8 } },
	{ 0xD2AB, { 0xD2, 0xAA } },
	{ 0xD2AD, { 0xD2, 0xAC } },
	{ 0xD2AF, { 0xD2, 0xAE } },
	{ 0xD2B1, { 0xD2, 0xB0 } },
	{ 0xD2B3, { 0xD2, 0xB2 } },
	{ 0xD2B5, { 0xD2, 0xB4 } },
	{ 0xD2B7, { 0xD2, 0xB6 } },
	{ 0xD2B9, { 0xD2, 0xB8 } },
	{ 0xD2BB, { 0xD2, 0xBA } },
	{ 0xD2BD, { 0xD2, 0xBC } },
	{ 0xD2BF, { 0xD2, 0xBE } },
	{ 0xD382, { 0xD3, 0x81 } },
	{ 0xD384, { 0xD3, 0x83 } },
	{ 0xD386, { 0xD3, 0x85 } },
	{ 0xD388, { 0xD3, 0x87 } },
	{ 0xD38A, { 0xD3, 0x89 } },
	{ 0xD38C, { 0xD3, 0x8B } },
	{ 0xD38E, { 0xD3, 0x8D } },
	{ 0xD38F, { 0xD3, 0x80 } },
	{ 0xD391, { 0xD3, 0x90 } },
	{ 0xD393, { 0xD3, 0x92 } },
	{ 0xD395, { 0xD3, 0x94 } },
	{ 0xD397, { 0xD3, 0x96 } },
	{ 0xD399, { 0xD3, 0x98 } },
	{ 0xD39B, { 0xD3, 0x9A } },
	{ 0xD39D, { 0xD3, 0x9C } },
	{ 0xD39F, { 0xD3, 0x9E } },
	{ 0xD3A1, { 0xD3, 0xA0 } },
	{ 0xD3A3, { 0xD3, 0xA2 } },
	{ 0xD3A5, { 0xD3, 0xA4 } },
	{ 0xD3A7, { 0xD3, 0xA6 } },
	{ 0xD3A9, { 0xD3, 0xA8 } },
	{ 0xD3AB, { 0xD3, 0xAA } },
	{ 0xD3AD, { 0xD3, 0xAC } },
	{ 0xD3AF, { 0xD3, 0xAE } },
	{ 0xD3B1, { 0xD3, 0xB0 } },
	{ 0xD3B3, { 0xD3, 0xB2 } },
	{ 0xD3B5, { 0xD3, 0xB4 } },
	{ 0xD3B7, { 0xD3, 0xB6 } },
	{ 0xD3B9, { 0xD3, 0xB8 } },
	{ 0xD3BB, { 0xD3, 0xBA } },
	{ 0xD3BD, { 0xD3, 0xBC } },
	{ 0xD3BF, { 0xD3, 0xBE } },
	{ 0xD481, { 0xD4, 0x80 } },
	{ 0xD483, { 0xD4, 0x82 } },
	{ 0xD485, { 0xD4, 0x84 } },
	{ 0xD487, { 0xD4, 0x86 } },
	{ 0xD489, { 0xD4, 0x88 } },
	{ 0xD48B, { 0xD4, 0x8A } },
	{ 0xD48D, { 0xD4, 0x8C } },
	{ 0xD48F, { 0xD4, 0x8E } },
	{ 0xD491, { 0xD4, 0x90 } },
	{ 0xD493, { 0xD4, 0x92 } },
	{ 0xD495, { 0xD4, 0x94 } },
	{ 0xD497, { 0xD4, 0x96 } },
	{ 0xD499, { 0xD4, 0x98 } },
	{ 0xD49B, { 0xD4, 0x9A } },
	{ 0xD49D, { 0xD4, 0x9C } },
	{ 0xD49F, { 0xD4, 0x9E } },
	{ 0xD4A1, { 0xD4, 0xA0 } },
	{ 0xD4A3, { 0xD4, 0xA2 } },
	{ 0xD4A5, { 0xD4, 0xA4 } },
	{ 0xD5A1, { 0xD4, 0xB1 } },
	{ 0xD5A2, { 0xD4, 0xB2 } },
	{ 0xD5A3, { 0xD4, 0xB3 } },
	{ 0xD5A4, { 0xD4, 0xB4 } },
	{ 0xD5A5, { 0xD4, 0xB5 } },
	{ 0xD5A6, { 0xD4, 0xB6 } },
	{ 0xD5A7, { 0xD4, 0xB7 } },
	{ 0xD5A8, { 0xD4, 0xB8 } },
	{ 0xD5A9, { 0xD4, 0xB9 } },
	{ 0xD5AA, { 0xD4, 0xBA } },
	{ 0xD5AB, { 0xD4, 0xBB } },
	{ 0xD5AC, { 0xD4, 0xBC } },
	{ 0xD5AD, { 0xD4, 0xBD } },
	{ 0xD5AE, { 0xD4, 0xBE } },
	{ 0xD5AF, { 0xD4, 0xBF } },
	{ 0xD5B0, { 0xD5, 0x80 } },
	{ 0xD5B1, { 0xD5, 0x81 } },
	{ 0xD5B2, { 0xD5, 0x82 } },
	{ 0xD5B3, { 0xD5, 0x83 } },
	{ 0xD5B4, { 0xD5, 0x84 } },
	{ 0xD5B5, { 0xD5, 0x85 } },
	{ 0xD5B6, { 0xD5, 0x86 } },
	{ 0xD5B7, { 0xD5, 0x87 } },
	{ 0xD5B8, { 0xD5, 0x88 } },
	{ 0xD5B9, { 0xD5, 0x89 } },
	{ 0xD5BA, { 0xD5, 0x8A } },
	{ 0xD5BB, { 0xD5, 0x8B } },
	{ 0xD5BC, { 0xD5, 0x8C } },
	{ 0xD5BD, { 0xD5, 0x8D } },
	{ 0xD5BE, { 0xD5, 0x8E } },
	{ 0xD5BF, { 0xD5, 0x8F } },
	{ 0xD680, { 0xD5, 0x90 } },
	{ 0xD681, { 0xD5, 0x91 } },
	{ 0xD682, { 0xD5, 0x92 } },
	{ 0xD683, { 0xD5, 0x93 } },
	{ 0xD684, { 0xD5, 0x94 } },
	{ 0xD685, { 0xD5, 0x95 } },
	{ 0xD686, { 0xD5, 0x96 } },
};

const struct {
	const uint16_t key;
	const uint8_t val_len;
	const uint8_t val[6];
} UTF8_LOWER_UPPER_2B_DBL[] = {
	{ 0xC4B1, 1, { 0x49 } },
	{ 0xC589, 3, { 0xCA, 0xBC, 0x4E } },
	{ 0xC5BF, 1, { 0x53 } },
	{ 0xC7B0, 3, { 0x4A, 0xCC, 0x8C } },
	{ 0xC8BF, 3, { 0xE2, 0xB1, 0xBE } },
	{ 0xC980, 3, { 0xE2, 0xB1, 0xBF } },
	{ 0xC990, 3, { 0xE2, 0xB1, 0xAF } },
	{ 0xC991, 3, { 0xE2, 0xB1, 0xAD } },
	{ 0xC992, 3, { 0xE2, 0xB1, 0xB0 } },
	{ 0xC9AB, 3, { 0xE2, 0xB1, 0xA2 } },
	{ 0xC9B1, 3, { 0xE2, 0xB1, 0xAE } },
	{ 0xC9BD, 3, { 0xE2, 0xB1, 0xA4 } },
	{ 0xCE90, 6, { 0xCE, 0x99, 0xCC, 0x88, 0xCC, 0x81 } },
	{ 0xCEB0, 6, { 0xCE, 0xA5, 0xCC, 0x88, 0xCC, 0x81 } },
	{ 0xD687, 4, { 0xD4, 0xB5, 0xD5, 0x92 } },
};

const struct {
	const uint32_t key;
	const uint8_t val[3];
} UTF8_LOWER_UPPER_3B[] = {
	{ 0xE1B5B9, { 0xEA, 0x9D, 0xBD } },
	{ 0xE1B5BD, { 0xE2, 0xB1, 0xA3 } },
	{ 0xE1B881, { 0xE1, 0xB8, 0x80 } },
	{ 0xE1B883, { 0xE1, 0xB8, 0x82 } },
	{ 0xE1B885, { 0xE1, 0xB8, 0x84 } },
	{ 0xE1B887, { 0xE1, 0xB8, 0x86 } },
	{ 0xE1B889, { 0xE1, 0xB8, 0x88 } },
	{ 0xE1B88B, { 0xE1, 0xB8, 0x8A } },
	{ 0xE1B88D, { 0xE1, 0xB8, 0x8C } },
	{ 0xE1B88F, { 0xE1, 0xB8, 0x8E } },
	{ 0xE1B891, { 0xE1, 0xB8, 0x90 } },
	{ 0xE1B893, { 0xE1, 0xB8, 0x92 } },
	{ 0xE1B895, { 0xE1, 0xB8, 0x94 } },
	{ 0xE1B897, { 0xE1, 0xB8, 0x96 } },
	{ 0xE1B899, { 0xE1, 0xB8, 0x98 } },
	{ 0xE1B89B, { 0xE1, 0xB8, 0x9A } },
	{ 0xE1B89D, { 0xE1, 0xB8, 0x9C } },
	{ 0xE1B89F, { 0xE1, 0xB8, 0x9E } },
	{ 0xE1B8A1, { 0xE1, 0xB8, 0xA0 } },
	{ 0xE1B8A3, { 0xE1, 0xB8, 0xA2 } },
	{ 0xE1B8A5, { 0xE1, 0xB8, 0xA4 } },
	{ 0xE1B8A7, { 0xE1, 0xB8, 0xA6 } },
	{ 0xE1B8A9, { 0xE1, 0xB8, 0xA8 } },
	{ 0xE1B8AB, { 0xE1, 0xB8, 0xAA } },
	{ 0xE1B8AD, { 0xE1, 0xB8, 0xAC } },
	{ 0xE1B8AF, { 0xE1, 0xB8, 0xAE } },
	{ 0xE1B8B1, { 0xE1, 0xB8, 0xB0 } },
	{ 0xE1B8B3, { 0xE1, 0xB8, 0xB2 } },
	{ 0xE1B8B5, { 0xE1, 0xB8, 0xB4 } },
	{ 0xE1B8B7, { 0xE1, 0xB8, 0xB6 } },
	{ 0xE1B8B9, { 0xE1, 0xB8, 0xB8 } },
	{ 0xE1B8BB, { 0xE1, 0xB8, 0xBA } },
	{ 0xE1B8BD, { 0xE1, 0xB8, 0xBC } },
	{ 0xE1B8BF, { 0xE1, 0xB8, 0xBE } },
	{ 0xE1B981, { 0xE1, 0xB9, 0x80 } },
	{ 0xE1B983, { 0xE1, 0xB9, 0x82 } },
	{ 0xE1B985, { 0xE1, 0xB9, 0x84 } },
	{ 0xE1B987, { 0xE1, 0xB9, 0x86 } },
	{ 0xE1B989, { 0xE1, 0xB9, 0x88 } },
	{ 0xE1B98B, { 0xE1, 0xB9, 0x8A } },
	{ 0xE1B98D, { 0xE1, 0xB9, 0x8C } },
	{ 0xE1B98F, { 0xE1, 0xB9, 0x8E } },
	{ 0xE1B991, { 0xE1, 0xB9, 0x90 } },
	{ 0xE1B993, { 0xE1, 0xB9, 0x92 } },
	{ 0xE1B995, { 0xE1, 0xB9, 0x94 } },
	{ 0xE1B997, { 0xE1, 0xB9, 0x96 } },
	{ 0xE1B999, { 0xE1, 0xB9, 0x98 } },
	{ 0xE1B99B, { 0xE1, 0xB9, 0x9A } },
	{ 0xE1B99D, { 0xE1, 0xB9, 0x9C } },
	{ 0xE1B99F, { 0xE1, 0xB9, 0x9E } },
	{ 0xE1B9A1, { 0xE1, 0xB9, 0xA0 } },
	{ 0xE1B9A3, { 0xE1, 0xB9, 0xA2 } },
	{ 0xE1B9A5, { 0xE1, 0xB9, 0xA4 } },
	{ 0xE1B9A7, { 0xE1, 0xB9, 0xA6 } },
	{ 0xE1B9A9, { 0xE1, 0xB9, 0xA8 } },
	{ 0xE1B9AB, { 0xE1, 0xB9, 0xAA } },
	{ 0xE1B9AD, { 0xE1, 0xB9, 0xAC } },
	{ 0xE1B9AF, { 0xE1, 0xB9, 0xAE } },
	{ 0xE1B9B1, { 0xE1, 0xB9, 0xB0 } },
	{ 0xE1B9B3, { 0xE1, 0xB9, 0xB2 } },
	{ 0xE1B9B5, { 0xE1, 0xB9, 0xB4 } },
	{ 0xE1B9B7, { 0xE1, 0xB9, 0xB6 } },
	{ 0xE1B9B9, { 0xE1, 0xB9, 0xB8 } },
	{ 0xE1B9BB, { 0xE1, 0xB9, 0xBA } },
	{ 0xE1B9BD, { 0xE1, 0xB9, 0xBC } },
	{ 0xE1B9BF, { 0xE1, 0xB9, 0xBE } },
	{ 0xE1BA81, { 0xE1, 0xBA, 0x80 } },
	{ 0xE1BA83, { 0xE1, 0xBA, 0x82 } },
	{ 0xE1BA85, { 0xE1, 0xBA, 0x84 } },
	{ 0xE1BA87, { 0xE1, 0xBA, 0x86 } },
	{ 0xE1BA89, { 0xE1, 0xBA, 0x88 } },
	{ 0xE1BA8B, { 0xE1, 0xBA, 0x8A } },
	{ 0xE1BA8D, { 0xE1, 0xBA, 0x8C } },
	{ 0xE1BA8F, { 0xE1, 0xBA, 0x8E } },
	{ 0xE1BA91, { 0xE1, 0xBA, 0x90 } },
	{ 0xE1BA93, { 0xE1, 0xBA, 0x92 } },
	{ 0xE1BA95, { 0xE1, 0xBA, 0x94 } },
	{ 0xE1BA96, { 0x48, 0xCC, 0xB1 } },
	{ 0xE1BA97, { 0x54, 0xCC, 0x88 } },
	{ 0xE1BA98, { 0x57, 0xCC, 0x8A } },
	{ 0xE1BA99, { 0x59, 0xCC, 0x8A } },
	{ 0xE1BA9A, { 0x41, 0xCA, 0xBE } },
	{ 0xE1BA9B, { 0xE1, 0xB9, 0xA0 } },
	{ 0xE1BAA1, { 0xE1, 0xBA, 0xA0 } },
	{ 0xE1BAA3, { 0xE1, 0xBA, 0xA2 } },
	{ 0xE1BAA5, { 0xE1, 0xBA, 0xA4 } },
	{ 0xE1BAA7, { 0xE1, 0xBA, 0xA6 } },
	{ 0xE1BAA9, { 0xE1, 0xBA, 0xA8 } },
	{ 0xE1BAAB, { 0xE1, 0xBA, 0xAA } },
	{ 0xE1BAAD, { 0xE1, 0xBA, 0xAC } },
	{ 0xE1BAAF, { 0xE1, 0xBA, 0xAE } },
	{ 0xE1BAB1, { 0xE1, 0xBA, 0xB0 } },
	{ 0xE1BAB3, { 0xE1, 0xBA, 0xB2 } },
	{ 0xE1BAB5, { 0xE1, 0xBA, 0xB4 } },
	{ 0xE1BAB7, { 0xE1, 0xBA, 0xB6 } },
	{ 0xE1BAB9, { 0xE1, 0xBA, 0xB8 } },
	{ 0xE1BABB, { 0xE1, 0xBA, 0xBA } },
	{ 0xE1BABD, { 0xE1, 0xBA, 0xBC } },
	{ 0xE1BABF, { 0xE1, 0xBA, 0xBE } },
	{ 0xE1BB81, { 0xE1, 0xBB, 0x80 } },
	{ 0xE1BB83, { 0xE1, 0xBB, 0x82 } },
	{ 0xE1BB85, { 0xE1, 0xBB, 0x84 } },
	{ 0xE1BB87, { 0xE1, 0xBB, 0x86 } },
	{ 0xE1BB89, { 0xE1, 0xBB, 0x88 } },
	{ 0xE1BB8B, { 0xE1, 0xBB, 0x8A } },
	{ 0xE1BB8D, { 0xE1, 0xBB, 0x8C } },
	{ 0xE1BB8F, { 0xE1, 0xBB, 0x8E } },
	{ 0xE1BB91, { 0xE1, 0xBB, 0x90 } },
	{ 0xE1BB93, { 0xE1, 0xBB, 0x92 } },
	{ 0xE1BB95, { 0xE1, 0xBB, 0x94 } },
	{ 0xE1BB97, { 0xE1, 0xBB, 0x96 } },
	{ 0xE1BB99, { 0xE1, 0xBB, 0x98 } },
	{ 0xE1BB9B, { 0xE1, 0xBB, 0x9A } },
	{ 0xE1BB9D, { 0xE1, 0xBB, 0x9C } },
	{ 0xE1BB9F, { 0xE1, 0xBB, 0x9E } },
	{ 0xE1BBA1, { 0xE1, 0xBB, 0xA0 } },
	{ 0xE1BBA3, { 0xE1, 0xBB, 0xA2 } },
	{ 0xE1BBA5, { 0xE1, 0xBB, 0xA4 } },
	{ 0xE1BBA7, { 0xE1, 0xBB, 0xA6 } },
	{ 0xE1BBA9, { 0xE1, 0xBB, 0xA8 } },
	{ 0xE1BBAB, { 0xE1, 0xBB, 0xAA } },
	{ 0xE1BBAD, { 0xE1, 0xBB, 0xAC } },
	{ 0xE1BBAF, { 0xE1, 0xBB, 0xAE } },
	{ 0xE1BBB1, { 0xE1, 0xBB, 0xB0 } },
	{ 0xE1BBB3, { 0xE1, 0xBB, 0xB2 } },
	{ 0xE1BBB5, { 0xE1, 0xBB, 0xB4 } },
	{ 0xE1BBB7, { 0xE1, 0xBB, 0xB6 } },
	{ 0xE1BBB9, { 0xE1, 0xBB, 0xB8 } },
	{ 0xE1BBBB, { 0xE1, 0xBB, 0xBA } },
	{ 0xE1BBBD, { 0xE1, 0xBB, 0xBC } },
	{ 0xE1BBBF, { 0xE1, 0xBB, 0xBE } },
	{ 0xE1BC80, { 0xE1, 0xBC, 0x88 } },
	{ 0xE1BC81, { 0xE1, 0xBC, 0x89 } },
	{ 0xE1BC82, { 0xE1, 0xBC, 0x8A } },
	{ 0xE1BC83, { 0xE1, 0xBC, 0x8B } },
	{ 0xE1BC84, { 0xE1, 0xBC, 0x8C } },
	{ 0xE1BC85, { 0xE1, 0xBC, 0x8D } },
	{ 0xE1BC86, { 0xE1, 0xBC, 0x8E } },
	{ 0xE1BC87, { 0xE1, 0xBC, 0x8F } },
	{ 0xE1BC90, { 0xE1, 0xBC, 0x98 } },
	{ 0xE1BC91, { 0xE1, 0xBC, 0x99 } },
	{ 0xE1BC92, { 0xE1, 0xBC, 0x9A } },
	{ 0xE1BC93, { 0xE1, 0xBC, 0x9B } },
	{ 0xE1BC94, { 0xE1, 0xBC, 0x9C } },
	{ 0xE1BC95, { 0xE1, 0xBC, 0x9D } },
	{ 0xE1BCA0, { 0xE1, 0xBC, 0xA8 } },
	{ 0xE1BCA1, { 0xE1, 0xBC, 0xA9 } },
	{ 0xE1BCA2, { 0xE1, 0xBC, 0xAA } },
	{ 0xE1BCA3, { 0xE1, 0xBC, 0xAB } },
	{ 0xE1BCA4, { 0xE1, 0xBC, 0xAC } },
	{ 0xE1BCA5, { 0xE1, 0xBC, 0xAD } },
	{ 0xE1BCA6, { 0xE1, 0xBC, 0xAE } },
	{ 0xE1BCA7, { 0xE1, 0xBC, 0xAF } },
	{ 0xE1BCB0, { 0xE1, 0xBC, 0xB8 } },
	{ 0xE1BCB1, { 0xE1, 0xBC, 0xB9 } },
	{ 0xE1BCB2, { 0xE1, 0xBC, 0xBA } },
	{ 0xE1BCB3, { 0xE1, 0xBC, 0xBB } },
	{ 0xE1BCB4, { 0xE1, 0xBC, 0xBC } },
	{ 0xE1BCB5, { 0xE1, 0xBC, 0xBD } },
	{ 0xE1BCB6, { 0xE1, 0xBC, 0xBE } },
	{ 0xE1BCB7, { 0xE1, 0xBC, 0xBF } },
	{ 0xE1BD80, { 0xE1, 0xBD, 0x88 } },
	{ 0xE1BD81, { 0xE1, 0xBD, 0x89 } },
	{ 0xE1BD82, { 0xE1, 0xBD, 0x8A } },
	{ 0xE1BD83, { 0xE1, 0xBD, 0x8B } },
	{ 0xE1BD84, { 0xE1, 0xBD, 0x8C } },
	{ 0xE1BD85, { 0xE1, 0xBD, 0x8D } },
	{ 0xE1BD91, { 0xE1, 0xBD, 0x99 } },
	{ 0xE1BD93, { 0xE1, 0xBD, 0x9B } },
	{ 0xE1BD95, { 0xE1, 0xBD, 0x9D } },
	{ 0xE1BD97, { 0xE1, 0xBD, 0x9F } },
	{ 0xE1BDA0, { 0xE1, 0xBD, 0xA8 } },
	{ 0xE1BDA1, { 0xE1, 0xBD, 0xA9 } },
	{ 0xE1BDA2, { 0xE1, 0xBD, 0xAA } },
	{ 0xE1BDA3, { 0xE1, 0xBD, 0xAB } },
	{ 0xE1BDA4, { 0xE1, 0xBD, 0xAC } },
	{ 0xE1BDA5, { 0xE1, 0xBD, 0xAD } },
	{ 0xE1BDA6, { 0xE1, 0xBD, 0xAE } },
	{ 0xE1BDA7, { 0xE1, 0xBD, 0xAF } },
	{ 0xE1BDB0, { 0xE1, 0xBE, 0xBA } },
	{ 0xE1BDB1, { 0xE1, 0xBE, 0xBB } },
	{ 0xE1BDB2, { 0xE1, 0xBF, 0x88 } },
	{ 0xE1BDB3, { 0xE1, 0xBF, 0x89 } },
	{ 0xE1BDB4, { 0xE1, 0xBF, 0x8A } },
	{ 0xE1BDB5, { 0xE1, 0xBF, 0x8B } },
	{ 0xE1BDB6, { 0xE1, 0xBF, 0x9A } },
	{ 0xE1BDB7, { 0xE1, 0xBF, 0x9B } },
	{ 0xE1BDB8, { 0xE1, 0xBF, 0xB8 } },
	{ 0xE1BDB9, { 0xE1, 0xBF, 0xB9 } },
	{ 0xE1BDBA, { 0xE1, 0xBF, 0xAA } },
	{ 0xE1BDBB, { 0xE1, 0xBF, 0xAB } },
	{ 0xE1BDBC, { 0xE1, 0xBF, 0xBA } },
	{ 0xE1BDBD, { 0xE1, 0xBF, 0xBB } },
	{ 0xE1BEB0, { 0xE1, 0xBE, 0xB8 } },
	{ 0xE1BEB1, { 0xE1, 0xBE, 0xB9 } },
	{ 0xE1BF90, { 0xE1, 0xBF, 0x98 } },
	{ 0xE1BF91, { 0xE1, 0xBF, 0x99 } },
	{ 0xE1BFA0, { 0xE1, 0xBF, 0xA8 } },
	{ 0xE1BFA1, { 0xE1, 0xBF, 0xA9 } },
	{ 0xE1BFA5, { 0xE1, 0xBF, 0xAC } },
	{ 0xE2858E, { 0xE2, 0x84, 0xB2 } },
	{ 0xE285B0, { 0xE2, 0x85, 0xA0 } },
	{ 0xE285B1, { 0xE2, 0x85, 0xA1 } },
	{ 0xE285B2, { 0xE2, 0x85, 0xA2 } },
	{ 0xE285B3, { 0xE2, 0x85, 0xA3 } },
	{ 0xE285B4, { 0xE2, 0x85, 0xA4 } },
	{ 0xE285B5, { 0xE2, 0x85, 0xA5 } },
	{ 0xE285B6, { 0xE2, 0x85, 0xA6 } },
	{ 0xE285B7, { 0xE2, 0x85, 0xA7 } },
	{ 0xE285B8, { 0xE2, 0x85, 0xA8 } },
	{ 0xE285B9, { 0xE2, 0x85, 0xA9 } },
	{ 0xE285BA, { 0xE2, 0x85, 0xAA } },
	{ 0xE285BB, { 0xE2, 0x85, 0xAB } },
	{ 0xE285BC, { 0xE2, 0x85, 0xAC } },
	{ 0xE285BD, { 0xE2, 0x85, 0xAD } },
	{ 0xE285BE, { 0xE2, 0x85, 0xAE } },
	{ 0xE285BF, { 0xE2, 0x85, 0xAF } },
	{ 0xE28684, { 0xE2, 0x86, 0x83 } },
	{ 0xE29390, { 0xE2, 0x92, 0xB6 } },
	{ 0xE29391, { 0xE2, 0x92, 0xB7 } },
	{ 0xE29392, { 0xE2, 0x92, 0xB8 } },
	{ 0xE29393, { 0xE2, 0x92, 0xB9 } },
	{ 0xE29394, { 0xE2, 0x92, 0xBA } },
	{ 0xE29395, { 0xE2, 0x92, 0xBB } },
	{ 0xE29396, { 0xE2, 0x92, 0xBC } },
	{ 0xE29397, { 0xE2, 0x92, 0xBD } },
	{ 0xE29398, { 0xE2, 0x92, 0xBE } },
	{ 0xE29399, { 0xE2, 0x92, 0xBF } },
	{ 0xE2939A, { 0xE2, 0x93, 0x80 } },
	{ 0xE2939B, { 0xE2, 0x93, 0x81 } },
	{ 0xE2939C, { 0xE2, 0x93, 0x82 } },
	{ 0xE2939D, { 0xE2, 0x93, 0x83 } },
	{ 0xE2939E, { 0xE2, 0x93, 0x84 } },
	{ 0xE2939F, { 0xE2, 0x93, 0x85 } },
	{ 0xE293A0, { 0xE2, 0x93, 0x86 } },
	{ 0xE293A1, { 0xE2, 0x93, 0x87 } },
	{ 0xE293A2, { 0xE2, 0x93, 0x88 } },
	{ 0xE293A3, { 0xE2, 0x93, 0x89 } },
	{ 0xE293A4, { 0xE2, 0x93, 0x8A } },
	{ 0xE293A5, { 0xE2, 0x93, 0x8B } },
	{ 0xE293A6, { 0xE2, 0x93, 0x8C } },
	{ 0xE293A7, { 0xE2, 0x93, 0x8D } },
	{ 0xE293A8, { 0xE2, 0x93, 0x8E } },
	{ 0xE293A9, { 0xE2, 0x93, 0x8F } },
	{ 0xE2B0B0, { 0xE2, 0xB0, 0x80 } },
	{ 0xE2B0B1, { 0xE2, 0xB0, 0x81 } },
	{ 0xE2B0B2, { 0xE2, 0xB0, 0x82 } },
	{ 0xE2B0B3, { 0xE2, 0xB0, 0x83 } },
	{ 0xE2B0B4, { 0xE2, 0xB0, 0x84 } },
	{ 0xE2B0B5, { 0xE2, 0xB0, 0x85 } },
	{ 0xE2B0B6, { 0xE2, 0xB0, 0x86 } },
	{ 0xE2B0B7, { 0xE2, 0xB0, 0x87 } },
	{ 0xE2B0B8, { 0xE2, 0xB0, 0x88 } },
	{ 0xE2B0B9, { 0xE2, 0xB0, 0x89 } },
	{ 0xE2B0BA, { 0xE2, 0xB0, 0x8A } },
	{ 0xE2B0BB, { 0xE2, 0xB0, 0x8B } },
	{ 0xE2B0BC, { 0xE2, 0xB0, 0x8C } },
	{ 0xE2B0BD, { 0xE2, 0xB0, 0x8D } },
	{ 0xE2B0BE, { 0xE2, 0xB0, 0x8E } },
	{ 0xE2B0BF, { 0xE2, 0xB0, 0x8F } },
	{ 0xE2B180, { 0xE2, 0xB0, 0x90 } },
	{ 0xE2B181, { 0xE2, 0xB0, 0x91 } },
	{ 0xE2B182, { 0xE2, 0xB0, 0x92 } },
	{ 0xE2B183, { 0xE2, 0xB0, 0x93 } },
	{ 0xE2B184, { 0xE2, 0xB0, 0x94 } },
	{ 0xE2B185, { 0xE2, 0xB0, 0x95 } },
	{ 0xE2B186, { 0xE2, 0xB0, 0x96 } },
	{ 0xE2B187, { 0xE2, 0xB0, 0x97 } },
	{ 0xE2B188, { 0xE2, 0xB0, 0x98 } },
	{ 0xE2B189, { 0xE2, 0xB0, 0x99 } },
	{ 0xE2B18A, { 0xE2, 0xB0, 0x9A } },
	{ 0xE2B18B, { 0xE2, 0xB0, 0x9B } },
	{ 0xE2B18C, { 0xE2, 0xB0, 0x9C } },
	{ 0xE2B18D, { 0xE2, 0xB0, 0x9D } },
	{ 0xE2B18E, { 0xE2, 0xB0, 0x9E } },
	{ 0xE2B18F, { 0xE2, 0xB0, 0x9F } },
	{ 0xE2B190, { 0xE2, 0xB0, 0xA0 } },
	{ 0xE2B191, { 0xE2, 0xB0, 0xA1 } },
	{ 0xE2B192, { 0xE2, 0xB0, 0xA2 } },
	{ 0xE2B193, { 0xE2, 0xB0, 0xA3 } },
	{ 0xE2B194, { 0xE2, 0xB0, 0xA4 } },
	{ 0xE2B195, { 0xE2, 0xB0, 0xA5 } },
	{ 0xE2B196, { 0xE2, 0xB0, 0xA6 } },
	{ 0xE2B197, { 0xE2, 0xB0, 0xA7 } },
	{ 0xE2B198, { 0xE2, 0xB0, 0xA8 } },
	{ 0xE2B199, { 0xE2, 0xB0, 0xA9 } },
	{ 0xE2B19A, { 0xE2, 0xB0, 0xAA } },
	{ 0xE2B19B, { 0xE2, 0xB0, 0xAB } },
	{ 0xE2B19C, { 0xE2, 0xB0, 0xAC } },
	{ 0xE2B19D, { 0xE2, 0xB0, 0xAD } },
	{ 0xE2B19E, { 0xE2, 0xB0, 0xAE } },
	{ 0xE2B1A1, { 0xE2, 0xB1, 0xA0 } },
	{ 0xE2B1A8, { 0xE2, 0xB1, 0xA7 } },
	{ 0xE2B1AA, { 0xE2, 0xB1, 0xA9 } },
	{ 0xE2B1AC, { 0xE2, 0xB1, 0xAB } },
	{ 0xE2B1B3, { 0xE2, 0xB1, 0xB2 } },
	{ 0xE2B1B6, { 0xE2, 0xB1, 0xB5 } },
	{ 0xE2B281, { 0xE2, 0xB2, 0x80 } },
	{ 0xE2B283, { 0xE2, 0xB2, 0x82 } },
	{ 0xE2B285, { 0xE2, 0xB2, 0x84 } },
	{ 0xE2B287, { 0xE2, 0xB2, 0x86 } },
	{ 0xE2B289, { 0xE2, 0xB2, 0x88 } },
	{ 0xE2B28B, { 0xE2, 0xB2, 0x8A } },
	{ 0xE2B28D, { 0xE2, 0xB2, 0x8C } },
	{ 0xE2B28F, { 0xE2, 0xB2, 0x8E } },
	{ 0xE2B291, { 0xE2, 0xB2, 0x90 } },
	{ 0xE2B293, { 0xE2, 0xB2, 0x92 } },
	{ 0xE2B295, { 0xE2, 0xB2, 0x94 } },
	{ 0xE2B297, { 0xE2, 0xB2, 0x96 } },
	{ 0xE2B299, { 0xE2, 0xB2, 0x98 } },
	{ 0xE2B29B, { 0xE2, 0xB2, 0x9A } },
	{ 0xE2B29D, { 0xE2, 0xB2, 0x9C } },
	{ 0xE2B29F, { 0xE2, 0xB2, 0x9E } },
	{ 0xE2B2A1, { 0xE2, 0xB2, 0xA0 } },
	{ 0xE2B2A3, { 0xE2, 0xB2, 0xA2 } },
	{ 0xE2B2A5, { 0xE2, 0xB2, 0xA4 } },
	{ 0xE2B2A7, { 0xE2, 0xB2, 0xA6 } },
	{ 0xE2B2A9, { 0xE2, 0xB2, 0xA8 } },
	{ 0xE2B2AB, { 0xE2, 0xB2, 0xAA } },
	{ 0xE2B2AD, { 0xE2, 0xB2, 0xAC } },
	{ 0xE2B2AF, { 0xE2, 0xB2, 0xAE } },
	{ 0xE2B2B1, { 0xE2, 0xB2, 0xB0 } },
	{ 0xE2B2B3, { 0xE2, 0xB2, 0xB2 } },
	{ 0xE2B2B5, { 0xE2, 0xB2, 0xB4 } },
	{ 0xE2B2B7, { 0xE2, 0xB2, 0xB6 } },
	{ 0xE2B2B9, { 0xE2, 0xB2, 0xB8 } },
	{ 0xE2B2BB, { 0xE2, 0xB2, 0xBA } },
	{ 0xE2B2BD, { 0xE2, 0xB2, 0xBC } },
	{ 0xE2B2BF, { 0xE2, 0xB2, 0xBE } },
	{ 0xE2B381, { 0xE2, 0xB3, 0x80 } },
	{ 0xE2B383, { 0xE2, 0xB3, 0x82 } },
	{ 0xE2B385, { 0xE2, 0xB3, 0x84 } },
	{ 0xE2B387, { 0xE2, 0xB3, 0x86 } },
	{ 0xE2B389, { 0xE2, 0xB3, 0x88 } },
	{ 0xE2B38B, { 0xE2, 0xB3, 0x8A } },
	{ 0xE2B38D, { 0xE2, 0xB3, 0x8C } },
	{ 0xE2B38F, { 0xE2, 0xB3, 0x8E } },
	{ 0xE2B391, { 0xE2, 0xB3, 0x90 } },
	{ 0xE2B393, { 0xE2, 0xB3, 0x92 } },
	{ 0xE2B395, { 0xE2, 0xB3, 0x94 } },
	{ 0xE2B397, { 0xE2, 0xB3, 0x96 } },
	{ 0xE2B399, { 0xE2, 0xB3, 0x98 } },
	{ 0xE2B39B, { 0xE2, 0xB3, 0x9A } },
	{ 0xE2B39D, { 0xE2, 0xB3, 0x9C } },
	{ 0xE2B39F, { 0xE2, 0xB3, 0x9E } },
	{ 0xE2B3A1, { 0xE2, 0xB3, 0xA0 } },
	{ 0xE2B3A3, { 0xE2, 0xB3, 0xA2 } },
	{ 0xE2B3AC, { 0xE2, 0xB3, 0xAB } },
	{ 0xE2B3AE, { 0xE2, 0xB3, 0xAD } },
	{ 0xE2B480, { 0xE1, 0x82, 0xA0 } },
	{ 0xE2B481, { 0xE1, 0x82, 0xA1 } },
	{ 0xE2B482, { 0xE1, 0x82, 0xA2 } },
	{ 0xE2B483, { 0xE1, 0x82, 0xA3 } },
	{ 0xE2B484, { 0xE1, 0x82, 0xA4 } },
	{ 0xE2B485, { 0xE1, 0x82, 0xA5 } },
	{ 0xE2B486, { 0xE1, 0x82, 0xA6 } },
	{ 0xE2B487, { 0xE1, 0x82, 0xA7 } },
	{ 0xE2B488, { 0xE1, 0x82, 0xA8 } },
	{ 0xE2B489, { 0xE1, 0x82, 0xA9 } },
	{ 0xE2B48A, { 0xE1, 0x82, 0xAA } },
	{ 0xE2B48B, { 0xE1, 0x82, 0xAB } },
	{ 0xE2B48C, { 0xE1, 0x82, 0xAC } },
	{ 0xE2B48D, { 0xE1, 0x82, 0xAD } },
	{ 0xE2B48E, { 0xE1, 0x82, 0xAE } },
	{ 0xE2B48F, { 0xE1, 0x82, 0xAF } },
	{ 0xE2B490, { 0xE1, 0x82, 0xB0 } },
	{ 0xE2B491, { 0xE1, 0x82, 0xB1 } },
	{ 0xE2B492, { 0xE1, 0x82, 0xB2 } },
	{ 0xE2B493, { 0xE1, 0x82, 0xB3 } },
	{ 0xE2B494, { 0xE1, 0x82, 0xB4 } },
	{ 0xE2B495, { 0xE1, 0x82, 0xB5 } },
	{ 0xE2B496, { 0xE1, 0x82, 0xB6 } },
	{ 0xE2B497, { 0xE1, 0x82, 0xB7 } },
	{ 0xE2B498, { 0xE1, 0x82, 0xB8 } },
	{ 0xE2B499, { 0xE1, 0x82, 0xB9 } },
	{ 0xE2B49A, { 0xE1, 0x82, 0xBA } },
	{ 0xE2B49B, { 0xE1, 0x82, 0xBB } },
	{ 0xE2B49C, { 0xE1, 0x82, 0xBC } },
	{ 0xE2B49D, { 0xE1, 0x82, 0xBD } },
	{ 0xE2B49E, { 0xE1, 0x82, 0xBE } },
	{ 0xE2B49F, { 0xE1, 0x82, 0xBF } },
	{ 0xE2B4A0, { 0xE1, 0x83, 0x80 } },
	{ 0xE2B4A1, { 0xE1, 0x83, 0x81 } },
	{ 0xE2B4A2, { 0xE1, 0x83, 0x82 } },
	{ 0xE2B4A3, { 0xE1, 0x83, 0x83 } },
	{ 0xE2B4A4, { 0xE1, 0x83, 0x84 } },
	{ 0xE2B4A5, { 0xE1, 0x83, 0x85 } },
	{ 0xEA9981, { 0xEA, 0x99, 0x80 } },
	{ 0xEA9983, { 0xEA, 0x99, 0x82 } },
	{ 0xEA9985, { 0xEA, 0x99, 0x84 } },
	{ 0xEA9987, { 0xEA, 0x99, 0x86 } },
	{ 0xEA9989, { 0xEA, 0x99, 0x88 } },
	{ 0xEA998B, { 0xEA, 0x99, 0x8A } },
	{ 0xEA998D, { 0xEA, 0x99, 0x8C } },
	{ 0xEA998F, { 0xEA, 0x99, 0x8E } },
	{ 0xEA9991, { 0xEA, 0x99, 0x90 } },
	{ 0xEA9993, { 0xEA, 0x99, 0x92 } },
	{ 0xEA9995, { 0xEA, 0x99, 0x94 } },
	{ 0xEA9997, { 0xEA, 0x99, 0x96 } },
	{ 0xEA9999, { 0xEA, 0x99, 0x98 } },
	{ 0xEA999B, { 0xEA, 0x99, 0x9A } },
	{ 0xEA999D, { 0xEA, 0x99, 0x9C } },
	{ 0xEA999F, { 0xEA, 0x99, 0x9E } },
	{ 0xEA99A3, { 0xEA, 0x99, 0xA2 } },
	{ 0xEA99A5, { 0xEA, 0x99, 0xA4 } },
	{ 0xEA99A7, { 0xEA, 0x99, 0xA6 } },
	{ 0xEA99A9, { 0xEA, 0x99, 0xA8 } },
	{ 0xEA99AB, { 0xEA, 0x99, 0xAA } },
	{ 0xEA99AD, { 0xEA, 0x99, 0xAC } },
	{ 0xEA9A81, { 0xEA, 0x9A, 0x80 } },
	{ 0xEA9A83, { 0xEA, 0x9A, 0x82 } },
	{ 0xEA9A85, { 0xEA, 0x9A, 0x84 } },
	{ 0xEA9A87, { 0xEA, 0x9A, 0x86 } },
	{ 0xEA9A89, { 0xEA, 0x9A, 0x88 } },
	{ 0xEA9A8B, { 0xEA, 0x9A, 0x8A } },
	{ 0xEA9A8D, { 0xEA, 0x9A, 0x8C } },
	{ 0xEA9A8F, { 0xEA, 0x9A, 0x8E } },
	{ 0xEA9A91, { 0xEA, 0x9A, 0x90 } },
	{ 0xEA9A93, { 0xEA, 0x9A, 0x92 } },
	{ 0xEA9A95, { 0xEA, 0x9A, 0x94 } },
	{ 0xEA9A97, { 0xEA, 0x9A, 0x96 } },
	{ 0xEA9CA3, { 0xEA, 0x9C, 0xA2 } },
	{ 0xEA9CA5, { 0xEA, 0x9C, 0xA4 } },
	{ 0xEA9CA7, { 0xEA, 0x9C, 0xA6 } },
	{ 0xEA9CA9, { 0xEA, 0x9C, 0xA8 } },
	{ 0xEA9CAB, { 0xEA, 0x9C, 0xAA } },
	{ 0xEA9CAD, { 0xEA, 0x9C, 0xAC } },
	{ 0xEA9CAF, { 0xEA, 0x9C, 0xAE } },
	{ 0xEA9CB3, { 0xEA, 0x9C, 0xB2 } },
	{ 0xEA9CB5, { 0xEA, 0x9C, 0xB4 } },
	{ 0xEA9CB7, { 0xEA, 0x9C, 0xB6 } },
	{ 0xEA9CB9, { 0xEA, 0x9C, 0xB8 } },
	{ 0xEA9CBB, { 0xEA, 0x9C, 0xBA } },
	{ 0xEA9CBD, { 0xEA, 0x9C, 0xBC } },
	{ 0xEA9CBF, { 0xEA, 0x9C, 0xBE } },
	{ 0xEA9D81, { 0xEA, 0x9D, 0x80 } },
	{ 0xEA9D83, { 0xEA, 0x9D, 0x82 } },
	{ 0xEA9D85, { 0xEA, 0x9D, 0x84 } },
	{ 0xEA9D87, { 0xEA, 0x9D, 0x86 } },
	{ 0xEA9D89, { 0xEA, 0x9D, 0x88 } },
	{ 0xEA9D8B, { 0xEA, 0x9D, 0x8A } },
	{ 0xEA9D8D, { 0xEA, 0x9D, 0x8C } },
	{ 0xEA9D8F, { 0xEA, 0x9D, 0x8E } },
	{ 0xEA9D91, { 0xEA, 0x9D, 0x90 } },
	{ 0xEA9D93, { 0xEA, 0x9D, 0x92 } },
	{ 0xEA9D95, { 0xEA, 0x9D, 0x94 } },
	{ 0xEA9D97, { 0xEA, 0x9D, 0x96 } },
	{ 0xEA9D99, { 0xEA, 0x9D, 0x98 } },
	{ 0xEA9D9B, { 0xEA, 0x9D, 0x9A } },
	{ 0xEA9D9D, { 0xEA, 0x9D, 0x9C } },
	{ 0xEA9D9F, { 0xEA, 0x9D, 0x9E } },
	{ 0xEA9DA1, { 0xEA, 0x9D, 0xA0 } },
	{ 0xEA9DA3, { 0xEA, 0x9D, 0xA2 } },
	{ 0xEA9DA5, { 0xEA, 0x9D, 0xA4 } },
	{ 0xEA9DA7, { 0xEA, 0x9D, 0xA6 } },
	{ 0xEA9DA9, { 0xEA, 0x9D, 0xA8 } },
	{ 0xEA9DAB, { 0xEA, 0x9D, 0xAA } },
	{ 0xEA9DAD, { 0xEA, 0x9D, 0xAC } },
	{ 0xEA9DAF, { 0xEA, 0x9D, 0xAE } },
	{ 0xEA9DBA, { 0xEA, 0x9D, 0xB9 } },
	{ 0xEA9DBC, { 0xEA, 0x9D, 0xBB } },
	{ 0xEA9DBF, { 0xEA, 0x9D, 0xBE } },
	{ 0xEA9E81, { 0xEA, 0x9E, 0x80 } },
	{ 0xEA9E83, { 0xEA, 0x9E, 0x82 } },
	{ 0xEA9E85, { 0xEA, 0x9E, 0x84 } },
	{ 0xEA9E87, { 0xEA, 0x9E, 0x86 } },
	{ 0xEA9E8C, { 0xEA, 0x9E, 0x8B } },
	{ 0xEFAC83, { 0x46, 0x46, 0x49 } },
	{ 0xEFAC84, { 0x46, 0x46, 0x4C } },
	{ 0xEFBD81, { 0xEF, 0xBC, 0xA1 } },
	{ 0xEFBD82, { 0xEF, 0xBC, 0xA2 } },
	{ 0xEFBD83, { 0xEF, 0xBC, 0xA3 } },
	{ 0xEFBD84, { 0xEF, 0xBC, 0xA4 } },
	{ 0xEFBD85, { 0xEF, 0xBC, 0xA5 } },
	{ 0xEFBD86, { 0xEF, 0xBC, 0xA6 } },
	{ 0xEFBD87, { 0xEF, 0xBC, 0xA7 } },
	{ 0xEFBD88, { 0xEF, 0xBC, 0xA8 } },
	{ 0xEFBD89, { 0xEF, 0xBC, 0xA9 } },
	{ 0xEFBD8A, { 0xEF, 0xBC, 0xAA } },
	{ 0xEFBD8B, { 0xEF, 0xBC, 0xAB } },
	{ 0xEFBD8C, { 0xEF, 0xBC, 0xAC } },
	{ 0xEFBD8D, { 0xEF, 0xBC, 0xAD } },
	{ 0xEFBD8E, { 0xEF, 0xBC, 0xAE } },
	{ 0xEFBD8F, { 0xEF, 0xBC, 0xAF } },
	{ 0xEFBD90, { 0xEF, 0xBC, 0xB0 } },
	{ 0xEFBD91, { 0xEF, 0xBC, 0xB1 } },
	{ 0xEFBD92, { 0xEF, 0xBC, 0xB2 } },
	{ 0xEFBD93, { 0xEF, 0xBC, 0xB3 } },
	{ 0xEFBD94, { 0xEF, 0xBC, 0xB4 } },
	{ 0xEFBD95, { 0xEF, 0xBC, 0xB5 } },
	{ 0xEFBD96, { 0xEF, 0xBC, 0xB6 } },
	{ 0xEFBD97, { 0xEF, 0xBC, 0xB7 } },
	{ 0xEFBD98, { 0xEF, 0xBC, 0xB8 } },
	{ 0xEFBD99, { 0xEF, 0xBC, 0xB9 } },
	{ 0xEFBD9A, { 0xEF, 0xBC, 0xBA } },
};

const struct {
	const uint32_t key;
	const uint8_t val_len;
	const uint8_t val[6];
} UTF8_LOWER_UPPER_3B_DBL[] = {
	{ 0xE1BD90, 4, { 0xCE, 0xA5, 0xCC, 0x93 } },
	{ 0xE1BD92, 6, { 0xCE, 0xA5, 0xCC, 0x93, 0xCC, 0x80 } },
	{ 0xE1BD94, 6, { 0xCE, 0xA5, 0xCC, 0x93, 0xCC, 0x81 } },
	{ 0xE1BD96, 6, { 0xCE, 0xA5, 0xCC, 0x93, 0xCD, 0x82 } },
	{ 0xE1BE80, 5, { 0xE1, 0xBC, 0x88, 0xCE, 0x99 } },
	{ 0xE1BE81, 5, { 0xE1, 0xBC, 0x89, 0xCE, 0x99 } },
	{ 0xE1BE82, 5, { 0xE1, 0xBC, 0x8A, 0xCE, 0x99 } },
	{ 0xE1BE83, 5, { 0xE1, 0xBC, 0x8B, 0xCE, 0x99 } },
	{ 0xE1BE84, 5, { 0xE1, 0xBC, 0x8C, 0xCE, 0x99 } },
	{ 0xE1BE85, 5, { 0xE1, 0xBC, 0x8D, 0xCE, 0x99 } },
	{ 0xE1BE86, 5, { 0xE1, 0xBC, 0x8E, 0xCE, 0x99 } },
	{ 0xE1BE87, 5, { 0xE1, 0xBC, 0x8F, 0xCE, 0x99 } },
	{ 0xE1BE88, 5, { 0xE1, 0xBC, 0x88, 0xCE, 0x99 } },
	{ 0xE1BE89, 5, { 0xE1, 0xBC, 0x89, 0xCE, 0x99 } },
	{ 0xE1BE8A, 5, { 0xE1, 0xBC, 0x8A, 0xCE, 0x99 } },
	{ 0xE1BE8B, 5, { 0xE1, 0xBC, 0x8B, 0xCE, 0x99 } },
	{ 0xE1BE8C, 5, { 0xE1, 0xBC, 0x8C, 0xCE, 0x99 } },
	{ 0xE1BE8D, 5, { 0xE1, 0xBC, 0x8D, 0xCE, 0x99 } },
	{ 0xE1BE8E, 5, { 0xE1, 0xBC, 0x8E, 0xCE, 0x99 } },
	{ 0xE1BE8F, 5, { 0xE1, 0xBC, 0x8F, 0xCE, 0x99 } },
	{ 0xE1BE90, 5, { 0xE1, 0xBC, 0xA8, 0xCE, 0x99 } },
	{ 0xE1BE91, 5, { 0xE1, 0xBC, 0xA9, 0xCE, 0x99 } },
	{ 0xE1BE92, 5, { 0xE1, 0xBC, 0xAA, 0xCE, 0x99 } },
	{ 0xE1BE93, 5, { 0xE1, 0xBC, 0xAB, 0xCE, 0x99 } },
	{ 0xE1BE94, 5, { 0xE1, 0xBC, 0xAC, 0xCE, 0x99 } },
	{ 0xE1BE95, 5, { 0xE1, 0xBC, 0xAD, 0xCE, 0x99 } },
	{ 0xE1BE96, 5, { 0xE1, 0xBC, 0xAE, 0xCE, 0x99 } },
	{ 0xE1BE97, 5, { 0xE1, 0xBC, 0xAF, 0xCE, 0x99 } },
	{ 0xE1BE98, 5, { 0xE1, 0xBC, 0xA8, 0xCE, 0x99 } },
	{ 0xE1BE99, 5, { 0xE1, 0xBC, 0xA9, 0xCE, 0x99 } },
	{ 0xE1BE9A, 5, { 0xE1, 0xBC, 0xAA, 0xCE, 0x99 } },
	{ 0xE1BE9B, 5, { 0xE1, 0xBC, 0xAB, 0xCE, 0x99 } },
	{ 0xE1BE9C, 5, { 0xE1, 0xBC, 0xAC, 0xCE, 0x99 } },
	{ 0xE1BE9D, 5, { 0xE1, 0xBC, 0xAD, 0xCE, 0x99 } },
	{ 0xE1BE9E, 5, { 0xE1, 0xBC, 0xAE, 0xCE, 0x99 } },
	{ 0xE1BE9F, 5, { 0xE1, 0xBC, 0xAF, 0xCE, 0x99 } },
	{ 0xE1BEA0, 5, { 0xE1, 0xBD, 0xA8, 0xCE, 0x99 } },
	{ 0xE1BEA1, 5, { 0xE1, 0xBD, 0xA9, 0xCE, 0x99 } },
	{ 0xE1BEA2, 5, { 0xE1, 0xBD, 0xAA, 0xCE, 0x99 } },
	{ 0xE1BEA3, 5, { 0xE1, 0xBD, 0xAB, 0xCE, 0x99 } },
	{ 0xE1BEA4, 5, { 0xE1, 0xBD, 0xAC, 0xCE, 0x99 } },
	{ 0xE1BEA5, 5, { 0xE1, 0xBD, 0xAD, 0xCE, 0x99 } },
	{ 0xE1BEA6, 5, { 0xE1, 0xBD, 0xAE, 0xCE, 0x99 } },
	{ 0xE1BEA7, 5, { 0xE1, 0xBD, 0xAF, 0xCE, 0x99 } },
	{ 0xE1BEA8, 5, { 0xE1, 0xBD, 0xA8, 0xCE, 0x99 } },
	{ 0xE1BEA9, 5, { 0xE1, 0xBD, 0xA9, 0xCE, 0x99 } },
	{ 0xE1BEAA, 5, { 0xE1, 0xBD, 0xAA, 0xCE, 0x99 } },
	{ 0xE1BEAB, 5, { 0xE1, 0xBD, 0xAB, 0xCE, 0x99 } },
	{ 0xE1BEAC, 5, { 0xE1, 0xBD, 0xAC, 0xCE, 0x99 } },
	{ 0xE1BEAD, 5, { 0xE1, 0xBD, 0xAD, 0xCE, 0x99 } },
	{ 0xE1BEAE, 5, { 0xE1, 0xBD, 0xAE, 0xCE, 0x99 } },
	{ 0xE1BEAF, 5, { 0xE1, 0xBD, 0xAF, 0xCE, 0x99 } },
	{ 0xE1BEB2, 5, { 0xE1, 0xBE, 0xBA, 0xCE, 0x99 } },
	{ 0xE1BEB3, 4, { 0xCE, 0x91, 0xCE, 0x99 } },
	{ 0xE1BEB4, 4, { 0xCE, 0x86, 0xCE, 0x99 } },
	{ 0xE1BEB6, 4, { 0xCE, 0x91, 0xCD, 0x82 } },
	{ 0xE1BEB7, 6, { 0xCE, 0x91, 0xCD, 0x82, 0xCE, 0x99 } },
	{ 0xE1BEBC, 4, { 0xCE, 0x91, 0xCE, 0x99 } },
	{ 0xE1BEBE, 2, { 0xCE, 0x99 } },
	{ 0xE1BF82, 5, { 0xE1, 0xBF, 0x8A, 0xCE, 0x99 } },
	{ 0xE1BF83, 4, { 0xCE, 0x97, 0xCE, 0x99 } },
	{ 0xE1BF84, 4, { 0xCE, 0x89, 0xCE, 0x99 } },
	{ 0xE1BF86, 4, { 0xCE, 0x97, 0xCD, 0x82 } },
	{ 0xE1BF87, 6, { 0xCE, 0x97, 0xCD, 0x82, 0xCE, 0x99 } },
	{ 0xE1BF8C, 4, { 0xCE, 0x97, 0xCE, 0x99 } },
	{ 0xE1BF92, 6, { 0xCE, 0x99, 0xCC, 0x88, 0xCC, 0x80 } },
	{ 0xE1BF93, 6, { 0xCE, 0x99, 0xCC, 0x88, 0xCC, 0x81 } },
	{ 0xE1BF96, 4, { 0xCE, 0x99, 0xCD, 0x82 } },
	{ 0xE1BF97, 6, { 0xCE, 0x99, 0xCC, 0x88, 0xCD, 0x82 } },
	{ 0xE1BFA2, 6, { 0xCE, 0xA5, 0xCC, 0x88, 0xCC, 0x80 } },
	{ 0xE1BFA3, 6, { 0xCE, 0xA5, 0xCC, 0x88, 0xCC, 0x81 } },
	{ 0xE1BFA4, 4, { 0xCE, 0xA1, 0xCC, 0x93 } },
	{ 0xE1BFA6, 4, { 0xCE, 0xA5, 0xCD, 0x82 } },
	{ 0xE1BFA7, 6, { 0xCE, 0xA5, 0xCC, 0x88, 0xCD, 0x82 } },
	{ 0xE1BFB2, 5, { 0xE1, 0xBF, 0xBA, 0xCE, 0x99 } },
	{ 0xE1BFB3, 4, { 0xCE, 0xA9, 0xCE, 0x99 } },
	{ 0xE1BFB4, 4, { 0xCE, 0x8F, 0xCE, 0x99 } },
	{ 0xE1BFB6, 4, { 0xCE, 0xA9, 0xCD, 0x82 } },
	{ 0xE1BFB7, 6, { 0xCE, 0xA9, 0xCD, 0x82, 0xCE, 0x99 } },
	{ 0xE1BFBC, 4, { 0xCE, 0xA9, 0xCE, 0x99 } },
	{ 0xE2B1A5, 2, { 0xC8, 0xBA } },
	{ 0xE2B1A6, 2, { 0xC8, 0xBE } },
	{ 0xEFAC80, 2, { 0x46, 0x46 } },
	{ 0xEFAC81, 2, { 0x46, 0x49 } },
	{ 0xEFAC82, 2, { 0x46, 0x4C } },
	{ 0xEFAC85, 2, { 0x53, 0x54 } },
	{ 0xEFAC86, 2, { 0x53, 0x54 } },
	{ 0xEFAC93, 4, { 0xD5, 0x84, 0xD5, 0x86 } },
	{ 0xEFAC94, 4, { 0xD5, 0x84, 0xD4, 0xB5 } },
	{ 0xEFAC95, 4, { 0xD5, 0x84, 0xD4, 0xBB } },
	{ 0xEFAC96, 4, { 0xD5, 0x8E, 0xD5, 0x86 } },
	{ 0xEFAC97, 4, { 0xD5, 0x84, 0xD4, 0xBD } },
};

const struct {
	const uint32_t key;
	const uint8_t val[4];
} UTF8_LOWER_UPPER_4B[] = {
	{ 0xF09090A8, { 0xF0, 0x90, 0x90, 0x80 } },
	{ 0xF09090A9, { 0xF0, 0x90, 0x90, 0x81 } },
	{ 0xF09090AA, { 0xF0, 0x90, 0x90, 0x82 } },
	{ 0xF09090AB, { 0xF0, 0x90, 0x90, 0x83 } },
	{ 0xF09090AC, { 0xF0, 0x90, 0x90, 0x84 } },
	{ 0xF09090AD, { 0xF0, 0x90, 0x90, 0x85 } },
	{ 0xF09090AE, { 0xF0, 0x90, 0x90, 0x86 } },
	{ 0xF09090AF, { 0xF0, 0x90, 0x90, 0x87 } },
	{ 0xF09090B0, { 0xF0, 0x90, 0x90, 0x88 } },
	{ 0xF09090B1, { 0xF0, 0x90, 0x90, 0x89 } },
	{ 0xF09090B2, { 0xF0, 0x90, 0x90, 0x8A } },
	{ 0xF09090B3, { 0xF0, 0x90, 0x90, 0x8B } },
	{ 0xF09090B4, { 0xF0, 0x90, 0x90, 0x8C } },
	{ 0xF09090B5, { 0xF0, 0x90, 0x90, 0x8D } },
	{ 0xF09090B6, { 0xF0, 0x90, 0x90, 0x8E } },
	{ 0xF09090B7, { 0xF0, 0x90, 0x90, 0x8F } },
	{ 0xF09090B8, { 0xF0, 0x90, 0x90, 0x90 } },
	{ 0xF09090B9, { 0xF0, 0x90, 0x90, 0x91 } },
	{ 0xF09090BA, { 0xF0, 0x90, 0x90, 0x92 } },
	{ 0xF09090BB, { 0xF0, 0x90, 0x90, 0x93 } },
	{ 0xF09090BC, { 0xF0, 0x90, 0x90, 0x94 } },
	{ 0xF09090BD, { 0xF0, 0x90, 0x90, 0x95 } },
	{ 0xF09090BE, { 0xF0, 0x90, 0x90, 0x96 } },
	{ 0xF09090BF, { 0xF0, 0x90, 0x90, 0x97 } },
	{ 0xF0909180, { 0xF0, 0x90, 0x90, 0x98 } },
	{ 0xF0909181, { 0xF0, 0x90, 0x90, 0x99 } },
	{ 0xF0909182, { 0xF0, 0x90, 0x90, 0x9A } },
	{ 0xF0909183, { 0xF0, 0x90, 0x90, 0x9B } },
	{ 0xF0909184, { 0xF0, 0x90, 0x90, 0x9C } },
	{ 0xF0909185, { 0xF0, 0x90, 0x90, 0x9D } },
	{ 0xF0909186, { 0xF0, 0x90, 0x90, 0x9E } },
	{ 0xF0909187, { 0xF0, 0x90, 0x90, 0x9F } },
	{ 0xF0909188, { 0xF0, 0x90, 0x90, 0xA0 } },
	{ 0xF0909189, { 0xF0, 0x90, 0x90, 0xA1 } },
	{ 0xF090918A, { 0xF0, 0x90, 0x90, 0xA2 } },
	{ 0xF090918B, { 0xF0, 0x90, 0x90, 0xA3 } },
	{ 0xF090918C, { 0xF0, 0x90, 0x90, 0xA4 } },
	{ 0xF090918D, { 0xF0, 0x90, 0x90, 0xA5 } },
	{ 0xF090918E, { 0xF0, 0x90, 0x90, 0xA6 } },
	{ 0xF090918F, { 0xF0, 0x90, 0x90, 0xA7 } },
};
