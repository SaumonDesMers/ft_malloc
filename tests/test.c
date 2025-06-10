#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int ac)
{
	int i;
	char *addr;

	i = 0;
	while (i < 1024)
	{
		if (ac > 1)
		{
			addr = (char*)malloc(1024 * i);
			if (addr == NULL)
			{
				write(1, "Failed to allocate memory\n", 26);
				return (1);
			}
			addr[0] = 42;
		}
		if (ac > 2)
		{
			free(addr);
		}
		i++;
	}
	return (0);
}