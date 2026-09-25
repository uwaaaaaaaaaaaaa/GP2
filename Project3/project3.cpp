//めんどいセキュリティ警告を無視
#define _CRT_SECURE_NO_WARNINGS
//1
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <vector>
#include <time.h>
#include <string.h>
//2
#define BOARD_WIDTH		(8)
#define BOARD_HEIGHT	(8)
#define MAX_MEMORIES 1000	//AIが覚えられる最大の盤面数
#define MAX_TURNS 100		//AIが覚えられる最大のターン数
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
	"●",
	"○",
	"・"
};

//AI用データもろもろ
typedef struct {
	char state[65];		//盤面の文字列
	double qValues[64];	//盤面の各マスに置いた時の重み
	int visitCount;		//特定の盤面に来た回数
} BoardMemory;

//AIの記憶データ（Qテーブル）
BoardMemory qTable[MAX_MEMORIES];
int qTableCount = 0;	//現在記憶している盤面の数

typedef struct {
	int memIndex;		//盤面の記憶インデックス
	int moveIndex;		//置いた場所
	int player;			//誰のターンだったか
} MoveHistory;

//AIの記憶データ（ターン）
MoveHistory gameHistory[MAX_TURNS];
int historyCount = 0;

enum {
	DIRECTION_UP,
	DIRECTION_UP_LEFT,
	DIRECTION_LEFT,
	DIRECTION_DOWN_LEFT,
	DIRECTION_DOWN,
	DIRECTION_DOWN_RIGHT,
	DIRECTION_RIGHT,
	DIRECTION_UP_RIGHT,
	DIRECTION_MAX,
};

VEC2 directions[DIRECTION_MAX]{
	{0, -1},
	{-1, -1},
	{-1, 0},
	{-1, 1},
	{0, 1},
	{1, 1},
	{1, 0},
	{1, -1},
};

enum {
	MODE_1P,
	MODE_2P,
	MODE_WATCH_AI,
	MODE_TRAIN_RANDOM,
	MODE_MAX
};

const char* modeNames[] = {
	"1P GAME",
	"2P GAME",
	"AI vs AI",
	"AI vs RANDOM(TRAIN)",
};

VEC2 cursorPosition;

int board[BOARD_HEIGHT][BOARD_WIDTH];

int turn;

const char* turnNames[] = {
	"黒",
	"白"
};

int mode;

bool isPlayer[TURN_MAX];

//6

int GetDiskCount(int _color) {
	int count = 0;
	for (int y = 0; y < BOARD_HEIGHT; y++) {
		for (int x = 0; x < BOARD_WIDTH; x++) {
			if (board[y][x] == _color) {
				count++;
			}
		}
	}
	return count;
}

VEC2 VecAdd(VEC2 _v0, VEC2 _v1) {
	return{
		_v0.x + _v1.x,
		_v0.y + _v1.y,
	};
}
void DrawScreen() {
	system("cls");
	for (int y = 0; y < BOARD_HEIGHT; y++) {
		for (int x = 0; x < BOARD_WIDTH; x++) {
			printf("%s", diskAA[board[y][x]]);
		}
	if(isPlayer[turn]){
			if (y == cursorPosition.y) {
				printf("←");
			}
	}
			printf("\n");
	}
	for (int x = 0; x < BOARD_WIDTH; x++) {
			if(isPlayer[turn]){
				if (x == cursorPosition.x) {
					printf("↑");
				}
				else {
					printf("　");
				}
			}
	}
			printf("\n");
	if (turn != TURN_NONE) {
		printf("%sのターンです\n", turnNames[turn]);
	}
	else {
		int blackCount = GetDiskCount(TURN_BLACK);
		int whiteCount = GetDiskCount(TURN_WHITE);
		int winner;
		if (blackCount > whiteCount) {
			winner = TURN_BLACK;
		}
		else if (blackCount < whiteCount) {
			winner = TURN_WHITE;
		}else{
			winner = TURN_NONE;
		}
		printf("%s%d-%s%d	", turnNames[TURN_BLACK], GetDiskCount(TURN_BLACK), turnNames[TURN_WHITE], GetDiskCount(TURN_WHITE));
		if (winner == TURN_NONE) {
			printf("引き分け\n");
		}
		else {
			printf("%sの勝ち！\n", turnNames[winner]);
		}
	}
}

bool CheckCanPlace(int _color, VEC2 _position, bool _turnOver = false) {
	bool canPlace = false;
	if (board[_position.y][_position.x] != TURN_NONE) {
		return false;
	}
	int opponent = _color ^ 1;
	for (int i = 0; i < DIRECTION_MAX; i++) {
		VEC2 currentPosition = _position;
		currentPosition = VecAdd(currentPosition, directions[i]);
		if ((currentPosition.x < 0) || (currentPosition.x >= BOARD_WIDTH) ||
			(currentPosition.y < 0) || (currentPosition.y >= BOARD_HEIGHT)) {
			continue;
		}
		if (board[currentPosition.y][currentPosition.x] != opponent) {
			continue;
		}
		while (1) {
			currentPosition = VecAdd(currentPosition, directions[i]);
		if ((currentPosition.x < 0) || (currentPosition.x >= BOARD_WIDTH) || (currentPosition.y < 0) || (currentPosition.y >= BOARD_HEIGHT)) {
			break;
		}
		if (board[currentPosition.y][currentPosition.x] == TURN_NONE) {
			break;
		}
		if (board[currentPosition.y][currentPosition.x] == _color) {
			canPlace = true;
			if (_turnOver) {
				VEC2 reversePosition = _position;
				reversePosition = VecAdd(reversePosition, directions[i]);
				do {
					board[reversePosition.y][reversePosition.x] = _color;
					reversePosition = VecAdd(reversePosition, directions[i]);
				} while (board[reversePosition.y][reversePosition.x] != _color);
			}
		}
		}
	}
	return canPlace;
}

VEC2 InputPosition() {
	while (true)
	{
		DrawScreen();
		switch (_getch())
		{
		case 'w':
			cursorPosition.y--;
			break;
		case 's':
			cursorPosition.y++;
			break;
		case 'a':
			cursorPosition.x--;
			break;
		case 'd':
			cursorPosition.x++;
			break;
		default:
			if(CheckCanPlace(turn, cursorPosition)){
				return cursorPosition;
			}
			else {
				printf("そこはおけません");
				_getch();
			}
			break;
		}
		cursorPosition.x = (BOARD_WIDTH + cursorPosition.x) % BOARD_WIDTH;
		cursorPosition.y = (BOARD_HEIGHT + cursorPosition.y) % BOARD_HEIGHT;
	}
}

bool CheckCanPlaceAll(int _color) {
	for (int y = 0; y < BOARD_HEIGHT; y++) {
		for (int x = 0; x < BOARD_WIDTH; x++) {
			VEC2 position = { x, y };
			if (CheckCanPlace(_color, position)) {
				return true;
			}
		}
	}
	return false;
}

void Init() {
	for (int y=0;y<BOARD_HEIGHT;y++) {
		for (int x = 0; x < BOARD_WIDTH; x++) {
			board[y][x] = TURN_NONE;
		}
	}
	board[4][3] = board[3][4] = TURN_BLACK;
	board[3][3] = board[4][4] = TURN_WHITE;
	turn = TURN_BLACK;
	cursorPosition = { 3, 3 };
	DrawScreen();
}

void SelectMode() {
	mode = MODE_1P;
	while (1) {
		system("cls");
		for (int i = 0; i < MODE_MAX; i++) {
			printf("%s", (i == mode) ? ">" : " ");
			printf("%s\n", modeNames[i]);
			printf("\n");
		}
		switch (_getch())
		{
		case 'w':
			mode--;
			break;
		case 's':
			mode++;
			break;
		default:
			switch (mode)
			{
			case MODE_1P:
				isPlayer[TURN_BLACK] = true;
				isPlayer[TURN_WHITE] = false;
				break;
			case MODE_2P:
				isPlayer[TURN_BLACK] = isPlayer[TURN_WHITE] = true;
				break;
			case MODE_WATCH_AI:	//AI学習用モード
				isPlayer[TURN_BLACK] = false;
				isPlayer[TURN_WHITE] = false;
				break;
			case MODE_TRAIN_RANDOM:
				isPlayer[TURN_BLACK] = false;
				isPlayer[TURN_WHITE] = false;
				break;
			}
			return;
		}
		mode = (MODE_MAX + mode) % MODE_MAX;
	}
}

//蓄積型AI用関数
//盤面の状態を64文字の文字列に変換
void GetBoardStateString(char* _outStr) {
	int index = 0;
	for (int y = 0; y < BOARD_HEIGHT; y++) {
		for (int x = 0; x < BOARD_WIDTH; x++) {
			//boardの値を文字0,1,2に変換
			_outStr[index] = '0' + board[y][x];
			index++;
		}
	}
	_outStr[64] = '\0';//配列の終わりを決めていないのでここでヌルを入れる
}

//盤面文字列から記憶のインデックスを探す関数（見つからないときは新しく保存する）
int GetOrAddMemoryIndex(const char* _stateStr) {
	//すでに記憶があるか探す
	for (int i = 0; i < qTableCount; i++) {
		if (strcmp(qTable[i].state, _stateStr) == 0) {
			return i;	//発見したらその番号を返す
		}
	}

	//なければ新しく登録する
	if (qTableCount < MAX_MEMORIES) {
		int newIdx = qTableCount;
		strcpy(qTable[newIdx].state, _stateStr);
		for (int i = 0; i < 64; i++) {
			qTable[newIdx].qValues[i] = 0.0;//重みの初期値はすべて０
		}
		qTable[newIdx].visitCount = 0;
		qTableCount++;
		return newIdx;
	}
	//記憶容量が満タンなときは仮で０を返す
	return 0;
}

//Qテーブルをファイルから読み込む
void LoadQTable() {
	FILE* fp = fopen("q_table.dat", "rb");
	if (fp == NULL) return;//ファイルがなければスキップ
	fread(&qTableCount, sizeof(int), 1, fp);
	fread(qTable, sizeof(BoardMemory), qTableCount, fp);
	fclose(fp);
}

//Qテーブルをファイルに保存する
void SaveQTable() {
	FILE* fp = fopen("q_table.dat", "wb");
	if (fp == NULL) return;
	fwrite(&qTableCount, sizeof(int), 1, fp);
	fwrite(qTable, sizeof(BoardMemory), qTableCount, fp);
	fclose(fp);

}

//ゲーム終了時に勝敗結果からQ値を更新する
void LearnFromGame(int winner) {
		double alpha = 0.0;	//学習率（新しい結果をどれくらい反映させるか）
	if(mode == MODE_WATCH_AI || MODE_TRAIN_RANDOM){
		alpha = 0.2;
	}
	else {
		alpha = 0.35;	//プレイヤー戦の場合はもう少し重くする
	}

	for (int i = 0; i < historyCount; i++) {
		int mIdx = gameHistory[i].memIndex;
		int posIdx = gameHistory[i].moveIndex;
		int p = gameHistory[i].player;

		//報酬の設定
		double reward = 0.0;
		if (winner == p) {
			reward == 1.0;	//勝ち
		}
		else if (winner == TURN_NONE) {
			reward == 0.0;	//引き分け
		}
		else {
			reward == -1.0;	//負け
		}

		double currentQ = qTable[mIdx].qValues[posIdx];
		qTable[mIdx].qValues[posIdx] += alpha * (reward - currentQ);
	}

	//次のゲームのために履歴をリセット
	historyCount = 0;

	//ファイルに保存
	SaveQTable();
}

//AIの手を決定する関数
VEC2 SelectAiMove(int _color) {
	//現在の盤面から、記憶テーブルのインデックスを取得
	char stateStr[65];
	GetBoardStateString(stateStr);
	int memIdx = GetOrAddMemoryIndex(stateStr);

	//置ける場所をリストアップ
	VEC2 vaildMoves[64];
	int validCount = 0;
	for (int y = 0; y < BOARD_HEIGHT; y++) {
		for (int x = 0; x < BOARD_WIDTH; x++) {
			VEC2 pos = { x, y };
			if (CheckCanPlace(_color, pos)) {
				vaildMoves[validCount] = pos;
				validCount++;
			}
		}
	}
	//置ける場所がなければ適当な値を返す
	if (validCount == 0) {
		VEC2 dummy = { 0, 0 };
		return dummy;
	}

	//行動選択（２０％→ランダム探索、８０％→記憶を利用）
	int epsilon = 20;//探索確立
	if ((rand() % 100) < epsilon) {
		//置ける場所から完全にランダムでえらぶ（探索）
		return vaildMoves[rand() % validCount];
	}
	else {
		//置ける場所のなかで一番Q値が高い場所を選ぶ
		int bestIdx = 0;
		double maxQ = -999999.0;	//初めはものすごく小さな値にしておく

		for (int i = 0; i < validCount; i++) {
			VEC2 pos = vaildMoves[i];
			int boardIdx = pos.y * BOARD_WIDTH + pos.x;		//(x,y)を0~63の１次元インデックスに変換
			double q = qTable[memIdx].qValues[boardIdx];

			if (q > maxQ) {
				maxQ = q;
				bestIdx = i;
			}
		}
		return vaildMoves[bestIdx];
	}
}
//なんかもうめんどくさくなってコピペ
int main() {
	LoadQTable();	// 過去の学習データの読み出し
start:
	historyCount = 0;	// リセット
	srand((unsigned int)time(NULL));
	SelectMode();
	Init();

	// ★ 修正点1: TRAIN_RANDOM モードの時だけ AIの黒/白 をランダム決定する
	if (mode == MODE_1P || mode == MODE_TRAIN_RANDOM) {
		bool playerIsBlack = (rand() % 2 == 0); // 50%の確率で人間（またはランダムCPU）が黒

		// true = 人間（またはランダムCPU）、false = AI思考
		isPlayer[TURN_BLACK] = playerIsBlack;
		isPlayer[TURN_WHITE] = !playerIsBlack;
	}

	while (1) {
		// 現在のturnに置ける場所があるか確認
		if (!CheckCanPlaceAll(turn)) {
			turn ^= 1;
			if (!CheckCanPlaceAll(turn)) {
				// 両者とも置けない→終了
				turn = TURN_NONE;
				DrawScreen();

				// 勝者を判定してAIに学習させる
				int blackCount = GetDiskCount(TURN_BLACK);
				int whiteCount = GetDiskCount(TURN_WHITE);
				int winner = TURN_NONE;
				if (blackCount > whiteCount) winner = TURN_BLACK;
				else if (blackCount < whiteCount) winner = TURN_WHITE;

				LearnFromGame(winner);	// 勝敗から学習してファイル保存
				_getch();
				goto start;
			}
			continue; // 片方だけ置けない→パスして次のループへ
		}

		VEC2 placePosition;
		bool isAiTurn = false; // ★ 今回の手が「AI思考」によるものかを記録するフラグ

		// ★ 修正点2: 人間が操作するモード（1P/2P）のときは素直にInputPositionへ
		if (mode == MODE_1P || mode == MODE_2P) {
			if (isPlayer[turn]) {
				placePosition = InputPosition();
			}
			else {
				// 1PモードのCPU（AI）のターン
				DrawScreen();
				_getch();
				placePosition = SelectAiMove(turn);
				isAiTurn = true; // AIの手
			}
		}
		else {
			// --- AI vs AI(WATCH) または AI vs RANDOM(TRAIN) モードの処理 ---

			// 画面描画の制御
			if (mode != MODE_WATCH_AI && mode != MODE_TRAIN_RANDOM) {
				DrawScreen();
				_getch();
			}
			else {
				DrawScreen(); // 高速化したい場合はこの描画をコメントアウト
			}

			// 特訓モードで isPlayer が true（ランダム担当）の場合
			if (mode == MODE_TRAIN_RANDOM && isPlayer[turn]) {
				// 完全ランダムで打つ処理
				VEC2 validMoves[64];
				int validCount = 0;
				for (int y = 0; y < BOARD_HEIGHT; y++) {
					for (int x = 0; x < BOARD_WIDTH; x++) {
						VEC2 pos = { x, y };
						if (CheckCanPlace(turn, pos)) {
							validMoves[validCount] = pos;
							validCount++;
						}
					}
				}
				placePosition = validMoves[rand() % validCount];
			}
			else {
				// AI（Q学習）で打つ処理
				placePosition = SelectAiMove(turn);
				isAiTurn = true; // AIの手
			}
		}

		// ★ 修正点3: 「AIが打った手」のときだけ履歴に記録する
		if (isAiTurn && historyCount < MAX_TURNS) {
			char stateStr[65];
			GetBoardStateString(stateStr);
			gameHistory[historyCount].memIndex = GetOrAddMemoryIndex(stateStr);
			gameHistory[historyCount].moveIndex = placePosition.y * BOARD_WIDTH + placePosition.x;
			gameHistory[historyCount].player = turn;
			historyCount++;
		}

		CheckCanPlace(turn, placePosition, true);
		board[placePosition.y][placePosition.x] = turn;
		turn ^= 1; // ★1手ごとに必ず1回だけ反転
	}
}