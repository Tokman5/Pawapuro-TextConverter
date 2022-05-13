#include <string>
#include "PTCTypes.h"
#include "PawaTable.h"
#include "PawaCodeConverter.h"

PawaCodeV2001::PawaCodeV2001(TargetGame target)
	: m_targetgame(target),
	m_number_of_SPchars(0),
	m_commandmode(CommandMode::Normal),
	m_command_StringCount(0),
	m_count_of_command_byte(0),
	m_command_log_level(0),
	m_str_for_command("")
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
	default:						number = numofTBLMAX;	break;
	}
	m_number_of_SPchars = number;
}

inline void PawaCodeV2001::ReInit(TargetGame target)
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

	//JIS��ꐅ���܂�
	if ((sjis >= 0x8140) && (sjis <= 0x9872)) {
		//�_(cell)�Ƌ�(row)�����߂�
		int row = 0;
		int cell = 0;
		
		SJISToRowCell(sjis, row, cell);

		//��_�R�[�h����p���v�������R�[�h�����߂�
		pcode = RowCellToPCode(row, cell);
	}
	//�ǉ���������
	else {
		for (int i = 0; i < m_number_of_SPchars; ++i) {
			if (sjis == specialcharacter[i][0]) {
				pcode = specialcharacter[i][2];
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
	u16 pcode12bit = pcode & 0x0FFF; //�p���v�������R�[�h
	u16 sjis = pcode12bit;

	//���ꕶ������
	if ((pcode12bit >= 0x0E57) && (pcode12bit <= 0x0E56 + m_number_of_SPchars)) {
		for (int i = 0; i < m_number_of_SPchars; ++i) {
			if (pcode12bit == specialcharacter[i][2]) {
				sjis = specialcharacter[i][0];
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


PawaCodeV2001::PCtoSJISFuncState PawaCodeV2001::PCodeToSJIS(const u16 pcode, const TargetMode gamemode, const int log_level, std::string& retstr, int& numofchar)
{
	//�T�N�Z�X���[�h�̃e�L�X�g����
	if (gamemode == TargetMode::success) {
		if ((this->m_commandmode == CommandMode::Command) && (m_count_of_command_byte == 0) && (pcode == 0x0000)) {
			//����Command���[�h���A�ǉ��ǂݍ��݃o�C�g�ł͂Ȃ����A0x0000�Ȃ�ACommandEnd���[�h�ɓ���
			m_commandmode = CommandMode::CommandEnd;
		}
		else if ((m_commandmode == CommandMode::Normal) && (pcode == 0xFFFF)) {
			//����Normal���[�h����0xFFFF�Ȃ�, Command���[�h�ɓ���
			m_commandmode = CommandMode::Command;
			m_str_for_command = "";
			m_command_StringCount = 0;
			m_count_of_command_byte = 0;
			m_command_log_level = 0;
		}
		else if ((m_commandmode == CommandMode::Command) && (m_count_of_command_byte > 0)) {
			//Command���[�h�Œǉ��̃o�C�g��ǂݍ��񂾎�
			if (log_level >= m_command_log_level) {
				//�w�肵�����O���x�����R�}���h�ŗL�̂��̂�荂����Ε\��
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
			//Command���[�h�̎��̏���
			auto it = TBL_successcommand.find(pcode);
			if (it != TBL_successcommand.end()) {
				//�w�肵���R�}���h���e�[�u�����猩���������̏���
				m_count_of_command_byte = std::get<0>(it->second);
				m_command_log_level = std::get<3>(it->second);
				//���͂������O���x�����e�[�u���̂��̂���������Ε����\��
				if (log_level >= m_command_log_level) {
					m_str_for_command += std::get<1>(it->second);
					m_command_StringCount += std::get<2>(it->second);
				}
			}
		}
		else if (m_commandmode == CommandMode::Normal) {
			//Normal���[�h�̎��̏���
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
	//�T�N�Z�X���[�h�ȊO�ł̃e�L�X�g����
	else {
		if ((pcode == 0xFFFF) || (pcode == 0xFFFE)) {
			//0xFFFF�܂���0xFFFE�Ȃ���s
			retstr = "\n";
			numofchar = -1;
		}
		else {
			//����ȊO�Ȃ當���\��
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
	//TODO: sjis ����utf32�ւ̕ϊ�
	//u32 utf32 = SomeConvert(sjis);
	//return utf32;
}

inline void PawaCodeV2001::PCodeToRowCell(u16 pcode, int& row, int& cell)
{
	u16 pcode12bit = pcode & 0x0FFF; //�p���v�������R�[�h

	//����
	if (pcode12bit >= 0x02C2) {
		//��ꐅ����JIS X 0208��16��`47��
		//1��ɂ�94��
		row = (pcode12bit - 0x02C2) / 94 + 16;
		cell = (pcode12bit - 0x02C2) % 94 + 1;
	}
	//�񊿎� 1��`8��
	else {
		row = pcode12bit / 94 + 1;
		cell = pcode12bit % 94 + 1;
	}
}

inline u16 PawaCodeV2001::RowCellToPCode(int row, int cell)
{
	//����
	if (row >= 16) {
		return ((row - 1) * 0x5E) + cell - 0x2C1;
	}
	//�񊿎�
	else {
		return ((row - 1) * 0x5E) + cell - 1;
	}
}


inline void PawaCodeV2001::SJISToRowCell(u16 sjis, int& row, int& cell)
{
	u8 tmplo = (sjis & 0xFF);
	//�_(cell)�Ƌ�(row)�����߂�
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

inline u16 PawaCodeV2001::RowCellToSJIS(int row, int cell)
{
	//��_����V�t�gJIS�R�[�h�����߂�
	u16 answer = (((row + 0x101) >> 1) << 8);
	if (row & 0x01) {
		//�悪��̏ꍇ
		answer += (cell <= 0x3F) ? cell + 0x3F : cell + 0x40;
	}
	else {
		//�悪�����̏ꍇ
		answer += cell + 0x9E;
	}
	return answer;
}