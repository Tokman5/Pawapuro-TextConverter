//�p���v���e�L�X�g�R���o�[�^
//

#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <vector>
#include "PawaCodeConverter.h"
#include "PTCTypes.h"



namespace {
	u8 display_mode = 0; //0 = �����\���@1 = �����\��
	int log_level = 0;	//����R�}���h�̏o�̓��x���@0=���s�A[��l��]�̂݁@1=�ʏ�@2=�S�o��
}

bool searchHelpCommand(size_t argc, char* argv[]) {

	for (size_t i = 0; i < argc; ++i) {
		if ((strcmp("-h", argv[i]) == 0) || (strcmp("--help", argv[i]) == 0)) {
			return true;
		}
	}

	return false;
}

int serarchLogLevelCommand(size_t argc, char* argv[]) {
	int ans = 0;
	for (size_t i = 1; i < argc; ++i) {
		if ((strcmp("-v0", argv[i]) == 0)) {
			ans = 0;
			break;
		}
		else if ((strcmp("-v1", argv[i]) == 0)) {
			ans = 1;
			break;
		}
		else if ((strcmp("-v2", argv[i]) == 0)) {
			ans = 2;
			break;
		}
	}

	return ans;
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
			u16 ans = pcc.PCodeToSJIS(inputchar);

			if (display_mode == 0) {
				std::printf("Output: %04X\n", ans);
			}
			else {
				char anschar[3]{ (ans >> 8),(ans & 0xFF),NULL };
				std::printf("%s\n", anschar);
			}
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
			u16 character = 0;
			for (auto v : inputstring) {
				character += static_cast<u8>(v);
				//1�o�C�g�ڂȂ��ʃo�C�g�ɕۑ�������1�o�C�g�ǂݍ���
				if (bytecount == 0) {
					character = (character << 8);
					++bytecount;
					continue;
				}
				else {	//�����\������
					if (display_mode == 0) {
						std::printf("%04X : %04X\n", character, pcc.SJISToPCode(character));
					}
					else {
						char mulchar[]{ static_cast<u8>(character >> 8),static_cast<u8>(character & 0xFF), NULL };
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


void FileReadMode(std::fstream* file)
{
	int bytecount = 0;
	pawacode::PawaCode pcc(pawacode::Target::pawa10k);
	
	file->seekg(0, std::ios::end);
	size_t size = file->tellg();
	file->seekg(0);
	//std::printf("size: %zX\n", size);

	u8* data = new u8[size];
	file->read(reinterpret_cast<char*>(data), size);

	{
		u16 inputchar = 0;
		u8 buffer = 0;
		int dispcount = 0;
		std::string dispChar;
		int moji_size = 0;
		for (size_t i = 0; i <= size; ++i) {
			buffer = data[i];
			pawacode::FuncState stat = pcc.PCodeToSJIS(buffer, pawacode::TargetGameMode::success, log_level, &dispChar, &moji_size);
			if (stat == pawacode::FuncState::pushedstring) {
				std::printf("%s", dispChar.c_str());
				if (moji_size == -1) {
					std::printf("\n");
					dispcount = 0;
				}

			}

		}
	}

	delete[] data;
}

int main(size_t argc, char* argv[]) {

	if (searchHelpCommand(argc, argv)) {
		std::cerr << R"**(Usage:	Pawapuro-TextConverter.exe
	Pawapuro-TextConverter.exe [FILE] [OPTION]
No Commands Specified: Interactive Mode
OPTION:
-v0 -v1 -v2		Log Level: 0 = Minimal, 1 = Normal, 2 = Verbose)**" << std::endl;
		return 0;
	}

	//��������Ńt�@�C���ǂݍ��݃��[�h
	if (argc >= 2) {
		std::fstream binfile(argv[1], std::ios::in | std::ios::binary);
		if (!binfile) {
			std::fprintf(stderr, "�G���[:�t�@�C�����J�����Ƃ��ł��܂���ł���\nFile: %s\n", argv[1]);
			return 1;
		}
		
		log_level = serarchLogLevelCommand(argc, argv);

		FileReadMode(&binfile);

		if (binfile) {
			binfile.close();
		}

	}
	//���������Ȃ���͎�t���[�h
	else if (argc <= 1)
	{
		int convmode = 0;
		while (true) {
			std::cout <<
"�ϊ����[�h��I��ł��������B\n1:�p���v�������R�[�h���V�t�gJIS\n2:�p���v�������R�[�h���V�t�gJIS(�����\��)\n\
3:�V�t�gJIS���p���v�������R�[�h\n4:�V�t�gJIS(�����\��)���p���v�������R�[�h\n5:�v���O�������I��\n";
			std::cin >> convmode;

			if (std::cin.fail()) {
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cerr << "���̓G���[" << std::endl;
				continue;
			}

			if (convmode == 2) {
				display_mode = 1;
				ToShiftJISMode();
			}
			//SJIS to PawaCode ���[�h
			else if (convmode == 3) {
				display_mode = 0;
				ToPawaCodeMode();
			}
			else if (convmode == 4) {
				display_mode = 1;
				ToPawaCodeMode();
			}
			else if (convmode == 5) {
				break;
			}
			//PawaCode to SJIS ���[�h
			else if (convmode == 1) {
				display_mode = 0;
				ToShiftJISMode();
			}
		}

	}

	return 0;
}

