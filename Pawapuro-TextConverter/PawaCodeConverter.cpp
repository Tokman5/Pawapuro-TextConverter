#include <string>
#include <string_view>
#include "PTCTypes.h"
#include "PawaTable.h"
#include "PawaCodeConverter.h"


inline void PawaCode::SJISToRowCell(u16 sjis, int& row, int& cell)
{
	u8 tmplo = (sjis & 0xFF);
	//点(cell)と区(row)を求める
	int odd = 0;


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

	row = (((sjis >> 8) - 0x70) * 2) - odd - 0x20;
}

inline u16 PawaCode::RowCellToSJIS(int row, int cell)
{
	//区点からシフトJISコードを求める
	u16 answer = (((row + 0x101) >> 1) << 8);
	if (row & 0x01) {
		//区が奇数の場合
		answer += (cell <= 0x3F) ? cell + 0x3F : cell + 0x40;
	}
	else {
		//区が偶数の場合
		answer += cell + 0x9E;
	}
	return answer;
}

PawaCodeV2001::PawaCodeV2001(TargetGame target)
	: m_targetgame(target),
	m_number_of_SPchars(0)
{
	m_commandmode = CommandMode::Normal;
	m_command_StringCount = 0;
	m_count_of_command_byte = 0;
	m_command_log_level = 0;
	m_str_for_command = "";

	int number;

	switch (target)
	{
	case TargetGame::pawa8:			number = numofTBL8;		break;
	case TargetGame::pawa8k:		number = numofTBL8k;	break;
	case TargetGame::pawa9:			number = numofTBL9;		break;
	case TargetGame::pawa9k:		number = numofTBL9k;	break;
	case TargetGame::pawa10:		number = numofTBL10;	break;
	case TargetGame::pawa10k:		number = numofTBL10k;	break;
	case TargetGame::pawa11:		number = numofTBL11;	break;
	case TargetGame::pawa11k:		number = numofTBL11k;	break;
	case TargetGame::pawa12:		number = numofTBL12;	break;
	case TargetGame::pawa12k:		number = numofTBL12k;	break;
	case TargetGame::pawa2009:		number = numofTBL2009;	break;
	default:						number = numofTBLMAX;	break;
	}
	m_number_of_SPchars = number;
}

void PawaCodeV2001::ReInit(TargetGame target)
{
	int number;

	switch (target)
	{
		case TargetGame::pawa8:			number = numofTBL8;		break;
		case TargetGame::pawa8k:		number = numofTBL8k;	break;
		case TargetGame::pawa9:			number = numofTBL9;		break;
		case TargetGame::pawa9k:		number = numofTBL9k;	break;
		case TargetGame::pawa10:		number = numofTBL10;	break;
		case TargetGame::pawa10k:		number = numofTBL10k;	break;
		case TargetGame::pawa11:		number = numofTBL11;	break;
		case TargetGame::pawa11k:		number = numofTBL11k;	break;
		case TargetGame::pawa12:		number = numofTBL12;	break;
		case TargetGame::pawa12k:		number = numofTBL12k;	break;
		case TargetGame::pawa2009:		number = numofTBL2009;	break;
		default:						number = numofTBL2009;	break;
	}
	m_number_of_SPchars = number;
	m_targetgame = target;

}

u16 PawaCodeV2001::SJISToPCode(u16 sjis)
{
	u16 pcode = sjis;

	//JIS第一水準まで
	if ((sjis >= 0x8140) && (sjis <= 0x9872)) {
		//点(cell)と区(row)を求める
		int row = 0;
		int cell = 0;
		
		SJISToRowCell(sjis, row, cell);

		//区点コードからパワプロ文字コードを求める
		pcode = RowCellToPCode(row, cell);
	}
	//追加漢字判定
	else {
		for (int i = 0; i < m_number_of_SPchars; ++i) {
			if (sjis == specialcharacterV2001[i][0]) {
				pcode = specialcharacterV2001[i][2];
				break;
			}
		}
	}

	return pcode;
}

u16 PawaCodeV2001::PCodeToSJIS(u16 pcode)
{
	if (pcode == 0xFFFF) {
		return 0xFFFF;
	}
	u16 pcode12bit = pcode & 0x0FFF; //パワプロ文字コード
	u16 sjis = pcode12bit;

	//特殊文字判定
	if ((pcode12bit >= 0x0E57) && (pcode12bit <= 0x0E56 + m_number_of_SPchars)) {
		for (int i = 0; i < m_number_of_SPchars; ++i) {
			if (pcode12bit == specialcharacterV2001[i][2]) {
				sjis = specialcharacterV2001[i][0];
				break;
			}
		}
	}
	else if (pcode12bit <= 0x0E56) {
		int row, cell;
		PCodeToRowCell(pcode12bit, row, cell);
		sjis = RowCellToSJIS(row, cell);
	}

	return sjis;
}


PawaCode::PCtoSJISFuncState PawaCodeV2001::PCodeToSJIS(const u16 pcode, const TargetMode gamemode, const int log_level, std::string& retstr, int& numofchar)
{
	//サクセスモードのテキスト処理
	if (gamemode == TargetMode::success) {
		if ((this->m_commandmode == CommandMode::Command) && (m_count_of_command_byte == 0) && (pcode == 0x0000)) {
			//もしCommandモードかつ、追加読み込みバイトではなくかつ、0x0000なら、CommandEndモードに入る
			m_commandmode = CommandMode::CommandEnd;
		}
		else if ((m_commandmode == CommandMode::Normal) && (pcode == 0xFFFF)) {
			//もしNormalモードかつ0xFFFFなら, Commandモードに入る
			m_commandmode = CommandMode::Command;
			m_str_for_command = "";
			m_command_StringCount = 0;
			m_count_of_command_byte = 0;
			m_command_log_level = 0;
		}
		else if ((m_commandmode == CommandMode::Command) && (m_count_of_command_byte > 0)) {
			//Commandモードで追加のバイトを読み込んだ時
			if (log_level >= m_command_log_level) {
				//指定したログレベルがコマンド固有のものより高ければ表示
				char conv[10];
				std::snprintf(conv, 10, "%04X", pcode);
				m_str_for_command += conv;
				if (m_count_of_command_byte == 1) {
					m_str_for_command += "]";
				}
				else {
					m_str_for_command += ":";
				}
			}
			--m_count_of_command_byte;
		}
		else if (m_commandmode == CommandMode::Command) {
			//Commandモードの時の処理
			auto it = TBL_successcommandV2001.find(pcode);
			if (it != TBL_successcommandV2001.end()) {
				//指定したコマンドがテーブルから見つかった時の処理
				m_count_of_command_byte = std::get<0>(it->second);
				m_command_log_level = std::get<3>(it->second);
				//入力したログレベルがテーブルのものよりも高ければ文字表示
				if (log_level >= m_command_log_level) {
					m_str_for_command += std::get<1>(it->second);
					m_command_StringCount += std::get<2>(it->second);
				}
			}
		}
		else if (m_commandmode == CommandMode::Normal) {
			//Normalモードの時の処理
			u16 ans = PCodeToSJIS(pcode);
			retstr = std::string{ static_cast<char>(ans >> 8), static_cast<char>(ans & 0xFF) };
			numofchar = 1;
			return PCtoSJISFuncState::normal;
		}
		else if (m_commandmode == CommandMode::CommandEnd) {
			//CommandEndモード
			retstr = m_str_for_command;
			numofchar = m_command_StringCount;
			m_count_of_command_byte = 0;
			m_str_for_command = "";
			m_commandmode = CommandMode::Normal;
			return PCtoSJISFuncState::normal;
		}
		return PCtoSJISFuncState::request_morebytes;
	}
	//サクセスモード以外でのテキスト処理
	else {
		if ((pcode == 0xFFFF) || (pcode == 0xFFFE)) {
			//0xFFFFまたは0xFFFEなら改行
			retstr = "\n";
			numofchar = -1;
		}
		else {
			//それ以外なら文字表示
			u16 ans = PCodeToSJIS(pcode);
			retstr = std::string{ static_cast<char>(ans >> 8), static_cast<char>(ans & 0xFF) };
			numofchar = 1;
		}
		return PCtoSJISFuncState::normal;
	}
}


u32 PawaCodeV2001::PCodeToUTF32(u16 pcode)
{
	if (pcode == 0xFFFF) {
		return 0xFFFFFFFF;
	}
	u16 pcode12bit = pcode & 0x0FFF;
	u16 sjis;
	switch (pcode12bit) {
		case 0x0E7B:	return 0x351F;
		case 0x0E7D:	return 0x20BB7;
		case 0x0E94:	return 0x2665;
		case 0x0E99:	return 0xF929;
		default:		sjis = PCodeToSJIS(pcode12bit);
	}
	//TODO: sjis からutf32への変換
	//u32 utf32 = SomeConvert(sjis);
	//return utf32;
}

inline void PawaCodeV2001::PCodeToRowCell(u16 pcode, int& row, int& cell) const
{
	u16 pcode12bit = pcode & 0x0FFF; //パワプロ文字コード

	//漢字
	if (pcode12bit >= 0x02C2) {
		//第一水準はJIS X 0208の16区～47区
		//1区につき94字
		row = (pcode12bit - 0x02C2) / 94 + 16;
		cell = (pcode12bit - 0x02C2) % 94 + 1;
	}
	//非漢字 1区～8区
	else {
		row = pcode12bit / 94 + 1;
		cell = pcode12bit % 94 + 1;
	}
}

inline u16 PawaCodeV2001::RowCellToPCode(int row, int cell) const
{
	//漢字
	if (row >= 16) {
		return ((row - 1) * 0x5E) + cell - 0x2C1;
	}
	//非漢字
	else {
		return ((row - 1) * 0x5E) + cell - 1;
	}
}


PawaCodeV2000::PawaCodeV2000(TargetGame target)
{
	m_isKetteiban = true;
	m_number_of_SPchars = numofTBL7k;
	m_commandmode = CommandMode::Normal;
	m_command_StringCount = 0;
	m_count_of_command_byte = 0;
	m_command_log_level = 0;
	m_str_for_command = "";

	switch (target)
	{
	case PawaCode::TargetGame::pawa7:
		m_isKetteiban = false;
		m_number_of_SPchars = numofTBL7;
		break;
	case PawaCode::TargetGame::pawa7k:
		m_number_of_SPchars = numofTBL7k;
		break;
	default:
		break;
	}
}

u16 PawaCodeV2000::SJISToPCode(u16 sjis)
{
	u16 pcode = sjis;

	//JIS第一水準まで
	if ((sjis >= 0x8140) && (sjis <= 0x9872)) {
		//点(cell)と区(row)を求める
		int row = 0;
		int cell = 0;

		SJISToRowCell(sjis, row, cell);

		//区点コードからパワプロ文字コードを求める
		pcode = RowCellToPCode(row, cell);
	}
	//追加漢字判定
	else {
		for (int i = 0; i < m_number_of_SPchars; ++i) {
			if (sjis == specialcharacterV2000[i][0]) {
				return specialcharacterV2000[i][2];

			}
		}
	}

	return pcode;
}

//パワプロ7文字コード→シフトJIS
u16 PawaCodeV2000::PCodeToSJIS(u16 pcode)
{
	if (pcode == 0xFFFF) {
		return 0xFFFF;
	}
	u16 pcode12bit = pcode & 0x0FFF; //パワプロ文字コード
	u16 sjis = pcode12bit;
	bool isNormalChar = false;

	//通常文字の範囲かどうか
	if ((pcode12bit <= 0x007F) ||
		((pcode12bit >= 0x00CF) && (pcode12bit <= 0x0216)) ||
		((pcode12bit >= 0x035F) &&(pcode12bit <= 0x0F32))) {
		int row, cell;
		isNormalChar = PCodeToRowCell(pcode12bit, row, cell);
		if (isNormalChar) {
			//通常の並びの文字の処理
			sjis = RowCellToSJIS(row, cell);

		}
	}
	
	if(!isNormalChar) {
		//特殊文字の処理
		for (int i = 0; i < m_number_of_SPchars; ++i) {
			if (pcode12bit == specialcharacterV2000[i][2]) {
				return specialcharacterV2000[i][0];

			}
		}
		sjis = specialcharacterV2000[30][0];
	}

	return sjis;
}

PawaCode::PCtoSJISFuncState PawaCodeV2000::PCodeToSJIS(const u16 pcode, const TargetMode gamemode, const int log_level, std::string& retstr, int& numofchar)
{
	//サクセスモード指定時の処理
	if ((gamemode == TargetMode::success)) {
		if ((this->m_commandmode == CommandMode::Command) && (m_count_of_command_byte == 0) && (pcode == 0x0000)) {
			//もしCommandモードかつ、追加読み込みバイトではなくかつ、0x0000なら、CommandEndモードに入る
			m_commandmode = CommandMode::CommandEnd;
		}
		else if ((m_commandmode == CommandMode::Normal) && (pcode == 0xFFFF)) {
			//もしNormalモードかつ0xFFFFなら, Commandモードに入る
			m_commandmode = CommandMode::Command;
			m_str_for_command = "";
			m_command_StringCount = 0;
			m_count_of_command_byte = 0;
			m_command_log_level = 0;
		}
		else if ((m_commandmode == CommandMode::Command) && (m_count_of_command_byte > 0)) {
			//Commandモードで追加のバイトを読み込んだ時
			if (log_level >= m_command_log_level) {
				//指定したログレベルがコマンド固有のものより高ければ表示
				char conv[10];
				std::snprintf(conv, 10, "%04X", pcode);
				m_str_for_command += conv;
				if (m_count_of_command_byte == 1) {
					m_str_for_command += "]";
				}
				else {
					m_str_for_command += ":";
				}
			}
			--m_count_of_command_byte;
		}
		else if (m_commandmode == CommandMode::Command) {
			//Commandモードの時の処理
			auto it = TBL_successcommandV2000.find(pcode);
			if (it != TBL_successcommandV2000.end()) {
				//指定したコマンドがテーブルから見つかった時の処理
				m_count_of_command_byte = std::get<0>(it->second);
				m_command_log_level = std::get<3>(it->second);
				//入力したログレベルがテーブルのものよりも高ければ文字表示
				if (log_level >= m_command_log_level) {
					m_str_for_command += std::get<1>(it->second);
					m_command_StringCount += std::get<2>(it->second);
				}
			}
		}
		else if (m_commandmode == CommandMode::Normal) {
			//Normalモードの時の処理
			u16 ans = PCodeToSJIS(pcode);
			retstr = std::string{ static_cast<char>(ans >> 8), static_cast<char>(ans & 0xFF) };
			numofchar = 1;
			return PCtoSJISFuncState::normal;
		}
		else if (m_commandmode == CommandMode::CommandEnd) {
			retstr = m_str_for_command;
			numofchar = m_command_StringCount;
			m_count_of_command_byte = 0;
			m_str_for_command = "";
			m_commandmode = CommandMode::Normal;
			return PCtoSJISFuncState::normal;
		}
		return PCtoSJISFuncState::request_morebytes;
	}
	//サクセスモード以外でのテキスト処理
	else {
		if ((pcode == 0xFFFF) || (pcode == 0xFFFE)) {
			//0xFFFFまたは0xFFFEなら改行
			retstr = "\n";
			numofchar = -1;
		}
		else {
			//それ以外なら文字表示
			u16 ans = PCodeToSJIS(pcode);
			retstr = std::string{ static_cast<char>(ans >> 8), static_cast<char>(ans & 0xFF) };
			numofchar = 1;
		}
		return PCtoSJISFuncState::normal;
	}
}

inline bool PawaCodeV2000::PCodeToRowCell(u16 pcode, int& row, int& cell) const
{
	u16 pcode12bit = pcode & 0x0FFF; //パワプロ文字コード

	int tmprow, tmpcell;
	//漢字
	if (pcode12bit >= 0x035F) {
		//第一水準はJIS X 0208の16区～47区
		tmprow = (pcode12bit - 0x035F) / 0xC0;
		tmpcell = (pcode12bit - 0x035F) % 0xC0;
	

		if (0x61 <= tmpcell) {
			//区が奇数の時
			tmpcell -= 0x61;
			if (tmpcell <= 0x3E) {
				++tmpcell;
			}
			else if (tmpcell == 0x3F) {
				return false;

			}

			row = tmprow * 2 + 17;
			cell = tmpcell;
		}
		else {
			//区が偶数の時
			if (tmpcell >= 0x5E) {
				return false;
			}

			row = tmprow * 2 + 16;
			cell = tmpcell + 1;
		}

	}
	//非漢字 1区～8区
	else {
		tmprow = pcode12bit / 0xC0 + 1;
		tmpcell = pcode12bit % 0xC0;

		if (0x5E >= tmpcell) {
			//区が奇数の時
			if (tmpcell <= 0x3E) {
				++tmpcell;
			}
			else if (tmpcell == 0x3F) {
				return false;

			}

			row = tmprow * 2 - 1;
			cell = tmpcell;
		}
		else {
			//区が偶数の時
			tmpcell -= 0x5F;

			if (tmpcell >= 0xBD) {
				return false;
			}

			row = tmprow * 2;
			cell = tmpcell + 1;
		}


	}


	return true;
}

inline u16 PawaCodeV2000::RowCellToPCode(int row, int cell) const
{
	u16 pcode = ((row - 1) / 2) * 0xC0;
	if (row >= 16) {
		pcode -= 0x0240;
	}

	row &= 1;

	if (row) {
		//区が奇数なら
		if (cell <= 0x3F) {
			pcode += (cell - 1);
		}
		else {
			pcode += cell;
		}
	}
	else {
		//区が偶数なら
		pcode += (cell + 0x5E);
	}
	return pcode;
}