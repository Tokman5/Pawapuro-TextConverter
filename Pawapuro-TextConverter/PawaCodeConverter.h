#pragma once

#include <string>

#include "PTCTypes.h"

enum class TargetGame {
	pawa7, pawa7k,
	pawa8, pawa8k,
	pawa9, pawa9k,
	pawa10, pawa10k,
	pawa11, pawa11k,
	pawa12, pawa12k,
	pawa2009,
};

class PawaCode {

};

class PawaCodeV2001 {
	public:
		enum class PCtoSJISFuncState {
			normal,
			request_morebytes,
		};

		enum class TargetMode {
			normal,
			success
		};

	public:
		PawaCodeV2001(TargetGame target);
		void ReInit(TargetGame target);
		u16 SJISToPCode(u16 sjis);
		u16 PCodeToSJIS(u16 pcode);		//サクセス構文を解釈しない
		PCtoSJISFuncState PCodeToSJIS(const u16 pcode, const TargetMode gamemode, const int log_level, std::string& retstr, int& numofchar);	//ゲームモードを指定するとサクセス構文を解釈する
		u32 PCodeToUTF32(u16 pcode);

	private:
		TargetGame m_targetgame;
		int m_number_of_SPchars;

		//サクセス構文用
		enum class CommandMode {
			Normal,
			Command,
			CommandEnd,
			Mode_null
		}m_commandmode;
		int m_command_StringCount;
		int m_count_of_command_byte;
		int m_command_log_level;
		std::string m_str_for_command;

	private:
		void PCodeToRowCell(u16 pcode, int& row, int& cell);
		u16 RowCellToPCode(int row, int cell);
		void SJISToRowCell(u16 sjis, int& row, int& cell);
		u16 RowCellToSJIS(int row, int cell);
};