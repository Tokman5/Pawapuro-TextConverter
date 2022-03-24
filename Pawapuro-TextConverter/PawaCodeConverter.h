#pragma once


#include "PTCTypes.h"

namespace pawacode {

	enum class Target {
		pawa8,pawa8k,
		pawa9,pawa9k,
		pawa10,pawa10k,
		pawa11,pawa11k,
		pawa12,pawa12k
	};

	enum class FuncState {
		request1stbyte,
		requestmorebyte,
		pushedstring
	};

	class PawaCode {
		public:
			PawaCode(pawacode::Target tar);
			u16 PCodeToSJIS(u16 input);
			u16 SJISToPCode(u16 input); //ShiftJISからの変換
			bool ToPawaCode(u16* dest, u8 input);
			//FuncState PcodeToSJIS(u16 input, std::string* ret, bool* isRealChar);
			//FuncState GetFuncState() const {return m_funcstate};
		private:

			pawacode::Target m_target;
			//FuncState m_funcstate;
			u16 m_numofSPCharacter;

			//u64 cvtbuffer64;
			/*enum class CommandMode {
				None,
				Command,
				NormalCommand,
				SpecialName
			}m_commandmode;*/

			u64 m_SJISbuffer64;
			u32 m_SJISbuffer32;
			u16 m_SJISbuffer16;
			u8  m_SJISbuffer8;
	};
}

/*サクセステキストストリング
* ret = 文字が完成したときに入れる
* isRealChar = pushedstring モード時に、ret にある string が通常の文字なら true, [効果音00]などのコマンド文字なら false
** true の時にカウントして改行処理に使う
************
* 初期 m_commandmode はNone
* request1stbyteモードの時、2バイト読み込む
* そのバイトがコマンドを表しているなら cvtbuffer64 に入れて m_commandmode を Command , requestmorebyte を返り値としてreturn
* そうでなければ iscommand は false, FuncStateを requestmorebyte をreturn
* requestmorebyteモードの時、次の2バイトを読み込む
* Commandモードの時、新しい2バイトが (cvtbuffer64 & 0x0FFF) +1に等しければ、m_commandmodeをSpecialNameにして特殊名前モード
* static size_t spnamecount を0から1ずつ足して cvtbuffer64 に退避してreturnを繰り返し、
** spnamecountが3になったらretに特殊名前を入れる、カウントはリセット
* CommandモードからSpecialNameにならなければ、NormalCommandモードにする
* 
* 
* 文字が完成したらretにstringを入れてisRealCharpushedstringモードを返す
* 呼び出し側は pushedstringモードの時のみ、文字表示を行う
* それ以外はループを進めて次の2バイトを読み込む
* pushedstringの時、isRealCharが true の時のみ、文字数カウントを1足して
* 1行18文字を目安に改行する
*/