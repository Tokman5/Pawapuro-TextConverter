//パワプロテキストコンバータ
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
	int menu_page = 0;	//メニュー画面のページナンバー
	u8 display_mode = 0; //0 = 数字表示　1 = 文字表示、2 = バイト列
	int log_level = 0;	//特殊コマンドの出力レベル　0=改行、[主人公]のみ　1=通常　2=全出力
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

	const char* menu_string[] = {"\n Page 1/3  パワプロ8～2009用\n 1 : パワプロ文字コード→シフトJIS\n 2 : パワプロ文字コード→シフトJIS(文字表示)\n\
 3 : シフトJIS→パワプロ文字コード\n 4 : シフトJIS→パワプロ文字コード(文字表示)\n 5 : シフトJIS→パワプロ文字コード(バイト列)\n 6 : シフトJIS→パワプロ文字コード(バイト列,暗号化)\n\n\
 8 : 次のページへ\n 9 : プログラムを終了\n",
"\n Page 2/3  パワプロ7用\n 1 : パワプロ文字コード→シフトJIS\n 2 : パワプロ文字コード→シフトJIS(文字表示)\n\
 3 : シフトJIS→パワプロ文字コード\n 4 : シフトJIS→パワプロ文字コード(文字表示)\n 5 : シフトJIS→パワプロ文字コード(バイト列)\n\n\
 7 : 前のページへ\n 8 : 次のページへ\n 9 : プログラムを終了\n",
"\n Page 3/3\n 1 : ファイル読み込みモードヘルプ\n\n 7 : 前のページへ\n\n 9 : プログラムを終了\n"};
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

	std::cout << "\nパワプロ文字コード→シフトJIS";
	while (true) {

		std::cout << "\n変換したいバイトを入力してください。(2バイト,Hex,0xFFFFで戻る)\n";
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

	std::cout << "\nシフトJIS→パワプロ文字コード";
	while (true) {

		std::cout << "\n変換したい文字を入力してください。(全角文字、半角endで戻る)\n";
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
			int encrypt_charcount = 0;
			std::vector<u16> row_array;
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
						std::printf("%04X : %04X\n", character, pcc->SJISToPCode(character));
					}
					else if (display_mode == 2) {	//バイト列表示モード
						u16 buf = pcc->SJISToPCode(character);
						std::printf("%02X %02X ", buf & 0xFF, buf >> 8);
						//std::printf("%02X%02X ", buf >> 8, buf & 0xFF); //ビッグエンディアンDebug用
					}
					else {							//文字表示モード
						char mulchar[]{ static_cast<u8>(character >> 8),static_cast<u8>(character & 0xFF), '\0' };
						std::printf("%s : %04X\n", mulchar, pcc->SJISToPCode(character));
					}

					if (display_mode == 2 && compress_mode == true) {	//バイト列表示モード(圧縮)
						row_array.emplace_back( pcc->SJISToPCode(character));
					}

					character = 0;
					bytecount = 0;
				}
			}

			if (display_mode == 2 && compress_mode == true) { //圧縮モード時、圧縮&表示処理
				std::vector<u16> compressed_array;
				PawaCodeV2002::CompressArray(row_array, compressed_array);

				size_t disp_count = 0;

				std::printf("\n");

				for (auto&& v : compressed_array) {
					std::printf("%02X %02X ", v & 0xFF, v >> 8);
					//std::printf("%02X%02X ", v >> 8, v & 0xFF); //ビッグエンディアン Debug用

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
	
	//ファイルオープン
	std::fstream file(path, std::ios::in | std::ios::binary);
	if (!file) {
		std::fprintf(stderr, "エラー:ファイルを開くことができませんでした\nFile: %s\n", path);
		return 1;
	}
	
	file.seekg(0, std::ios::end);
	size_t size = file.tellg();
	file.seekg(0);
	//std::printf("size: %zX\n", size);

	//データコピー
	u8* data = new u8[size];
	file.read(reinterpret_cast<char*>(data), size);

	if (file) {
		file.close();
	}

	//変換処理
	{
		u16 inputchar = 0;
		u8 buffer = 0;
		int dispcount = 0;
		std::string dispChar;
		int moji_size = 0;
		for (size_t i = 0; i <= size; ++i) {
			buffer = data[i];
			if (!(i & 1)) {
				//読み込みバイトが1バイト目ならループを進めてもう1バイト読み込む
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

	//引数ありでファイル読み込みモード
	if (argc >= 2) {

		log_level = searchLogLevelCommand(argc, argv);

		if (!searchTargetPawaGame(argc, argv)) {
			std::fprintf(stderr, "エラー：無効な作品パラメータ：%s",argv[errstatus]);
			return 1;
		}

		if (!searchTargetGameMode(argc, argv)) {
			std::fprintf(stderr, "エラー：無効なゲームモード：%s", argv[errstatus]);
			return 1;
		}

		return FileReadMode(argv[1]);
		

	}

	//引数無しなら入力受付モード
	else if (argc <= 1)
	{
		std::printf("パワプロ文字コードコンバータ\n");
		int convmode = 0;
		while (true) {
			std::printf("%s", menu_string[menu_page]);
			std::cin >> convmode;

			if (std::cin.fail()) {
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cerr << "入力エラー" << std::endl;
				continue;
			}

			//9が入力されたらプログラムを終了する
			if (convmode == 9) {
				break;
			}

			//1ページ目の処理
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
			//2ページ目の処理
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
			//3ページ目の処理
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

