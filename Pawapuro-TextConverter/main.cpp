//�p���v���e�L�X�g�R���o�[�^
//

#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <vector>
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
			u16 ans = pcc.PCodeToSJIS(inputchar);

			if (Displaymode == 0) {
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
					if (Displaymode == 0) {
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
			/*
			if (bytecount == 0 ){
				buffer = data[i];
				++bytecount;
				continue;
			}
			else {
				inputchar = buffer + (data[i] << 8);
				u16 ans = pcc.PCodeToSJIS(inputchar);
				if ((ans & 0x0FFF) >= 0x0FFE) {
					std::printf("\n");
					dispcount = 0;
				}
				else {
					u8 anschar[3]{ (ans >> 8),(ans & 0xFF),NULL };
					std::printf("%s", anschar);
					++dispcount;
					if (dispcount >= 18) {
						std::printf("\n");
						dispcount = 0;
					}
				}

			}
			bytecount = 0;
			inputchar = 0;
			buffer = 0;
			*/

			buffer = data[i];
			pawacode::FuncState stat = pcc.PCodeToSJIS(buffer, pawacode::TargetGameMode::success, &dispChar, &moji_size);
			if (stat == pawacode::FuncState::pushedstring) {
				std::printf("%s", dispChar.c_str());
				//dispcount += moji_size;
				//if ((moji_size == -1) || (dispcount >= 18)) {
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
		fprintf(stderr, "Usage: Pawapuro-TextConverter.exe [OPTION]\n" );
		return 0;
	}

	//��������Ńt�@�C���ǂݍ��݃��[�h
	if (argc == 2) {
		std::fstream binfile(argv[1], std::ios::in | std::ios::binary);
		if (!binfile) {
			std::fprintf(stderr, "�G���[:�t�@�C�����J�����Ƃ��ł��܂���ł���\nFile: %s\n", argv[1]);
			return 1;
		}
		
		FileReadMode(&binfile);

		if (binfile) {
			binfile.close();
		}

	}
	//���������Ȃ���͎�t���[�h
	else if (argc <= 1)
	{
		int convmode = 0;
		std::cout << "�ϊ����[�h��I��ł��������B\n1:�p���v�������R�[�h���V�t�gJIS\n2:�p���v�������R�[�h���V�t�gJIS(�����\��)\n\
3:�V�t�gJIS���p���v�������R�[�h\n4:�V�t�gJIS(�����\��)���p���v�������R�[�h\n";
		std::cin >> convmode;

		if (convmode == 2) {
			Displaymode = 1;
			ToShiftJISMode();
		}
		//SJIS to PawaCode ���[�h
		else if (convmode == 3) {
			Displaymode = 0;
			ToPawaCodeMode();
		}
		else if (convmode == 4) {
			Displaymode = 1;
			ToPawaCodeMode();
		}
		//PawaCode to SJIS ���[�h
		else {
			Displaymode = 0;
			ToShiftJISMode();
		}

	}

	return 0;
}

