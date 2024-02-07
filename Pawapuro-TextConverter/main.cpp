//パワプロテキストコンバータ
//2022-2023

#include <algorithm>
#include <charconv>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <locale>
#include <memory>
#include <string>
#include <vector>
#include "PawaCodeConverter.h"
#include "PTCTypes.h"



namespace {
	int s_menu_page = 0;	//メニュー画面のページナンバー
	u8 s_display_mode = 0; //0 = 数字表示　1 = 文字表示(未使用)、2 = バイト列
	int s_log_level = 0;	//特殊コマンドの出力レベル　0=改行、[主人公]のみ　1=通常　2=全出力
	bool s_compress_mode = false;
	PawaCode::TargetGame s_target_game = PawaCode::TargetGame::pawa2009;
	PawaCode::TargetMode s_target_mode = PawaCode::TargetMode::normal;

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
-g <TARGET>
-t <TARGET>
--game <TARGET>
--target <TARGET>     Target Game: pawa7 pawa7k
                                   pawa8 pawa8k pawa9 pawa9k
                                   pawa10 pawa10k pawa11 pawa11k
                                   pawa12 pawa12k pawa2009
-m <GAMEMODE>
--mode <GAMEMODE>       Game Mode: normal success
-----------------------------------------------)**";

	const char* menu_string[] = {"\n Page 1/3  パワプロ8～2009用\n 1 : パワプロ文字コード(1文字)→シフトJIS\n 2 : パワプロ文字コード(バイト列)→シフトJIS\n\
 3 : パワプロ文字コード(圧縮バイト列)→シフトJIS\n 4 : シフトJIS→パワプロ文字コード\n 5 : シフトJIS→パワプロ文字コード(バイト列)\n 6 : シフトJIS→パワプロ文字コード(バイト列,圧縮)\n\n\
 8 : 次のページへ\n 9 : プログラムを終了\n",
"\n Page 2/3  パワプロ7用\n 1 : パワプロ文字コード(1文字)→シフトJIS\n 2 : パワプロ文字コード(バイト列)→シフトJIS\n\
 3 : シフトJIS→パワプロ文字コード\n 4 : シフトJIS→パワプロ文字コード(バイト列)\n\n\n\
 7 : 前のページへ\n 8 : 次のページへ\n 9 : プログラムを終了\n",
"\n Page 3/3\n 1 : ファイル読み込みモードヘルプ\n\n 7 : 前のページへ\n\n 9 : プログラムを終了\n"};
}

bool searchHelpCommand(int argc, char* argv[]) {

	for (size_t i = 0; i < argc; ++i) {
		if ((strcmp("-h", argv[i]) == 0) || (strcmp("--help", argv[i]) == 0) || (strcmp("/h", argv[i]) == 0) || (strcmp("/?", argv[i]) == 0)) {
			return true;
		}
	}

	return false;
}

int searchLogLevelCommand(int argc, char* argv[]) {
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

bool searchTargetPawaGame(int argc, char* argv[]) {
	for (int i = 1; i < argc - 1; ++i) {
		if ((strcmp("-t", argv[i]) == 0)|| (strcmp("--target", argv[i]) == 0) ||
			(strcmp("-g", argv[i]) == 0) || (strcmp("--game", argv[i]) == 0)) {
				if ((strcmp("7", argv[i + 1]) == 0) || (strcmp("pawa7", argv[i + 1]) == 0)) { ::s_target_game = PawaCode::TargetGame::pawa7; }
				else if ((strcmp("7k", argv[i + 1]) == 0) || (strcmp("pawa7k", argv[i + 1]) == 0)) { ::s_target_game = PawaCode::TargetGame::pawa7k; }
				else if ((strcmp("8", argv[i + 1]) == 0) || (strcmp("pawa8", argv[i + 1]) == 0)) { ::s_target_game = PawaCode::TargetGame::pawa8; }
				else if ((strcmp("8k", argv[i + 1]) == 0) || (strcmp("pawa8k", argv[i + 1]) == 0)) { ::s_target_game = PawaCode::TargetGame::pawa8k; }
				else if ((strcmp("9", argv[i + 1]) == 0) || (strcmp("pawa9", argv[i + 1]) == 0)) { ::s_target_game = PawaCode::TargetGame::pawa9; }
				else if ((strcmp("9k", argv[i + 1]) == 0) || (strcmp("pawa9k", argv[i + 1]) == 0)) { ::s_target_game = PawaCode::TargetGame::pawa9k; }
				else if ((strcmp("10", argv[i + 1]) == 0) || (strcmp("pawa10", argv[i + 1]) == 0)) { ::s_target_game = PawaCode::TargetGame::pawa10; }
				else if ((strcmp("10k", argv[i + 1]) == 0) || (strcmp("pawa10k", argv[i + 1]) == 0)) { ::s_target_game = PawaCode::TargetGame::pawa10k; }
				else if ((strcmp("11", argv[i + 1]) == 0) || (strcmp("pawa11", argv[i + 1]) == 0)) { ::s_target_game = PawaCode::TargetGame::pawa11; }
				else if ((strcmp("11k", argv[i + 1]) == 0) || (strcmp("pawa11k", argv[i + 1]) == 0)) { ::s_target_game = PawaCode::TargetGame::pawa11k; }
				else if ((strcmp("12", argv[i + 1]) == 0) || (strcmp("pawa12", argv[i + 1]) == 0)) { ::s_target_game = PawaCode::TargetGame::pawa12; }
				else if ((strcmp("12k", argv[i + 1]) == 0) || (strcmp("pawa12k", argv[i + 1]) == 0)) { ::s_target_game = PawaCode::TargetGame::pawa12k; }
				else if ((strcmp("15", argv[i + 1]) == 0) || (strcmp("pawa15", argv[i + 1]) == 0)) { ::s_target_game = PawaCode::TargetGame::pawa15; }
				else if ((strcmp("2009", argv[i + 1]) == 0) || (strcmp("pawa2009", argv[i + 1]) == 0)) { ::s_target_game = PawaCode::TargetGame::pawa2009; }
				else {
					errstatus = i + 1;
					return false;
				}
			break;
		}
	}
	return true;
}

bool searchTargetGameMode(int argc, char* argv[]) {
	for (int i = 1; i < argc - 1; ++i) {
		if ((strcmp("-m", argv[i]) == 0) || (strcmp("--mode", argv[i]) == 0)) {
			if ((strcmp("normal", argv[i + 1]) == 0)|| (strcmp("none", argv[i + 1]) == 0)) {
				s_target_mode = PawaCodeV2001::TargetMode::normal;
			}
			else if ((strcmp("success", argv[i + 1]) == 0)) {
				s_target_mode = PawaCodeV2001::TargetMode::success;
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
	if ((::s_target_game == PawaCode::TargetGame::pawa7) || (::s_target_game == PawaCode::TargetGame::pawa7k)) {
		pcc = std::make_unique<PawaCodeV2000>(PawaCodeV2000(::s_target_game));
	}
	else {
		pcc = std::make_unique<PawaCodeV2001>(PawaCodeV2001(::s_target_game));
	}

	pcc->SetTargetMode(PawaCode::TargetMode::normal);

	std::cout << "\nパワプロ文字コード→シフトJIS";
	while (true) {

		std::cout << "\n変換したいバイトを入力してください。(2バイト,Hex,FFFFで戻る)\n";
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
			const u16 ans = pcc->PCodeToSJIS(inputchar);

			const char anschar[3]{ (ans >> 8),(ans & 0xFF),'\0' };
			std::printf("%04X: %04X: %s\n", inputchar & 0xFFF, ans, anschar);
		}
	}
	return;
}

void ToShiftJISModeEx()
{
	std::string inputstring;
	std::unique_ptr<PawaCode> pcc = nullptr;
	if ((::s_target_game == PawaCode::TargetGame::pawa7) || (::s_target_game == PawaCode::TargetGame::pawa7k)) {
		pcc = std::make_unique<PawaCodeV2000>(PawaCodeV2000(::s_target_game));
	}
	else {
		pcc = std::make_unique<PawaCodeV2002>(PawaCodeV2002(::s_target_game));
	}

	while (getchar() != '\n');
	std::cout << "\nパワプロ文字コード(バイト列)→シフトJIS";
	while (true) {
		pcc->SetTargetMode(PawaCode::TargetMode::normal);
		std::cout << "\n変換したいバイト列を入力してください。(Hex,リトルエンディアン,\"end\"で戻る)\n";
		if (std::getline(std::cin, inputstring)) {
			if (inputstring.size() == 3) {
				std::transform(inputstring.cbegin(), inputstring.cend(), inputstring.begin(), [](unsigned char c) {return std::tolower(c, std::locale::classic()); });
				if (inputstring == "end") {
					break;
				}
			}

			//16進数として有効な文字を抽出
			inputstring.erase(std::remove_if(inputstring.begin(), inputstring.end(), [](unsigned char c) { return !std::isxdigit(c, std::locale::classic()); }), inputstring.end());

			std::vector<u16> output;

			if (const size_t arraysize = inputstring.size(); arraysize) {
#ifdef _DEBUG
				printf("%s\n", inputstring.c_str()); 
#endif

				//文字列をu16の数値へ変換
				for (size_t i = 0; i < (arraysize / 4); i++) {
					const char inpchr[5]{ inputstring[i * 4 + 2],inputstring[i * 4 + 3],inputstring[i * 4],inputstring[i * 4 + 1],'\0' };
					u16 conv;
					std::from_chars(&inpchr[0], &inpchr[4], conv, 16);
					if (s_compress_mode == false) {
						output.emplace_back(pcc->PCodeToSJIS(conv)); //シフトJISへ変換してoutputに追加
					}
					else {
						output.emplace_back(conv);
					}
				}
			}


			//文字列の出力
			if (output.size() > 0) {
				if (s_compress_mode) {
					std::vector<u16> tmparray;
					bool ret = PawaCodeV2002::DecompressArray(output, tmparray);
					output = std::move(tmparray);
					for (auto& v : output) {
						v = pcc->PCodeToSJIS(v);
					}
				}
				
				for (const auto v : output) {
					const char conv[3]{ (v >> 8),(v & 0xFF),'\0' };
					std::printf("%s", conv);
				}
				std::printf("\n");
			}
		}
		
	}
}

void ToPawaCodeMode()
{
	std::string inputstring;
	std::unique_ptr<PawaCode> pcc = nullptr;
	if ((::s_target_game == PawaCode::TargetGame::pawa7) || (::s_target_game == PawaCode::TargetGame::pawa7k)) {
		pcc = std::make_unique<PawaCodeV2000>(PawaCodeV2000(::s_target_game));
	}
	else {
		pcc = std::make_unique<PawaCodeV2001>(PawaCodeV2001(::s_target_game));
	}

	pcc->SetTargetMode(PawaCode::TargetMode::normal);

	std::cout << "\nシフトJIS→パワプロ文字コード";
	while (true) {

		std::cout << "\n変換したい文字を入力してください。(全角文字、半角\"end\"で戻る)\n";
		std::cin >> inputstring;
		//入力エラー処理
		if (std::cin.fail()) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cerr << "入力エラー" << std::endl;
			continue;
		}
		else {
			if (inputstring == "end" || inputstring == "END") {
				break;
			}

			//文字処理
			int bytecount = 0;
			u16 character = 0;
			int encrypt_charcount = 0;
			std::vector<u16> row_array;
			for (const auto& v : inputstring) {
				character += static_cast<u8>(v);
				//1バイト目なら上位バイトに保存しもう1バイト読み込む
				if (bytecount == 0) {
					character = (character << 8);
					++bytecount;
					continue;
				}
				else {	//文字表示処理
					if (s_display_mode == 0) {
						const char mulchar[]{ static_cast<const char>(character >> 8),static_cast<const char>(character & 0xFF), '\0' };
						std::printf("%s (%04X): %04X\n", mulchar, character, pcc->SJISToPCode(character));
					}
					else if (s_display_mode == 2) {	//バイト列表示モード
						const u16 buf = pcc->SJISToPCode(character);
						std::printf("%02X %02X ", buf & 0xFF, buf >> 8);
						//std::printf("%02X%02X ", buf >> 8, buf & 0xFF); //ビッグエンディアンDebug用
					}

					if (s_display_mode == 2 && s_compress_mode == true) {	//バイト列表示モード(圧縮)
						row_array.emplace_back( pcc->SJISToPCode(character));
					}

					character = 0;
					bytecount = 0;
				}
			}

			if (s_display_mode == 2 && s_compress_mode == true) { //圧縮モード時、圧縮&表示処理
				std::vector<u16> compressed_array;
				PawaCodeV2002::CompressArray(row_array, compressed_array);

				size_t disp_count = 0;

				std::printf("\n");

				for (const auto& v : compressed_array) {
					std::printf("%02X %02X ", v & 0xFF, v >> 8);

					if (disp_count >= 2) {
						std::printf(" ");
						disp_count = 0;
					}
					else { 
						disp_count++; 
					}
				}

#ifdef _DEBUG
				std::printf("\n");
				disp_count = 0;
				for (const auto& v : compressed_array) {
					std::printf("%02X%02X ", v >> 8, v & 0xFF); //ビッグエンディアン Debug用

					if (disp_count >= 2) {
						std::printf(" ");
						disp_count = 0;
					}
					else {
						disp_count++;
					}
				}
#endif // _DEBUG
				
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
	if ((::s_target_game == PawaCode::TargetGame::pawa7) || (::s_target_game == PawaCode::TargetGame::pawa7k)) {
		pcc = std::make_unique<PawaCodeV2000>(PawaCodeV2000(::s_target_game));
	}
	else if ((::s_target_game == PawaCode::TargetGame::pawa8) || (::s_target_game == PawaCode::TargetGame::pawa8k)) {
		pcc = std::make_unique<PawaCodeV2001>(PawaCodeV2001(::s_target_game));
	}
	else {
		pcc = std::make_unique<PawaCodeV2002>(PawaCodeV2002(::s_target_game));
	}

	pcc->SetTargetMode(s_target_mode);
	
	//ファイルオープン
	std::fstream file(path, std::ios::in | std::ios::binary);
	if (!file) {
		std::fprintf(stderr, "エラー:ファイルを開くことができませんでした\nFile: %s\n", path);
		return 1;
	}

	const size_t filesize = std::filesystem::file_size(path);

	//データコピー
	std::vector<u8> datavec;
	datavec.resize(filesize);
	file.read(reinterpret_cast<char*>(datavec.data()), filesize);

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
		for (size_t i = 0; i < filesize; ++i) {
			buffer = datavec[i];
			if (!(i & 1)) {
				//読み込みバイトが1バイト目ならループを進めてもう1バイト読み込む
				inputchar = buffer;
				continue;
			}

			inputchar += (buffer << 8);

			PawaCode::PCtoSJISFuncState stat = pcc->PCodeToSJIS(inputchar, s_log_level, dispChar, moji_size);

			if (stat == PawaCode::PCtoSJISFuncState::normal) {
				std::printf("%s", dispChar.c_str());

				if (s_target_mode == PawaCode::TargetMode::success) {
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

	return 0;
}

int main(int argc, char* argv[]) 
{
	if (searchHelpCommand(argc, argv)) {
		std::fprintf(stderr,"%s\n", usagestring);
		return 0;
	}

	//引数ありでファイル読み込みモード
	if (argc >= 2) {

		s_log_level = searchLogLevelCommand(argc, argv);

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
			std::printf("%s", menu_string[s_menu_page]);
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
			if (s_menu_page == 0) {
				s_compress_mode = false;
				::s_target_game = PawaCode::TargetGame::pawa2009;
				if (convmode == 1) {
					s_display_mode = 0;
					ToShiftJISMode();
				}
				else if (convmode == 2) {
					s_compress_mode = false;
					ToShiftJISModeEx();
				}
				else if (convmode == 3) {
					s_compress_mode = true;
					ToShiftJISModeEx();
				}
				else if (convmode == 4) {
					s_display_mode = 0;
					ToPawaCodeMode();
				}
				else if (convmode == 5) {
					s_display_mode = 2;
					ToPawaCodeMode();
				}
				else if (convmode == 6) {
					s_display_mode = 2;
					s_compress_mode = true;
					ToPawaCodeMode();
				}
				else if (convmode == 8) {
					++s_menu_page;
				}
			}
			//2ページ目の処理
			else if (s_menu_page == 1) {
				s_compress_mode = false;
				::s_target_game = PawaCode::TargetGame::pawa7k;
				if (convmode == 1) {
					s_display_mode = 0;
					ToShiftJISMode();
				}
				else if (convmode == 2) {
					ToShiftJISModeEx();
				}
				else if (convmode == 3) {
					s_display_mode = 0;
					ToPawaCodeMode();
				}
				else if (convmode == 4) {
					s_display_mode = 2;
					ToPawaCodeMode();
				}
				else if (convmode == 5) {

				}
				else if (convmode == 7) {
					--s_menu_page;
				}
				else if (convmode == 8) {
					++s_menu_page;
				}
			}
			//3ページ目の処理
			else if (s_menu_page == 2) {
				if (convmode == 1) {
					printf("%s\n", usagestring);
				}
				else if (convmode == 7) {
					--s_menu_page;
				}
				/*else if (convmode == 8) {
					++s_menu_page;
				}*/
			}
		}

	}

	return 0;
}

