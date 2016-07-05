#include <cstdio>

#include "ascentapp.h"

int main(int argc, char* argv[]) {
	AscentApp app;
	
	for (int i = 1; i < argc; i++)
		printf("%d: %s\n", i, argv[i]);

	return app.OnExecute();
}
