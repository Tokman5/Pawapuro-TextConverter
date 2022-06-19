#include <string>
#include <string_view>
#include "PTCTypes.h"
#include "PawaTable.h"
#include "PawaCodeConverter.h"


inline void PawaCode::SJISToRowCell(u16 sjis, int& row, int& cell)
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

inline u16 PawaCode::RowCellToSJIS(int row, int cell)
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

PawaCodeV2001::PawaCodeV2001(TargetGame target)
	: m_targetgame(target),
	m_number_of_SPchars(0)
{
	m_targetmode = TargetMode::normal;
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
	u16 pcode12bit = pcode & 0x0FFF; //�p���v�������R�[�h
	if (pcode12bit == 0x0FFF) {
		return 0x0FFF;
	}
	else if (pcode12bit == 0x0FFE) {
		return 0x0FFE;
	}
	u16 sjis = pcode12bit;

	//���ꕶ������
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


PawaCode::PCtoSJISFuncState PawaCodeV2001::PCodeToSJIS(const u16 pcode, const int log_level, std::string& retstr, int& numofchar)
{
	//�T�N�Z�X���[�h�̃e�L�X�g����
	if (m_targetmode == TargetMode::success) {
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
			auto it = TBL_successcommandV2001.find(pcode);
			if (it != TBL_successcommandV2001.end()) {
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
			//CommandEnd���[�h
			retstr = std::move(m_str_for_command);
			numofchar = m_command_StringCount;
			m_count_of_command_byte = 0;
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
			retstr = std::move(std::string{ static_cast<char>(ans >> 8), static_cast<char>(ans & 0xFF) });
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

inline void PawaCodeV2001::PCodeToRowCell(u16 pcode, int& row, int& cell) const
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

inline u16 PawaCodeV2001::RowCellToPCode(int row, int cell) const
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


PawaCodeV2000::PawaCodeV2000(TargetGame target)
{
	m_targetmode = TargetMode::normal;
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
			if (sjis == specialcharacterV2000[i][0]) {
				return specialcharacterV2000[i][2];

			}
		}
	}

	return pcode;
}

//�p���v��7�����R�[�h���V�t�gJIS
u16 PawaCodeV2000::PCodeToSJIS(u16 pcode)
{
	if (pcode == 0xFFFF) {
		return 0xFFFF;
	}
	u16 pcode12bit = pcode & 0x0FFF; //�p���v�������R�[�h
	u16 sjis = pcode12bit;
	bool isNormalChar = false;

	//�ʏ핶���͈̔͂��ǂ���
	if ((pcode12bit <= 0x007F) ||
		((pcode12bit >= 0x00CF) && (pcode12bit <= 0x0216)) ||
		((pcode12bit >= 0x035F) &&(pcode12bit <= 0x0F32))) {
		int row, cell;
		isNormalChar = PCodeToRowCell(pcode12bit, row, cell);
		if (isNormalChar) {
			//�ʏ�̕��т̕����̏���
			sjis = RowCellToSJIS(row, cell);

		}
	}
	
	if(!isNormalChar) {
		//���ꕶ���̏���
		for (int i = 0; i < m_number_of_SPchars; ++i) {
			if (pcode12bit == specialcharacterV2000[i][2]) {
				return specialcharacterV2000[i][0];

			}
		}
		sjis = specialcharacterV2000[30][0];
	}

	return sjis;
}

PawaCode::PCtoSJISFuncState PawaCodeV2000::PCodeToSJIS(const u16 pcode, const int log_level, std::string& retstr, int& numofchar)
{
	//�T�N�Z�X���[�h�w�莞�̏���
	if ((m_targetmode == TargetMode::success)) {
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
			auto it = TBL_successcommandV2000.find(pcode);
			if (it != TBL_successcommandV2000.end()) {
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
			retstr = std::move(m_str_for_command);
			numofchar = m_command_StringCount;
			m_count_of_command_byte = 0;
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
			retstr = std::move(std::string{ static_cast<char>(ans >> 8), static_cast<char>(ans & 0xFF) });
			numofchar = 1;
		}
		return PCtoSJISFuncState::normal;
	}
}

inline bool PawaCodeV2000::PCodeToRowCell(u16 pcode, int& row, int& cell) const
{
	u16 pcode12bit = pcode & 0x0FFF; //�p���v�������R�[�h

	int tmprow, tmpcell;
	//����
	if (pcode12bit >= 0x035F) {
		//��ꐅ����JIS X 0208��16��`47��
		tmprow = (pcode12bit - 0x035F) / 0xC0;
		tmpcell = (pcode12bit - 0x035F) % 0xC0;
	

		if (0x61 <= tmpcell) {
			//�悪��̎�
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
			//�悪�����̎�
			if (tmpcell >= 0x5E) {
				return false;
			}

			row = tmprow * 2 + 16;
			cell = tmpcell + 1;
		}

	}
	//�񊿎� 1��`8��
	else {
		tmprow = pcode12bit / 0xC0 + 1;
		tmpcell = pcode12bit % 0xC0;

		if (0x5E >= tmpcell) {
			//�悪��̎�
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
			//�悪�����̎�
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
		//�悪��Ȃ�
		if (cell <= 0x3F) {
			pcode += (cell - 1);
		}
		else {
			pcode += cell;
		}
	}
	else {
		//�悪�����Ȃ�
		pcode += (cell + 0x5E);
	}
	return pcode;
}



//�p���v��9�`�T�N�Z�X�p
size_t PawaCodeV2002::CompressArray(const std::vector<u16>& row_source, std::vector<u16>& target)
{
	target.clear();

	size_t index = 0;
	target.reserve(std::ceil( row_source.size() * 0.75 ));

	for (size_t i = 0, size = row_source.size(); i < size; i++) {
		switch (i % 4) {
			case 0:
				target.emplace_back(row_source[i] << 4);
				index++;
				break;
			case 1:
				target[index - 1] += ((row_source[i] >> 8) & 0xF);
				target.emplace_back(row_source[i] << 8);
				index++;
				break;
			case 2:
				target[index - 1] += (row_source[i] & 0x0FF0) >> 4;
				target.emplace_back((row_source[i] & 0xF) << 12);
				index++;
				break;
			case 3:
				target[index - 1] += (row_source[i] & 0x0FFF);
				break;
			default:
				break;
		}
	}

	return index;
}

bool PawaCodeV2002::DecompressArray(const std::vector<u16>& compressed, std::vector<u16>& target)
{
	target.clear();

	size_t index = 0; //�𓀌�̗v�f��
	bool remainder = false;

	for (size_t i = 0, size = compressed.size(); i < size; i++) {
		switch (i % 3) {
			case 0:
				target.emplace_back((compressed[i] >> 4) & 0x0FFF);
				target.emplace_back((compressed[i] & 0xF) << 8);
				index += 1;
				remainder = true;
				break;
			case 1:
				target[index] += (compressed[i] >> 8) & 0xFF;
				target.emplace_back((compressed[i] & 0xFF) << 4);
				index += 1;
				remainder = true;
				break;
			case 2:
				target[index] += (compressed[i] >> 12) & 0xF;
				target.emplace_back(compressed[i] & 0x0FFF);
				index += 2;
				remainder = false;
				break;
			default:
				break;
		}

	}

	return remainder;
}

PawaCodeV2002::PawaCodeV2002(TargetGame game)
	:PawaCodeV2001(game)
{
	m_commandmode = CommandMode::Command;
	m_compressedArray.clear();

	switch (game)
	{
	case PawaCode::TargetGame::pawa9:
	case PawaCode::TargetGame::pawa9k:
		break;
	case PawaCode::TargetGame::pawa10:
	case PawaCode::TargetGame::pawa10k:
		m_ptr_commandTBL = &TBL_successcommandV2003;
		break;
	case PawaCode::TargetGame::pawa11:
	case PawaCode::TargetGame::pawa11k:
		break;
	case PawaCode::TargetGame::pawa12:
	case PawaCode::TargetGame::pawa12k:
		break;
	case PawaCode::TargetGame::pawa2009:
		break;
	default:
		break;
	}

	m_ptr_commandTBL = &TBL_successcommandV2003; // temp
}

PawaCode::PCtoSJISFuncState PawaCodeV2002::PCodeToSJIS(const u16 pcode, const int log_level, std::string& retstr, int& numofchar)
{
	//�T�N�Z�X���[�h�̃e�L�X�g����
	if (m_targetmode == TargetMode::success) {
		if (this->m_count_of_command_byte > 0) {
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
		else {
			//�ǉ��̓ǂݍ��݃o�C�g�������Ƃ�

			if ((this->m_commandmode == CommandMode::Command)) {
				//����Command���[�h�̎�
				switch (pcode) {
					//case 0x0000:
					case 0x0100:
					case 0x0200:
					case 0x0300:
					case 0x0400:
					case 0x0500:
					case 0x0600:
					case 0x0700:
						//�R�}���h���[�h�I���
						retstr = std::move(m_str_for_command);
						numofchar = m_command_StringCount;
						m_count_of_command_byte = 0;
						m_commandmode = CommandMode::Normal;
						return PCtoSJISFuncState::normal;

						break;
					default:
						//�R�}���h����
						auto it = m_ptr_commandTBL->find(pcode);
						if (it != m_ptr_commandTBL->end()) {			//�w�肵���R�}���h���e�[�u�����猩���������̏���
							m_count_of_command_byte = std::get<0>(it->second);
							m_command_log_level = std::get<3>(it->second);
							if (log_level >= m_command_log_level) {		//���͂������O���x�����e�[�u���̂��̂���������Ε����\��
								m_str_for_command += std::get<1>(it->second);
								m_command_StringCount += std::get<2>(it->second);
							}
						}
						return PCtoSJISFuncState::request_morebytes;
						break;
				}
			}
			else if ((this->m_commandmode == CommandMode::Normal) && (pcode & 0x0FFF) == 0x0FFF) {	//Normal���[�h����R�}���h���[�h�ɓ��鎞�̏���
				retstr.clear();
				std::vector<u16> row_array;
				row_array.reserve(std::ceil(m_compressedArray.size() * 1.5));

				if (pcode != 0xFFFF) {
					if (m_compressedArray.size() % 3 == 1) {
						m_compressedArray.emplace_back(pcode & 0xFF00) ;
					}
					else if (m_compressedArray.size() % 3 == 2) {
						m_compressedArray.emplace_back(pcode & 0xF000);
					}
				}

				if (PawaCodeV2002::DecompressArray(m_compressedArray, row_array)){	//���k�o�C�g��̉�
					row_array.pop_back();
				}

				for (const auto& v : row_array) {		//���o�C�g�񂩂當���ւ̕ϊ�
					if ((v & 0x0FFF) == 0x0FFE) {		//0xFFE�Ȃ���s
						retstr += '\n';
					}
					else {								//�����łȂ���Ε����ɕϊ�
						u16 sjis = PawaCodeV2001::PCodeToSJIS(v);
						retstr.push_back((sjis >> 8) & 0xFF);
						retstr.push_back(sjis & 0xFF);
					}
				}

				m_commandmode = CommandMode::Command;
				numofchar = row_array.size();
				m_compressedArray.clear();
				m_str_for_command = "";
				m_command_StringCount = 0;
				m_count_of_command_byte = 0;
				m_command_log_level = 0;
				return PCtoSJISFuncState::normal;
			}
			else {
				//Normal���[�h���̏���
				m_compressedArray.emplace_back(pcode);	//Command���[�h�ɓ���܂Ńo�b�t�@�ɗ��߂�

			}

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
			u16 ans = PawaCodeV2001::PCodeToSJIS(pcode);
			retstr = std::move(std::string{ static_cast<char>(ans >> 8), static_cast<char>(ans & 0xFF) });
			numofchar = 1;
		}
		return PCtoSJISFuncState::normal;
	}

}