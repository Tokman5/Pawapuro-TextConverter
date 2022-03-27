#pragma once
#include <string>

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
		request_LObyte,
		request_HIbyte,
		request_morebyte,
		pushedstring
	};

	enum class TargetGameMode {
		none,
		success
	};

	class PawaCode {
		public:
			PawaCode(pawacode::Target tar);
			u16 PCodeToSJIS(u16 input);
			u16 SJISToPCode(u16 input); //ShiftJIS‚©‚ç‚Ì•ÏŠ·
			FuncState PCodeToSJIS(u8 input, TargetGameMode target, std::string* ret, int* numofRealChar);
			FuncState GetFuncState() const { return m_funcstate; };
		private:

			pawacode::Target m_target;
			FuncState m_funcstate;
			u16 m_numofSPCharacter;

			u32 m_cvtbuffer32;
			enum class CommandMode {
				Normal,
				Command,
				CommandEnd,
				Mode_null
			}m_commandmode;

			u16 m_SJISbuffer16;
	};
}
