#include <stdio.h>
#include "cprocessing.h"
#include <stdbool.h>
#include <math.h>

#define Black CP_Color_Create(0, 0, 0, 255)
#define White CP_Color_Create(255, 255, 255, 255)
#define Blue  CP_Color_Create(0, 0, 255, 255)
#define Red  CP_Color_Create(255, 0, 0, 255)
#define Green CP_Color_Create(0, 255, 0, 255)
#define gravity CP_Vector_Set(0, 1000)
#define jump CP_Vector_Set(0, -550)
#define left CP_Vector_Set(-400, 0)
#define right CP_Vector_Set(400, 0)
#define dash CP_Vector_Set(750, 0)
#define maxjumpcount 1
#define maxdashcount 1
#define maxdashtimer 200

//Player variables
float player_size = 20;
int jumpcount, dashcount, dashtimer;
CP_Vector player, movementH, movementV;
bool InAir, shift, holdshift = false, keyright, keyleft, dashcooldown, dashright, dashleft, keyjump, reset, deathstate = false, control = false;
CP_Vector* pointer = &player;
enum DeadOrALive { alive, dead };
bool has_crossline = false;
int deathcounter = 0;

//Door variables
bool ButtonDetection, Vdoordetect, Hdoordetect;
CP_Vector door, doorsize, button, doorlength, test, hdoormove, vdoormove;

//Enemy Patrolling
#define enemyright CP_Vector_Set(100, 0)
#define enemyleft CP_Vector_Negate(enemyright)
typedef struct EnemyPatrols
{
    CP_Vector enemymovement;
    CP_Vector enemy_pos;
} EnemyPatrols;
EnemyPatrols Enemy1, Enemy2, Enemy3, Enemy4;

//Turret Shooting & spike trap
CP_Vector spikecollision;
bool active_laser = true;

typedef struct NewTurrets
{
    CP_Vector VerticalTurretProjectile;
} NewTurrets;
NewTurrets Newturret1, Newturret2, Newturret3, Newturret4, Newturret5, Newturret6, Newturret7, Newturret8, Newturret9, Newturret10;

float TurretWidth;
float TurretHeight;
CP_Vector Projectilespeed;

//Laser Trap
typedef struct Lasers
{
    float laserheight;
    float intervaltime;
} Lasers;
Lasers Laser1, Laser2, Laser3;

//GUI things
#define ButtonWidth 150.f
#define ButtonHeight 50.f
#define WindowWidth (float)CP_System_GetWindowWidth()
#define WindowHeight (float)CP_System_GetWindowHeight()
enum GUI { Mainmenu, Stage1, Stage2, Stage3, SelectStage, Credits, Instruction, Pause, Unpause, CompleteStage, UncompleteStage, FailStage, muted, unmuted };
int gamestate, playerstate, pausestate, WinLoseState, mutestate = unmuted;

//Stage variables
float buffer = 7;
float rectX, rectY, rectW, rectH, buttonW, buttonH;
char buffer1[16] = { 0 };
char buffer2[16] = { 0 };
float timestart;
float endTimer = 0;
float pausedTimer = 0;
bool freeze = false;

//Digipen logo fade
int FadeState = 0;
float alpha = 0;

//Images
CP_Image StartButton = NULL;
CP_Image SelectStageButton = NULL;
CP_Image CreditsButton = NULL;
CP_Image Stage1Button = NULL;
CP_Image Stage2Button = NULL;
CP_Image Stage3Button = NULL;
CP_Image MainmenuButton = NULL;
CP_Image MainmenuBackground = NULL;
CP_Image BackgroundWord = NULL;
CP_Image LoseScreen = NULL;
CP_Image WinScreen = NULL;
CP_Image TryAgain = NULL;
CP_Image PauseScreen = NULL;
CP_Image ResumeButton = NULL;
CP_Image InGameBackground = NULL;
CP_Image InstructionsPage = NULL;
CP_Image InstructionButton = NULL;
CP_Image quitbutton = NULL;
CP_Image nextbutton = NULL;
CP_Image DigipenSGRED = NULL;
CP_Image Creditspage = NULL;

//Sound
CP_Sound MenuMusic = NULL;
CP_Sound StageMusic = NULL;
CP_Sound TurretSound = NULL;
CP_Sound JumpSound = NULL;
CP_Sound UI_Sound = NULL;
#define Menu_BGM CP_SOUND_GROUP_0
#define Stage_BGM CP_SOUND_GROUP_1
#define Sound_Effect CP_SOUND_GROUP_2 
#define Sound_UI CP_SOUND_GROUP_3  
#define Butt_Door CP_SOUND_GROUP_4
bool doormusic = false;
float vol = 0;