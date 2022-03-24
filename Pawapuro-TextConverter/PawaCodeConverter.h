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
			u16 SJISToPCode(u16 input); //ShiftJIS����̕ϊ�
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

/*�T�N�Z�X�e�L�X�g�X�g�����O
* ret = ���������������Ƃ��ɓ����
* isRealChar = pushedstring ���[�h���ɁAret �ɂ��� string ���ʏ�̕����Ȃ� true, [���ʉ�00]�Ȃǂ̃R�}���h�����Ȃ� false
** true �̎��ɃJ�E���g���ĉ��s�����Ɏg��
************
* ���� m_commandmode ��None
* request1stbyte���[�h�̎��A2�o�C�g�ǂݍ���
* ���̃o�C�g���R�}���h��\���Ă���Ȃ� cvtbuffer64 �ɓ���� m_commandmode �� Command , requestmorebyte ��Ԃ�l�Ƃ���return
* �����łȂ���� iscommand �� false, FuncState�� requestmorebyte ��return
* requestmorebyte���[�h�̎��A����2�o�C�g��ǂݍ���
* Command���[�h�̎��A�V����2�o�C�g�� (cvtbuffer64 & 0x0FFF) +1�ɓ�������΁Am_commandmode��SpecialName�ɂ��ē��ꖼ�O���[�h
* static size_t spnamecount ��0����1�������� cvtbuffer64 �ɑޔ�����return���J��Ԃ��A
** spnamecount��3�ɂȂ�����ret�ɓ��ꖼ�O������A�J�E���g�̓��Z�b�g
* Command���[�h����SpecialName�ɂȂ�Ȃ���΁ANormalCommand���[�h�ɂ���
* 
* 
* ����������������ret��string������isRealCharpushedstring���[�h��Ԃ�
* �Ăяo������ pushedstring���[�h�̎��̂݁A�����\�����s��
* ����ȊO�̓��[�v��i�߂Ď���2�o�C�g��ǂݍ���
* pushedstring�̎��AisRealChar�� true �̎��̂݁A�������J�E���g��1������
* 1�s18������ڈ��ɉ��s����
*/