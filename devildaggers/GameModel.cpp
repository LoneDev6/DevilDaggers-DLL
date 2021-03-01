#pragma once

#include <windows.h>
#include <iostream>

#include "Vector3.h"

//v3.1
class CPlayer {
public:
	char pad_0000[36]; //0x0000
	char sPlayerName[1]; //0x0024
	char pad_0025[119]; //0x0025
	char sVersion[1+4]; //0x009C
	char pad_009D[39-4]; //0x009D
	char sLevelName[1+15]; //0x00C4
	char pad_00C5[203-15]; //0x00C5
	float fTimer; //0x0190
	char pad_0194[16]; //0x0194
	bool bAlive; //0x01A4
	char pad_01A5[31]; //0x01A5
	uint32_t iGems; //0x01C4
	char pad_01C8[140]; //0x01C8
	char sLevelName_2[1+15]; //0x0254
	char pad_0255[383-15]; //0x0255
	char sPlayerName_2[1]; //0x03D4
	char pad_03D5[343]; //0x03D5
	char sPlayerName_3[1]; //0x052C
	char pad_052D[275]; //0x052D
	float N00000190; //0x0640
	char pad_0644[4]; //0x0644
	float N00000195; //0x0648
	float N00000193; //0x064C
	char pad_0650[16]; //0x0650
	float fCamX; //0x0660
	float fCamY; //0x0664
	float fCamZ; //0x0668
	char pad_066C[484]; //0x066C
}; //Size: 0x0850


//v1 (probably also v3.0)
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
