#include "raylib.h"
#include <stdint.h>
#include <math.h>

#define global static
#define WIDTH 1280
#define HEIGHT 720

#define TEX_WIDTH 160
#define TEX_HEIGHT 90

#define M_PI 3.14159265358979323846

#define METHOD_CIRCLE_MIDPOINT 0
#define METHOD_CIRCLE_DDA 1
#define METHOD_CIRCLE_ANALYTICAL_DTHETA 2
#define METHOD_CIRCLE_JESKOS 3
#define METHOD_ELLIPSE_ANALYTICAL_DTHETA 4
#define METHOD_ELLIPSE_MIDPOINT 5

// Selected algorithm
#define DRAW_METHOD 2

global Color frame_buffer[TEX_WIDTH * TEX_HEIGHT];

void set_8_px(int cx, int cy, int x, int y, Color color) {
	frame_buffer[(cx + x) + (cy + y) * TEX_WIDTH] = color;
	frame_buffer[(cx - x) + (cy + y) * TEX_WIDTH] = color;
	frame_buffer[(cx + x) + (cy - y) * TEX_WIDTH] = color;
	frame_buffer[(cx - x) + (cy - y) * TEX_WIDTH] = color;
	frame_buffer[(cx + y) + (cy + x) * TEX_WIDTH] = color;
	frame_buffer[(cx - y) + (cy + x) * TEX_WIDTH] = color;
	frame_buffer[(cx + y) + (cy - x) * TEX_WIDTH] = color;
	frame_buffer[(cx - y) + (cy - x) * TEX_WIDTH] = color;
}

void set_4_px(int cx, int cy, int x, int y, Color color) {
	frame_buffer[(cx + x) + (cy + y) * TEX_WIDTH] = color;
	frame_buffer[(cx - x) + (cy + y) * TEX_WIDTH] = color;
	frame_buffer[(cx + x) + (cy - y) * TEX_WIDTH] = color;
	frame_buffer[(cx - x) + (cy - y) * TEX_WIDTH] = color;
}

float degToRad(float deg) { return deg * M_PI / 180.0f; }

void draw_ellipse_midpoint(int cx, int cy, int rx, int ry, Color color) {
	int x = rx;
	int y = 0;

	int ry2 = ry * ry;
	int rx2 = rx * rx;
	const float invRx2 = 1.0f / rx2;
	const float invRy2 = 1.0f / ry2;
	float D = 0.5 * (1 - 2 * rx) * invRx2 + invRy2;
	while (ry2 * x > rx2 * y) {
		set_4_px(cx, cy, x, y, color);
		if (D > 0) {
			D += (-2 * x + 3) * invRx2 + (2 * y + 3) * invRy2;
			x--;
		} else {
			D += (2 * y + 3) * invRy2;
		}
		y++;
	}

	D = (x - 1) * (x - 1) * invRx2 + (y + 0.5) * (y + 0.5) * invRy2 - 1;
	while (x >= 0) {
		set_4_px(cx, cy, x, y, color);

		if (D > 0) {
			D += (-2 * x + 3) * invRx2;
		} else {
			D += (-2 * x + 3) * invRx2 + (2 * y + 2) * invRy2;
			y++;
		}
		x--;
	}
}
void draw_ellipse_analytical_dtheta(int cx, int cy, int rx, int ry, Color color) {
	float theta = 0;
	const float invRx = 1.0f / rx;
	const float invRy = 1.0f / ry;
	const float thetaEnd = degToRad(90);
	const float sinThetaMax = 1 - invRy;
	while (theta <= thetaEnd) {
		const float cosTheta = cos(theta);
		const float sinTheta = sin(theta);
		float x = rx * cosTheta;
		float y = ry * sinTheta;
		set_4_px(cx, cy, round(x), round(y), color);
		float dTheta = acos(-invRx + cosTheta) - theta;
		if (sinTheta < sinThetaMax) {
			dTheta = fmin(dTheta, asin(invRy + sinTheta) - theta);
		}
		theta += dTheta;
	}
	// Draw top and bottom
	frame_buffer[(cx + 0) + (cy + ry) * TEX_WIDTH] = color;
	frame_buffer[(cx + 0) + (cy - ry) * TEX_WIDTH] = color;
}

void draw_circle_analytical_dtheta(int cx, int cy, int radius, Color color, bool approximate) {
	float theta = 0;
	const float thetaEnd = degToRad(45);
	const float invR = 1.0f / radius;

	float cosTheta = cos(theta);
	float sinTheta = sin(theta);
	while (theta <= thetaEnd) {
		const float cosTheta = cos(theta);
		const float sinTheta = sin(theta);
		float x = radius * cosTheta;
		float y = radius * sinTheta;
		set_8_px(cx, cy, round(x), round(y), color);
		if (approximate) {
			// dTheta = ds / r ~ 1 / r (i.e 1 px arc len)
			theta += invR;
		} else {
			float dThetaX = acos(-invR + cosTheta) - theta;
			float dThetaY = asin(invR + sinTheta) - theta;
			theta += fmin(dThetaX, dThetaY);
		}
	}
}

void draw_circle_midpoint(int cx, int cy, int radius, Color color) {
	int x = radius;
	int y = 0;
	int D = 3 - 2 * radius;
	int iter = 0;
	while (x >= y) {
		set_8_px(cx, cy, x, y, color);
		if (D > 0) {
			D += 4 * (y - x) + 10;
			x--;
		} else {
			D += 4 * y + 6;
		}
		y++;
	}
}

void draw_circle_dda(int cx, int cy, int radius, Color color) {
	float x = 0;
	float y = radius;
	const float xEnd = 0.707f * radius;
	while (x <= xEnd) {
		set_8_px(cx, cy, round(x), round(y), color);
		y = y - (x / y);
		x++;
	}
}

// https://schwarzers.com/algorithms/
void draw_circle_jeskos(int cx, int cy, int radius, Color color) {
	int y = 0;
	int t1 = radius >> 4;
	int x = radius;
	while (x >= y) {
		set_8_px(cx, cy, x, y, color);
		y++;
		t1 += y;
		int t2 = t1 - x;
		if (t2 >= 0) {
			t1 = t2;
			x--;
		}
	}
}

int main(void) {
	InitWindow(WIDTH, HEIGHT, "CircleZoo");

	SetTargetFPS(60);
	Image img = GenImageColor(TEX_WIDTH, TEX_HEIGHT, BLACK);
	Texture2D tex = LoadTextureFromImage(img);
	SetTextureFilter(tex, TEXTURE_FILTER_POINT);
	UnloadImage(img);
	const char* method_names[] = {"Midpoint Circle", "DDA", "Analytical dTheta", "Jeskos", "Analytical Ellipse",
								  "Midpoint Ellipse"};
	uint32_t frame_counter = 0;
	while (!WindowShouldClose()) {
		ClearBackground(BLACK);
		DrawText(method_names[DRAW_METHOD], 10, 10, 20, WHITE);
		int x_center = TEX_WIDTH >> 1;
		int y_center = TEX_HEIGHT >> 1;
		int circle_radius = 40;
		int circle_radius_y = 20;

		Color color = PINK;
		const int NUM_ITERS = 1000;
		uint64_t cycle_cnt = 0;
		for (int i = 0; i < NUM_ITERS; i++) {
			frame_counter++;
			uint64_t start = __rdtsc();
#if DRAW_METHOD == METHOD_CIRCLE_MIDPOINT
			draw_circle_midpoint(x_center, y_center, circle_radius, color);
#elif DRAW_METHOD == METHOD_CIRCLE_DDA
			draw_circle_dda(x_center, y_center, circle_radius, color);
#elif DRAW_METHOD == METHOD_CIRCLE_ANALYTICAL_DTHETA
			draw_circle_analytical_dtheta(x_center, y_center, circle_radius, color, false);
#elif DRAW_METHOD == METHOD_CIRCLE_JESKOS
			draw_circle_jeskos(x_center, y_center, circle_radius, color);
#elif DRAW_METHOD == METHOD_ELLIPSE_ANALYTICAL_DTHETA
			draw_ellipse_analytical_dtheta(x_center, y_center, circle_radius, circle_radius_y, color);
#elif DRAW_METHOD == METHOD_ELLIPSE_MIDPOINT
			draw_ellipse_midpoint(x_center, y_center, circle_radius, circle_radius_y, color);
#endif
			uint64_t end = __rdtsc();
			cycle_cnt += end - start;
		}
		if (frame_counter % 60) {
			printf("Num cycles: %llu\n", cycle_cnt / NUM_ITERS);
		}
		UpdateTexture(tex, frame_buffer);
		DrawTexturePro(tex, (Rectangle){0, 0, TEX_WIDTH, TEX_HEIGHT}, (Rectangle){0, 0, WIDTH, HEIGHT}, (Vector2){0, 0},
					   0.0f, WHITE);

		EndDrawing();
	}
	CloseWindow();
	return 0;
}