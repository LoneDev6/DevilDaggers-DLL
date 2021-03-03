#pragma once

#define HERO_PTR_ADDRESS 0x228978;


#include <windows.h>
#include <iostream>

#include "Vector3.h"

template <class T>
class GameDataStructure
{
public:
	static T* init(const intptr_t addr)
	{
		return *reinterpret_cast<T**>(addr);
	}
};

#pragma pack(push, 4) //"pack" is used to fix wrong offsets
class Hero : public GameDataStructure<Hero>
{
public:
	char pad_0000[20]; //0x0000
	char sPlayerName1[32]; //0x0014
	float fSeconds; //0x0034
	int32_t iGems; //0x0038
	char pad_003C[16]; //0x003C
	int64_t iGems2; //0x004C
	char pad_0054[8]; //0x0054
	int64_t iKilledEnemies; //0x005C
	int64_t iCounter_skull_1_HUGE_PEAK_BUG; //0x0064
	char pad_006C[68]; //0x006C
	char sPlayerName2[32]; //0x00B0
	char pad_00D0[224]; //0x00D0
	char sPlayerName3[32]; //0x01B0
	char pad_01D0[88]; //0x01D0
	char sGameVersion[8]; //0x0228
	char pad_0230[32]; //0x0230
	char sLevelName[8]; //0x0250
	char pad_0258[64]; //0x0258
	bool bTopDownCamera; //0x0298
	char pad_0299[47]; //0x0299
	float fPickedUpGemTimer_maybe; //0x02C8
	char pad_02CC[32]; //0x02CC
	float fSkyLight; //0x02EC
	char pad_02F0[44]; //0x02F0
	float fTimeSinceSceneStart_unscaled; //0x031C
	char pad_0320[12]; //0x0320
	float fTimeSinceSceneStart_scaled; //0x032C
	bool bAlive; //0x0330
	char pad_0331[3]; //0x0331
	int32_t iGems3; //0x0334
	char pad_0338[88]; //0x0338
	int32_t iCounterEnemiesAndSpawners; //0x0390
	int32_t iCounter_skull_1_HUGE_PEAK_BUG_2; //0x0394
	char pad_0398[36]; //0x0398
	float fTilesShrinkTime; //0x03BC
	char pad_03C0[32]; //0x03C0
	char sLevelName_2[8]; //0x03E0
	char pad_03E8[152]; //0x03E8
	char sLeaderboardFirstUsername[32]; //0x0480
	char sLeaderboardSecondUsername[32]; //0x04A0
	class unk_leaderboardEntry *pLeaderboardStuff; //0x04C0
	char pad_04C8[40]; //0x04C8
	class unk_leaderboardEntry_2 *pLeaderboardStuff_2; //0x04F0
	char pad_04F8[104]; //0x04F8
	char sPlayerName4[8]; //0x0560
	char pad_0568[40]; //0x0568
	class DownloadMessage *pDownloadMessage; //0x0590
	char pad_0598[288]; //0x0598
	char sPlayerName5[8]; //0x06B8
	char pad_06C0[24]; //0x06C0
	class SteamStuff *pSteamStuff; //0x06D8
	char pad_06E0[268]; //0x06E0
	float fCamX; //0x07EC
	float fCamY; //0x07F0
	float fCamZ; //0x07F4
}; //Size: 0x07F8

class unk_leaderboardEntry_2
{
public:
	char sUsername[32]; //0x0000
}; //Size: 0x0020

class unk_leaderboardEntry
{
public:
	char sUsername[32]; //0x0000
}; //Size: 0x0020

class DownloadMessage
{
public:
	char message[32]; //0x0000
}; //Size: 0x0020

class SteamStuff
{
public:
	char sSteamUserID[17]; //0x0000
}; //Size: 0x0011


#pragma pack(pop)//"pack" is used to fix wrong offsets
