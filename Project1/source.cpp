//[1]ヘッダーをインクルードする
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
//[2]定数を定義する場所
#define SPELL_COST 3
#define MAP_WIDTH	16
#define MAP_HEIGHT	16
#define SCREEN_WIDTH 16
#define SCREEN_HEIGHT 12

//[3]列挙定数を定義する場所
//[3-1]モンスターの種類を定義する
enum {
	MONSTER_PLAYER, //[3-1-1]プレイヤー
	MONSTER_SLIME, //[3-1-2]スライム
	MONSTER_BOSS,
	MONSTER_MAX		 //[3-1-3]モンスターの種類の数
};

//[3-2]キャラクターの種類を定義する
enum {
	CHARACTER_PLAYER, //[3-2-1]プレイヤー
	CHARACTER_MONSTER, //[3-2-2]モンスター
	CHARACTER_MAX		 //[3-2-3]キャラクターの種類の数
};

//[3-3]コマンドの種類を定義する
enum {
	COMMAND_FIGHT, //[3-3-1]戦う
	COMMAND_SPELL, //[3-3-2]呪文
	COMMAND_RUN, //[3-3-3]逃げる
	COMMAND_MAX		 //[3-3-4]コマンドの種類の数
};

//3-4
enum {
	MAP_FIELD,
	MAP_KING_CATSLE,
	MAP_BOSS_CATSLE,
	MAP_MAX,
};

//[4]構造体を定義する場所

//[4-1]キャラクターの構造体を定義する
typedef struct {
	int hp; //[4-1-1]HP
	int maxHp; //[4-1-2]最大HP
	int mp; //[4-1-3]MP
	int maxMp; //[4-1-4]最大MP
	int attack;
	char name[4 * 2 + 1]; //[4-1-5]名前
	char aa[256];
	int command; //[4-1-6]コマンド
	int target;
} CHARACTER;


//[5]変数を定義する場所
//[5-1]モンスターのステータスの配列を宣言する
CHARACTER monsters[MONSTER_MAX] = {
	//[5-1-1]プレイヤーのステータスを初期化する
	{
		100,		//[5-1-2]int hp			HP
		100,		//[5-1-3]int maxHp		最大HP
		15,		//[5-1-4]int mp			MP
		15,		//[5-1-5]int maxMp		最大MP
		50,		//攻撃力
		"勇者"	//[5-1-7]char name[4 * 2 + 1]		名前
	},

	//[5-1-8]MONSTER_SLIME　スライム
	{
		3,		//[5-1-9]int hp			HP
		3,		//[5-1-10]int maxHp		最大HP
		0,		//[5-1-11]int mp			MP
		0,		//[5-1-12]int maxMp		最大MP
		2,		//攻撃力
		"スライム", //[5-1-13]char name[4 * 2 + 1]	名前

		//[5-1-15]char aa[256]		アスキーアート
		"／・Д・＼\n"
		"～～～～～"
	},

	//[5-1-16]MONSTER_BOSS
	{
		255,
		255,
		0,
		0,
		50,
		"まおう",

		"     A @ A\n"
		" Ψ (▼皿▼)Ψ"
}
};

int currentMap;
int playerX = 6;
int playerY = 12;

//[5-2]キャラクターの配列を宣言する
CHARACTER characters[MONSTER_MAX];

//[5-3]コマンドの名前を宣言する
char commandNames[COMMAND_MAX][4 * 2 + 1] = {
	"たたかう", //[5-3-1]COMMAND_FIGHT
	"じゅもん", //[5-3-2]COMMAND_SPELL
	"にげる"  //[5-3-3]COMMAND_RUN
};

//5-4
char map[MAP_MAX][MAP_HEIGHT][MAP_WIDTH + 1] = {
	{
		"~~~~~~~~~~~~~~~~",
		"~~MMMMM~~MMMM.~~",
		"~M...M.##..M...~",
		"~M.M.M.~~M.M.M.~",
		"~M.M...~~M...M.~",
		"~M.MMMM~~MMMM..~",
		"~M..MM.~~~~~~#~~",
		"~~~~M.M.~~~~~#~~",
		"~~M.MM~~~~BMM..~",
		"~~...MM..M.MMM.~",
		"~...~~M~~M...M.~",
		"~..~~~KMMMMM.M.~",
		"~~~~.....M...M.~",
		"~...~~~..M.MM..~",
		"~~....~.......~~",
		"~~~~~~~~~~~~~~~~"
	},
	{
		"HHH.......HHH",
		"H.H..... .H.H",
		"HHHHHHHHHHHHH",
		".H.........H.",
		".H.HHH.HHH.H.",
		".H.H0H.H1H.H.",
		".H.........H.",
		".HW.......WH.",
		".HY.......YH.",
		"HHHHHH.HHHHHH",
		"H.H~~~#~~~H.H",
		"HHH~~~#~~~HHH",
		"............."
	},
	{
		"HHH.......HHH",
		"H.H.......H.H",
		"HHHHHHHHHHHHH",
		".H....H....H.",
		".H..WHHHW..H.",
		".H..YH2HY..H.",
		".H.........H.",
		".H..W...W..H.",
		".H..Y...Y..H.",
		".H.........H.",
		"HHHHHH.HHHHHH",
		"H.H~~~#~~~H.H",
		"HHH~~~#~~~HHH",
		"~~~~~~#~~~~~~",
		"~~~~~~#~~~~~~",
		"............."
	},
};
//[6]関数を宣言する場所

//[6-1]ゲームを初期化する関数を宣言する
void Init() {
	//[6-1-1]プレイヤーの配列を初期化する
	characters[MONSTER_PLAYER] = monsters[CHARACTER_PLAYER];
	currentMap = MAP_KING_CATSLE;

	playerX = 4;
	playerY = 6;
};

//[6-2]戦闘シーンを描画する関数を宣言する
void DrawBattleScreen() {
	//[6-2-1]画面をクリアする
	system("cls");
	//[6-2-2]プレイヤーの名前を表示する
	printf("%s\n", characters[CHARACTER_PLAYER].name);

	//[6-2-3]プレイヤーのステータスも表示する
	printf("HP:%d/%d MP:%d/%d\n", 
		characters[CHARACTER_PLAYER].hp, 
		characters[CHARACTER_PLAYER].maxHp, 
		characters[CHARACTER_PLAYER].mp, 
		characters[CHARACTER_PLAYER].maxMp);

	//[6-2-4]1行空ける
	printf("\n");

	//[6-2-5]モンスターのアスキーアートを表示する
	printf("%s\n", characters[CHARACTER_MONSTER].aa);

	//[6-2-6]モンスターのHPを表示する
	printf("%s (HP:%d/%d)\n", 
		characters[CHARACTER_MONSTER].name,
		characters[CHARACTER_MONSTER].hp, 
		characters[CHARACTER_MONSTER].maxHp);

	//[6-2-7]1行空ける
	printf("\n");
}

//[6-3]戦闘シーンのコマンドを表示する関数を宣言する
void SelectCommand() {
	characters[CHARACTER_PLAYER].command = COMMAND_FIGHT;
	//[6-3-2]コマンドが決定されるまでループする
	while (1) {
		//[6-3-3]戦闘画面を描画する関数を呼び出す
		DrawBattleScreen();
		//[6-3-4]コマンドの一覧を表示する
		for(int i = 0; i < COMMAND_MAX; i++) {
			//[6-3-5]選択中のコマンドから
			if (i == characters[CHARACTER_PLAYER].command) {
				//[6-3-6]カーソルを描画する
				printf(">");
			}
			//選択中のコマンドでなければ
			else {
				//[6-3-8]全角スペースを描画する
				printf(" ");
			}
			//[6-3-9]コマンドの名前を表示する
			printf("%s\n", commandNames[i]);
			
		}
		printf("%d", characters[CHARACTER_PLAYER].command);
		switch (_getch())
		{
		case 'w':
			characters[CHARACTER_PLAYER].command--;
			break;
		case's':
			characters[CHARACTER_PLAYER].command++;
			break;
		default:
			return;
		}
		characters[CHARACTER_PLAYER].command = (COMMAND_MAX + characters[CHARACTER_PLAYER].command) % COMMAND_MAX;

	}
}

//[6-4]戦闘シーンの関数を宣言する
void Battle(int _monster) {
	//[6-4-1]モンスターのステータスを初期化する
	characters[CHARACTER_MONSTER] = monsters[_monster];
	characters[CHARACTER_PLAYER].target = CHARACTER_MONSTER;
	characters[CHARACTER_MONSTER].target = CHARACTER_PLAYER;
	//[6-4-4]戦闘シーンの画面を描画する関数を呼び出す
	DrawBattleScreen();
	//[6-4-5]戦闘シーンの最初のメッセージを表示する
	printf("%sが　あらわれた！\n", characters[CHARACTER_MONSTER].name);
	//[6-4-6]キーボード入力を受け付ける
	_getch();
	//[6-4-7]戦闘が終了するまでループする
	while (1) {
		//[6-4-8]戦闘シーンのコマンドを表示する関数を呼び出す
		SelectCommand();
		//[6-4-9]各キャラクターを反復する
		for (int i = 0; i < CHARACTER_MAX; i++) {
			//[6-4-10]戦闘シーンの画面を描画する関数を呼び出す
			DrawBattleScreen();
			//[6-4-11]選択されたコマンドで分岐する
			switch (characters[i].command) 
			{
			case COMMAND_FIGHT: 					//[6-4-12]戦う
			{
				//[6-4-13]攻撃するメッセージを表示する
				printf("%sの こうげき！\n", characters[i].name);
				//[6-4-14]キーボード入力を受け付ける
				_getch();
				int damage = 1 + rand() % characters[i].attack;
				characters[characters[i].target].hp -= damage;
				if (characters[characters[i].target].hp < 0) {	//[6-4-17]
					characters[characters[i].target].hp = 0;
				}
				DrawBattleScreen();
				printf("%sに　%dのダメージ！\n", characters[characters[i].target].name, damage);
				_getch();
				break;
			}

			case COMMAND_SPELL: 					//[6-4-13]呪文
				if (characters[i].mp < SPELL_COST) {
					printf("MPがたりないぜ！！！\n");
					_getch();
					break;
				}
				characters[i].mp -= SPELL_COST;
				DrawBattleScreen();
				printf("%sはヒールをとなえた！\n", characters[i].name);
				_getch();

				characters[i].hp = characters[i].maxHp;
				DrawBattleScreen();

				printf("%sのきずがかいふくしたぜ！/n", characters[i].name);

				_getch();
				break;

			case COMMAND_RUN: 					//[6-4-14]逃げる
				printf("%sは　にげたぜ！！！\n", characters[i].name);
				_getch();
				return;
				break;

			default:
				break;
			}
			if (characters[characters[i].target].hp <= 0) {
				switch (characters[i].target)
				{
				case CHARACTER_PLAYER:	//[6-4-41]
					printf("しんだぜ！！！！");
					return;
					break;
				case CHARACTER_MONSTER: //[6-4-43]
					strcpy_s(characters[characters[i].target].aa, "\n");
					DrawBattleScreen();
					printf(" % sを　たおした！\n", characters[characters[i].target].name);
					return;
					break;
				}
				_getch();
			}
		}
	}
}

//6-5
void DrawMap() {
	system("cls");
	printf("%s\n", characters[CHARACTER_PLAYER].name);
	printf("HP:%d/%d MP:%d/%d\n", characters[CHARACTER_PLAYER].hp,
		characters[CHARACTER_PLAYER].maxHp, characters[CHARACTER_PLAYER].mp,
		characters[CHARACTER_PLAYER].maxMp);
	printf("\n");
	for (int y = playerY - SCREEN_HEIGHT / 2; y < playerY + SCREEN_HEIGHT / 2; y++) {
		for (int x = playerX - SCREEN_WIDTH / 2; x < playerX + SCREEN_WIDTH / 2; x++) {
			if ((x == playerX) && (y == playerY)) {
				printf("勇");
			}
			else if ((x < 0) || (x >= MAP_WIDTH) || (y<0) || (y >= MAP_HEIGHT) || (map[currentMap][y][x] == '\0')) {
				switch (currentMap)
				{
				case MAP_FIELD: printf("～"); break;
				case MAP_KING_CATSLE: printf(". "); break;
				case MAP_BOSS_CATSLE: printf(". "); break;
				}
			}
			else{
				switch (map[currentMap][y][x]) {
				case '~': printf("～"); break;
				case '.': printf("．"); break;
				case 'M': printf("Ｍ"); break;
				case '#': printf("＃"); break;
				case 'K': printf("王"); break;
				case 'B': printf("魔"); break;
				case 'H': printf("□"); break;
				case 'W': printf("炎"); break;
				case 'Y': printf("Ｙ"); break;
				case '0': printf("王"); break;
				case '1': printf("姫"); break;
				case '2': printf("魔"); break;
				}
			}
		}
		printf("\n");
	}
}

//[6-6]プログラムの実行開始点を宣言する
int main() {
	srand((unsigned int)time(NULL));

	//[6-6-2]ゲームを初期化する関数を呼び出す
	Init();

	//メインループ
	while (1) {
		
		DrawMap();
		int lastPlayerX = playerX;
		int lastPlayerY = playerY;
		switch (_getch()) {
		case'w': playerY--; break;
		case's': playerY++; break;
		case'a': playerX--; break;
		case'd': playerX++; break;
		}
		
		printf("\n");

		if((playerX < 0) || (playerX >= MAP_WIDTH)
			|| (playerY < 0) || (playerY >= MAP_HEIGHT)
			|| (map[currentMap][playerY][playerX] == '\0')) {
			switch (currentMap)
			{
			case MAP_KING_CATSLE: 
				currentMap = MAP_FIELD;

				playerX = 6;
				playerY = 12;
				break;
			case MAP_BOSS_CATSLE:
				currentMap = MAP_FIELD;
				
				playerX = 10;
				playerY = 9;
				break;
			}
		}

		switch (map[currentMap][playerY][playerX])
		{
		case 'K':
			currentMap = MAP_KING_CATSLE;

			playerX = 6;
			playerY = 12;
			break;
		case 'B':
			currentMap = MAP_BOSS_CATSLE;
			playerX = 6;
			playerY = 15;
			break;
		case '0':
			printf("*「おお、ゆうしゃよ！\n""ひがしの　まじょうの　まおうを\n""たおし　せかいを	すくってくれ！\n");
			_getch();
			break;
		case '1':
			printf("かみに　いのりをささげます。\n""おお　かみよ！\n""ゆうしゃさまに　しゅくふくを！\n");
			characters[CHARACTER_PLAYER].hp = characters[CHARACTER_PLAYER].maxHp;
			characters[CHARACTER_PLAYER].mp = characters[CHARACTER_PLAYER].maxMp;
			_getch();
			break;

		case '2':
			printf("*「おろかな　にんげんよ！\n""わが　やぼうを　はばむものは\n""このよから　けしさってくれる！\n");
			Battle(MONSTER_BOSS);
			_getch();
			if (characters[CHARACTER_MONSTER].hp <= 0) {
				system("cls");
				printf("まおうは　たおされた。\n""せかいは　めつぼうの　ききから　すくわれたが、\n""そのご　ゆうしゃを　みたものは　いないという\n""T H E  E N D");
				return 0;
			}
		break;
		}

		switch (map[currentMap][playerY][playerX])
		{
		case'.':
		case'#':
			if ((currentMap == MAP_FIELD) && (rand() % 16 == 0)) {
				Battle(MONSTER_SLIME);
			}
			break;
		default:
			playerX = lastPlayerX;
			playerY = lastPlayerY;
			break;
		}

		if (characters[CHARACTER_PLAYER].hp <= 0) {
			Init();
			DrawMap();

			printf("＊「おお　ゆうしゃよ！\n""かみが　そなたを　すくわれた！\n""ゆうしゃに　えいこうあれ！\n");
			_getch();
		}
	}
	
}