#ifndef CHECK_H

# define CHECK_H

# include <fcntl.h>
# include "../libft/inc/libft.h"
# include "map.h"

/* Basic checks to see if the correct file has been opened */
char	**check_params(int argc, char **argv, t_lay *lay);

/* Runs all map checker and builds map as a matrix */
char	**check_map(int fd, t_lay *lay);

#endif
