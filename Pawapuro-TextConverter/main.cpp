//�p���v���e�L�X�g�R���o�[�^
//

#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <memory>
#include <vector>
#include "PawaCodeConverter.h"
#include "PTCTypes.h"



namespace {
	int menu_page = 0;	//���j���[��ʂ̃y�[�W�i���o�[
	u8 display_mode = 0; //0 = �����\���@1 = �����\���A2 = �o�C�g��
	int log_level = 0;	//����R�}���h�̏o�̓��x���@0=���s�A[��l��]�̂݁@1=�ʏ�@2=�S�o��
	bool compress_mode = false;
	PawaCode::TargetGame target_game = PawaCode::TargetGame::pawa2009;
	PawaCode::TargetMode target_mode = PawaCode::TargetMode::normal;

	int errstatus = 0;
}

namespace {
	const char* const usagestring = R"**(
-----------------------------------------------
Usage:	Pawapuro-TextConverter.exe
	Pawapuro-TextConverter.exe [FILE] [OPTION]
No Commands Specified: Interactive Mode
OPTION:

-v0 -v1 -v2           Log Level: 0 = Minimal, 1 = Normal, 2 = Verbose
-g <TARGET> -t <TARGET>
--game <TARGET>
--target <TARGET>     Target Game: pawa7 pawa7k
                                   pawa8 pawa8k pawa9 pawa9k
                                   pawa10 pawa10k pawa11 pawa11k
                                   pawa12 pawa12k pawa2009
-m <GAMEMODE>
--mode <GAMEMODE>       Game Mode: normal success
-----------------------------------------------)**";

	const char* menu_string[] = {"\n Page 1/3  �p���v��8�`2009�p\n 1 : �p���v�������R�[�h���V�t�gJIS\n 2 : �p���v�������R�[�h���V�t�gJIS(�����\��)\n\
 3 : �V�t�gJIS���p���v�������R�[�h\n 4 : �V�t�gJIS���p���v�������R�[�h(�����\��)\n 5 : �V�t�gJIS���p���v�������R�[�h(�o�C�g��)\n 6 : �V�t�gJIS���p���v�������R�[�h(�o�C�g��,�Í���)\n\n\
 8 : ���̃y�[�W��\n 9 : �v���O�������I��\n",
"\n Page 2/3  �p���v��7�p\n 1 : �p���v�������R�[�h���V�t�gJIS\n 2 : �p���v�������R�[�h���V�t�gJIS(�����\��)\n\
 3 : �V�t�gJIS���p���v�������R�[�h\n 4 : �V�t�gJIS���p���v�������R�[�h(�����\��)\n 5 : �V�t�gJIS���p���v�������R�[�h(�o�C�g��)\n\n\
 7 : �O�̃y�[�W��\n 8 : ���̃y�[�W��\n 9 : �v���O�������I��\n",
"\n Page 3/3\n 1 : �t�@�C���ǂݍ��݃��[�h�w���v\n\n 7 : �O�̃y�[�W��\n\n 9 : �v���O�������I��\n"};
}

bool searchHelpCommand(size_t argc, char* argv[]) {

	for (size_t i = 0; i < argc; ++i) {
		if ((strcmp("-h", argv[i]) == 0) || (strcmp("--help", argv[i]) == 0)|| (strcmp("/h", argv[i]) == 0)) {
			return true;
		}
	}

	return false;
}

int searchLogLevelCommand(size_t argc, char* argv[]) {
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

bool searchTargetPawaGame(size_t argc, char* argv[]) {
	for (size_t i = 1; i < argc - 1; ++i) {
		if ((strcmp("-t", argv[i]) == 0)|| (strcmp("--target", argv[i]) == 0) ||
			(strcmp("-g", argv[i]) == 0) || (strcmp("--game", argv[i]) == 0)) {
				if ((strcmp("7", argv[i + 1]) == 0) || (strcmp("pawa7", argv[i + 1]) == 0)) { target_game = PawaCode::TargetGame::pawa7; }
				else if ((strcmp("7k", argv[i + 1]) == 0) || (strcmp("pawa7k", argv[i + 1]) == 0)) { target_game = PawaCode::TargetGame::pawa7k; }
				else if ((strcmp("8", argv[i + 1]) == 0) || (strcmp("pawa8", argv[i + 1]) == 0)) { target_game = PawaCode::TargetGame::pawa8; }
				else if ((strcmp("8k", argv[i + 1]) == 0) || (strcmp("pawa8k", argv[i + 1]) == 0)) { target_game = PawaCode::TargetGame::pawa8k; }
				else if ((strcmp("9", argv[i + 1]) == 0) || (strcmp("pawa9", argv[i + 1]) == 0)) { target_game = PawaCode::TargetGame::pawa9; }
				else if ((strcmp("9k", argv[i + 1]) == 0) || (strcmp("pawa9k", argv[i + 1]) == 0)) { target_game = PawaCode::TargetGame::pawa9k; }
				else if ((strcmp("10", argv[i + 1]) == 0) || (strcmp("pawa10", argv[i + 1]) == 0)) { target_game = PawaCode::TargetGame::pawa10; }
				else if ((strcmp("10k", argv[i + 1]) == 0) || (strcmp("pawa10k", argv[i + 1]) == 0)) { target_game = PawaCode::TargetGame::pawa10k; }
				else if ((strcmp("11", argv[i + 1]) == 0) || (strcmp("pawa11", argv[i + 1]) == 0)) { target_game = PawaCode::TargetGame::pawa11; }
				else if ((strcmp("11k", argv[i + 1]) == 0) || (strcmp("pawa11k", argv[i + 1]) == 0)) { target_game = PawaCode::TargetGame::pawa11k; }
				else if ((strcmp("12", argv[i + 1]) == 0) || (strcmp("pawa12", argv[i + 1]) == 0)) { target_game = PawaCode::TargetGame::pawa12; }
				else if ((strcmp("12k", argv[i + 1]) == 0) || (strcmp("pawa12k", argv[i + 1]) == 0)) { target_game = PawaCode::TargetGame::pawa12k; }
				else if ((strcmp("2009", argv[i + 1]) == 0) || (strcmp("pawa2009", argv[i + 1]) == 0)) { target_game = PawaCode::TargetGame::pawa2009; }
				else {
					errstatus = i + 1;
					return false;
				}
			break;
		}
	}
	return true;
}

bool searchTargetGameMode(size_t argc, char* argv[]) {
	for (size_t i = 1; i < argc - 1; ++i) {
		if ((strcmp("-m", argv[i]) == 0) || (strcmp("--mode", argv[i]) == 0)) {
			if ((strcmp("normal", argv[i + 1]) == 0)|| (strcmp("none", argv[i + 1]) == 0)) {
				target_mode = PawaCodeV2001::TargetMode::normal;
			}
			else if ((strcmp("success", argv[i + 1]) == 0)) {
				target_mode = PawaCodeV2001::TargetMode::success;
			}
			else {
				errstatus = i + 1;
				return false;
			}
			break;
		}
	}
	return true;
}

void ToShiftJISMode() 
{
	u16 inputchar;
	std::unique_ptr<PawaCode> pcc = nullptr;
	if ((target_game == PawaCode::TargetGame::pawa7) || (target_game == PawaCode::TargetGame::pawa7k)) {
		pcc = std::make_unique<PawaCodeV2000>(PawaCodeV2000(target_game));
	}
	else {
		pcc = std::make_unique<PawaCodeV2001>(PawaCodeV2001(target_game));
	}

	pcc->SetTargetMode(PawaCode::TargetMode::normal);

	std::cout << "\n�p���v�������R�[�h���V�t�gJIS";
	while (true) {

		std::cout << "\n�ϊ��������o�C�g����͂��Ă��������B(2�o�C�g,Hex,0xFFFF�Ŗ߂�)\n";
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
			u16 ans = pcc->PCodeToSJIS(inputchar);

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
	std::unique_ptr<PawaCode> pcc = nullptr;
	if ((target_game == PawaCode::TargetGame::pawa7) || (target_game == PawaCode::TargetGame::pawa7k)) {
		pcc = std::make_unique<PawaCodeV2000>(PawaCodeV2000(target_game));
	}
	else {
		pcc = std::make_unique<PawaCodeV2001>(PawaCodeV2001(target_game));
	}

	pcc->SetTargetMode(PawaCode::TargetMode::normal);

	std::cout << "\n�V�t�gJIS���p���v�������R�[�h";
	while (true) {

		std::cout << "\n�ϊ���������������͂��Ă��������B(�S�p�����A���pend�Ŗ߂�)\n";
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
			int encrypt_charcount = 0;
			std::vector<u16> row_array;
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
						std::printf("%04X : %04X\n", character, pcc->SJISToPCode(character));
					}
					else if (display_mode == 2) {	//�o�C�g��\�����[�h
						u16 buf = pcc->SJISToPCode(character);
						std::printf("%02X %02X ", buf & 0xFF, buf >> 8);
						//std::printf("%02X%02X ", buf >> 8, buf & 0xFF); //�r�b�O�G���f�B�A��Debug�p
					}
					else {							//�����\�����[�h
						char mulchar[]{ static_cast<u8>(character >> 8),static_cast<u8>(character & 0xFF), '\0' };
						std::printf("%s : %04X\n", mulchar, pcc->SJISToPCode(character));
					}

					if (display_mode == 2 && compress_mode == true) {	//�o�C�g��\�����[�h(���k)
						row_array.emplace_back( pcc->SJISToPCode(character));
					}

					character = 0;
					bytecount = 0;
				}
			}

			if (display_mode == 2 && compress_mode == true) { //���k���[�h���A���k&�\������
				std::vector<u16> compressed_array;
				PawaCodeV2002::CompressArray(row_array, compressed_array);

				size_t disp_count = 0;

				std::printf("\n");

				for (auto&& v : compressed_array) {
					std::printf("%02X %02X ", v & 0xFF, v >> 8);
					//std::printf("%02X%02X ", v >> 8, v & 0xFF); //�r�b�O�G���f�B�A�� Debug�p

					if (disp_count >= 2) {
						std::printf(" ");
						disp_count = 0;
					}
					else { 
						disp_count++; 
					}
				}
				
				row_array.clear();
			}
		}
	}

	pcc.release();
	return;
}


int FileReadMode(char* path)
{
	int bytecount = 0;
	std::unique_ptr<PawaCode> pcc = nullptr;
	if ((target_game == PawaCode::TargetGame::pawa7) || (target_game == PawaCode::TargetGame::pawa7k)) {
		pcc = std::make_unique<PawaCodeV2000>(PawaCodeV2000(target_game));
	}
	else if ((target_game == PawaCode::TargetGame::pawa8) || (target_game == PawaCode::TargetGame::pawa8k)) {
		pcc = std::make_unique<PawaCodeV2001>(PawaCodeV2001(target_game));
	}
	else {
		pcc = std::make_unique<PawaCodeV2002>(PawaCodeV2002(target_game));
	}

	pcc->SetTargetMode(target_mode);
	
	//�t�@�C���I�[�v��
	std::fstream file(path, std::ios::in | std::ios::binary);
	if (!file) {
		std::fprintf(stderr, "�G���[:�t�@�C�����J�����Ƃ��ł��܂���ł���\nFile: %s\n", path);
		return 1;
	}
	
	file.seekg(0, std::ios::end);
	size_t size = file.tellg();
	file.seekg(0);
	//std::printf("size: %zX\n", size);

	//�f�[�^�R�s�[
	u8* data = new u8[size];
	file.read(reinterpret_cast<char*>(data), size);

	if (file) {
		file.close();
	}

	//�ϊ�����
	{
		u16 inputchar = 0;
		u8 buffer = 0;
		int dispcount = 0;
		std::string dispChar;
		int moji_size = 0;
		for (size_t i = 0; i <= size; ++i) {
			buffer = data[i];
			if (!(i & 1)) {
				//�ǂݍ��݃o�C�g��1�o�C�g�ڂȂ烋�[�v��i�߂Ă���1�o�C�g�ǂݍ���
				inputchar = buffer;
				continue;
			}

			inputchar += (buffer << 8);

			PawaCode::PCtoSJISFuncState stat = pcc->PCodeToSJIS(inputchar, log_level, dispChar, moji_size);

			if (stat == PawaCode::PCtoSJISFuncState::normal) {
				std::printf("%s", dispChar.c_str());

				if (target_mode == PawaCode::TargetMode::success) {
					//temp
				}
				else {
					dispcount += moji_size;
					if ((moji_size == -1) || (dispcount % 36 == 0)) {
						std::printf("\n");
						dispcount = 0;
					}
				}
			}

		}
	}

	delete[] data;
	return 0;
}

int main(size_t argc, char* argv[]) 
{
	if (searchHelpCommand(argc, argv)) {
		std::fprintf(stderr,"%s\n", usagestring);
		return 0;
	}

	//��������Ńt�@�C���ǂݍ��݃��[�h
	if (argc >= 2) {

		log_level = searchLogLevelCommand(argc, argv);

		if (!searchTargetPawaGame(argc, argv)) {
			std::fprintf(stderr, "�G���[�F�����ȍ�i�p�����[�^�F%s",argv[errstatus]);
			return 1;
		}

		if (!searchTargetGameMode(argc, argv)) {
			std::fprintf(stderr, "�G���[�F�����ȃQ�[�����[�h�F%s", argv[errstatus]);
			return 1;
		}

		return FileReadMode(argv[1]);
		

	}

	//���������Ȃ���͎�t���[�h
	else if (argc <= 1)
	{
		std::printf("�p���v�������R�[�h�R���o�[�^\n");
		int convmode = 0;
		while (true) {
			std::printf("%s", menu_string[menu_page]);
			std::cin >> convmode;

			if (std::cin.fail()) {
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cerr << "���̓G���[" << std::endl;
				continue;
			}

			//9�����͂��ꂽ��v���O�������I������
			if (convmode == 9) {
				break;
			}

			//1�y�[�W�ڂ̏���
			if (menu_page == 0) {
				compress_mode = false;
				target_game = PawaCode::TargetGame::pawa2009;
				if (convmode == 1) {
					display_mode = 0;
					ToShiftJISMode();
				}
				else if (convmode == 2) {
					display_mode = 1;
					ToShiftJISMode();
				}
				else if (convmode == 3) {
					display_mode = 0;
					ToPawaCodeMode();
				}
				else if (convmode == 4) {
					display_mode = 1;
					ToPawaCodeMode();
				}
				else if (convmode == 5) {
					display_mode = 2;
					ToPawaCodeMode();
				}
				else if (convmode == 6) {
					display_mode = 2;
					compress_mode = true;
					ToPawaCodeMode();
				}
				else if (convmode == 8) {
					++menu_page;
				}
			}
			//2�y�[�W�ڂ̏���
			else if (menu_page == 1) {
				compress_mode = false;
				target_game = PawaCode::TargetGame::pawa7k;
				if (convmode == 1) {
					display_mode = 0;
					ToShiftJISMode();
				}
				else if (convmode == 2) {
					display_mode = 1;
					ToShiftJISMode();
				}
				else if (convmode == 3) {
					display_mode = 0;
					ToPawaCodeMode();
				}
				else if (convmode == 4) {
					display_mode = 1;
					ToPawaCodeMode();
				}
				else if (convmode == 5) {
					display_mode = 2;
					ToPawaCodeMode();
				}
				else if (convmode == 7) {
					--menu_page;
				}
				else if (convmode == 8) {
					++menu_page;
				}
			}
			//3�y�[�W�ڂ̏���
			else if (menu_page == 2) {
				if (convmode == 1) {
					printf("%s\n", usagestring);
				}
				else if (convmode == 7) {
					--menu_page;
				}
				/*else if (convmode == 8) {
					++menu_page;
				}*/
			}
		}

	}

	return 0;
}

