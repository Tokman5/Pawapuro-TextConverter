//パワプロテキストコンバータ
//

#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <vector>
#include "PawaCodeConverter.h"
#include "PTCTypes.h"



namespace {
	u8 display_mode = 0; //0 = 数字表示　1 = 文字表示、2 = バイト列
	int log_level = 0;	//特殊コマンドの出力レベル　0=改行、[主人公]のみ　1=通常　2=全出力
	pawacode::Target target_game;
	pawacode::TargetGameMode target_gamemode;
}

bool searchHelpCommand(size_t argc, char* argv[]) {

	for (size_t i = 0; i < argc; ++i) {
		if ((strcmp("-h", argv[i]) == 0) || (strcmp("--help", argv[i]) == 0)) {
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

pawacode::Target searchTargetPawaGame(size_t argc, char* argv[]) {
	using namespace pawacode;
	pawacode::Target ans = Target::pawa12k;
	for (size_t i = 1; i < argc - 1; ++i) {
		if ((strcmp("-t", argv[i]) == 0)|| (strcmp("--target", argv[i]) == 0)) {
			if ((strcmp("8", argv[i + 1]) == 0) ||(strcmp("pawa8", argv[i + 1]) == 0)) { ans = Target::pawa8; }
			else if ((strcmp("8k", argv[i + 1]) == 0) || (strcmp("pawa8k", argv[i + 1]) == 0)) { ans = Target::pawa8k; }
			else if ((strcmp("9", argv[i + 1]) == 0) || (strcmp("pawa9", argv[i + 1]) == 0)) { ans = Target::pawa9; }
			else if ((strcmp("9k", argv[i + 1]) == 0) || (strcmp("pawa9k", argv[i + 1]) == 0)) { ans = Target::pawa9k; }
			else if ((strcmp("10", argv[i + 1]) == 0) || (strcmp("pawa10", argv[i + 1]) == 0)) { ans = Target::pawa10; }
			else if ((strcmp("10k", argv[i + 1]) == 0) || (strcmp("pawa10k", argv[i + 1]) == 0)) { ans = Target::pawa10k; }
			else if ((strcmp("11", argv[i + 1]) == 0) || (strcmp("pawa11", argv[i + 1]) == 0)) { ans = Target::pawa11; }
			else if ((strcmp("11k", argv[i + 1]) == 0) || (strcmp("pawa11k", argv[i + 1]) == 0)) { ans = Target::pawa11k; }
			else if ((strcmp("12", argv[i + 1]) == 0) || (strcmp("pawa12", argv[i + 1]) == 0)) { ans = Target::pawa12; }
			else if ((strcmp("12k", argv[i + 1]) == 0) || (strcmp("pawa12k", argv[i + 1]) == 0)) { ans = Target::pawa12k; }

			break;
		}
	}
	return ans;
}

pawacode::TargetGameMode searchTargetGameMode(size_t argc, char* argv[]) {
	using namespace pawacode;
	TargetGameMode ans = TargetGameMode::none;
	for (size_t i = 1; i < argc - 1; ++i) {
		if ((strcmp("-g", argv[i]) == 0) || (strcmp("--gamemode", argv[i]) == 0)) {
			if ((strcmp("normal", argv[i + 1]) == 0)|| (strcmp("none", argv[i + 1]) == 0)) {
				ans = TargetGameMode::none;
			}
			else if ((strcmp("success", argv[i + 1]) == 0)) {
				ans = TargetGameMode::success;
			}
			break;
		}
	}
	return ans;
}

void ToShiftJISMode() 
{
	u16 inputchar;
	pawacode::PawaCode pcc(target_game);

	std::cout << "\nパワプロ文字コード→シフトJIS";
	while (true) {

		std::cout << "\n変換したいバイトを入力してください。(2バイト,Hex,0xFFFFで終了)\n";
		std::cin >> std::hex >> inputchar;
		//入力エラー処理
		if (std::cin.fail()) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cerr << "入力エラー" << std::endl;
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
	pawacode::PawaCode pcc(target_game);

	std::cout << "\nシフトJIS→パワプロ文字コード";
	while (true) {

		std::cout << "\n変換したい文字を入力してください。(2バイト文字、endで終了)\n";
		std::cin >> inputstring;
		//入力エラー処理
		if (std::cin.fail()) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cerr << "入力エラー" << std::endl;
			continue;
		}
		else {
			if (inputstring == "end") {
				break;
			}
			//文字処理
			int bytecount = 0;
			u16 character = 0;
			for (auto v : inputstring) {
				character += static_cast<u8>(v);
				//1バイト目なら上位バイトに保存しもう1バイト読み込む
				if (bytecount == 0) {
					character = (character << 8);
					++bytecount;
					continue;
				}
				else {	//文字表示処理
					if (display_mode == 0) {
						std::printf("%04X : %04X\n", character, pcc.SJISToPCode(character));
					}
					else if (display_mode == 2) {
						u16 buf = pcc.SJISToPCode(character);
						std::printf("%02X %02X ", buf & 0xFF, buf >> 8);
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
	pawacode::PawaCode pcc(target_game);
	
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
			pawacode::FuncState stat = pcc.PCodeToSJIS(buffer, target_gamemode, log_level, &dispChar, &moji_size);

			if (stat == pawacode::FuncState::pushedstring) {
				std::printf("%s", dispChar.c_str());

				if (target_gamemode == pawacode::TargetGameMode::success) {
					if (moji_size == -1) {
						std::printf("\n");
						dispcount = 0;
					}
				}
				else {
					dispcount += moji_size;
					if ((moji_size == -1) || (dispcount % 18 == 0)) {
						std::printf("\n");
						dispcount = 0;
					}
				}
			}

		}
	}

	delete[] data;
}

int main(size_t argc, char* argv[]) {

	if (searchHelpCommand(argc, argv)) {
		std::cerr << R"**(
Usage:	Pawapuro-TextConverter.exe
	Pawapuro-TextConverter.exe [FILE] [OPTION]
No Commands Specified: Interactive Mode
OPTION:

-v0 -v1 -v2           Log Level: 0 = Minimal, 1 = Normal, 2 = Verbose
-t <TARGET>
--target <TARGET>     Target Game: pawa8 pawa8k pawa9 pawa9k
                                   pawa10 pawa10k pawa11 pawa11k pawa12 pawa12k
-g <GAMEMODE>
--gamemode <GAMEMODE> Game Mode: normal success)**" << std::endl;
		return 0;
	}

	//引数ありでファイル読み込みモード
	if (argc >= 2) {
		std::fstream binfile(argv[1], std::ios::in | std::ios::binary);
		if (!binfile) {
			std::fprintf(stderr, "エラー:ファイルを開くことができませんでした\nFile: %s\n", argv[1]);
			return 1;
		}
		
		log_level = searchLogLevelCommand(argc, argv);
		target_game = searchTargetPawaGame(argc, argv);
		target_gamemode = searchTargetGameMode(argc, argv);

		FileReadMode(&binfile);
		if (binfile) {
			binfile.close();
		}

	}

	//引数無しなら入力受付モード
	else if (argc <= 1)
	{
		int convmode = 0;
		while (true) {
			std::cout <<
"変換モードを選んでください。\n1:パワプロ文字コード→シフトJIS\n2:パワプロ文字コード→シフトJIS(文字表示)\n\
3:シフトJIS→パワプロ文字コード\n4:シフトJIS→パワプロ文字コード(文字表示)\n5:シフトJIS→パワプロ文字コード(バイト列)\n6:プログラムを終了\n";
			std::cin >> convmode;

			if (std::cin.fail()) {
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cerr << "入力エラー" << std::endl;
				continue;
			}

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
				break;
			}
		}

	}

	return 0;
}

