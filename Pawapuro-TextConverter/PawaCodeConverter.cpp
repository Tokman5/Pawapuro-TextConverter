

#include "PTCTypes.h"
#include "PawaCodeConverter.h"
#include "PawaTable.h"

pawacode::PawaCode::PawaCode(){};


u16 pawacode::PawaCode::ToShiftJIS(u16 input)
{
	if (input == 0xFFFF) {
		return 0xFFFF;
	}

	u16 input12bit = input & 0x0FFF; //文字コード
	return input12bit;
}

u16 pawacode::PawaCode::ToPawaCode(u16 input) {
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
			}
		}
	}
	

	
	return pcode;
}





bool pawacode::PawaCode::ToPawaCode(u16* dest, u8 input) {
	static u8 bytecount=0;

	//下位バイトかどうか判定
	if (bytecount == 0) {
		SJISbuffer16 = input;
		++bytecount;
		return false;
	}
	SJISbuffer16 = SJISbuffer16 + (input << 8);

	*dest = this->ToPawaCode(SJISbuffer16);
	SJISbuffer16 = 0;
	bytecount = 0;
	return true;
}