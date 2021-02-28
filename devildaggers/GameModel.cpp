#pragma once

#include <windows.h>
#include <iostream>

#include "Vector3.h"

class CPlayer {
public:
	char pad_0000[96]; //0x0000
	char sPlayerName_idk[4]; //0x0060
	char pad_0064[92]; //0x0064
	char sVersion_idk[4]; //0x00C0
	char pad_00C4[24]; //0x00C4
	char sGameMode2_blankSometimes[4]; //0x00DC
	char pad_00E0[108]; //0x00E0
	int32_t iFOV; //0x014C
	char pad_0150[64]; //0x0150
	float fTimer; //0x0190
	char pad_0194[16]; //0x0194
	bool bAlive; //0x01A4
	char pad_01A5[27]; //0x01A5
	int32_t iGems; //0x01C0
	char pad_01C4[56]; //0x01C4
	int32_t iEnemiesCounter; //0x01FC
	char pad_0200[36]; //0x0200
	char sGameMode[5]; //0x0224
	char pad_0229[308]; //0x0229
	int8_t N000002E8; //0x035D
	bool bReplayMode; //0x035E
	char pad_035F[230]; //0x035F
	char cPlayerName[8]; //0x0445
	char pad_044D[264]; //0x044D
	Vector3 vCamera; //0x0555
	char pad_0561[748]; //0x0561
}; //Size: 0x084D


/*public:
	char pad_0000[400]; //0x0000
	float m_unk_timer; //0x0190
	char pad_0194[12]; //0x0194
	float m_score; //0x01A0
	bool m_alive; //0x01A4
	char pad_01A8[24]; //0x01A8
	uint32_t m_gems; //0x01C0 //not resetting on death
	char pad_01C4[1200]; //0x01C4
}; //Size: 0x0674
*/
