//�p���v���e�L�X�g�R���o�[�^
//

#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include "PawaCodeConverter.h"
#include "PTCTypes.h"


//�t�@�C���|�C���^
std::FILE* fp;


bool searchHelpCommand(size_t argc, char* argv[]) {

	for (size_t i = 0; i < argc; ++i) {
		if ((strcmp("-h", argv[i]) == 0) || (strcmp("--help", argv[i]) == 0)) {
			return true;
		}
	}

	return false;
}


void ToShiftJISMode() 
{
	u16 inputchar;
	pawacode::PawaCode pcc;
	while (true) {

		std::cout << "�p���v�������R�[�h���V�t�gJIS\n�ϊ��������o�C�g����͂��Ă��������B(2�o�C�g,Hex)\n";
		std::cin >> std::hex >> inputchar;
		//���̓G���[����
		if (std::cin.fail()) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cerr << "���̓G���[" << std::endl;
			continue;
		}
		else {

			std::cout << std::hex << inputchar << std::endl;
		}
	}
	return;
}

void ToPawaCodeMode()
{
	std::string inputstring;
	pawacode::PawaCode pcc;
	while (true) {

		std::cout << "�V�t�gJIS���p���v�������R�[�h\n�ϊ���������������͂��Ă��������B(2�o�C�g����)\n";
		std::cin >> inputstring;
		//���̓G���[����
		if (std::cin.fail()) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cerr << "���̓G���[" << std::endl;
			continue;
		}
		else {
			int bytecount = 0;
			u16 character = 0;
			for (auto v : inputstring) {

				character += static_cast<u8>(v);

				if (bytecount == 0) {
					character = (character << 8);
					++bytecount;
					continue;
				}
				else {

					std::cout << std::hex << character << " : " << pcc.ToPawaCode(character) << std::endl;
					character = 0;
					bytecount = 0;
				}

				//std::cout <<std::hex<< v << ":"  << character << std::endl;
			}
		}
	}
	return;
}

int main(size_t argc, char* argv[]) {

	if (searchHelpCommand(argc, argv)) {
		std::cerr << "Usage: Pawapuro-TextConverter.exe [OPTION]" << std::endl;
		return 0;
	}

	for (size_t i = 0; i < argc; ++i) {
		std::printf("%s\n", argv[i]);
	}



//	pawacode::PawaCode pcc;

	
//////////////////////���������Ȃ���͎�t���[�h////////////////////////////
	if (argc <= 1)
	{
		int convmode = 0;
		std::cout << "�ϊ����[�h��I��ł��������B\n1:�p���v�������R�[�h���V�t�gJIS\n2:�V�t�gJIS���p���v�������R�[�h\n";
		std::cin >> convmode;

		//SJIS to PawaCode ���[�h
		if (convmode == 2) {
			ToPawaCodeMode();
		}
		//PawaCode to SJIS ���[�h
		else {
			ToShiftJISMode();
		}

	}

	return 0;
}

