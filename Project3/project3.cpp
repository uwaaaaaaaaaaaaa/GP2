//1
#include <stdio.h>
#include <stdlib.h>
//2
#define BOARD_WIDTH		(8)
#define BOARD_HEIGHT	(8)
//3
enum {
	TURN_BLACK,
	TURN_WHITE,
	TURN_NONE,
	TURN_MAX
};
//4
typedef struct {
	int x, y;
} VEC2;
//5
const char* diskAA[TURN_MAX] = {
	"Åú",
	"Åõ",
	"ÅE"
};

VEC2 cursorPosition;

int board[BOARD_HEIGHT][BOARD_WIDTH];
//6
void DrawScreen() {
	system("cls");
	for (int y = 0; y < BOARD_HEIGHT; y++) {
		for (int x = 0; x < BOARD_WIDTH; x++) {
			printf("%s", diskAA[board[y][x]]);
		}
			if (y == cursorPosition.y) {
				printf("Å©");
			}
			printf("\n");
	}
			for (int x = 0; x < BOARD_WIDTH; x++) {
				if (x == cursorPosition.x) {
					printf("Å™");
				}
				else {
					printf("Å@");
				}
			}
			printf("\n");
}

VEC2 InputPosition() {
	while (true)
	{
		DrawScreen();
	}
}

void Init() {
	for (int y=0;y<BOARD_HEIGHT;y++) {
		for (int x = 0; x < BOARD_WIDTH; x++) {
			board[y][x] = TURN_NONE;
		}
	}
	board[4][3] = board[3][4] = TURN_BLACK;
	board[3][3] = board[4][4] = TURN_WHITE;
	DrawScreen();
}

int main() {
	Init();
	while (1) {
		VEC2 placePosition;

		placePosition = InputPosition();
	}
}