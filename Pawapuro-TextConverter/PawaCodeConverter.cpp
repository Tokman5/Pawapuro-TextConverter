#include <string>
#include <unordered_map>
#include <tuple>
#include "PTCTypes.h"
#include "PawaCodeConverter.h"
#include "PawaTable.h"

//inline u16 JIStoSJIS(u16 input);

pawacode::PawaCode::PawaCode(pawacode::Target tar) :
	m_target(tar),
	m_funcstate(FuncState::request_LObyte),
	m_cvtbuffer32(0),
	m_commandmode(pawacode::PawaCode::CommandMode::Normal),
	m_SJISbuffer16(0)
{
	u16 number;

	switch (tar)
	{
	case pawacode::Target::pawa8:		number = numofTBL8;		break;
	case pawacode::Target::pawa8k:		number = NULL;			break;
	case pawacode::Target::pawa9:		number = NULL;			break;
	case pawacode::Target::pawa9k:		number = numofTBL9k;	break;
	case pawacode::Target::pawa10:		number = numofTBL10;	break;
	case pawacode::Target::pawa10k:		number = numofTBL10k;	break;
	case pawacode::Target::pawa11:		number = NULL;			break;
	case pawacode::Target::pawa11k:		number = numofTBL11k;	break;
	case pawacode::Target::pawa12:		number = NULL;			break;
	case pawacode::Target::pawa12k:		number = numofTBL12k;	break;
	default:							number = NULL;			break;
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

	//特殊文字判定
	if ((input12bit >= 0x0E57) && (input12bit <= 0x0E56 + m_numofSPCharacter)) {
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

pawacode::FuncState pawacode::PawaCode::PCodeToSJIS(const u8 input, const pawacode::TargetGameMode target, const int loglvl, std::string* ret, int* numofRealChar)
{
	using namespace pawacode;

	//サクセスモードのテキスト処理
	if (target == TargetGameMode::success) {
		static std::string str_for_command{};
		static int command_StringCount = 0;
		static int count_of_more_u16 = 0;
		static int last_log_level = 0;
		//下位バイト待ちの時
		if ((this->m_funcstate == FuncState::request_LObyte) || (this->m_funcstate == FuncState::pushedstring)) {
			m_cvtbuffer32 = input;
			m_funcstate = FuncState::request_HIbyte;
			return FuncState::request_HIbyte;
		}
		//上位バイト待ちの時
		else if (this->m_funcstate == FuncState::request_HIbyte) {
			m_cvtbuffer32 += (static_cast<u16>(input) << 8);

			if ((m_commandmode == CommandMode::Command) && (count_of_more_u16 == 0) && (m_cvtbuffer32 == 0x0000)) {
				//もしCommandモードかつ、追加読み込みバイトではなくかつ、0x0000なら、CommandEndモードに入る
				m_commandmode = CommandMode::CommandEnd;

			}
			else if ((m_commandmode == CommandMode::Normal) && (static_cast<u16>(m_cvtbuffer32) == 0xFFFF)) {
				//もしNormalモードかつ0xFFFFなら, Commandモードに入る
				m_commandmode = CommandMode::Command;
				str_for_command = "";
				command_StringCount = 0;
				count_of_more_u16 = 0;
			}
			else if ((m_commandmode == CommandMode::Command) && (count_of_more_u16 > 0)) {
				//Commandモードで追加のバイトを読み込んだ時
				if (loglvl >= last_log_level) {
					char conv[10];
					std::snprintf(conv, 10, "%04X", m_cvtbuffer32);
					str_for_command += conv;
					if (count_of_more_u16 == 1) {
						str_for_command += "]";
					}
					else {
						str_for_command += ":";
					}
				}
				--count_of_more_u16;
			}
			else if (m_commandmode == CommandMode::Command) {
				//Commandモードの時の処理
				auto it = TBL_successcommand.find(static_cast<u16>(m_cvtbuffer32));
				if (it != TBL_successcommand.end()) {
					//指定したコマンドがテーブルから見つかった時の処理
					count_of_more_u16 = std::get<0>(it->second);
					last_log_level = std::get<3>(it->second);
					//入力したログレベルがテーブルのものよりも高ければ文字表示
					if (loglvl >= last_log_level) {
						str_for_command += std::get<1>(it->second);
						command_StringCount += std::get<2>(it->second);
					}
				}
				else {
					//未登録の時の処理


				}



			}
			else if (m_commandmode == CommandMode::Normal) {
				//Normalモードの時の処理
				u16 ans = PCodeToSJIS(static_cast<u16>(m_cvtbuffer32));
				*ret = std::string{ static_cast<char>(ans >> 8), static_cast<char>(ans & 0xFF) };
				*numofRealChar = 1;
				m_funcstate = FuncState::request_LObyte;
				m_cvtbuffer32 = 0;
				return FuncState::pushedstring;
			}
			else if (m_commandmode == CommandMode::CommandEnd) {
				//CommandEndモードの時の処理 Commandモードで解釈した文字列を送出する
				*ret =  str_for_command;
				*numofRealChar = command_StringCount;
				str_for_command.clear();
				command_StringCount = 0;
				count_of_more_u16 = 0;
				m_commandmode = CommandMode::Normal;
				m_funcstate = FuncState::request_LObyte;
				return FuncState::pushedstring;
			}

			m_funcstate = FuncState::request_LObyte;
			return FuncState::request_LObyte;
		}
	}


	//サクセスモード以外でのテキスト処理
	else {
		//下位バイト待ちの時
		if ((this->m_funcstate == FuncState::request_LObyte) || (this->m_funcstate == FuncState::pushedstring)) {
			m_cvtbuffer32 = input;
			m_funcstate = FuncState::request_HIbyte;
			return FuncState::request_HIbyte;
		}
		//上位バイト待ちの時
		else if (this->m_funcstate == FuncState::request_HIbyte) {
			m_cvtbuffer32 += (input << 8);

			if ((static_cast<u16>(m_cvtbuffer32) == 0xFFFF)|| (static_cast<u16>(m_cvtbuffer32) == 0xFFFE)) {
				//0xFFFFまたは0xFFFEなら改行
				*ret = "\n";
				*numofRealChar = -1;
				m_funcstate = FuncState::request_LObyte;
				return FuncState::pushedstring;
			}
			else {
				//それ以外なら文字表示
				u16 ans = PCodeToSJIS(static_cast<u16>(m_cvtbuffer32));
				*ret = std::string{ static_cast<char>(ans >> 8), static_cast<char>(ans & 0xFF) };
				*numofRealChar = 1;
				m_funcstate = FuncState::request_LObyte;
				m_cvtbuffer32 = 0;
				return FuncState::pushedstring;
			}

		
		}
	}
}