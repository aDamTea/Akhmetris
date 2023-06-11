#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<raylib.h>

// Height and width of game field
#define HEIGHT 20
#define WIDTH 10

// <---------------- Templates for each type of Tetramino ---------------------------->
int tetroI_temp[2][4][4] = { {{1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
							{{1, 0, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}} };
int tetroJ_temp[4][4][4] = { {{1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
							{{1, 1, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}},
							{{1, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
							{{0, 1, 0, 0}, {0, 1, 0, 0}, {1, 1, 0 ,0}, {0, 0, 0, 0}} };
int tetroL_temp[4][4][4] = { {{0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
							{{1, 0, 0, 0}, {1, 0, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},
							{{1, 1, 1, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
							{{1, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}} };
int tetroO_temp[1][4][4] = { 
							{{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}} 
						   };
int tetroT_temp[4][4][4] = { {{0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
							{{1, 0, 0, 0}, {1, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}},
							{{1, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
							{{0, 1, 0, 0}, {1, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}} };
int tetroS_temp[2][4][4] = { {{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
							{{1, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}} };
int tetroZ_temp[2][4][4] = { {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
							{{0, 1, 0, 0}, {1, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}} };
// <---------------------------------------------------------------------------------->

// Variables to make code reading much easier
enum BasicVars {
	DEFAULT = 1, NO_TICK = 0,
	GAME_OVER = 0, WAITING_FOR_ACTION = 1, MINO_ON_FIELD = 2, GAME_PAUSED = 3,
	MINO_RIGHT = 1, MINO_LEFT = 0,
	DELETE_MINO = 1, DRAW_MINO = 0,
	HOLD_NOT_PRESSED = 0, HOLD_PRESSED = 1, NEW_HOLD = 2,
	NO_MINO = 0, MINO_I = 1, MINO_J = 2, MINO_L = 3, MINO_O = 4, MINO_T = 5, MINO_S = 6, MINO_Z = 7
};

// The struct of block - part of minos and game field
typedef struct Block {
	int coordX;
	int coordY;
	int color;
	int type;
} Block;

// The struct of Tetramino containing information about it's: type, rotations, startcoords and each block
typedef struct Tetramino {
	int color;
	int size;
	int temps;
	int rotation;
	int startCoord;
	Block blocks[4];
} Tetramino;

// The struct of game field consisting of blocks
typedef struct Field {
	Block blocks[HEIGHT][WIDTH];
} Field;

// The main struct - game. 
typedef struct Game {
	int gameState;
	int tickRate;
	long int tick;
	int score;
	int lines;
	int akhmetris;
	int level;
	int prevLines;
	int holdState;
	Tetramino* hold;
	Tetramino* mino;
	Field* field;
} Game;

// <---------------------- Operations with structures -------------------------------->
Block* createBlock(int coordX, int coordY, int color, int type) {
	Block* block = (Block*)malloc(sizeof(Block));
	block->coordX = coordX;
	block->coordY = coordY;
	block->color = color;
	block->type = type;
	return block;
}

void deleteBlock(Block* block) {
	if (block) {
		free(block);
	}
}

Tetramino* createTetramino(int mino[4][4][4], int startCoord, int color, int temps, int rotation) {
	Tetramino* tetramino = (Tetramino*)malloc(sizeof(Tetramino));
	tetramino->startCoord = startCoord;
	tetramino->temps = temps;
	tetramino->color = color;
	tetramino->rotation = rotation;
	int size = 0;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (mino[rotation][i][j]) {
				tetramino->blocks[size].coordX = j + startCoord;
				tetramino->blocks[size].coordY = i;
				tetramino->blocks[size].color = color;
				tetramino->blocks[size].type = 1;
				size++;
			}
		}
	}
	tetramino->size = size;
	return tetramino;
}

void deleteTetramino(Tetramino* tetramino) {
	if (tetramino) {
		free(tetramino);
	}
}

Field* createField() {
	Field* field = (Field*)malloc(sizeof(Field));
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			field->blocks[i][j].coordX = j;
			field->blocks[i][j].coordY = i;
			field->blocks[i][j].color = 0;
			field->blocks[i][j].type = 0;
		}
	}
	return field;
}

void deleteField(Field* field) {
	if (field) {
		free(field);
	}
}

Game* createGame() {
	Game* game = (Game*)malloc(sizeof(Game));
	game->gameState = GAME_PAUSED;
	game->tickRate = 40;
	game->tick = 0;
	game->score = 0;
	game->lines = 0;
	game->prevLines = 0;
	game->akhmetris = 0;
	game->level = 0;
	game->holdState = NEW_HOLD;
	game->field = createField();
	return game;
}

void deleteGame(Game* game) {
	if (game) {
		if (game->field) {
			deleteField(game->field);
		}
		if (game->mino) {
			deleteTetramino(game->mino);
		}
		if (game->hold) {
			deleteTetramino(game->hold);
		}
		free(game);
	}
}
// <---------------------------------------------------------------------------------->

// Colors of each type of mino (this shoud be made another way, i dont have enough time)
Color minoColor(int mino_type) {
	Color color;
	switch (mino_type) {
	case MINO_I: 
		color = SKYBLUE;
		break;
	case MINO_J: 
		color = BLUE;
		break;
	case MINO_L: 
		color = ORANGE;
		break;
	case MINO_O: 
		color = YELLOW;
		break;
	case MINO_T: 
		color = PURPLE;
		break;
	case MINO_S: 
		color = GREEN;
		break;
	case MINO_Z: 
		color = RED;
		break;
	default:
		color = BLACK;
		break;
	}
	return color;
}

// Updates position of mino on game field, or deletes mino
void drawMino(Game* game, int isDelete) {
	for (int i = 0; i < game->mino->size; i++) {
		int coordX = game->mino->blocks[i].coordX;
		int coordY = game->mino->blocks[i].coordY;
		if (isDelete) {
			game->field->blocks[coordY][coordX].color = 0;
			game->field->blocks[coordY][coordX].type = 0;
		}
		else {
			game->field->blocks[coordY][coordX].color = game->mino->color;
			game->field->blocks[coordY][coordX].type = 1;
		}
	}
}

// My realization of level system. Logic of increasing game speed
void levelSystem(Game* game) {
	if (game->level == 0 && game->lines > 0) {
		game->level++;
		game->prevLines = game->lines;
		game->tickRate = 35;
	}
	else if (game->level > 0 && game->level < 6 && game->lines - game->prevLines > 2) {
		game->level++;
		game->prevLines = game->lines;
		game->tickRate -= 1;
	}
	else if (game->level > 5 && game->level < 11 && game->lines - game->prevLines > 4) {
		printf("Hello1\n");
		game->level++;
		game->prevLines = game->lines;
		game->tickRate -= 1;
	}
	else if (game->level > 10 && game->level < 16 && game->lines - game->prevLines > 6) {
		printf("Hello2\n");
		game->level++;
		game->prevLines = game->lines;
		game->tickRate -= 1;
	}
	else if (game->level > 15 && game->level < 21 && game->lines - game->prevLines > 9) {
		printf("Hello3\n");
		game->level++;
		game->prevLines = game->lines;
		game->tickRate -= 1;
	}
}

// Checking lines. Inefficient AF but works great. 
void checkLines(Game* game) {
	if (game->gameState == MINO_ON_FIELD) return;
	int lines = 0;
	for (int i = 0; i < HEIGHT; i++) {
		int isLine = 1;
		for (int j = 0; j < WIDTH; j++) {
			if (game->field->blocks[i][j].type == 0) {
				isLine = 0;
				break;
			}
		}
		if (isLine) {
			lines++;
			for (int j = 0; j < WIDTH; j++) {
				game->field->blocks[i][j].type = 0;
				game->field->blocks[i][j].color = 0;
			}
			for (int a = i; a > 0; a--) {
				for (int b = 0; b < WIDTH; b++) {
					game->field->blocks[a][b].type = game->field->blocks[a - 1][b].type;
					game->field->blocks[a][b].color = game->field->blocks[a - 1][b].color;
					game->field->blocks[a - 1][b].type = 0;
					game->field->blocks[a - 1][b].color = 0;
				}
			}
		}
	}
	if (lines == 1) game->score += 40;
	else if (lines == 2) game->score += 100;
	else if (lines == 3) game->score += 300;
	else if (lines >= 4) {
		game->score += 1200;
		game->akhmetris += 1;
	}
	game->lines += lines;
	levelSystem(game);
}

// Checks if something is under the mino
int collision(Game* game) {
	int collides = 0;
	for (int i = 0; i < game->mino->size; i++) {
		int coordX = game->mino->blocks[i].coordX;
		int coordY = game->mino->blocks[i].coordY;
		if (coordY == HEIGHT - 1 || game->field->blocks[coordY + 1][coordX].type == 2) {
			collides = 1;
			break;
		}
	}
	if (collides) {
		for (int i = 0; i < game->mino->size; i++) {
			int coordX = game->mino->blocks[i].coordX;
			int coordY = game->mino->blocks[i].coordY;
			game->field->blocks[coordY][coordX].type = 2;
		}
		deleteTetramino(game->mino);
		game->gameState = WAITING_FOR_ACTION;
		if (game->holdState == HOLD_PRESSED) game->holdState = HOLD_NOT_PRESSED;
	}
	return collides;
}

// Let's us choose the right type of mino
Tetramino* chooseTetramino(int color, int rotation) {
	Tetramino* mino = (Tetramino*)malloc(sizeof(Tetramino));
	srand(time(NULL));
	if (color == 0) {
		color = rand() % 7 + 1;
	}
	switch (color) {
	case MINO_I:
		mino = createTetramino(tetroI_temp, 3, MINO_I, 2, rotation);
		break;
	case MINO_J:
		mino = createTetramino(tetroJ_temp, 3, MINO_J, 4, rotation);
		break;
	case MINO_L:
		mino = createTetramino(tetroL_temp, 3, MINO_L, 4, rotation);
		break;
	case MINO_O:
		mino = createTetramino(tetroO_temp, 4, MINO_O, 1, rotation);
		break;
	case MINO_T:
		mino = createTetramino(tetroT_temp, 3, MINO_T, 4, rotation);
		break;
	case MINO_S:
		mino = createTetramino(tetroS_temp, 3, MINO_S, 2, rotation);
		break;
	case MINO_Z:
		mino = createTetramino(tetroZ_temp, 3, MINO_Z, 2, rotation);
		break;
	default:
		break;
	}
	return mino;
}

//  Add new mino when previous is stacked
void placeNewMino(Game* game, int color) {
	game->mino = chooseTetramino(color, 0);
	for (int i = 0; i < game->mino->size; i++) {
		int coordX = game->mino->blocks[i].coordX;
		int coordY = game->mino->blocks[i].coordY;
		if (game->field->blocks[coordY][coordX].type == 2) {
			game->gameState = GAME_OVER;
			return;
		}
	}
	game->gameState = 2;
	drawMino(game, DRAW_MINO);
}

// Moves mino one block down
void moveMinoDown(Game* game) {
	for (int i = 0; i < game->mino->size; i++) {
		int coordX = game->mino->blocks[i].coordX;
		int coordY = game->mino->blocks[i].coordY;
		game->field->blocks[coordY][coordX].color = 0;
		game->field->blocks[coordY][coordX].type = 0;
		game->mino->blocks[i].coordY++;
	}
	drawMino(game, DRAW_MINO);
}

// Rotation system. I tried to do it as close to original as possible
void rotateMino(Game* game) {
	// If Tetramino has no rotations temps ignore rotation
	if (game->mino->temps == 1) return;
	int minCoordY = HEIGHT;
	int minCoordX = WIDTH;
	for (int i = 0; i < game->mino->size; i++) {
		if (game->mino->blocks[i].coordX < minCoordX) {
			minCoordX = game->mino->blocks[i].coordX;
		}
		if (game->mino->blocks[i].coordY < minCoordY) {
			minCoordY = game->mino->blocks[i].coordY;
		}
	}
	int rotation = game->mino->rotation;
	// Creating temporal mino to check if we can place it on the field
	Tetramino* checkMino = (Tetramino*)malloc(sizeof(Tetramino));
	switch (rotation) {
	case 0:
		checkMino = chooseTetramino(game->mino->color, 1);
		for (int i = 0; i < checkMino->size; i++) {
			checkMino->blocks[i].coordX += (minCoordX - game->mino->startCoord + 1);
			checkMino->blocks[i].coordY += minCoordY;
		}
		rotation = 1;
		break;
	case 1:
		checkMino = chooseTetramino(game->mino->color, (game->mino->temps) - 2);
		for (int i = 0; i < checkMino->size; i++) {
			checkMino->blocks[i].coordX += (minCoordX - game->mino->startCoord - 1);
			checkMino->blocks[i].coordY += minCoordY + 1;
		}
		rotation = game->mino->temps - 2;
		break;
	case 2:
		checkMino = chooseTetramino(game->mino->color, 3);
		for (int i = 0; i < checkMino->size; i++) {
			checkMino->blocks[i].coordX += (minCoordX - game->mino->startCoord);
			checkMino->blocks[i].coordY += minCoordY - 1;
		}
		rotation = 3;
		break;
	case 3:
		checkMino = chooseTetramino(game->mino->color, 0);
		for (int i = 0; i < checkMino->size; i++) {
			checkMino->blocks[i].coordX += (minCoordX - game->mino->startCoord);
			checkMino->blocks[i].coordY += minCoordY;
		}
		rotation = 0;
		break;
	default:
		break;
	}
	for (int i = 0; i < checkMino->size; i++) {
		int coordX = checkMino->blocks[i].coordX;
		int coordY = checkMino->blocks[i].coordY;
		if (game->field->blocks[coordY][coordX].type == 2) return;
		if (coordX >= WIDTH || coordX < 0 || coordY >= HEIGHT || coordY < 0) return;
	}
	// If we can place mino makes temporal mino our main mino
	drawMino(game, DELETE_MINO);
	for (int i = 0; i < game->mino->size; i++) {
		game->mino->blocks[i].coordX = checkMino->blocks[i].coordX;
		game->mino->blocks[i].coordY = checkMino->blocks[i].coordY;
		game->mino->rotation = rotation;
	}
	deleteTetramino(checkMino);
	drawMino(game, DRAW_MINO);
}

// Moves mino right or left
void moveMinoHorizontally(Game* game, int right_or_left) {
	for (int i = 0; i < game->mino->size; i++) {
		int coordX = game->mino->blocks[i].coordX;
		int coordY = game->mino->blocks[i].coordY;
		// If something interfers, ignore moving
		if (right_or_left) {
			if ((coordX + 1) == WIDTH || game->field->blocks[coordY][coordX + 1].type == 2) {
				return;
			}
		}
		else {
			if (coordX == 0 || game->field->blocks[coordY][coordX - 1].type == 2) {
				return;
			}
		}
	}
	// Removes previous location of mino (idk why it won't work without it, it leaves trail so...)
	for (int i = 0; i < game->mino->size; i++) {
		int coordX = game->mino->blocks[i].coordX;
		int coordY = game->mino->blocks[i].coordY;
		game->field->blocks[coordY][coordX].color = 0;
		game->field->blocks[coordY][coordX].type = 0;
		if (right_or_left) {
			game->mino->blocks[i].coordX++;
		}
		else {
			game->mino->blocks[i].coordX--;
		}
	}
	drawMino(game, DRAW_MINO);
}

// Instantly drops mino down
void dropMino(Game* game) {
	if (game->gameState != 2) return;
	while (!collision(game)) {
		moveMinoDown(game);
	}
}

// Realization of hold from original game
void holdMino(Game* game) {
	if (game->gameState != MINO_ON_FIELD) return;
	if (game->holdState == HOLD_PRESSED) return;
	else if (game->holdState == NEW_HOLD) {
		game->hold = chooseTetramino(game->mino->color, 0);
		game->holdState = HOLD_PRESSED;
		drawMino(game, DELETE_MINO);
		deleteTetramino(game->mino);
		placeNewMino(game, NO_MINO);
	} 
	else {
		int temporalMinoType = game->mino->color;
		drawMino(game, DELETE_MINO);
		game->mino = game->hold;
		drawMino(game, DRAW_MINO);
		game->hold = chooseTetramino(temporalMinoType, 0);
		game->holdState = HOLD_PRESSED;
	}
}

// Logic of our game. How it behaves if different situations
void gameTick(Game* game, int mode) {
	int tickRate = game->tickRate;
	if (mode == NO_TICK) tickRate = 5;
	if (game->tick % tickRate == 0) {
		// printf("Logic tick: %d, %d\n", game->tick, game->tickRate); // DEBUG
		if (game->gameState == WAITING_FOR_ACTION) {
			checkLines(game);
			placeNewMino(game, NO_MINO);
		}
		else if (game->gameState == MINO_ON_FIELD) {
			if (!collision(game)) moveMinoDown(game);
		}
	}
	game->tick++;
}

int main() {
	InitWindow(485, 600, "Akhmetris");
	SetTargetFPS(60);
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	Game* game = createGame();
	int savedState = 1;
	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(CLITERAL(Color) {30, 30, 30, 255});
		GetFontDefault();

		if (game->gameState == GAME_PAUSED) {
			DrawText("ENTER - Start/continue game\nTAB - Menu\nECS - Close game", 30, 100, 25, RAYWHITE);
			DrawText("UP - Rotate\nDOWN - Drop instantly\nLEFT, RIGHT - Move left or right\nSPACE - Hold\nLEFT SHIFT - Speed up", 30, 300, 25, GRAY);
			if (IsKeyPressed(KEY_ENTER)) game->gameState = savedState;
		}
		else if (game->gameState == GAME_OVER) {
			DrawText("GAME OVER...", 30, 100, 60, RAYWHITE);
			DrawText(TextFormat("SCORE:\t%d", game->score), 30, 230, 25, GRAY);
			DrawText(TextFormat("LINES:\t%d", game->lines), 30, 260, 25, GRAY);
			DrawText(TextFormat("TETRIS:\t%d", game->akhmetris), 30, 290, 25, GRAY);
			DrawText(TextFormat("LEVEL:\t%d", game->level), 30, 320, 25, GRAY);
			DrawText("ENTER - Start/continue game\nECS - Close game", 30, 400, 25, RAYWHITE);
			if (IsKeyPressed(KEY_ENTER)) {
				CloseWindow();
				main();
			}
		}
		else {
			DrawRectangle(50, 50, 250, 500, RAYWHITE);
			DrawRectangle(325, 75, 110, 110, RAYWHITE);
			DrawText("HOLD", 350, 50, 25, RAYWHITE);
			DrawText(TextFormat("SCORE:\n%d", game->score), 325, 225, 25, GRAY);
			DrawText(TextFormat("LINES:\n%d", game->lines), 325, 300, 25, GRAY);
			DrawText(TextFormat("TETRIS:\n%d", game->akhmetris), 325, 375, 25, GRAY);
			DrawText(TextFormat("LEVEL:\n%d", game->level), 325, 450, 25, GRAY);

			if (game->holdState != NEW_HOLD) {
				int startCoordX, startCoordY;
				if (game->hold->color == MINO_I) {
					startCoordX = 330; startCoordY = 120;
				}
				else if (game->hold->color == MINO_O) {
					startCoordX = 355; startCoordY = 105;
				} 
				else {
					startCoordX = 342; startCoordY = 105;
				}
				for (int i = 0; i < game->hold->size; i++) {
					int coordX = game->hold->blocks[i].coordX;
					int coordY = game->hold->blocks[i].coordY;
					int startCoord = game->hold->startCoord;
					DrawRectangle(startCoordX + 25 * (coordX - startCoord), startCoordY + 25 * coordY, 25, 25, minoColor(game->hold->color));
				}
			}

			if (IsKeyPressed(KEY_DOWN)) dropMino(game);
			if (IsKeyPressed(KEY_UP)) rotateMino(game);
			if (IsKeyPressed(KEY_SPACE)) holdMino(game);
			if (IsKeyPressed(KEY_RIGHT)) moveMinoHorizontally(game, MINO_RIGHT);
			if (IsKeyPressed(KEY_LEFT)) moveMinoHorizontally(game, MINO_LEFT);
			if (IsKeyPressed(KEY_TAB)) {
				savedState = game->gameState;
				game->gameState = GAME_PAUSED;
			}

			for (int i = 0; i < HEIGHT; i++) {
				for (int j = 0; j < WIDTH; j++) {
					if (game->field->blocks[i][j].type != 0) {
						DrawRectangle(50 + (j * 25), 50 + (i * 25), 25, 25, minoColor(game->field->blocks[i][j].color));
					}
				}
			}

			if (IsKeyDown(KEY_RIGHT_SHIFT)) gameTick(game, NO_TICK);
			else gameTick(game, DEFAULT);
		}

		EndDrawing();
	}
	CloseWindow();
	return 0;
}