#include "gear2d.h"

int main(int argc, char **argv, char ** env) {
	// account for scene file
	if (argc > 1) gear2d::engine::load(argv[1]);
	else gear2d::engine::load();
	gear2d::engine::run();
}
