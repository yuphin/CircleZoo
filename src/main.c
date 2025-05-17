#include "raylib.h"

int main(void) {
	const int screenWidth = 800;
	const int screenHeight = 450;

	InitWindow(screenWidth, screenHeight, "LineZoo");

	SetTargetFPS(60);
	// Main game loop
	while (!WindowShouldClose()) {
		ClearBackground(RAYWHITE);
		DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

		EndDrawing();
	}
	CloseWindow();
	return 0;
}