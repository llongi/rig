# Example of a mini-parser, used for older array implementations

static inline bool internal_parse_typedata_key(TypeData tdata, const uint8_t *t, size_t tlen) {
	uint8_t key_type = 0;
	bool seen_num = false;

	for (size_t i = 0, td = 0, acc = 0, mul = 1000; i < tlen; i++) {
		switch (t[i]) {
			// Repetition count: max. 4 digits, 0 to 9
			// Determines the number of times to repeat a data specifier,
			// must be the first key, thus if any other were already
			// encountered, such as data specifiers, we fail
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				if ((key_type) || (mul < 1)) {
					ERRET(EINVAL, false);
				}
				acc += (size_t)(t[i] - '0') * mul;
				mul /= 10;
				seen_num = true;
				break;

			// Data specifiers: i, u, d, p, s, a
			// On first encounter, it's the key,
			// else we fail, as only one is permitted
			case 'i':
				if (!key_type) { key_type = 'i'; break; }
				ERRET(EINVAL, false);

			case 'u':
				if (!key_type) { key_type = 'u'; break; }
				ERRET(EINVAL, false);

			case 'd':
				if (!key_type) { key_type = 'd'; break; }
				ERRET(EINVAL, false);

			case 'p':
				if (!key_type) { key_type = 'p'; break; }
				ERRET(EINVAL, false);

			case 's':
				if (!key_type) { key_type = 's'; break; }
				ERRET(EINVAL, false);

			case 'a':
				 // Arrays not permitted as keys
				ERRET(EINVAL, false);

			// Vertical line finishes up
			// Validate the needed variables, insert the data into
			// the array and reset the counters to initial state
			case '|':
				// Validate variables
				if (!key_type) {
					ERRET(EINVAL, false);
				}
				if (!seen_num) { acc = 1; } // No numbers seen, default to 1

				// Insert data into array
				tdata[td].count = (uint16_t)acc;
				tdata[td].flags = 0;
				tdata[td].key_type = key_type;
				tdata[td].val_type = 0;
				td++;

				// Reset counters
				acc = 0;
				mul = 1000;
				key_type = 0;
				seen_num = false;
				break;

			// Everything else is an error
			default:
				ERRET(EINVAL, false);
		}
	}

	return (true);
}

static inline bool internal_parse_typedata_key_val(TypeData tdata, const uint8_t *t, size_t tlen) {
	uint8_t key_type = 0, val_type = 0, flags = 0;
	bool seen_num = false, seen_colon = false;

	for (size_t i = 0, td = 0, acc = 0, mul = 1000; i < tlen; i++) {
		switch (t[i]) {
			// Repetition count: max. 4 digits, 0 to 9
			// Determines the number of times to repeat a data specifier,
			// must be the first key, thus if any other were already
			// encountered, such as data specifiers or flagss, we fail
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				if ((key_type) || (seen_colon) || (mul < 1)) {
					ERRET(EINVAL, false);
				}
				acc += (size_t)(t[i] - '0') * mul;
				mul /= 10;
				seen_num = true;
				break;

			// Data specifiers: i, u, d, p, s, a
			// On first encounter, it's the key, on second, it's the value,
			// else we fail, as only two are permitted
			case 'i':
				if (!key_type) { key_type = 'i'; break; }
				if (!val_type) { val_type = 'i'; break; }
				ERRET(EINVAL, false);

			case 'u':
				if (!key_type) { key_type = 'u'; break; }
				if (!val_type) { val_type = 'u'; break; }
				ERRET(EINVAL, false);

			case 'd':
				if (!key_type) { key_type = 'd'; break; }
				if (!val_type) { val_type = 'd'; break; }
				ERRET(EINVAL, false);

			case 'p':
				if (!key_type) { key_type = 'p'; break; }
				if (!val_type) { val_type = 'p'; break; }
				ERRET(EINVAL, false);

			case 's':
				if (!key_type) { key_type = 's'; break; }
				if (!val_type) { val_type = 's'; break; }
				ERRET(EINVAL, false);

			case 'a':
				 // Arrays not permitted as keys
				if (!key_type) { ERRET(EINVAL, false); }
				if (!val_type) { val_type = 'a'; break; }
				ERRET(EINVAL, false);

			// Colon introduces flagss
			// Must be after key and value specifiers,
			// only one colon permitted
			case ':':
				if ((!key_type) || (!val_type) || (seen_colon)) {
					ERRET(EINVAL, false);
				}
				seen_colon = true;
				break;

			// flagss: R and D
			// Must be after the colon,
			// only one flags permitted
			case 'R':
				if ((!seen_colon) || (flags)) {
					ERRET(EINVAL, false);
				}
				flags = 'R';
				break;

			case 'D':
				if ((!seen_colon) || (flags)) {
					ERRET(EINVAL, false);
				}
				flags = 'D';
				break;

			// Vertical line finishes up
			// Validate the needed variables, insert the data into
			// the array and reset the counters to initial state
			case '|':
				// Validate variables
				if ((!key_type) || (!val_type) || (seen_colon && !flags)) {
					ERRET(EINVAL, false);
				}
				if (!seen_num) { acc = 1; } // No numbers seen, default to 1
				if (!flags) { flags = 'D'; } // No flags seen, default to DUPLICATE

				// Insert data into array
				tdata[td].count = (uint16_t)acc;
				tdata[td].flags = flags;
				tdata[td].key_type = key_type;
				tdata[td].val_type = val_type;
				td++;

				// Reset counters
				acc = 0;
				mul = 1000;
				key_type = 0;
				val_type = 0;
				flags = 0;
				seen_num = false;
				seen_colon = false;
				break;

			// Everything else is an error
			default:
				ERRET(EINVAL, false);
		}
	}

	return (true);
}
