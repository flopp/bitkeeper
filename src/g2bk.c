#include "system.h"
#include "sccs.h"

static void	print_name(char *);

/*
 * _g2bk - convert gfile names to sfile names
 */
int
g2bk_main(int ac, char **av)
{
	int	i;

	if ((ac > 1) && strcmp(av[ac-1], "-")) {
		for (i = 1; i < ac; ++i) {
			print_name(av[i]);
		}
	} else {
		char	buf[MAXPATH];

		while (fnext(buf, stdin)) {
			chop(buf);
			print_name(buf);
		}
	}
	return (0);
}

static void
print_name(char *name)
{
	name = name2sccs(name);
	printf("%s\n", name);
	free(name);
}