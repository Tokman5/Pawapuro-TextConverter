

#include "PTCTypes.h"
#include "PawaCodeConverter.h"
#include "PawaTable.h"

inline u16 JIStoSJIS(u16 input);

pawacode::PawaCode::PawaCode(pawacode::Target tar) :
	m_target(tar),
	m_SJISbuffer64(0),
	m_SJISbuffer32(0),
	m_SJISbuffer16(0),
	m_SJISbuffer8(0)
{
	u16 number;

	switch (tar)
	{
	case pawacode::Target::pawa8:
		number = numofTBL8;
		break;
	case pawacode::Target::pawa8k:
		number = NULL;
		break;
	case pawacode::Target::pawa9:
		number = NULL;
		break;
	case pawacode::Target::pawa9k:
		number = numofTBL9k;
		break;
	case pawacode::Target::pawa10:
		number = numofTBL10;
		break;
	case pawacode::Target::pawa10k:
		number = numofTBL10k;
		break;
	case pawacode::Target::pawa11:
		number = NULL;
		break;
	case pawacode::Target::pawa11k:
		number = numofTBL11k;
		break;
	case pawacode::Target::pawa12:
		number = NULL;
		break;
	case pawacode::Target::pawa12k:
		number = numofTBL12k;
		break;
	default:
		number = NULL;
		break;
	}
	m_numofSPCharacter = number ;
}

u16 pawacode::PawaCode::PCodeToSJIS(u16 input)
{
	if (input == 0xFFFF) {
		return 0xFFFF;
	}
	u16 input12bit = input & 0x0FFF; //パワプロ文字コード
	u16 answer = input12bit;
	//特殊名前判定
	if (input12bit >= 0x0F00) {

	}
	//特殊文字判定
	else if ((input12bit >= 0x0E57) && (input12bit <= 0x0E56 + m_numofSPCharacter)) {
		for (int i = 0; i < m_numofSPCharacter; ++i) {
			if (input12bit == specialcharacter[i][1]) {
				answer = specialcharacter[i][0];
				break;
			}
		}
	}
	else if (input12bit <= 0x0E56) {
		u16 row = 0;	//区
		u16 cell = 0;	//点

		//漢字
		if (input12bit >= 0x02C2) {
			//第一水準はJIS X 0208の16区～47区
			//1区につき94字
			row = (input12bit - 0x02C2) / 94 + 16;
			cell = (input12bit - 0x02C2) % 94 + 1;
		}
		//非漢字 1区～8区
		else {
			row = input12bit / 94 + 1;
			cell = input12bit % 94 + 1;
		}

		//区点からシフトJISコードを求める
		answer = (((row + 0x101) >> 1) << 8);
		if (row & 0x01) {
			//区が奇数の場合
			answer += (cell <= 0x3F) ? cell + 0x3F : cell + 0x40;
		}
		else {
			//区が偶数の場合
			answer += cell + 0x9E;
		}
	}


	return answer;
}

u16 pawacode::PawaCode::SJISToPCode(u16 input) {
	u16 pcode=input;
//	u8 tmphi = (input >> 8);
	u8 tmplo = (input & 0xFF);

	//JIS第一水準まで
	if ((input >= 0x8140) && (input <= 0x9872)) {
		//点(cell)と区(row)を求める
		u16 row = 0;
		u16 cell = 0;
		u16 odd = 0;


		if ((tmplo >= 0x40) && (tmplo <= 0x7E)) {
			cell = tmplo - 0x1F - 0x20;
			odd = 1;
		}
		else if ((tmplo >= 0x80) && (tmplo <= 0x9E)) {
			cell = tmplo - 0x20 - 0x20;
			odd = 1;
		}
		else {
			cell = tmplo - 0x7E - 0x20;
			odd = 0;
		}

		row = (((input >> 8)-0x70) * 2) - odd - 0x20;

		//区点コードからパワプロ文字コードを求める
		//非漢字
		if (input <= 0x84BE) {
			pcode = ((row - 1) * 0x5E) + cell - 1;
		}
		//漢字
		else {
			pcode = ((row - 1) * 0x5E) + cell - 0x2C1;
		}
	}
	//追加漢字判定
	else {
		for (int i = 0; i < numofTBL12k; ++i) {
			if (input == specialcharacter[i][0]) {
				pcode = specialcharacter[i][1];
				break;
			}
		}
	}
	

	
	return pcode;
}





bool pawacode::PawaCode::ToPawaCode(u16* dest, u8 input) {
	static u8 bytecount=0;

	//下位バイトかどうか判定
	if (bytecount == 0) {
		m_SJISbuffer16 = input;
		++bytecount;
		return false;
	}
	m_SJISbuffer16 = m_SJISbuffer16 + (input << 8);

	*dest = this->SJISToPCode(m_SJISbuffer16);
	m_SJISbuffer16 = 0;
	bytecount = 0;
	return true;
}

