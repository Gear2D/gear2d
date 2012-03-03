#include "gear2d.h"
#include <stdio.h>

int main(int argc, char **argv, char ** env) {
	// account for scene file
	if (argc > 1) {
		if (argv[1][0] == '-' && argv[1][1] == 'v') {
			printf("%s", gear2d::engine::version());
			exit(0);
		} else {
			gear2d::engine::load(argv[1]);
		}
	}
	else gear2d::engine::load();
	gear2d::engine::run();
}
