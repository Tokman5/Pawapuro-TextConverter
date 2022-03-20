#pragma once
#include "PTCTypes.h"

constexpr u16 numofTBL8 = 39;		//�g�܂�
constexpr u16 numofTBL9k = 48;		//���܂�
constexpr u16 numofTBL10 = 49;		//糂܂�
constexpr u16 numofTBL10k = 49;		//糂܂�
constexpr u16 numofTBL11k = 53;		//�Y�܂�
constexpr u16 numofTBL12k = 57;		//��܂�

//���ꕶ���e�[�u���A8�`12��
// [SJIS][�p�������R�[�h]
static const u16 specialcharacter[][2]{ 
	//�E �� �N
	{0xE245,0x0E57} , {0x9AE3,0x0E58} , {0x9F4E ,0x0E59},
	//�� �A �t �L
	{0x9980,0x0E5A}, {0x9C41,0x0E5B}, {0xE474 ,0x0E5C},{0xE94C,0x0E5D},
	//�F � �V ��
	{0xE446,0x0E5E},{0xE1A9,0x0E5F},{0xE056,0x0E60},{0x9E8A,0x0E61},
	//�� �� �� ��
	{0x9B89,0x0E62},{0x9AE6,0x0E63},{0xE1C1,0x0E64},{0x9EB9,0x0E65},
	//� � �� ��
	{0xE1BC,0x0E66},{0xE298,0x0E67},{0xE6E2,0x0E68},{0x98A5,0x0E69},
	//�_ �w �� ��
	{0xE05F,0x0E6A},{0x9E77,0x0E6B},{0x9FBA,0x0E6C},{0xE0F8,0x0E6D},
	//�� � �M �S
	{0x98EC,0x0E6E},{0xEAA0,0x0E6F},{0xE54D,0x0E70},{0xE153,0x0E71},
	//� �� �� ��
	{0xE3A9,0x0E72},{0xFAB1,0x0E73},{0x9DEE,0x0E74},{0xFBFC,0x0E75},
	//�i (��) (�w���) �� �� (��)
	{0xE469,0x0E76},{0x81A8,0x0E77},{0x81A8,0x0E78},{0x9BDE,0x0E79},{0xE2C0,0x0E7A},{0x90A8,0x0E7B},
	//�a (�g) � ��
	{0xFB61,0x0E7C},{0x8B67,0x0E7D},{0xE7B2,0x0E7E},{0x9FD3,0x0E7F},
	//�� �� �z �I
	{0xE1D2,0x0E80},{0xFB84,0x0E81},{0x997A,0x0E82},{0x9B49,0x0E83},
	//�� �m �� �
	{0xE3C4,0x0E84},{0xE66D,0x0E85},{0x9AA0,0x0E86},{0xE7B3,0x0E87},
	//�� � �� �Y
	{0xE4DD,0x0E88},{0xE7AF,0x0E89},{0xE095,0x0E8A},{0xFB59,0x0E8B},
	//� �� �� ��
	{0xE2A8,0x0E8C},{0x9BB8,0x0E8D},{0xE9CD,0x0E8E},{0xE8E8,0x0E8F}
};