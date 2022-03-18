#pragma once


#include "PTCTypes.h"

namespace pawacode {
	class PawaCode {
		public:
			PawaCode();
			u16 ToShiftJIS(u16 input);
			u16 ToPawaCode(u16 input); //ShiftJIS‚©‚ç‚Ì•ÏŠ·
			bool ToPawaCode(u16* dest, u8 input);
		private:


			u64 SJISbuffer64=0;
			u32 SJISbuffer32=0;
			u16 SJISbuffer16=0;
			u8  SJISbuffer8=0;
	};
}