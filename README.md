# so_long — Pac-Man

A small Pac-Man-style 2D game written in C for the 42 `so_long` project.

The original 42 assignment is intentionally simple: read a map from a `.ber` file, render it in a window with MiniLibX, let the player collect everything, and reach the exit. I used that foundation to build a playable Pac-Man version with animated sprites, ghosts, a movement counter, and a few extra game-state mechanics.

## What the project is about

`so_long` is one of the 42 projects focused on learning through constraints rather than a large framework. The requirements are small, but they force you to deal with several parts of a real program at once:

- parsing and validating user input;
- managing a 2D map;
- handling keyboard and window events;
- drawing textures and sprites with MiniLibX;
- updating game state inside a loop;
- allocating and freeing memory correctly;
- keeping the Makefile and project structure under control.

The mandatory version only needs a player, walls, collectibles, free space, and an exit. This version uses the same map rules, then adds ghosts and animation as bonus-style features.

## Game rules

Run the game with a map as its only argument:

```bash
./bin/so_long path/to/map.ber
```

The map is made from these characters:

| Character | Meaning |
| --- | --- |
| `0` | Empty space |
| `1` | Wall |
| `C` | Collectible |
| `E` | Exit |
| `P` | Pac-Man starting position |
| `G` | Ghost starting position (extension) |

A valid map must:

- use the `.ber` extension;
- be rectangular;
- be surrounded by walls;
- contain at least one player, one collectible, and one exit;
- contain no characters outside the set above.

The parser accepts multiple `P` and `G` positions. With multiple players, the game only finishes after every player has reached an available exit. The subject does not require checking whether a valid path exists, so this project validates the structure of the map but does not run a path-finding check before starting.

If the map is invalid, the program exits with `Error` followed by a more specific message.

Example map:

```text
111111
1P0C01
1000E1
111111
```

## Controls

- `W`, `A`, `S`, `D` or the arrow keys — choose a direction;
- `R` — reset the current game;
- `Q` or `Esc` — quit;
- the window close button — quit cleanly.

The player keeps moving in the selected direction until a wall blocks the way. A turn is applied when the player reaches a point where the requested direction is legal. Collectibles are removed as they are picked up, and the exit only accepts players after all collectibles are gone.

The current move count is rendered inside the game window using a small retro-style font. It is limited to four digits.

## How it works

### 1. The map is read and checked

`main.c` starts by passing the command-line arguments to `check_params()` in `check.c`.

The map checker then:

1. opens the `.ber` file;
2. reads it one line at a time with `get_next_line`;
3. checks row lengths and wall borders;
4. counts players, ghosts, exits, and collectibles;
5. rejects unexpected characters;
6. converts the map into a `char **` matrix with `ft_split`.

The validation state lives in `t_err`, while the basic dimensions and object counts are stored in `t_lay`.

### 2. The game state is initialized

`init_game()` creates the main `t_game` state, stores a backup copy of the original map, initializes MiniLibX, and creates the window.

`ft_newgame()` then:

- records the map dimensions;
- loads the static map sprites;
- creates linked lists for Pac-Man and ghosts;
- loads directional and special animation frames;
- initializes the movement counter and frame counter;
- registers the MLX loop, keyboard hook, and window-close hook.

Most of the long-lived state is kept in `t_game`. Each player or ghost uses a `t_player` node containing its grid position, window position, direction, movement state, legal moves, and animation lists.

### 3. Grid movement is separated from screen movement

The game keeps two positions for each character:

- `pos` — the character's tile position on the map matrix;
- `win_pos` — its pixel position inside the MLX window.

A successful move updates the grid position first. The render functions then move the sprite toward the new pixel position a little at a time. This keeps movement smooth instead of making the character jump directly from one tile to the next.

The Mac and Linux render files use different movement increments and redraw rates because the two MiniLibX environments behave differently on this project.

### 4. The MLX loop updates the world

The loop hook in `player.c` advances the frame counter and calls the game update logic. Depending on the current state, the update cycle can:

- redraw the map;
- move Pac-Man toward its target tile;
- update ghost movement;
- advance sprite animations;
- redraw collectibles or exits that a ghost has passed over;
- update the score display;
- trigger the win or death sequence.

The game ends when every collectible has been collected and every player has reached an exit. It also ends if a ghost catches Pac-Man. Cleanup is handled by `end_game()` and the sprite/player freeing functions.

## Ghosts

Ghosts are the main gameplay extension beyond the mandatory part.

Each ghost:

1. finds the closest active Pac-Man using Euclidean distance;
2. checks which neighboring tiles are legal;
3. avoids walls, other ghosts, and unnecessary reverse turns;
4. chooses the available direction that gets it closest to the target;
5. moves through the world independently of keyboard input.

Ghosts can be loaded in different colors. When enough collectibles have been eaten, the game enters panic mode, changes the ghost animation, and adjusts the movement timing.

A collision either happens when a player tries to move into a ghost or when a ghost moves into a player. That sets `pac_dying`, stops normal play, and starts the death animation.

## Animation and rendering

Sprites are stored in linked lists. Each node contains one MLX image for a frame of an animation. The animation code advances through the list and loops back to the first frame when it reaches the end.

The project includes animations for:

- Pac-Man moving in four directions;
- ghost movement;
- ghost panic mode;
- Pac-Man's death sequence;
- the score digits.

The map and sprite-loading code is kept separate from the update logic. This makes it possible to redraw the world without rebuilding the entire game state every frame.

## Project structure

```text
.
├── Makefile
├── inc/
│   ├── check.h       # Map validation interfaces
│   ├── colors.h      # Terminal color definitions
│   ├── game.h        # Game structs and function declarations
│   └── map.h         # Map structs and parser interfaces
├── src/
│   ├── main.c        # Program entry point
│   ├── check.c       # Argument and map setup
│   ├── map.c         # Map reading and validation
│   ├── game_mac.c    # MacOS game loop and hooks
│   ├── game_linux.c  # Linux game loop and hooks
│   ├── player.c      # Player nodes, collisions, and update loop
│   ├── playerlist.c  # Player and ghost linked lists
│   ├── pacman.c      # Pac-Man loading, drawing, and movement requests
│   ├── ghosts.c      # Ghost loading, drawing, and movement
│   ├── chase.c       # Ghost target selection and chase direction
│   ├── legal.c       # Legal movement checks
│   ├── utils.c       # Movement, tile swapping, reset, and debug output
│   ├── sprites.c      # Static sprite loading, drawing, and cleanup
│   ├── render_mac.c  # Smooth MacOS redraw logic
│   ├── render_linux.c # Smooth Linux redraw logic
│   ├── anim.c        # Death animation and animation cleanup
│   ├── anim_dir.c    # Directional and panic animations
│   ├── load_dir.c    # Directional sprite loading
│   └── score.c       # On-screen move counter
└── sprites/
    ├── Pac-Man/
    ├── Ghosts/
    └── Other/
```

## Building and running

### Requirements

You need:

- a C compiler such as `clang` or `gcc`;
- `make`;
- MiniLibX configured for your operating system;
- the project dependencies `libft` and `get_next_line`.

The Makefile is set up to clone `libft` and `get_next_line` when those directories are missing. MiniLibX is platform-specific and is not included in this repository.

On MacOS, the Makefile uses the Mac implementation and links against OpenGL and AppKit. On Linux, it switches to the Linux implementation and links against X11 and Xext.

### Commands

```bash
# Build the executable
make

# Run a specific map
./bin/so_long path/to/map.ber

# Build and run a map through the Makefile
make test MAP=path/to/map.ber

# Remove object files
make clean

# Remove object files, libraries, and the binary
make fclean

# Rebuild from scratch
make re

# Run Norminette checks, when the required dependencies are present
make norminette
```

The `play` and `play2` Makefile targets expect collections of maps under `tests/` and `tests/other-maps/`. Those map folders are not part of the current checkout, so use `make test MAP=...` with your own `.ber` file unless you add them.

## Notes from the 42 subject

The official `so_long` subject describes the project as a small 2D game built to practise C, basic algorithms, windows, colors, events, textures, memory management, and MiniLibX. Its mandatory requirements cover the map format, four-direction movement, collision with walls, collectibles, an exit, and clean window shutdown.

The subject's listed bonus ideas are:

- enemies that can make the player lose;
- sprite animation;
- displaying the movement count directly in the window.

This project implements all three of those bonus-style additions with ghosts, animated sprite lists, and an in-window move counter.

Reference material:

- [42 `so_long` subject](https://github.com/madebypixel02/so_long/blob/main/en.subject.pdf)
- [Reference Pac-Man implementation](https://github.com/madebypixel02/so_long)
- [MiniLibX](https://github.com/42Paris/minilibx-linux)

## What I learned

The most useful part of this project was learning how many small systems have to agree for a game to feel simple. The map parser, linked-list state, keyboard hooks, animation frames, redraw timing, collision checks, and cleanup code all depend on each other. Pac-Man looks straightforward when it is running, but making the pieces work together without leaks or broken state was the real exercise.
