#include "../common/holberton-core.h"
#include <stdlib.h>

#define IS_SPACE(car) ( (car == ' ' ) || (car == '\t') )
#define IS_SEP(car) (car == CORE_ASM_SEP)
#define IS_COMMENT(car) (car == CORE_ASM_COMMENT)
#define IS_LABEL(car) (car == CORE_ASM_LABEL)

static void	 clean(char* line) {
	while (*line) {
		if (IS_COMMENT(*line) || *line == '\n')
			*line = 0;
		else
			line++;
	}
}

int   parse(char* line, char** output, int max)
{
	int			count = 0;
	int			state = 0;

	clean(line);

	// first we read the opcode ( or command )
	while (*line && max > count && state != -1) {
		char c = *line;

		switch (state) {
		case 0:
			if (!IS_SPACE(c)) {
				output[count++] = line;
				state = 1;
			}
			break;
		case 1:
			if (IS_LABEL(c)) {
				line++;
				*line = 0;
				state = 0;
			}
			if (IS_SPACE(c)) {
				*line = 0;
				if (*output[0] == '.') state = 2;
				else state = 3;
			}
			break;
		case 2:
			if (!IS_SPACE(c)) {
				output[count++] = line;
				state = -1;
			}
			break;
		case 3:
			if (!IS_SPACE(c)) {
				output[count++] = line;
				state = 4;
			}
			break;
		case 4:
			if (IS_SEP(c))
				*line = 0, state = 3;
			if (IS_SPACE(c))
				*line = 0;
			break;
		default:
			return -1;
		}
		line++;
	}
	return count;
}
