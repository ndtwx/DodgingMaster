/******************************************************************************************************************************************************
#  All content © 2021 DigiPen Institute of Technology Singapore, all rights reserved.                                                                 #
#  Academic Year: Trimester 1 Fall 2021                                                                                                               #
#  Team Name: Team Rocket                                                                                                                             #
#  Module CSD1400                                                                                                                                     #
# --------------------------------------------------------------------------------------------------------------------------------------------------- #
#  Ng Zuo Xian, Amadeus    - In charge of character movement code and the main collision logic between character and platform                         #
#  Eugene Ho Shu Sheng     - In charge of static trap design (spikes, lazer), dynamic projectiles (turret shooting projectiles) and art of the game   #
#  Jerel Peck Ler Hern     - In charge of UI, dynamic trap (Patrolling), first-level troubleshooting before integrating to main program               #
#  Andy Tang Wei Xian      - In charge of level design, sound effect and game sound music                                                             #
#  Cheang Wen Ding         - In charge of QOL functions such as time, number of death, bugfixing for integration to main program                      #
******************************************************************************************************************************************************/

#include <stdio.h>
#include "cprocessing.h"
#include <stdbool.h>
#include <math.h>
#include "game.h"

//general

//Main collision function between character with any platform
void wall(float wall_posX, float wall_posY, float wall_width, float wall_height)
{
	if (player.x > wall_posX - player_size && player.x < wall_posX + wall_width && player.y > wall_posY - player_size && player.y < wall_posY + wall_height)//within rect 
	{
		InAir = false;
		if (keyright == true && player.x > wall_posX - player_size && player.x < wall_posX + wall_width && player.y > wall_posY - player_size + buffer && player.y < wall_posY + wall_height - buffer) //entering from left
		{
			InAir = true;
			keyright = false;
			dashright = false;
			dashcount = 0;
			player.x = wall_posX - player_size;
		}
		else if (keyleft == true && player.x > wall_posX - player_size && player.x < wall_posX + wall_width && player.y > wall_posY - player_size + buffer && player.y < wall_posY + wall_height - buffer) //entering from right
		{
			InAir = true;
			keyleft = false;
			dashleft = false;
			dashcount = 0;
			player.x = wall_posX + wall_width;
		}
		else if (player.y < wall_posY + (wall_height / 2) && player.y > wall_posY - player_size && player.x > wall_posX - player_size && player.x < wall_posX + wall_width)
		{
			movementV.y = 0;
			player.y = wall_posY - player_size;

		}
		else if (player.y > wall_posY + (wall_height / 2) && player.y <wall_posY + wall_height && player.x >wall_posX - player_size && player.x < wall_posX + wall_width)
		{
			movementV.y = 0;
			player.y = wall_posY + wall_height;
			InAir = true;
		}
		if ((player.x > wall_posX - player_size && player.x < wall_posY + wall_width && player.y > wall_posY + wall_height) || player.x < 0 || player.y > 900)
		{
			pausestate = Pause;
			playerstate = dead;
			WinLoseState = FailStage;
			reset = true;
		}
	}
	if (player.x >= wall_posX + wall_width || player.x <= wall_posX - player_size)
	{
		if (player.y == wall_posY - player_size)
		{
			InAir = true;
		}
	}

}
//Character movement
void movement(void)
{
	if (InAir == true)
	{
		movementV = CP_Vector_Add(movementV, CP_Vector_Scale(gravity, CP_System_GetDt()));
	}
	else if (InAir == false)
	{
		movementV = CP_Vector_Zero();
		jumpcount = 0;
	}
	if (CP_Input_KeyTriggered(KEY_A) || (CP_Input_KeyTriggered(KEY_LEFT)))
	{
		keyleft = true;
	}
	else if (CP_Input_KeyReleased(KEY_A) || (CP_Input_KeyReleased(KEY_LEFT)))
	{
		keyleft = false;
	}
	if (CP_Input_KeyTriggered(KEY_D) || (CP_Input_KeyTriggered(KEY_RIGHT)))
	{
		keyright = true;
	}
	else if (CP_Input_KeyReleased(KEY_D) || (CP_Input_KeyReleased(KEY_RIGHT)))
	{
		keyright = false;
	}
	if (CP_Input_KeyTriggered(KEY_LEFT_SHIFT))
	{
		shift = true;
	}
	else if (CP_Input_KeyReleased(KEY_LEFT_SHIFT))
	{
		shift = false;
		holdshift = false;
	}
	else if (CP_Input_KeyDown(KEY_LEFT_SHIFT))
	{
		holdshift = true;
	}
	if (keyleft == true)
	{
		movementH = left;
	}
	if (keyleft == false)
	{
		movementH = CP_Vector_Zero();
	}
	if (keyright == true)
	{
		movementH = right;
	}

	if (CP_Input_KeyTriggered(KEY_SPACE))
	{
		if (jumpcount < maxjumpcount)
		{
			CP_Sound_PlayAdvanced(JumpSound, 0.2f, 1.0f, FALSE, Sound_Effect);
			movementV = jump;
			InAir = true;
			jumpcount++;
		}
	}
	if (keyright == true && shift == true && dashcooldown == false)
	{
		dashright = true;
	}
	if (keyleft == true && shift == true && dashcooldown == false)
	{
		dashleft = true;
	}
	//cooldown
	if (dashcooldown == true)
	{
		dashtimer += 1;
		if (dashtimer > maxdashtimer)
		{
			dashtimer = 0;
			movementH = CP_Vector_Zero();
			dashcooldown = false;
		}
		if (holdshift == true)
		{
			dashcooldown = true;

		}
	}
	//dash
	if (dashcooldown == false)
	{
		if (dashright == true)
		{
			movementV = CP_Vector_Zero();
			movementH = CP_Vector_Add(right, dash);
			dashcount++;
		}
		if (dashleft == true)
		{
			movementV = CP_Vector_Zero();
			movementH = CP_Vector_Subtract(left, dash);
			dashcount++;
		}
		if (dashcount >= 20)
		{
			dashleft = false;
			dashright = false;
			dashcount = 0;
			dashcooldown = true;
		}
	}

	//prevent from moving when pressing left right keys together
	if ((keyleft == true) && (keyright == true))
	{
		movementH = CP_Vector_Zero();
	}
}
void pause_algorithm(void)
{
	if ((CP_Input_KeyTriggered(KEY_P) || CP_Input_KeyTriggered(KEY_ESCAPE)) && pausestate == Unpause)
	{
		freeze = true;
		has_crossline = false;
		pausestate = Pause;
	}
	else if ((CP_Input_KeyTriggered(KEY_P) || CP_Input_KeyTriggered(KEY_ESCAPE)) && pausestate == Pause)
	{
		freeze = false;
		pausestate = Unpause;
	}
}
//Door interaction with button activation
void Door(CP_Vector hdoor, CP_Vector vdoor, CP_Vector dsize, CP_Vector dbutton, float dbuttonwidth, float dbuttonheight)
{
	if (hdoor.x == 0 && hdoor.y == 0)//vdoor case
	{
		if (player.x == button.x + dbuttonwidth)
		{
			ButtonDetection = true;
			Vdoordetect = true;
		}
		if (player.y == button.y - player_size)
		{
			ButtonDetection = true;
			Vdoordetect = true;
		}
		if (ButtonDetection == true && Vdoordetect == true)
		{
			CP_Vector speed = CP_Vector_Set(0, 100);
			doorlength = CP_Vector_Subtract(doorlength, CP_Vector_Scale(speed, CP_System_GetDt()));
			doormusic = true;

		}
		else if (ButtonDetection == false)
		{
			doorlength = dsize;
		}
		if (doorlength.y > 0)
		{
			CP_Graphics_DrawRect(vdoor.x, vdoor.y, doorlength.x, doorlength.y);
			wall(vdoor.x, vdoor.y, doorlength.x, doorlength.y);
		}
		if (doorlength.y < 0 || doorlength.y > 900)
		{
			Vdoordetect = false;

		}
	}
	else if (vdoor.x == 0 && vdoor.y == 0)//hdoor case
	{
		if (player.x == button.x + dbuttonwidth)
		{
			ButtonDetection = true;
			Hdoordetect = true;
		}
		if (player.y == button.y - player_size)
		{
			ButtonDetection = true;
			Hdoordetect = true;
		}
		if (ButtonDetection == true && Hdoordetect == true)
		{
			CP_Vector movement = CP_Vector_Set(100, 0);
			hdoormove = CP_Vector_Add(hdoormove, CP_Vector_Scale(movement, CP_System_GetDt()));
			doormusic = true;
		}
		else if (ButtonDetection == false)
		{
			hdoormove = hdoor;
		}
		if (hdoormove.y < hdoor.x + dsize.x)
		{
			CP_Graphics_DrawRect(hdoormove.x, hdoormove.y, dsize.x, dsize.y);
			wall(hdoormove.x, hdoormove.y, dsize.x, dsize.y);
		}
		if (hdoormove.x < 0 || hdoormove.x > 1600)
		{
			Hdoordetect = false;

		}
	}

	if (reset == true)
	{
		doorlength = dsize;
		ButtonDetection = false;
		Hdoordetect = false;
		Vdoordetect = false;
		endTimer = 0;
		freeze = false;
		pausedTimer = 0;
		has_crossline = false;
		keyleft = false;
		keyright = false;
		dashleft = false;
		dashright = false;
		shift = false;
		movementV = CP_Vector_Zero();
		movementH = CP_Vector_Zero();
		reset = false;
	}
	CP_Graphics_DrawRect(dbutton.x, dbutton.y, dbuttonwidth, dbuttonheight);
	wall(dbutton.x, dbutton.y, dbuttonwidth, dbuttonheight);
}
//Fading for digipen Logo
void fade(void)
{
	if (FadeState == 0)
	{
		alpha += 100 * CP_System_GetDt();
		if (alpha >= 255)
		{

			FadeState = 1;
		}
	}
	if (FadeState == 1)
	{
		alpha -= 100 * CP_System_GetDt();
		if (alpha <= 0)
		{
			alpha = 0;
		}
	}
	if (alpha != 0)
	{
		CP_Settings_Fill(Black);
		CP_Graphics_DrawRect(0, 0, 1600, 900);
	}
	CP_Image_Draw(DigipenSGRED, 400, 300, 800, 300, (int)alpha);
}
//Deathcounter
void death(void)
{
	if (playerstate == dead && control == false)
	{
		deathstate = true;
		control = true;
	}
	else if (deathstate == true)
	{
		deathcounter++;
		deathstate = false;

	}
	else if (playerstate == alive)
	{
		control = false;
	}
	sprintf_s(buffer2, sizeof(buffer2), "%d", deathcounter);
}

//GUI

//Return Button == return to main menu
void returnbutton(float returnbuttonX, float returnbuttonY)
{
	CP_Image_Draw(MainmenuButton, returnbuttonX, returnbuttonY, ButtonWidth, ButtonHeight, 255);
	if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && CP_Input_GetMouseX() > returnbuttonX && CP_Input_GetMouseX() < returnbuttonX + ButtonWidth && CP_Input_GetMouseY() > returnbuttonY
		&& CP_Input_GetMouseY() < returnbuttonY + ButtonHeight)
	{
		CP_Sound_PlayAdvanced(UI_Sound, 1.0f, 1.0f, FALSE, Sound_UI);
		if (gamestate != Mainmenu)
		{
			gamestate = Mainmenu;
			pausestate = Unpause;
			WinLoseState = UncompleteStage;
			playerstate = alive;
			has_crossline = false;
		}
	}
}
void NextStageButton(void)
{
	CP_Settings_Fill(White);
	CP_Image_Draw(nextbutton, 930, 630, ButtonWidth, ButtonHeight, 255);
	//CP_Graphics_DrawRect(930, 630, ButtonWidth, ButtonHeight);
	CP_Settings_Fill(Black);
	//CP_Font_DrawText("Next", 930 + ButtonWidth / 2, 630 + ButtonHeight / 2);
	if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && CP_Input_GetMouseX() > 930 && CP_Input_GetMouseX() < 930 + ButtonWidth && CP_Input_GetMouseY() > 630
		&& CP_Input_GetMouseY() < 630 + ButtonHeight)
	{
		CP_Sound_PlayAdvanced(UI_Sound, 1.0f, 1.0f, FALSE, Sound_UI);
		if (gamestate == Stage1 && WinLoseState == CompleteStage)
		{
			gamestate = Stage2;
			pausestate = Unpause;
			WinLoseState = UncompleteStage;
			*pointer = CP_Vector_Set(10.f, 790.f);
			keyleft = false;
			keyright = false;
			deathcounter = 0;
			reset = true;
		}
		else if (gamestate == Stage2)
		{
			gamestate = Stage3;
			pausestate = Unpause;
			WinLoseState = UncompleteStage;
			*pointer = CP_Vector_Set(30.f, 200.f);
			keyleft = false;
			keyright = false;
			deathcounter = 0;
			reset = true;
		}
		else if (gamestate == Stage3)
		{
			gamestate = Credits;
			pausestate = Unpause;
			WinLoseState = UncompleteStage;
			deathcounter = 0;
			reset = true;
		}
	}
}
void Mainmenu_page(void)
{
	CP_Image_Draw(MainmenuBackground, 0, 0, 1600, 900, 255);
	CP_Image_Draw(BackgroundWord, 500, 50, 600, 250, 255);
	float AllButtonX = WindowWidth / 2 - (ButtonWidth / 2);
	float CreditsButtonY = WindowHeight - 400;
	float SelectStageButtonY = CreditsButtonY - 100;
	float StartButtonY = SelectStageButtonY - 100;
	float InstructionButtonY = CreditsButtonY + 100;
	float QuitButtonY = InstructionButtonY + 100;

	CP_Image_Draw(StartButton, AllButtonX, StartButtonY, ButtonWidth, ButtonHeight, 255);
	CP_Image_Draw(SelectStageButton, AllButtonX, SelectStageButtonY, ButtonWidth, ButtonHeight, 255);
	CP_Image_Draw(CreditsButton, AllButtonX, CreditsButtonY, ButtonWidth, ButtonHeight, 255);
	CP_Image_Draw(InstructionButton, AllButtonX, InstructionButtonY, ButtonWidth, ButtonHeight, 255);
	CP_Image_Draw(quitbutton, AllButtonX, QuitButtonY, ButtonWidth, ButtonHeight, 255);

	float MuteButtonX = 30;
	float MuteButtonY = 850;
	CP_Settings_Fill(White);
	CP_Graphics_DrawRect(MuteButtonX, MuteButtonY, 30, 30);

	//Start Button
	if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && CP_Input_GetMouseX() > AllButtonX && CP_Input_GetMouseX() < AllButtonX + ButtonWidth && CP_Input_GetMouseY() > StartButtonY
		&& CP_Input_GetMouseY() < StartButtonY + ButtonHeight)
	{
		CP_Sound_PlayAdvanced(UI_Sound, 1.0f, 1.0f, FALSE, Sound_UI);
		reset = true;
		endTimer = 0;
		deathcounter = 0;
		gamestate = Stage1;
		playerstate = alive;
		pausestate = Unpause;
		*pointer = CP_Vector_Set(50.f, 870.f);
	}

	//Stages Button
	if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && CP_Input_GetMouseX() > AllButtonX && CP_Input_GetMouseX() < AllButtonX + ButtonWidth && CP_Input_GetMouseY() > SelectStageButtonY
		&& CP_Input_GetMouseY() < SelectStageButtonY + ButtonHeight)
	{
		CP_Sound_PlayAdvanced(UI_Sound, 1.0f, 1.0f, FALSE, Sound_UI);
		gamestate = SelectStage;
		pausestate = Unpause;
		WinLoseState = UncompleteStage;
		playerstate = alive;

	}

	//Credits Button
	if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && CP_Input_GetMouseX() > AllButtonX && CP_Input_GetMouseX() < AllButtonX + ButtonWidth && CP_Input_GetMouseY() > CreditsButtonY
		&& CP_Input_GetMouseY() < CreditsButtonY + ButtonHeight)
	{
		CP_Sound_PlayAdvanced(UI_Sound, 1.0f, 1.0f, FALSE, Sound_UI);
		gamestate = Credits;
	}

	//How to play Button
	if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && CP_Input_GetMouseX() > AllButtonX && CP_Input_GetMouseX() < AllButtonX + ButtonWidth && CP_Input_GetMouseY() > InstructionButtonY
		&& CP_Input_GetMouseY() < InstructionButtonY + ButtonHeight)
	{
		CP_Sound_PlayAdvanced(UI_Sound, 1.0f, 1.0f, FALSE, Sound_UI);
		gamestate = Instruction;
	}

	if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && CP_Input_GetMouseX() > AllButtonX && CP_Input_GetMouseX() < AllButtonX + ButtonWidth && CP_Input_GetMouseY() > QuitButtonY
		&& CP_Input_GetMouseY() < QuitButtonY + ButtonHeight)
	{
		CP_Engine_Terminate();
	}

	if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && mutestate == unmuted && CP_Input_GetMouseX() > MuteButtonX && CP_Input_GetMouseX() < MuteButtonX + 30 && CP_Input_GetMouseY() > MuteButtonY
		&& CP_Input_GetMouseY() < MuteButtonY + 30) //Mute Button Collision
	{
		CP_Sound_StopGroup(Menu_BGM);
		CP_Sound_StopGroup(Stage_BGM);
		CP_Sound_SetGroupVolume(Sound_Effect, 0.0f);
		CP_Sound_SetGroupVolume(Sound_UI, 0.0f);
		mutestate = muted;
	}
	else if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && mutestate == muted && CP_Input_GetMouseX() > MuteButtonX && CP_Input_GetMouseX() < MuteButtonX + 30 && CP_Input_GetMouseY() > MuteButtonY
		&& CP_Input_GetMouseY() < MuteButtonY + 30)
	{
		CP_Sound_PlayAdvanced(MenuMusic, 0.5f, 1.0f, TRUE, Menu_BGM);
		CP_Sound_PlayAdvanced(StageMusic, 0.5f, 1.0f, TRUE, Stage_BGM);
		CP_Sound_SetGroupVolume(Sound_Effect, 1.0f);
		CP_Sound_SetGroupVolume(Sound_UI, 1.0f);
		mutestate = unmuted;
	}

	if (mutestate == muted)
	{
		CP_Settings_Stroke(Red);
		CP_Graphics_DrawLine(30, 850, 30 + 30, 850 + 30);
		CP_Graphics_DrawLine(30, 850 + 30, 30 + 30, 850);
		CP_Settings_Stroke(Black);
	}
}
void Selectstage_page(void)
{
	CP_Image_Draw(MainmenuBackground, 0, 0, 1600, 900, 255);
	CP_Image_Draw(BackgroundWord, 500, 50, 600, 250, 255);
	float Stage2ButtonX = WindowWidth / 2 - (ButtonWidth / 2);
	float Stage1ButtonX = Stage2ButtonX - ButtonWidth - 50;
	float Stage3ButtonX = Stage2ButtonX + ButtonWidth + 50;
	float AllButtonY = WindowHeight - 600;

	CP_Image_Draw(Stage1Button, Stage1ButtonX, AllButtonY, ButtonWidth, ButtonHeight, 255);
	CP_Image_Draw(Stage2Button, Stage2ButtonX, AllButtonY, ButtonWidth, ButtonHeight, 255);
	CP_Image_Draw(Stage3Button, Stage3ButtonX, AllButtonY, ButtonWidth, ButtonHeight, 255);

	if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && CP_Input_GetMouseX() > Stage1ButtonX && CP_Input_GetMouseX() < Stage1ButtonX + ButtonWidth && CP_Input_GetMouseY() > AllButtonY
		&& CP_Input_GetMouseY() < AllButtonY + ButtonHeight)
	{
		CP_Sound_PlayAdvanced(UI_Sound, 1.0f, 1.0f, FALSE, Sound_UI);
		gamestate = Stage1;
		reset = true;
		deathcounter = 0;
		endTimer = 0;
		*pointer = CP_Vector_Set(50.f, 870.f);
	}

	if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && CP_Input_GetMouseX() > Stage2ButtonX && CP_Input_GetMouseX() < Stage2ButtonX + ButtonWidth && CP_Input_GetMouseY() > AllButtonY
		&& CP_Input_GetMouseY() < AllButtonY + ButtonHeight)
	{
		CP_Sound_PlayAdvanced(UI_Sound, 1.0f, 1.0f, FALSE, Sound_UI);
		gamestate = Stage2;
		reset = true;
		deathcounter = 0;
		endTimer = 0;
		*pointer = CP_Vector_Set(10.f, 790.f);
	}

	if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && CP_Input_GetMouseX() > Stage3ButtonX && CP_Input_GetMouseX() < Stage3ButtonX + ButtonWidth && CP_Input_GetMouseY() > AllButtonY
		&& CP_Input_GetMouseY() < AllButtonY + ButtonHeight)
	{
		CP_Sound_PlayAdvanced(UI_Sound, 1.0f, 1.0f, FALSE, Sound_UI);
		gamestate = Stage3;
		reset = true;
		deathcounter = 0;
		endTimer = 0;
		*pointer = CP_Vector_Set(30, 200);
	}
	returnbutton(30, 820);
}
void Credits_page(void)
{
	CP_Image_Draw(Creditspage, 0, 0, 1600, 900, 255);
	returnbutton(30, 820);
}
//How to play page
void Instruction_page(void)
{
	CP_Image_Draw(InstructionsPage, 0, 0, 1600, 900, 255);
	returnbutton(30, 820);
}
void Pausescreen(void)
{
	CP_Image_Draw(PauseScreen, 500, 200, 600, 500, 255);
	//Resume button

	CP_Image_Draw(ResumeButton, 930, 630, ButtonWidth, ButtonHeight, 255);
	if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && CP_Input_GetMouseX() > 930 && CP_Input_GetMouseX() < 930 + ButtonWidth && CP_Input_GetMouseY() > 630
		&& CP_Input_GetMouseY() < 630 + ButtonHeight)
	{
		CP_Sound_PlayAdvanced(UI_Sound, 1.0f, 1.0f, FALSE, Sound_UI);
		freeze = false;
		pausestate = Unpause;
	}
	returnbutton(530, 630);
}
//Character died by traps or clip out of bound
void Failscreen(void)
{
	CP_Image_Draw(LoseScreen, 500, 200, 600, 500, 255);

	has_crossline = false;

	//Try again button
	CP_Image_Draw(TryAgain, 930, 630, ButtonWidth, ButtonHeight, 255);
	if (CP_Input_MouseTriggered(MOUSE_BUTTON_1) && CP_Input_GetMouseX() > 930 && CP_Input_GetMouseX() < 930 + ButtonWidth && CP_Input_GetMouseY() > 630
		&& CP_Input_GetMouseY() < 630 + ButtonHeight)
	{
		CP_Sound_PlayAdvanced(UI_Sound, 1.0f, 1.0f, FALSE, Sound_UI);
		if (gamestate == Stage1)
		{
			gamestate = Stage1;
			pausestate = Unpause;
			WinLoseState = UncompleteStage;
			playerstate = alive;
			keyleft = false;
			keyright = false;
			pausedTimer = 0;
			reset = true;
			*pointer = CP_Vector_Set(50.f, 870.f);
		}
		else if (gamestate == Stage2)
		{
			gamestate = Stage2;
			pausestate = Unpause;
			WinLoseState = UncompleteStage;
			playerstate = alive;
			keyleft = false;
			keyright = false;
			pausedTimer = 0;
			reset = true;
			*pointer = CP_Vector_Set(10.f, 790.f);
		}
		else if (gamestate == Stage3)
		{
			gamestate = Stage3;
			pausestate = Unpause;
			WinLoseState = UncompleteStage;
			playerstate = alive;
			keyleft = false;
			keyright = false;
			pausedTimer = 0;
			reset = true;
			*pointer = CP_Vector_Set(30.f, 200.f);
		}

	}
	CP_Settings_Fill(White);
	CP_Font_DrawText(buffer1, 950, 391);
	CP_Font_DrawText(buffer2, 950, 473);
	returnbutton(530, 630);
}
//Completing the stage without dying
void CompletionPopup(void)
{
	CP_Settings_Fill(White);
	if (gamestate == Stage1)
	{
		CP_Image_Draw(WinScreen, 500, 200, 600, 500, 255);
		CP_Font_DrawText(buffer1, 950, 391);
		CP_Font_DrawText(buffer2, 950, 473);
	}
	else if (gamestate == Stage2)
	{
		CP_Image_Draw(WinScreen, 500, 200, 600, 500, 255);
		CP_Font_DrawText(buffer1, 950, 391);
		CP_Font_DrawText(buffer2, 950, 473);
	}
	else if (gamestate == Stage3)
	{
		CP_Image_Draw(WinScreen, 500, 200, 600, 500, 255);
		CP_Font_DrawText(buffer1, 950, 391);
		CP_Font_DrawText(buffer2, 950, 473);
	}
	returnbutton(530, 630);
	NextStageButton();

}

//Traps

//Drawing of spikes included with spikes collision logic
void spiketrap_test(float spikeposX, float spikeposY, float lengthofspikes, float HeightofSpikes)
{
	float SpikeWidth = 30.f;
	int numberofspikes = (int)(lengthofspikes / SpikeWidth);
	float intersectWidth = 0.0f;
	float intersectHeight = 0.0f;
	spikecollision = CP_Vector_Set((player.x + (player_size / 2)) - (spikeposX + (lengthofspikes / 2)), (player.y + (player_size / 2)) - (spikeposY - (HeightofSpikes / 2)));


	for (int i = 0; i < numberofspikes; i++)
	{
		CP_Settings_Fill(CP_Color_Create(248, 191, 69, 255));
		CP_Graphics_DrawTriangle(spikeposX, spikeposY, spikeposX + SpikeWidth / 2, spikeposY - HeightofSpikes, spikeposX + SpikeWidth, spikeposY);

		spikeposX += SpikeWidth;
	}
	if (HeightofSpikes > 0) {

		intersectWidth = (player_size / 2) + (lengthofspikes / 2);
		intersectHeight = (player_size / 2) + (HeightofSpikes / 2);
		if ((fabs(spikecollision.x) < intersectWidth && fabs(spikecollision.y) < intersectHeight) || (fabs(spikecollision.x) < intersectWidth && player.y > (spikeposY - HeightofSpikes) && player.y < spikeposY))
		{
			pausestate = Pause;
			playerstate = dead;
			WinLoseState = FailStage;
		}
	}
	else if (HeightofSpikes < 0) {


		intersectWidth = (player_size / 2) + (lengthofspikes / 2);
		intersectHeight = (player_size / 2) - (HeightofSpikes / 2);

		if ((fabs(spikecollision.x) < intersectWidth && fabs(spikecollision.y) < intersectHeight))
		{
			pausestate = Pause;
			playerstate = dead;
			WinLoseState = FailStage;
		}
	}
}
//Drawing of turret, included shooting projectile and projectile collision logic
void TurretCollision(float projectileposx, float projectileposy, float projectile_width, float projectile_height, float angle, float turretposx, float turretposy, float endingpoint, NewTurrets* TurretNum)
{
	if (angle == 0)
	{
		Projectilespeed = CP_Vector_Set(0.0f, 188.0f);
		if (TurretNum->VerticalTurretProjectile.y > endingpoint) {
			TurretNum->VerticalTurretProjectile = CP_Vector_Set(turretposx, turretposy);
			CP_Sound_PlayAdvanced(TurretSound, 0.05f, 1.0f, FALSE, Sound_Effect);
		}
		TurretWidth = 20.0f;
		TurretHeight = 60.0f;
		if (pausestate != Pause)
		{
			TurretNum->VerticalTurretProjectile = CP_Vector_Add(TurretNum->VerticalTurretProjectile, CP_Vector_Scale(Projectilespeed, CP_System_GetDt()));

		}
		CP_Settings_Fill(CP_Color_Create(239, 127, 198, 255));
		CP_Graphics_DrawRectAdvanced(TurretNum->VerticalTurretProjectile.x + 5, TurretNum->VerticalTurretProjectile.y, projectile_width + 7.0f, projectile_height + 7.0f, angle, 20.0f);
	}
	else if (angle == 90)
	{
		Projectilespeed = CP_Vector_Set(188.0f, 0.0f);
		if (TurretNum->VerticalTurretProjectile.x > endingpoint) {
			TurretNum->VerticalTurretProjectile = CP_Vector_Set(turretposx + 70.0f, turretposy);
			CP_Sound_PlayAdvanced(TurretSound, 0.05f, 1.0f, FALSE, Sound_Effect);
		}
		TurretWidth = 60.0f;
		TurretHeight = 20.0f;
		if (pausestate != Pause)
		{
			TurretNum->VerticalTurretProjectile = CP_Vector_Add(TurretNum->VerticalTurretProjectile, CP_Vector_Scale(Projectilespeed, CP_System_GetDt()));

		}
		CP_Settings_Fill(CP_Color_Create(239, 127, 198, 255));
		CP_Graphics_DrawRectAdvanced(TurretNum->VerticalTurretProjectile.x + 15, TurretNum->VerticalTurretProjectile.y + 6, projectile_width + 7.0f, projectile_height + 7.0f, angle, 20.0f);
		projectileposx += 15.0f;
		projectileposy += 6.0f;
		projectile_width += 1.0f;
		projectile_height += 1.0f;

	}
	else if (angle == -90)
	{
		Projectilespeed = CP_Vector_Negate(CP_Vector_Set(188.0f, 0.0f));
		if (TurretNum->VerticalTurretProjectile.x < endingpoint) {
			TurretNum->VerticalTurretProjectile = CP_Vector_Set(turretposx - 40.0f, turretposy);
			CP_Sound_PlayAdvanced(TurretSound, 0.05f, 1.0f, FALSE, Sound_Effect);
		}
		TurretWidth = 60.0f;
		TurretHeight = 20.0f;
		if (pausestate != Pause)
		{
			TurretNum->VerticalTurretProjectile = CP_Vector_Add(TurretNum->VerticalTurretProjectile, CP_Vector_Scale(Projectilespeed, CP_System_GetDt()));

		}
		CP_Settings_Fill(CP_Color_Create(239, 127, 198, 255));
		CP_Graphics_DrawRectAdvanced(TurretNum->VerticalTurretProjectile.x + 15, TurretNum->VerticalTurretProjectile.y + 16, projectile_width + 7.0f, projectile_height + 7.0f, angle, 20.0f);
		projectileposx += 15.0f;
		projectileposy += 16.0f;
		projectile_width += 0.0f;
		projectile_height += 0.0f;

	}
	float radianangle = CP_Math_Radians(angle);

	float rotated_vectorX = ((projectile_width * (float)(cos(radianangle))) - ((projectile_height) * (float)(sin(radianangle))));
	float rotated_vectorY = ((projectile_width * (float)(sin(radianangle))) + ((projectile_height) * (float)(cos(radianangle))));

	float newrotatedpointx = rotated_vectorX + projectileposx;
	float newrotatedpointy = rotated_vectorY + projectileposy;

	float newvectorX = (player.x + (player_size / 2)) - (newrotatedpointx + (projectile_width / 2));
	float newvectorY = (player.y + (player_size / 2)) - (newrotatedpointy + (projectile_height / 2));


	float deltaWidth = (player_size / 2) + (projectile_width / 2);
	float deltaHeight = (player_size / 2) + (projectile_height / 2);

	if (fabs(newvectorX) < deltaWidth && fabs(newvectorY) < deltaHeight)
	{
		playerstate = dead;
		WinLoseState = FailStage;
		pausestate = Pause;
	}

	CP_Settings_Fill(CP_Color_Create(248, 253, 166, 255));
	CP_Graphics_DrawRect(turretposx, turretposy, TurretWidth, TurretHeight);
	wall(turretposx, turretposy, TurretWidth, TurretHeight);


}
//Drawing of enemy patrolling, included how far the distance the enemy need to travel
void EnemyPatrolling(float playerposx, float playerposy, float PatrolDistance, float EnemyPosX, float EnemyPosY, float EnemyHeight, EnemyPatrols* EnemyNumber)
{
	if (pausestate != Pause)
	{
		float Length = CP_Vector_Length(CP_Vector_Set(PatrolDistance, 0));
		if (EnemyNumber->enemy_pos.x > (EnemyPosX + Length - player_size))
		{
			EnemyNumber->enemymovement = enemyleft;
		}
		else if (EnemyNumber->enemy_pos.x < EnemyPosX)
		{
			EnemyNumber->enemymovement = enemyright;
		}
		if (player.x > EnemyNumber->enemy_pos.x - player_size && player.x < EnemyNumber->enemy_pos.x + player_size && player.y > EnemyNumber->enemy_pos.y && player.y < EnemyNumber->enemy_pos.y + EnemyHeight)
		{
			playerstate = dead;
			pausestate = Pause;
			WinLoseState = FailStage;
		}
		EnemyNumber->enemy_pos = CP_Vector_Add(EnemyNumber->enemy_pos, CP_Vector_Scale(EnemyNumber->enemymovement, CP_System_GetDt()));
		EnemyNumber->enemy_pos.y = EnemyPosY;

	}
	CP_Settings_Fill(CP_Color_Create(150, 235, 255, 255));
	CP_Graphics_DrawRect(EnemyNumber->enemy_pos.x, EnemyNumber->enemy_pos.y, player_size, EnemyHeight);
}
//Drawing of lazer turret, included lazer and lazer collision logic
void drawlasergun(float laserposx, float laserposy, float angle, float laserlength, float lasertime, float cooldown, Lasers* LaserNumber) {
	CP_Settings_Fill(CP_Color_Create(248, 253, 166, 255));
	float laserWidth = 60.0f;
	float laserHeight = 20.0f;
	LaserNumber->laserheight = 20.0f;
	float gunposx;
	float gunposy;
	if (angle == 180.0f) {
		gunposx = laserposx + laserWidth;
		gunposy = laserposy;
		CP_Settings_Stroke(Black);
		CP_Settings_StrokeWeight(2.2f);
		CP_Graphics_DrawRectAdvanced(gunposx, gunposy, laserWidth, laserHeight, angle, 0.0f);
		wall(laserposx, laserposy - laserHeight, laserWidth, laserHeight);
		if (pausestate == Pause)
		{
			CP_Graphics_DrawRectAdvanced(gunposx, gunposy, laserWidth, laserHeight, angle, 0.0f);
		}
	}
	else if (angle == 90.0f) {
		gunposx = laserposx;
		gunposy = laserposy - laserWidth;
		CP_Settings_Stroke(Black);
		CP_Settings_StrokeWeight(2.2f);
		CP_Graphics_DrawRectAdvanced(gunposx, gunposy, laserWidth, laserHeight, angle, 0.0f);
		wall(gunposx - laserHeight, gunposy, laserHeight, laserWidth);
		if (pausestate == Pause)
		{
			CP_Graphics_DrawRectAdvanced(gunposx, gunposy, laserWidth, laserHeight, angle, 0.0f);
		}
	}
	if (angle == 270.0f) {
		gunposx = laserposx;
		gunposy = laserposy + laserWidth;
		CP_Settings_Stroke(Black);
		CP_Settings_StrokeWeight(2.2f);
		CP_Graphics_DrawRectAdvanced(gunposx, gunposy, laserWidth, laserHeight, angle, 0.0f);
		wall(laserposx, laserposy, laserHeight, laserWidth);
		if (pausestate == Pause)
		{
			CP_Graphics_DrawRectAdvanced(gunposx, gunposy, laserWidth, laserHeight, angle, 0.0f);
		}

	}
	else if (angle == 0.0f) {
		gunposx = laserposx - laserWidth;
		gunposy = laserposy;
		CP_Settings_Stroke(Black);
		CP_Settings_StrokeWeight(2.2f);
		CP_Graphics_DrawRectAdvanced(gunposx, gunposy, laserWidth, laserHeight, angle, 0.0f);
		wall(gunposx, gunposy, laserWidth, laserHeight);
		if (pausestate == Pause)
		{
			CP_Graphics_DrawRectAdvanced(gunposx, gunposy, laserWidth, laserHeight, angle, 0.0f);
		}
	}

	float onesecond = CP_System_GetDt() * CP_System_GetFrameRate();
	float laserstarttime = lasertime;
	float laserstoptime = laserstarttime + cooldown;

	if (pausestate != Pause)
	{
		if (LaserNumber->intervaltime < laserstarttime * onesecond) {
			active_laser = true;
			LaserNumber->intervaltime += CP_System_GetDt();

		}
		else {
			active_laser = false;
			if (LaserNumber->intervaltime < laserstoptime * onesecond) {
				LaserNumber->intervaltime += CP_System_GetDt();
			}
			else {

				LaserNumber->intervaltime = 0.0f;
			}
		}
	}

	if (active_laser == true) {

		CP_Settings_Fill(CP_Color_Create(239, 127, 198, 255));
		CP_Graphics_DrawRectAdvanced(laserposx, laserposy, laserlength, LaserNumber->laserheight, angle, 0.0f);
		float rotated_vectorX = 0.0f;
		float rotated_vectorY = 0.0f;
		float newvectorX = 0.0f;
		float newvectorY = 0.0f;
		float deltaWidth = 0.0f;
		float deltaHeight = 0.0f;

		float radianangle = CP_Math_Radians(angle);
		if (angle == 0.0f) {
			newvectorX = ((player.x + (player_size / 2)) - (laserposx + laserlength / 2));
			newvectorY = ((player.y + (player_size / 2)) - (laserposy + laserHeight / 2));


			deltaWidth = (player_size / 2) + (laserlength / 2);
			deltaHeight = (player_size / 2) + (LaserNumber->laserheight / 2);

		}
		else if (angle == 90.0f) {
			rotated_vectorX = (((laserlength) * (float)(cos(radianangle))) - ((LaserNumber->laserheight) * (float)(sin(radianangle))));
			rotated_vectorY = (((laserlength) * (float)(sin(radianangle))) + ((LaserNumber->laserheight) * (float)(cos(radianangle))));


			newvectorX = (player.x + (player_size / 2)) - (laserposx + ((float)(rotated_vectorX) / 2));
			newvectorY = (player.y + (player_size / 2)) - (laserposy + ((float)(rotated_vectorY) / 2));

			deltaWidth = (player_size / 2) + ((float)fabs(rotated_vectorX) / 2);
			deltaHeight = (player_size / 2) + ((float)fabs(rotated_vectorY) / 2);

		}
		else if (angle == 180.0f) {
			rotated_vectorX = (((laserlength) * (float)(cos(radianangle))) - ((LaserNumber->laserheight) * (float)(sin(radianangle))));
			rotated_vectorY = (((laserlength) * (float)(sin(radianangle))) + ((LaserNumber->laserheight) * (float)(cos(radianangle))));


			newvectorX = (player.x + (player_size / 2)) - (laserposx + ((float)(rotated_vectorX) / 2));
			newvectorY = (player.y + (player_size / 2)) - (laserposy + ((float)(rotated_vectorY) / 2));


			deltaWidth = (player_size / 2) + ((float)fabs(rotated_vectorX) / 2);
			deltaHeight = (player_size / 2) + ((float)fabs(rotated_vectorY) / 2);

		}

		else if (angle == 270) {
			rotated_vectorX = (((laserlength) * (float)(cos(radianangle))) - ((LaserNumber->laserheight) * (float)(sin(radianangle))));
			rotated_vectorY = (((laserlength) * (float)(sin(radianangle))) + ((LaserNumber->laserheight) * (float)(cos(radianangle))));

			newvectorX = (player.x + (player_size / 2)) - (laserposx + ((float)(rotated_vectorX) / 2));
			newvectorY = (player.y + (player_size / 2)) - (laserposy + ((float)(rotated_vectorY) / 2));

			deltaWidth = (player_size / 2) + ((float)fabs(rotated_vectorX) / 2);
			deltaHeight = (player_size / 2) + ((float)fabs(rotated_vectorY) / 2);
		}
		if (fabs(newvectorX) < deltaWidth && fabs(newvectorY) < deltaHeight)
		{
			WinLoseState = FailStage;
			playerstate = dead;
			pausestate = Pause;
		}
	}
}

//Stages
void StartEndPoint(float playerposx, float playerposy)
{
	CP_Image_Draw(InGameBackground, 0, 0, 1600, 900, 255);
	sprintf_s(buffer1, sizeof(buffer1), "%.2f", endTimer);
	float EndPointDoorW = 60;
	float EndPointDoorH = 130;
	if (gamestate == Stage1)
	{
		CP_Settings_Fill(Green);
		CP_Graphics_DrawRect(1500, 270, EndPointDoorW, EndPointDoorH);
		CP_Settings_Stroke(Black);
		CP_Graphics_DrawCircle(1510, 340, 10);
		if (playerstate == alive && playerposx > 1500 && playerposx < 1500 + EndPointDoorW && playerposy > 270 && playerposy < 270 + EndPointDoorH)
		{
			pausestate = Pause;
			WinLoseState = CompleteStage;
		}
		if ((player.x < 50 || player.x > 50 || player.y < 870))
		{
			if (has_crossline == false && playerstate == alive && freeze == false)
			{
				timestart = CP_System_GetSeconds();
				has_crossline = true;
			}
			if (playerstate == alive && pausestate == Unpause && freeze == false)
			{
				endTimer = pausedTimer + (CP_System_GetSeconds() - timestart);
			}
			if (freeze == true)
			{
				timestart = CP_System_GetSeconds();
				pausedTimer = endTimer;
			}
		}

	}
	else if (gamestate == Stage2)
	{
		CP_Settings_Fill(Green);
		CP_Graphics_DrawRect(40, 270, EndPointDoorW, EndPointDoorH);
		CP_Settings_Stroke(Black);
		CP_Graphics_DrawCircle(50, 340, 10);
		if (playerstate == alive && player.x > 40 && player.x < 40 + EndPointDoorW && player.y > 270 && player.y < 270 + EndPointDoorH)
		{
			pausestate = Pause;
			WinLoseState = CompleteStage;
		}
		if ((player.x > 10 || player.y < 790))
		{
			if (has_crossline == false && playerstate == alive)
			{
				timestart = CP_System_GetSeconds();
				has_crossline = true;
			}
			if (playerstate == alive && pausestate == Unpause && freeze == false)
			{
				endTimer = pausedTimer + (CP_System_GetSeconds() - timestart);
			}
			if (freeze == true)
			{
				timestart = CP_System_GetSeconds();
				pausedTimer = endTimer;
			}
		}
	}
	else if (gamestate == Stage3)
	{
		CP_Settings_Fill(Green);
		CP_Graphics_DrawRect(40, 300, EndPointDoorW, EndPointDoorH);
		CP_Settings_Stroke(Black);
		CP_Graphics_DrawCircle(50, 370, 10);
		if (playerstate == alive && player.x > 40 && player.x < 40 + EndPointDoorW && player.y > 300 && player.y < 300 + EndPointDoorH)
		{
			pausestate = Pause;
			WinLoseState = CompleteStage;
		}
		if ((player.x < 30 || player.x > 30 || player.y < 200))
		{
			if (has_crossline == false && playerstate == alive)
			{
				timestart = CP_System_GetSeconds();
				has_crossline = true;
			}
			if (playerstate == alive && pausestate == Unpause && freeze == false)
			{
				endTimer = pausedTimer + (CP_System_GetSeconds() - timestart);
			}
			if (freeze == true)
			{
				timestart = CP_System_GetSeconds();
				pausedTimer = endTimer;
			}
		}
	}
	CP_Settings_TextSize(28);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Settings_Fill(White);
	CP_Font_DrawText(buffer1, 1560, 30);
}
void drawstage1(void)
{
	//top border
	rectX = 0.f;
	rectY = 0.f;
	rectW = 1600.f;
	rectH = 10.f;
	CP_Settings_Fill(Black);
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);
	//bottom border
	rectX = 0.f;
	rectY = 890.f;
	rectW = 1600.f;
	rectH = 900.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);
	//Left border
	rectX = 0.f;
	rectY = 0.f;
	rectW = 10.f;
	rectH = 900.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);
	//rightborder
	rectX = 1590.f;
	rectY = 0.f;
	rectW = 10.f;
	rectH = 900.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//wall1
	rectX = 300.f;
	rectY = 800.f;
	rectW = 60.f;
	rectH = 100.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//wall2
	rectX = 600.f;
	rectY = 700.f;
	rectW = 60.f;
	rectH = 200.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//wall3
	rectX = 900.f;
	rectY = 660.f;
	rectW = 60.f;
	rectH = 240.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//wall4
	rectX = 1200.f;
	rectY = 620.f;
	rectW = 400.f;
	rectH = 380.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//platform1
	rectX = 0.f;
	rectY = 480.f;
	rectW = 470.f;
	rectH = 70.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//platform2
	rectX = 1300.f;
	rectY = 470.f;
	rectW = 300.f;
	rectH = 70.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//platform 3
	rectX = 1400.f;
	rectY = 400.f;
	rectW = 300.f;
	rectH = 70.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//platform4
	rectX = 900.f;
	rectY = 300.f;
	rectW = 200.f;
	rectH = 70.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);
	//platform5
	rectX = 400.f;
	rectY = 250.f;
	rectW = 200.f;
	rectH = 70.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//platform6
	rectX = 400.f;
	rectY = 320.f;
	rectW = 70.f;
	rectH = 160.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//platform7
	rectX = 10.f;
	rectY = 340.f;
	rectW = 200.f;
	rectH = 50.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	door = CP_Vector_Set(1450, 10);
	doorsize = CP_Vector_Set(10, 388);
	button = CP_Vector_Set(10, 399);
	buttonW = 15;
	buttonH = 70;
	CP_Settings_Fill(Red);
	Door(CP_Vector_Zero(), door, doorsize, button, buttonW, buttonH);
}
void drawstage2(void)
{
	//top border
	rectX = 0.f;
	rectY = 0.f;
	rectW = 1600.f;
	rectH = 10.f;
	CP_Settings_Fill(Black);
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//bottom border
	rectX = 0.f;
	rectY = 890.f;
	rectW = 1600.f;
	rectH = 900.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//Left border
	rectX = 0.f;
	rectY = 0.f;
	rectW = 10.f;
	rectH = 900.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//rightborder
	rectX = 1590.f;
	rectY = 0.f;
	rectW = 10.f;
	rectH = 900.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//P1
	rectW = 70.f;
	rectH = 80.f;
	rectX = 10.f;
	rectY = 810.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//P2
	rectW = 70.f;
	rectH = 30.f;
	rectX = 290.f;
	rectY = 860.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//P3
	rectW = 70.f;
	rectH = 31.f;
	rectX = 600.f;
	rectY = 859.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//P4 
	rectW = 70.f;
	rectH = 32.f;
	rectX = 940.f;
	rectY = 858.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//P5
	rectW = 150.f;
	rectH = 10.f;
	rectX = 750.f;
	rectY = 750.;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//P6
	rectW = 610.f;
	rectH = 10.f;
	rectX = 10.f;
	rectY = 700.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//P7
	rectW = 300.f;
	rectH = 300.f;
	rectX = 10.f;
	rectY = 400.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//P8
	rectW = 200.f;
	rectH = 10.f;
	rectX = 550.f;
	rectY = 570.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//P9
	rectW = 200.f;
	rectH = 10.f;
	rectX = 860.f;
	rectY = 450.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//P10
	rectW = 200.f;
	rectH = 10.f;
	rectX = 400.f;
	rectY = 351.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//P11
	rectW = 90.f;
	rectH = 51.f;
	rectX = 220.f;
	rectY = 349.;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//P12
	rectW = 200.f;
	rectH = 50.f;
	rectX = 10.f;
	rectY = 80.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//P13
	rectW = 300.f;
	rectH = 10.f;
	rectX = 300.f;
	rectY = 150.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//P14
	rectW = 250.f;
	rectH = 10.f;
	rectX = 750.f;
	rectY = 240.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//P15
	rectW = 150.f;
	rectH = 10.f;
	rectX = 1200.f;
	rectY = 350.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//black square test
	rectW = 30.f;
	rectH = 30.f;
	rectX = 1180.f;
	rectY = 590.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//blue horizontal door
	door = CP_Vector_Set(1210, 600);
	doorsize = CP_Vector_Set(380, 10);
	button = CP_Vector_Set(10, 20);
	buttonW = 15;
	buttonH = 55;
	CP_Settings_Fill(Blue);
	Door(door, CP_Vector_Zero(), doorsize, button, buttonW, buttonH);

	//red doors and button (Bottom right)
	door = CP_Vector_Set(1190, 620);
	doorsize = CP_Vector_Set(10, 270);
	button = CP_Vector_Set(1380, 870);
	buttonW = 60;
	buttonH = 20;
	CP_Settings_Fill(Red);
	Door(CP_Vector_Zero(), door, doorsize, button, buttonW, buttonH);

	//red door2 (Top Left)
	door = CP_Vector_Set(190, 130);
	doorsize = CP_Vector_Set(10, 270);
	button = CP_Vector_Set(1380, 870);
	buttonW = 60;
	buttonH = 20;
	CP_Settings_Fill(Red);
	Door(CP_Vector_Zero(), door, doorsize, button, buttonW, buttonH);

}
void drawstage3(void)
{
	//top border
	rectX = 0.f;
	rectY = 0.f;
	rectW = 1600.f;
	rectH = 10.f;
	CP_Settings_Fill(Black);
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//bottom border
	rectX = 0.f;
	rectY = 890.f;
	rectW = 1600.f;
	rectH = 900.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//Left border
	rectX = 0.f;
	rectY = 0.f;
	rectW = 10.f;
	rectH = 900.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//rightborder
	rectX = 1590.f;
	rectY = 0.f;
	rectW = 10.f;
	rectH = 900.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//starting platform
	rectW = 1330.f;
	rectH = 30.f;
	rectX = 10.f;
	rectY = 220.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//far right wall
	rectW = 20.f;
	rectH = 501.f;
	rectX = 1320.f;
	rectY = 250.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//middle wall
	rectW = 20.f;
	rectH = 350.f;
	rectX = 1000.f;
	rectY = 250.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//left most wall
	rectW = 20.f;
	rectH = 400.f;
	rectX = 350.f;
	rectY = 250.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//platform1
	rectW = 90.f;
	rectH = 30.f;
	rectX = 200.f;
	rectY = 190.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//platform2
	rectW = 90.f;
	rectH = 30.f;
	rectX = 550.f;
	rectY = 191.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//platform3
	rectW = 90.f;
	rectH = 30.f;
	rectX = 900.f;
	rectY = 192.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//platform4
	rectW = 90.f;
	rectH = 30.f;
	rectX = 1250.f;
	rectY = 189.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//pathdown(pd1)
	rectW = 70.f;
	rectH = 20.f;
	rectX = 1340.f;
	rectY = 249.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//pd2
	rectW = 70.f;
	rectH = 20.f;
	rectX = 1520.f;
	rectY = 350.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//pd3
	rectW = 70.f;
	rectH = 20.f;
	rectX = 1340.f;
	rectY = 500.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//pd4
	rectW = 70.f;
	rectH = 20.f;
	rectX = 1520.f;
	rectY = 650.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//pd5
	rectW = 200.f;
	rectH = 20.f;
	rectX = 1210.f;
	rectY = 740.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//buttonsegment(bs) 1
	rectW = 200.f;
	rectH = 20.f;
	rectX = 900.f;
	rectY = 601.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//bs2
	rectW = 100.f;
	rectH = 20.f;
	rectX = 1230.f;
	rectY = 460.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//button platform
	rectW = 100.f;
	rectH = 20.f;
	rectX = 1000.f;
	rectY = 430.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//P5
	rectW = 300.f;
	rectH = 20.f;
	rectX = 700.f;
	rectY = 800.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//bottom z platform wall
	rectW = 20.f;
	rectH = 200.f;
	rectX = 700.f;
	rectY = 600.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//P6
	rectW = 130.f;
	rectH = 20.f;
	rectX = 700.f;
	rectY = 710.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//P7
	rectW = 170.f;
	rectH = 20.f;
	rectX = 550.f;
	rectY = 580.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//P8
	rectW = 170.f;
	rectH = 20.f;
	rectX = 550.f;
	rectY = 451.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//P9(enemy patrolling here)
	rectW = 470.f;
	rectH = 20.f;
	rectX = 10.f;
	rectY = 799.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//P10
	rectW = 160.f;
	rectH = 20.f;
	rectX = 10.f;
	rectY = 700.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//P11
	rectW = 160.f;
	rectH = 20.f;
	rectX = 200.f;
	rectY = 550.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//end platform
	rectW = 150.f;
	rectH = 20.f;
	rectX = 10.f;
	rectY = 430.f;
	CP_Graphics_DrawRect(rectX, rectY, rectW, rectH);
	wall(rectX, rectY, rectW, rectH);

	//red door
	door = CP_Vector_Set(150, 250);
	doorsize = CP_Vector_Set(10, 180);
	button = CP_Vector_Set(1030, 410);
	buttonW = 60;
	buttonH = 20;
	CP_Settings_Fill(Red);
	Door(CP_Vector_Zero(), door, doorsize, button, buttonW, buttonH);
}

void callstage1(void)
{
	drawstage1();
	spiketrap_test(360, 890, 240, 50);
	spiketrap_test(660, 890, 240, 50);
	spiketrap_test(960, 890, 240, 50);
}
void callstage2(void)
{
	drawstage2();
	spiketrap_test(80.f, 890.f, 210.f, 20.f);
	spiketrap_test(360.f, 890.f, 240.f, 20.f);
	spiketrap_test(670.f, 890.f, 270.f, 20.f);
	spiketrap_test(1200.f, 890.f, 180.f, 20.f); //Left of Button
	spiketrap_test(1440.f, 890.f, 150.f, 20.f); //Right of Button
	spiketrap_test(530.f, 700.f, 90.f, 20.f); //P6
	spiketrap_test(220.f, 349.f, 90.f, 20.f); //P11
	TurretCollision(Newturret1.VerticalTurretProjectile.x, Newturret1.VerticalTurretProjectile.y, 8.f, 20.f, 0, 440.f, 10.f, 120.f, &Newturret1);
	TurretCollision(Newturret2.VerticalTurretProjectile.x, Newturret2.VerticalTurretProjectile.y, 8.f, 20.f, 90, 80.f, 820.f, 1010.f, &Newturret2);
	TurretCollision(Newturret3.VerticalTurretProjectile.x, Newturret3.VerticalTurretProjectile.y, 8.f, 20.f, 90, 310.f, 665.f, 1180.f, &Newturret3);
	TurretCollision(Newturret4.VerticalTurretProjectile.x, Newturret4.VerticalTurretProjectile.y, 8.f, 20.f, -90, 1530.f, 320.f, 200.f, &Newturret4);
	TurretCollision(Newturret5.VerticalTurretProjectile.x, Newturret5.VerticalTurretProjectile.y, 8.f, 20.f, -90, 1530.f, 425.f, 310.f, &Newturret5);
}
void callstage3(void)
{
	drawstage3();
	spiketrap_test(200.f, 190.f, 90.f, 20.f); //P1
	spiketrap_test(550.f, 191.f, 90.f, 20.f); //P2
	spiketrap_test(900.f, 192.f, 90.f, 20.f); //P3
	spiketrap_test(1250.f, 189.f, 90.f, 20.f); //P4
	spiketrap_test(600.f, 580.f, 120.f, 20.f); //P7
	spiketrap_test(10.f, 700.f, 90.f, 20.f); //P10
	spiketrap_test(260.f, 550.f, 90.f, 20.f); //P11
	spiketrap_test(600.f, 471.f, 120.f, -20.f); //Below P8
	spiketrap_test(980.f, 621.f, 120.f, -20.f); //BS1
	EnemyPatrolling(player.x, player.y, 260, 290, 120, 100, &Enemy1); //P1 to P2
	EnemyPatrolling(player.x, player.y, 260, 990, 120, 100, &Enemy2); //P3 to P4
	EnemyPatrolling(player.x, player.y, 470, 10, 719, 80, &Enemy3); //P9
	EnemyPatrolling(player.x, player.y, 280, 720, 730, 70, &Enemy4); //P5
	drawlasergun(1530.0f, 110.0f, 180.0f, 1518.0f, 2.0f, 3.0f, &Laser1); //Top
	drawlasergun(70.0f, 870.0f, 0.0f, 1518.0f, 3.0f, 2.0f, &Laser2); //Bottom
	drawlasergun(570.0f, 310.0f, 90.0f, 140.0f, 1.0f, 2.0f, &Laser3); //Middle
	CP_Settings_Stroke(Black);
	TurretCollision(Newturret6.VerticalTurretProjectile.x, Newturret6.VerticalTurretProjectile.y, 8.f, 20.f, 0, 1450.f, 10.f, 890.f, &Newturret6);
	TurretCollision(Newturret7.VerticalTurretProjectile.x, Newturret7.VerticalTurretProjectile.y, 8.f, 20.f, 0, 800.f, 250.f, 710.f, &Newturret7);
	TurretCollision(Newturret8.VerticalTurretProjectile.x, Newturret8.VerticalTurretProjectile.y, 8.f, 20.f, 0, 450.f, 250.f, 799.f, &Newturret8);
	TurretCollision(Newturret9.VerticalTurretProjectile.x, Newturret9.VerticalTurretProjectile.y, 8.f, 20.f, 0, 200.f, 250.f, 550.f, &Newturret9);
	TurretCollision(Newturret10.VerticalTurretProjectile.x, Newturret10.VerticalTurretProjectile.y, 8.f, 20.f, 0, 1150.0f, 250.f, 890.f, &Newturret10);
}

void InitializeEnemyPatrolStage3(void)
{
	Enemy1.enemy_pos = CP_Vector_Set(290, 120);
	Enemy2.enemy_pos = CP_Vector_Set(990, 120);
	Enemy3.enemy_pos = CP_Vector_Set(10, 719);
	Enemy4.enemy_pos = CP_Vector_Set(720, 740);
	Enemy1.enemymovement = enemyright;
	Enemy2.enemymovement = enemyright;
	Enemy3.enemymovement = enemyright;
	Enemy4.enemymovement = enemyright;
}
void InitializeTurretShooting(void)
{
	Newturret1.VerticalTurretProjectile = CP_Vector_Set(440.0f, 10.0f);
	Newturret2.VerticalTurretProjectile = CP_Vector_Set(80.0f, 820.0f);
	Newturret3.VerticalTurretProjectile = CP_Vector_Set(310.0f, 665.0f);
	Newturret4.VerticalTurretProjectile = CP_Vector_Set(1530.0f, 320.0f);
	Newturret5.VerticalTurretProjectile = CP_Vector_Set(1530.0f, 425.0f);
	Newturret6.VerticalTurretProjectile = CP_Vector_Set(1450.0f, 10.0f);
	Newturret7.VerticalTurretProjectile = CP_Vector_Set(800.0f, 250.0f);
	Newturret8.VerticalTurretProjectile = CP_Vector_Set(450.0f, 250.0f);
	Newturret9.VerticalTurretProjectile = CP_Vector_Set(200.0f, 250.0f);
	Newturret10.VerticalTurretProjectile = CP_Vector_Set(1150.0f, 250.0f);
}

//System
void LoadImages(void)
{
	StartButton = CP_Image_Load("./Assets/Start button.jpg");
	SelectStageButton = CP_Image_Load("./Assets/Stagebutton.jpg");
	CreditsButton = CP_Image_Load("./Assets/Credits button.jpg");
	Stage1Button = CP_Image_Load("./Assets/Stage 1 button.jpg");
	Stage2Button = CP_Image_Load("./Assets/Stage 2 button.jpg");
	Stage3Button = CP_Image_Load("./Assets/Stage 3 button.jpg");
	MainmenuButton = CP_Image_Load("./Assets/return.jpg");
	BackgroundWord = CP_Image_Load("./Assets/backgroundwords.jpg");
	MainmenuBackground = CP_Image_Load("./Assets/gamebackground.jpg");
	LoseScreen = CP_Image_Load("./Assets/youlose.jpg");
	WinScreen = CP_Image_Load("./Assets/youwon.jpg");
	TryAgain = CP_Image_Load("./Assets/tryagain.jpg");
	PauseScreen = CP_Image_Load("./Assets/pause.jpg");
	ResumeButton = CP_Image_Load("./Assets/resume.jpg");
	InGameBackground = CP_Image_Load("./Assets/ingamebackground.jpg");
	InstructionsPage = CP_Image_Load("./Assets/InstructionsPage.jpg");
	InstructionButton = CP_Image_Load("./Assets/InstructionButton.jpg");
	quitbutton = CP_Image_Load("./Assets/quitbutton.jpg");
	nextbutton = CP_Image_Load("./Assets/nextbutton.jpg");
	DigipenSGRED = CP_Image_Load("./Assets/DigipenSGRED.jpg");
	Creditspage = CP_Image_Load("./Assets/creditspage.jpg");
}
void LoadMusic(void)
{
	MenuMusic = CP_Sound_Load("./Songs/SWING.wav");
	StageMusic = CP_Sound_Load("./Songs/BUNNY'S PLACE.wav");
	TurretSound = CP_Sound_Load("./Songs/TURRET.wav");
	JumpSound = CP_Sound_Load("./Songs/JUMP_1.wav");
	UI_Sound = CP_Sound_Load("./Songs/MenuButton.wav");

	CP_Sound_PlayAdvanced(MenuMusic, 0.6f, 1.0f, TRUE, Menu_BGM);
	CP_Sound_PlayAdvanced(StageMusic, 0.6f, 1.0f, TRUE, Stage_BGM);
}

void mute(void)
{
	if (CP_Input_KeyTriggered(KEY_M) && mutestate == unmuted)
	{
		CP_Sound_StopGroup(Menu_BGM);
		CP_Sound_StopGroup(Stage_BGM);
		CP_Sound_SetGroupVolume(Sound_Effect, 0.0f);
		CP_Sound_SetGroupVolume(Sound_UI, 0.0f);
		mutestate = muted;
	}
	else if (CP_Input_KeyTriggered(KEY_M) && mutestate == muted)
	{
		CP_Sound_PlayAdvanced(MenuMusic, 0.5f, 1.0f, TRUE, Menu_BGM);
		CP_Sound_PlayAdvanced(StageMusic, 0.5f, 1.0f, TRUE, Stage_BGM);
		CP_Sound_SetGroupVolume(Sound_Effect, 1.0f);
		CP_Sound_SetGroupVolume(Sound_UI, 1.0f);
		mutestate = unmuted;
	}
}
void VolumeControl(void)
{
	if ((CP_Input_KeyTriggered(KEY_EQUAL) || (CP_Input_KeyTriggered(KEY_KP_ADD))) && (vol >= 0.0 && vol < 10.0))
	{
		vol += 0.5;
		CP_Sound_SetGroupVolume(Menu_BGM, vol);
		CP_Sound_SetGroupVolume(Stage_BGM, vol);
	}
	if ((CP_Input_KeyTriggered(KEY_MINUS) || (CP_Input_KeyTriggered(KEY_KP_SUBTRACT))) && ((vol > 10.0 || vol <= 10.0) && vol >= 1.0))
	{
		vol -= 0.5;
		CP_Sound_SetGroupVolume(Menu_BGM, vol);
		CP_Sound_SetGroupVolume(Stage_BGM, vol);
	}

}
void ost_music(void)
{
	//Main Menu Background Music
	if (gamestate == Mainmenu || gamestate == Credits || gamestate == SelectStage || gamestate == Instruction)

	{
		CP_Sound_ResumeGroup(Menu_BGM);
	}
	else
	{
		CP_Sound_PauseGroup(Menu_BGM);
	}

	//Stages Backgroud Music
	if (gamestate == Stage1 || gamestate == Stage2 || gamestate == Stage3)
	{
		CP_Sound_ResumeGroup(Stage_BGM);
	}
	else
	{
		CP_Sound_PauseGroup(Stage_BGM);
	}

}
void music(void)
{
	ost_music();
	mute();
	VolumeControl();
}

void game_init(void)
{
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_H_CENTER);
	CP_Settings_ImageMode(CP_POSITION_CORNER);
	CP_System_SetWindowSize(1600, 900);
	CP_System_SetFrameRate(144);
	LoadImages();
	LoadMusic();
	playerstate = alive;
	gamestate = Mainmenu;
	pausestate = Unpause;
	WinLoseState = UncompleteStage;
	mutestate = unmuted;
	player = CP_Vector_Set(50.f, 870.f);
	InitializeTurretShooting();
	InitializeEnemyPatrolStage3();

}
void game_update(void)
{
	CP_Graphics_ClearBackground(White);
	pause_algorithm();
	StartEndPoint(player.x, player.y);
	music();
	death();

	if (gamestate == Mainmenu)
	{
		Mainmenu_page();
		fade();
	}
	else if (gamestate == SelectStage)
	{
		Selectstage_page();
	}
	else if (gamestate == Credits)
	{
		Credits_page();
	}
	else if (gamestate == Instruction)
	{
		Instruction_page();
	}
	else if (gamestate == Stage1)
	{
		callstage1();
		if (playerstate == alive && pausestate == Unpause)
		{
			movement();
			player = CP_Vector_Add(player, CP_Vector_Scale(movementH, CP_System_GetDt()));
			player = CP_Vector_Add(player, CP_Vector_Scale(movementV, CP_System_GetDt()));
			CP_Settings_Fill(White);
			CP_Graphics_DrawRect(player.x, player.y, player_size, player_size); //Draw Player
		}
		else if (pausestate == Pause)
		{

			if (WinLoseState == CompleteStage && playerstate == alive)
			{
				CP_Settings_Fill(Green);
				CP_Graphics_DrawRect(player.x, player.y, player_size, player_size);
				CompletionPopup();
			}
			else if (WinLoseState == UncompleteStage)
			{
				CP_Settings_Fill(White);
				CP_Graphics_DrawRect(player.x, player.y, player_size, player_size);
				Pausescreen();
			}
			else if (WinLoseState == FailStage && playerstate == dead)
			{
				CP_Settings_Fill(Red);
				CP_Graphics_DrawRect(player.x, player.y, player_size, player_size);
				Failscreen();
			}
		}
	}
	else if (gamestate == Stage2)
	{
		callstage2();
		if (playerstate == alive && pausestate == Unpause)
		{
			movement();
			player = CP_Vector_Add(player, CP_Vector_Scale(movementH, CP_System_GetDt()));
			player = CP_Vector_Add(player, CP_Vector_Scale(movementV, CP_System_GetDt()));
			CP_Settings_Fill(White);
			CP_Graphics_DrawRect(player.x, player.y, player_size, player_size); //Draw Player
		}
		else if (pausestate == Pause)
		{

			if (WinLoseState == CompleteStage)
			{
				CP_Settings_Fill(Green);
				CP_Graphics_DrawRect(player.x, player.y, player_size, player_size);
				CompletionPopup();
			}
			else if (WinLoseState == UncompleteStage)
			{
				CP_Settings_Fill(White);
				CP_Graphics_DrawRect(player.x, player.y, player_size, player_size);
				Pausescreen();
			}
			else if (WinLoseState == FailStage && playerstate == dead)
			{
				CP_Settings_Fill(Red);
				CP_Graphics_DrawRect(player.x, player.y, player_size, player_size);
				Failscreen();
			}
		}
	}
	else if (gamestate == Stage3)
	{
		callstage3();
		if (playerstate == alive && pausestate == Unpause)
		{
			movement();
			player = CP_Vector_Add(player, CP_Vector_Scale(movementH, CP_System_GetDt()));
			player = CP_Vector_Add(player, CP_Vector_Scale(movementV, CP_System_GetDt()));
			CP_Settings_Fill(White);
			CP_Graphics_DrawRect(player.x, player.y, player_size, player_size); //Draw Player
		}
		else if (pausestate == Pause)
		{
			if (WinLoseState == CompleteStage)
			{
				CP_Settings_Fill(Green);
				CP_Graphics_DrawRect(player.x, player.y, player_size, player_size);
				CompletionPopup();
			}
			else if (WinLoseState == UncompleteStage)
			{
				CP_Settings_Fill(White);
				CP_Graphics_DrawRect(player.x, player.y, player_size, player_size);
				Pausescreen();
			}
			else if (WinLoseState == FailStage && playerstate == dead)
			{
				CP_Settings_Fill(Red);
				CP_Graphics_DrawRect(player.x, player.y, player_size, player_size);
				Failscreen();
			}
		}
	}

}
void game_exit(void)
{
	//image
	CP_Image_Free(&StartButton);
	CP_Image_Free(&SelectStageButton);
	CP_Image_Free(&CreditsButton);
	CP_Image_Free(&Stage1Button);
	CP_Image_Free(&Stage2Button);
	CP_Image_Free(&Stage3Button);
	CP_Image_Free(&MainmenuButton);
	CP_Image_Free(&BackgroundWord);
	CP_Image_Free(&MainmenuBackground);
	CP_Image_Free(&LoseScreen);
	CP_Image_Free(&WinScreen);
	CP_Image_Free(&TryAgain);
	CP_Image_Free(&PauseScreen);
	CP_Image_Free(&ResumeButton);
	CP_Image_Free(&InGameBackground);
	CP_Image_Free(&InstructionsPage);
	CP_Image_Free(&InstructionButton);
	CP_Image_Free(&quitbutton);
	CP_Image_Free(&nextbutton);
	CP_Image_Free(&DigipenSGRED);
	CP_Image_Free(&Creditspage);

	//sound
	CP_Sound_Free(&MenuMusic);
	CP_Sound_Free(&StageMusic);
	CP_Sound_Free(&TurretSound);
	CP_Sound_Free(&JumpSound);
	CP_Sound_Free(&UI_Sound);
}

