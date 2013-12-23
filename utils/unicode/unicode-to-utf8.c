#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <rig.h>

static RIG_QUEUE qqq = NULL;
static size_t curr_longest = 0;

uint32_t unicode_to_utf8(uint32_t uc) {
	// Byte order mark (BOM)
	if (uc == 0xFEFF) {
		return (0);
	}

	// Illegal surrogates
	if ((uc >= 0xD800) && (uc <= 0xDFFF)) {
		return (0);
	}

	// ASCII range
	if (uc <= 0x007F) {
		return (uc);
	}

	// 2 byte sequence
	if (uc <= 0x07FF) {
		uint32_t res = 0;

		res = 0xC0 | (uc >> 6);
		res <<= 8;
		res |= 0x80 | (uc & 0x003F);

		return (res);
	}

	// 3 byte sequence
	if (uc <= 0xFFFF) {
		uint32_t res = 0;

		res = 0xE0 | (uc >> 12);
		res <<= 8;
		res |= 0x80 | ((uc >> 6) & 0x003F);
		res <<= 8;
		res |= 0x80 | (uc & 0x003F);

		return (res);
	}

	// 4 byte sequence
	if (uc <= 0x10FFFF) {
		uint32_t res = 0;

		res = 0xF0 | (uc >> 18);
		res <<= 8;
		res |= 0x80 | ((uc >> 12) & 0x003F);
		res <<= 8;
		res |= 0x80 | ((uc >> 6) & 0x003F);
		res <<= 8;
		res |= 0x80 | (uc & 0x003F);

		return (res);
	}

	return (0);
}

void write_string_out(const char *first_str, const char *second_str, const char *third_str, const char *fourth_str, size_t first_len, size_t total_blen) {
	if (total_blen > 6)
		printf("BUG BUG BUG\n");

	bool first_init = true;
	char str_print[65];
	char *str_out = str_print;

	if (total_blen == first_len) {
		str_out += sprintf(str_out, "\t{ 0x%s, { ", first_str);
	}
	else {
		str_out += sprintf(str_out, "\t{ 0x%s, %zu, { ", first_str, total_blen);
	}

	while (*second_str) {
		if (first_init) {
			first_init = false;
			str_out += sprintf(str_out, "0x%.2s", second_str);
		}
		else {
			str_out += sprintf(str_out, ", 0x%.2s", second_str);
		}

		second_str += 2;
	}

	while (*third_str) {
		if (first_init) {
			first_init = false;
			str_out += sprintf(str_out, "0x%.2s", third_str);
		}
		else {
			str_out += sprintf(str_out, ", 0x%.2s", third_str);
		}

		third_str += 2;
	}

	while (*fourth_str) {
		if (first_init) {
			first_init = false;
			str_out += sprintf(str_out, "0x%.2s", fourth_str);
		}
		else {
			str_out += sprintf(str_out, ", 0x%.2s", fourth_str);
		}

		fourth_str += 2;
	}

	sprintf(str_out, " } },\n");

	if (total_blen == first_len) {
		printf("%s", str_print);
	}
	else {
		if (total_blen > curr_longest) {
			curr_longest = total_blen;
		}

		str_out = malloc(strlen(str_print) + 1);
		strcpy(str_out, str_print);
		rig_queue_put(qqq, str_out);
	}
}

void write_out(uint32_t first, uint32_t second, uint32_t third, uint32_t fourth, const char *struct_name) {
	static uint8_t struct_mode = 1;

	first = unicode_to_utf8(first);
	second = unicode_to_utf8(second);
	third = unicode_to_utf8(third);
	fourth = unicode_to_utf8(fourth);

	char first_str[9];
	sprintf(first_str, "%X", first);
	size_t first_len = strlen(first_str) / 2;

	char second_str[9];
	sprintf(second_str, "%X", second);
	size_t total_blen = strlen(second_str) / 2;

	char third_str[9];
	if (third != 0) {
		sprintf(third_str, "%X", third);
		total_blen += strlen(third_str) / 2;
	}
	else {
		*third_str = '\0';
	}

	char fourth_str[9];
	if (fourth != 0) {
		sprintf(fourth_str, "%X", fourth);
		total_blen += strlen(fourth_str) / 2;
	}
	else {
		*fourth_str = '\0';
	}

	if (first_len <= 1) {
		// Lets not output the 1 byte table, A-Z => a-z and a-z => A-Z can be done much faster by fixed addition/subtraction
	}
	else if (first_len <= 2) {
		if (struct_mode == 1) {
			printf("#include <stdint.h>\n\n");
			printf("const struct {\n\tconst uint16_t key;\n\tconst uint8_t val[2];\n} %s_%dB[] = {\n", struct_name, ++struct_mode);
		}

		 write_string_out(first_str, second_str, third_str, fourth_str, first_len, total_blen);
	}
	else if (first_len <= 3) {
		if (struct_mode == 2) {
			if (!rig_queue_empty(qqq)) {
				printf("};\n\nconst struct {\n\tconst uint16_t key;\n\tconst uint8_t val_len;\n\tconst uint8_t val[%zu];\n} %s_%dB_DBL[] = {\n", curr_longest, struct_name, struct_mode);
				curr_longest = 0;
			}

			while (!rig_queue_empty(qqq)) {
				char *str;
				rig_queue_get(qqq, &str);
				printf("%s", str);
				free(str);
			}

			printf("};\n\nconst struct {\n\tconst uint32_t key;\n\tconst uint8_t val[3];\n} %s_%dB[] = {\n", struct_name, ++struct_mode);
		}

		 write_string_out(first_str, second_str, third_str, fourth_str, first_len, total_blen);
	}
	else if (first_len <= 4) {
		if (struct_mode == 3) {
			if (!rig_queue_empty(qqq)) {
				printf("};\n\nconst struct {\n\tconst uint32_t key;\n\tconst uint8_t val_len;\n\tconst uint8_t val[%zu];\n} %s_%dB_DBL[] = {\n", curr_longest, struct_name, struct_mode);
				curr_longest = 0;
			}

			while (!rig_queue_empty(qqq)) {
				char *str;
				rig_queue_get(qqq, &str);
				printf("%s", str);
				free(str);
			}

			printf("};\n\nconst struct {\n\tconst uint32_t key;\n\tconst uint8_t val[4];\n} %s_%dB[] = {\n", struct_name, ++struct_mode);
		}

		 write_string_out(first_str, second_str, third_str, fourth_str, first_len, total_blen);
	}
	else {
		// Should never happen
		printf("BUG BUG BUG\n");
	}
}

int main(int argc, char *argv[]) {
	FILE *fp_normal, *fp_special;

	if (argc != 4)
		exit(EXIT_FAILURE);

	fp_normal = fopen(argv[1], "r");
	if (fp_normal == NULL)
		exit(EXIT_FAILURE);

	fp_special = fopen(argv[2], "r");
	if (fp_special == NULL)
		exit(EXIT_FAILURE);

	qqq = rig_queue_init(0, sizeof(???));

	uint32_t first_normal, second_normal;
	uint32_t first_special, second_special;
	uint32_t third_special, fourth_special;
	bool read_normal = true;
	bool read_special = true;
	char *line_normal = NULL; size_t len_normal = 0;
	char *line_special = NULL; size_t len_special = 0;

	while (read_normal || read_special) {
		if (read_normal) {
			first_normal = 0, second_normal = 0;

			if (getline(&line_normal, &len_normal, fp_normal) != -1) {
				sscanf(line_normal, "%X %X", &first_normal, &second_normal);
			}
			else {
				first_normal = UINT32_MAX;
			}

			read_normal = false;
		}

		if (read_special) {
			first_special = 0, second_special = 0, third_special = 0, fourth_special = 0;

			if (getline(&line_special, &len_special, fp_special) != -1) {
				sscanf(line_special, "%X %X %X %X", &first_special, &second_special, &third_special, &fourth_special);
			}
			else {
				first_special = UINT32_MAX;
			}

			read_special = false;
		}

		if (first_normal == UINT32_MAX && first_special == UINT32_MAX) {
			// Finished, let's exit
			read_normal = false;
			read_special = false;
		}
		else if (first_normal == first_special && first_special != 0) {
			write_out(first_special, second_special, third_special, fourth_special, argv[3]);

			read_normal = true;
			read_special = true;
		}
		else if (first_normal > first_special && first_special != 0) {
			write_out(first_special, second_special, third_special, fourth_special, argv[3]);

			read_normal = false;
			read_special = true;
		}
		else if (first_normal < first_special && first_normal != 0) {
			write_out(first_normal, second_normal, 0, 0, argv[3]);

			read_normal = true;
			read_special = false;
		}
		else {
			// Should never happen
			read_normal = false;
			read_special = false;

			printf("BUG BUG BUG\n");
		}
	}

	if (!rig_queue_empty(qqq)) {
		printf("};\n\nconst struct {\n\tconst uint32_t key;\n\tconst uint8_t val_len;\n\tconst uint8_t val[%zu];\n} %s_%dB_DBL[] = {\n", curr_longest, argv[3], 4);
		curr_longest = 0;
	}

	while (!rig_queue_empty(qqq)) {
		char *str;
		rig_queue_get(qqq, &str);
		printf("%s", str);
		free(str);
	}

	printf("};\n");

	rig_queue_destroy(&qqq);

	fclose(fp_normal);
	fclose(fp_special);

	exit(EXIT_SUCCESS);
}
