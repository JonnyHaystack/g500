#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <endian.h>

#include "g500.h"

int get_button_num (uint16_t bits);

int main (int argc, char *argv[]) {
	uint8_t buffer[G500_PAGE_SIZE];

	int ret, r = 0;
	do {
		ret = read (0, &buffer[r], G500_PAGE_SIZE-r);
		if (ret == -1) {
			perror ("read");
			return EXIT_FAILURE;
		}
		r += ret;
	} while (r != G500_PAGE_SIZE && ret != 0);
	
	int offset = 0;
	if (argc > 1)
		offset = strtol (argv[1], NULL, 0);
	
	int i = 2*offset;
	int end = 0;
	while (!end) {
		struct g500_macro_item_t *macro_item = (struct g500_macro_item_t *)&buffer[i];
		if (i % 2 == 0)
			printf ("%02X\t", i/2);
		else
			printf ("\t");
		switch (macro_item->type) {
		case G500_MACRO_PADDING:
			printf ("PADDING\n");
			break;
		case G500_MACRO_END:
			printf ("END\n");
			end = 1;
			break;
		case G500_MACRO_KEY_PRESS:
			printf ("KEY PRESS 0x%02hhX\n", macro_item->value.key_usage);
			break;
		case G500_MACRO_KEY_RELEASE:
			printf ("KEY RELEASE 0x%02hhX\n", macro_item->value.key_usage);
			break;
		case G500_MACRO_MODIFIER_PRESS:
			printf ("MODIFIER PRESS 0x%02hhX\n", macro_item->value.modifier_bits);
			break;
		case G500_MACRO_MODIFIER_RELEASE:
			printf ("MODIFIER RELEASE 0x%02hhX\n", macro_item->value.modifier_bits);
			break;
		case G500_MACRO_WHEEL:
			printf ("WHEEL %hhd\n", macro_item->value.wheel);
			break;
		case G500_MACRO_BUTTON_PRESS:
			printf ("MOUSE BUTTON PRESS %d\n", g500_get_button_num (le16toh (macro_item->value.button)));
			break;
		case G500_MACRO_BUTTON_RELEASE:
			printf ("MOUSE BUTTON RELEASE %d\n", g500_get_button_num (le16toh (macro_item->value.button)));
			break;
		case G500_MACRO_CONSUMER_CONTROL:
			printf ("CONSUMER CONTROL %X\n", be16toh (macro_item->value.cc_usage));
			break;
		case G500_MACRO_DELAY:
			printf ("DELAY %dms\n", be16toh (macro_item->value.delay));
			break;
		case G500_MACRO_JUMP:
			printf ("JUMP 0x%02hhX:0x%02hhX\n", macro_item->value.jump.page, macro_item->value.jump.offset);
			break;
		case G500_MACRO_REPEAT:
			printf ("REPEAT 0x%02hhX:0x%02hhX\n",
				macro_item->value.jump.page,
				macro_item->value.jump.offset);
			break;
		case G500_MACRO_WAIT_HOLD:
			printf ("WAIT HOLD %dms 0x%02hhX:0x%02hhX\n",
				be16toh (macro_item->value.hold.delay),
				macro_item->value.hold.page,
				macro_item->value.hold.offset);
				break;
		default:
			//fprintf (stderr, "Unknown type 0x%02hhX\n", macro_item->type);
			printf ("Unknown type 0x%02hhX\n", macro_item->type);
			i++;
			continue;
		}
		i += g500_macro_item_len (macro_item);
	}
	return EXIT_SUCCESS;
}


