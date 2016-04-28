#include "files.h"

char *files_get_level(int num)
{
	switch(num)
	{
	case 1:
		return LEVEL_1;
	case 2:
		return LEVEL_2;
	case 3: 
		return LEVEL_3;
	}
	return 0;
}