//パワプロテキストコンバータ
//

#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include "PawaCodeConverter.h"
#include "PTCTypes.h"


//ファイルポインタ
std::FILE* fp;

//シフトJIS→パワプロ文字コードモード
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

			std::printf( "Output: %04X\n", pcc.PCodeToSJIS(inputchar));
		}
	}
	return;
}

void ToPawaCodeMode()
{
	std::string inputstring;
	pawacode::PawaCode pcc(pawacode::Target::pawa12k);

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
			u32 character = 0;
			for (auto v : inputstring) {
				character += static_cast<u8>(v);
				//1バイト目なら上位バイトに保存しもう1バイト読み込む
				if (bytecount == 0) {
					character = (character << 8);
					++bytecount;
					continue;
				}
				else {	//文字表示処理
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

//引数無しなら入力受付モード
	if (argc <= 1)
	{
		int convmode = 0;
		std::cout << "変換モードを選んでください。\n1:パワプロ文字コード→シフトJIS\n2:シフトJIS→パワプロ文字コード\n3:シフトJIS(文字表示)→パワプロ文字コード\n";
		std::cin >> convmode;

		//SJIS to PawaCode モード
		if (convmode == 2) {
			Displaymode = 0;
			ToPawaCodeMode();
		}
		else if (convmode == 3) {
			Displaymode = 1;
			ToPawaCodeMode();
		}
		//PawaCode to SJIS モード
		else {
			ToShiftJISMode();
		}

	}

	return 0;
}

