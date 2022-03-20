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

	class PawaCode {
		public:
			PawaCode(pawacode::Target tar);
			u16 PCodeToSJIS(u16 input);
			u16 SJISToPCode(u16 input); //ShiftJIS‚©‚ç‚Ì•ÏŠ·
			bool ToPawaCode(u16* dest, u8 input);
		private:

			pawacode::Target m_target;
			u16 m_numofSPCharacter;

			u64 m_SJISbuffer64;
			u32 m_SJISbuffer32;
			u16 m_SJISbuffer16;
			u8  m_SJISbuffer8;
	};
}