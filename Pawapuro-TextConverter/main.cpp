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
	pawacode::PawaCode pcc;

	std::cout << "\nパワプロ文字コード→シフトJIS";
	while (true) {

		std::cout << "\n変換したいバイトを入力してください。(2バイト,Hex)\n";
		std::cin >> std::hex >> inputchar;
		//入力エラー処理
		if (std::cin.fail()) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cerr << "入力エラー" << std::endl;
			continue;
		}
		else {

			std::printf( "Output: %04X\n", pcc.ToShiftJIS(inputchar));
		}
	}
	return;
}

void ToPawaCodeMode()
{
	std::string inputstring;
	pawacode::PawaCode pcc;

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

				if (bytecount == 0) {
					character = (character << 8);
					++bytecount;
					continue;
				}
				else {
					if (Displaymode == 0) {
						std::cout << std::hex << character << " : " << pcc.ToPawaCode(character) << std::endl;
					}
					else {
						char mulchar[]{ static_cast<char>(character >> 8),static_cast<char>(character & 0xFF), NULL };
						std::printf("%s : %04X\n", mulchar , pcc.ToPawaCode(character));
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
		std::cerr << "Usage: Pawapuro-TextConverter.exe [OPTION]" << std::endl;
		return 0;
	}

	for (size_t i = 0; i < argc; ++i) {
		std::printf("%s\n", argv[i]);
	}





	
//////////////////////引数無しなら入力受付モード////////////////////////////
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

