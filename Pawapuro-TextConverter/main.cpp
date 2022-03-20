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

//�V�t�gJIS���p���v�������R�[�h���[�h
namespace {
	u8 Displaymode = 0;
}

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
	pawacode::PawaCode pcc(pawacode::Target::pawa12k);

	std::cout << "\n�p���v�������R�[�h���V�t�gJIS";
	while (true) {

		std::cout << "\n�ϊ��������o�C�g����͂��Ă��������B(2�o�C�g,Hex,0xFFFF�ŏI��)\n";
		std::cin >> std::hex >> inputchar;
		//���̓G���[����
		if (std::cin.fail()) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cerr << "���̓G���[" << std::endl;
			continue;
		}
		else {
			if (inputchar == 0xFFFF) {
				break;
			}

			std::printf( "Output: %04X\n", pcc.PCodeToSJIS(inputchar));
		}
	}
	return;
}

void ToPawaCodeMode()
{
	std::string inputstring;
	pawacode::PawaCode pcc(pawacode::Target::pawa12k);

	std::cout << "\n�V�t�gJIS���p���v�������R�[�h";
	while (true) {

		std::cout << "\n�ϊ���������������͂��Ă��������B(2�o�C�g�����Aend�ŏI��)\n";
		std::cin >> inputstring;
		//���̓G���[����
		if (std::cin.fail()) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cerr << "���̓G���[" << std::endl;
			continue;
		}
		else {
			if (inputstring == "end") {
				break;
			}
			//��������
			int bytecount = 0;
			u32 character = 0;
			for (auto v : inputstring) {
				character += static_cast<u8>(v);
				//1�o�C�g�ڂȂ��ʃo�C�g�ɕۑ�������1�o�C�g�ǂݍ���
				if (bytecount == 0) {
					character = (character << 8);
					++bytecount;
					continue;
				}
				else {	//�����\������
					if (Displaymode == 0) {
						std::printf("%04X : %04X\n", character, pcc.SJISToPCode(character));
					}
					else {
						char mulchar[]{ static_cast<char>(character >> 8),static_cast<char>(character & 0xFF), NULL };
						std::printf("%s : %04X\n", mulchar, pcc.SJISToPCode(character));
					}


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
		fprintf(stderr, "Usage: Pawapuro-TextConverter.exe [OPTION]\n" );
		return 0;
	}

//���������Ȃ���͎�t���[�h
	if (argc <= 1)
	{
		int convmode = 0;
		std::cout << "�ϊ����[�h��I��ł��������B\n1:�p���v�������R�[�h���V�t�gJIS\n2:�V�t�gJIS���p���v�������R�[�h\n3:�V�t�gJIS(�����\��)���p���v�������R�[�h\n";
		std::cin >> convmode;

		//SJIS to PawaCode ���[�h
		if (convmode == 2) {
			Displaymode = 0;
			ToPawaCodeMode();
		}
		else if (convmode == 3) {
			Displaymode = 1;
			ToPawaCodeMode();
		}
		//PawaCode to SJIS ���[�h
		else {
			ToShiftJISMode();
		}

	}

	return 0;
}

