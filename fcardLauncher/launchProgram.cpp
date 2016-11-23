
#include "stdafx.h"
#include <Windows.h>
#include <shlobj.h>
#include "launchProgram.h"

bool execProgram(UINT16 dfc)
{
	WCHAR desktop[_MAX_PATH], commandLine[_MAX_PATH];

	/* get felica.dll path */
	SHGetSpecialFolderPath(NULL, desktop, CSIDL_DESKTOPDIRECTORY, FALSE);
	_snwprintf_s(commandLine,
		(size_t)_MAX_PATH,
		(size_t)_MAX_PATH,
		L"C:\\Windows\\notepad.exe %s\\test.txt",
		desktop);

	STARTUPINFO si;// �X�^�[�g�A�b�v���
	PROCESS_INFORMATION pi;// �v���Z�X���
						   //STARTUPINFO �\���̂̓��e���擾 
	ZeroMemory(&si, sizeof(si));

	if (FindWindow(L"Notepad", NULL) == NULL) {
		int ret = 0;
		ret = CreateProcess(
			NULL,					// ���s�\���W���[���̖��O
			commandLine,	// �R�}���h���C���̕�����
			NULL,					// �Z�L�����e�B�L�q�q
			NULL,					// �Z�L�����e�B�L�q�q
			FALSE,					// �n���h���̌p���I�v�V����
			CREATE_NEW_PROCESS_GROUP,					// �쐬�̃t���O 
														//CREATE_NEW_PROCESS_GROUP	: �V���ȃv���Z�X
			NULL,					// �V�������u���b�N
			NULL,					// �J�����g�f�B���N�g���̖��O
			&si,					// �X�^�[�g�A�b�v���
			&pi					// �v���Z�X���
		);
		if (ret) {
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
		}
	}
	else {
		;
	}
	return true;
}

