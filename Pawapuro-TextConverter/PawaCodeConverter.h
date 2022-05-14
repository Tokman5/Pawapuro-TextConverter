#pragma once

#include <string>

#include "PTCTypes.h"


class PawaCode {
	public:
		enum class TargetGame {
			pawa7, pawa7k,
			pawa8, pawa8k,
			pawa9, pawa9k,
			pawa10, pawa10k,
			pawa11, pawa11k,
			pawa12, pawa12k,
			pawa2009,
		};
		
		enum class PCtoSJISFuncState {
			normal,
			request_morebytes,
		};

		enum class TargetMode {
			normal,
			success
		};

	public:
		virtual u16 SJISToPCode(u16 sjis) =0;
		virtual u16 PCodeToSJIS(u16 pcode) =0;
		virtual PCtoSJISFuncState PCodeToSJIS(const u16 pcode, const TargetMode gamemode, const int log_level, std::string& retstr, int& numofchar) = 0;
	protected:
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

		
	protected:
		void SJISToRowCell(u16 sjis, int& row, int& cell);
		u16 RowCellToSJIS(int row, int cell);
};

class PawaCodeV2001 :public PawaCode {				//パワプロ8～2009用
	public:

	public:
		PawaCodeV2001(TargetGame target);
		void ReInit(TargetGame target);
		u16 SJISToPCode(u16 sjis) override;
		u16 PCodeToSJIS(u16 pcode) override;		//サクセス構文を解釈しない
		PCtoSJISFuncState PCodeToSJIS(const u16 pcode, const TargetMode gamemode, const int log_level, std::string& retstr, int& numofchar) override;	//ゲームモードを指定するとサクセス構文を解釈する
		u32 PCodeToUTF32(u16 pcode);

	protected:
		TargetGame m_targetgame;
		int m_number_of_SPchars;


	protected:
		void PCodeToRowCell(u16 pcode, int& row, int& cell) const;
		u16 RowCellToPCode(int row, int cell) const;
};

class PawaCodeV2000 :public PawaCode{				//パワプロ7用
	public:
		PawaCodeV2000(TargetGame target);
		PawaCodeV2000() :PawaCodeV2000(TargetGame::pawa7k) {};
		u16 SJISToPCode(u16 sjis) override;
		u16 PCodeToSJIS(u16 pcode) override;		//サクセス構文を解釈しない
		PCtoSJISFuncState PCodeToSJIS(const u16 pcode, const TargetMode gamemode, const int log_level, std::string& retstr, int& numofchar) override;

	protected:
		bool m_isKetteiban;
		int m_number_of_SPchars;

	protected:
		bool PCodeToRowCell(u16 pcode, int& row, int& cell) const;	//隙間にある文字かどうかチェック falseで隙間にある文字
		u16 RowCellToPCode(int row, int cell) const;
};