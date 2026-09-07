#include <fcntl.h>
#include "../inc/check.h"
#include "../inc/game.h"
#include <stdio.h>

int	main(int argc, char **argv)
{
	char	**map;
	t_lay	lay;

	map = check_params(argc, argv, &lay);
	init_game(map, lay);
	return (0);
}
