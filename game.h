#define GAME_NAME "BeautyDrive"

enum {
	CMD_ACCEL,
	CMD_BRAKE,
	CMD_LEFT,
	CMD_RIGHT,
	CMD_QUIT
};
void Game_Init ();
void Game_Shutdown ();
void Game_Frame ();
