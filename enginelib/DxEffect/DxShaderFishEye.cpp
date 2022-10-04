#include "stdafx.h"

#include "DxEffectFishEye.h"

//	Note : ���� ���� ����.
//
DWORD	DxEffectFishEye::m_dwEffect;

/*
char DxEffectFishEye::m_strEffect[] =
{
	"vs.1.0	\n" //

	//;--------------------------------------------------------------------  
	//;	Vertex �� World*View �� ���� ----- r0 �� ���

	// r0 �� World*View �� ����� ������ �ִ´�.
	"m4x4 r1, v0, c10	\n"
	"m4x4 r0, r1, c14	\n"

	//;--------------------------------------------------------------------  
	//;	Vertex �� ���� ��Ű�� ���� �۾� ----- r1 �� ���

	// x�� y�� c17.x(2.5) �� �����ش�.	
	"mul r1, r0, c29.xxzz	\n" 

	//; compute normalized distance from camera (camera in view-space is alwasy at (0,0,0))
	//; r1.x = sqrt(x^2 + y^2)/z
	//; sqrt(x^2 + y^2) = �Ÿ�			
	"dp3 r1.x, r1, r1		\n" //	; �Ÿ�^2
	"rsq r1.y, r1.x			\n" //		; 1/�Ÿ�
	"mul r1.y, r1.y, r0.z	\n" //	; z/�Ÿ�
	"rcp r1.x, r1.y			\n" //		; �Ÿ�/z

	//; and take the absolute value
	"max r1.x, r1.x, -r1.x	\n" //	; �Ÿ�/z �� ���� ��

	//; compute (k*d + 1)
	"mad r1.z, r1.x, c29.y, c29.w	\n" //	; (2*�Ÿ�/z)+1 = (2*�Ÿ� + z)/z

	//; compute (k+1)/(k*d + 1) -- this is the reciprocal formula of the above 
	//; referenced web-page because our distances are generally not less than 1.0!
	"rcp r1.z, r1.z			\n" //		; z/(2*�Ÿ� + z)
	"mul r1.xy, c30.x, r1.z	\n" //	; 3*z/(2*�Ÿ� + z)

	//;-------------------------------------------------------------------- 
	//;	Vertex �� ���Ž����ְ� �� Vertex�� Projection �Ѵ�.

	// �������� ��ȯ �����ش�.
	"mul r0.xy, r0, r1	\n" 

	//���� ��ȯ��Ų ������ Projection ���� �ع�����. 
	"m4x4 oPos, r0, c18	\n" 

	//;--------------------------------------------------------------------  
	//;	Normal ���͸� ���ͼ� ���� ���ͷ� ����	------ r0 �� ���

	// Normal ���͸� WorldView ��ķ� ��ȯ
	"m3x3 r0, v1, c10	\n" 

	// ��ȯ�� ���� ���� ���ͷ� ����� �۾�
	"dp3 r0.w, r0, r0	\n" 
	"rsq r0.w, r0.w		\n" 
	"mul r0, r0, r0.w	\n" 

	//;-------------------------------------------------------------------- 
	//;		Diffuse Į�� ����

	//; The directional light source.
	"dp3 r4.x, r0, -c0		\n" //

	"mul r5, r4.x, c1		\n" //	�� ��ǻ�� �� ����
	"mul r1, r5, c26		\n" //	�� ��ǻ�� �� ���� & ��Ʈ���� ��ǻ��
	"max r1.xyz, r1.xyz, c28.x	\n"
	"add r1.xyz, r1.xyz, c27		\n" //	�� ��ǻ�� �� ���� & ��Ʈ���� ��ǻ�� & �����Ʈ

	// ī�޶󿡼� ������
	"add r2, v0, -c8	\n"
	"dp3 r2.w, r2, r2	\n" 
	"rsq r2.w, r2.w		\n" 
	"mul r2, r2, r2.w	\n" 

	// ���� ���� ���� ���� ���ؼ� ���� ��´�.
	"add r2, r2, c0		\n"
//	"mul r2, r2, X		\n"		//	X �� ��ü�� ����ŧ�� ��

	"mul r3, r2, r2		\n"	// ^2
	"mul r2, r3, r3		\n"	// ^4
	"mul r2, r2, r2		\n"	// ^16
	"mul r2, r2, r3		\n"	// ^32


	"add oD0, r1, r2	\n" //
	

	//;--------------------------------------------------------------------
	//;		Texture �� ������.

	"mov oT0.xy, v2	\n" //	;  u,v ���� �����ͼ� ���� �ִ´�.
};
*/

DWORD	DxEffectFishEye::dwFisheyeVertexShader[] = {
	0xfffe0100, 0x0004fffe, 0x454c4946, 0x68736966,
	0x2e657965, 0x00687376, 0x0002fffe, 0x454e494c,
	0x00000001, 0x0002fffe, 0x454e494c, 0x00000002,
	0x0002fffe, 0x454e494c, 0x00000003, 0x0002fffe,
	0x454e494c, 0x00000004, 0x0002fffe, 0x454e494c,
	0x00000004, 0x00000014, 0x800f0001, 0x90e40000,
	0xa0e4000a, 0x0002fffe, 0x454e494c, 0x00000005,
	0x0002fffe, 0x454e494c, 0x00000005, 0x00000014,
	0x800f0000, 0x80e40001, 0xa0e4000e, 0x0002fffe,
	0x454e494c, 0x00000006, 0x0002fffe, 0x454e494c,
	0x00000007, 0x0002fffe, 0x454e494c, 0x00000008,
	0x0002fffe, 0x454e494c, 0x00000008, 0x00000005,
	0x800f0001, 0x80e40000, 0xa0a0001d, 0x0002fffe,
	0x454e494c, 0x00000009, 0x0002fffe, 0x454e494c,
	0x0000000a, 0x0002fffe, 0x454e494c, 0x0000000b,
	0x0002fffe, 0x454e494c, 0x0000000b, 0x00000008,
	0x80010001, 0x80e40001, 0x80e40001, 0x0002fffe,
	0x454e494c, 0x0000000c, 0x0002fffe, 0x454e494c,
	0x0000000c, 0x00000007, 0x80020001, 0x80000001,
	0x0002fffe, 0x454e494c, 0x0000000d, 0x0002fffe,
	0x454e494c, 0x0000000d, 0x00000005, 0x80020001,
	0x80550001, 0x80aa0000, 0x0002fffe, 0x454e494c,
	0x0000000e, 0x0002fffe, 0x454e494c, 0x0000000e,
	0x00000006, 0x80010001, 0x80550001, 0x0002fffe,
	0x454e494c, 0x0000000f, 0x0002fffe, 0x454e494c,
	0x00000010, 0x0002fffe, 0x454e494c, 0x00000010,
	0x0000000b, 0x80010001, 0x80000001, 0x81000001,
	0x0002fffe, 0x454e494c, 0x00000011, 0x0002fffe,
	0x454e494c, 0x00000012, 0x0002fffe, 0x454e494c,
	0x00000013, 0x0002fffe, 0x454e494c, 0x00000013,
	0x00000004, 0x80040001, 0x80000001, 0xa055001d,
	0xa0ff001d, 0x0002fffe, 0x454e494c, 0x00000014,
	0x0002fffe, 0x454e494c, 0x00000015, 0x0002fffe,
	0x454e494c, 0x00000015, 0x00000006, 0x80040001,
	0x80aa0001, 0x0002fffe, 0x454e494c, 0x00000016,
	0x0002fffe, 0x454e494c, 0x00000016, 0x00000005,
	0x80030001, 0xa000001e, 0x80aa0001, 0x0002fffe,
	0x454e494c, 0x00000017, 0x0002fffe, 0x454e494c,
	0x00000018, 0x0002fffe, 0x454e494c, 0x00000019,
	0x0002fffe, 0x454e494c, 0x00000019, 0x00000005,
	0x80030000, 0x80e40000, 0x80e40001, 0x0002fffe,
	0x454e494c, 0x0000001a, 0x0002fffe, 0x454e494c,
	0x0000001b, 0x0002fffe, 0x454e494c, 0x0000001c,
	0x0002fffe, 0x454e494c, 0x0000001c, 0x00000014,
	0xc00f0000, 0x80e40000, 0xa0e40012, 0x0002fffe,
	0x454e494c, 0x0000001d, 0x0002fffe, 0x454e494c,
	0x0000001e, 0x0002fffe, 0x454e494c, 0x0000001f,
	0x0002fffe, 0x454e494c, 0x0000001f, 0x00000017,
	0x800f0000, 0x90e40001, 0xa0e4000a, 0x0002fffe,
	0x454e494c, 0x00000020, 0x0002fffe, 0x454e494c,
	0x00000021, 0x0002fffe, 0x454e494c, 0x00000022,
	0x0002fffe, 0x454e494c, 0x00000022, 0x00000008,
	0x80080000, 0x80e40000, 0x80e40000, 0x0002fffe,
	0x454e494c, 0x00000023, 0x0002fffe, 0x454e494c,
	0x00000023, 0x00000007, 0x80080000, 0x80ff0000,
	0x0002fffe, 0x454e494c, 0x00000024, 0x0002fffe,
	0x454e494c, 0x00000024, 0x00000005, 0x800f0000,
	0x80e40000, 0x80ff0000, 0x0002fffe, 0x454e494c,
	0x00000025, 0x0002fffe, 0x454e494c, 0x00000026,
	0x0002fffe, 0x454e494c, 0x00000027, 0x0002fffe,
	0x454e494c, 0x00000027, 0x00000008, 0x80010004,
	0x80e40000, 0xa1e40000, 0x0002fffe, 0x454e494c,
	0x00000028, 0x0002fffe, 0x454e494c, 0x00000029,
	0x0002fffe, 0x454e494c, 0x00000029, 0x00000005,
	0x800f0005, 0x80000004, 0xa0e40001, 0x0002fffe,
	0x454e494c, 0x0000002a, 0x0002fffe, 0x454e494c,
	0x0000002a, 0x00000005, 0x800f0001, 0x80e40005,
	0xa0e4001a, 0x0002fffe, 0x454e494c, 0x0000002b,
	0x0002fffe, 0x454e494c, 0x0000002b, 0x0000000b,
	0x80070001, 0x80a40001, 0xa000001c, 0x0002fffe,
	0x454e494c, 0x0000002c, 0x0002fffe, 0x454e494c,
	0x0000002c, 0x00000002, 0x80070001, 0x80a40001,
	0xa0e4001b, 0x0002fffe, 0x454e494c, 0x0000002d,
	0x0002fffe, 0x454e494c, 0x0000002e, 0x0002fffe,
	0x454e494c, 0x0000002f, 0x0002fffe, 0x454e494c,
	0x0000002f, 0x00000002, 0x800f0002, 0x90e40000,
	0xa1e40008, 0x0002fffe, 0x454e494c, 0x00000030,
	0x0002fffe, 0x454e494c, 0x00000030, 0x00000008,
	0x80080002, 0x80e40002, 0x80e40002, 0x0002fffe,
	0x454e494c, 0x00000031, 0x0002fffe, 0x454e494c,
	0x00000031, 0x00000007, 0x80080002, 0x80ff0002,
	0x0002fffe, 0x454e494c, 0x00000032, 0x0002fffe,
	0x454e494c, 0x00000032, 0x00000005, 0x800f0002,
	0x80e40002, 0x80ff0002, 0x0002fffe, 0x454e494c,
	0x00000033, 0x0002fffe, 0x454e494c, 0x00000034,
	0x0002fffe, 0x454e494c, 0x00000035, 0x0002fffe,
	0x454e494c, 0x00000035, 0x00000002, 0x800f0002,
	0x80e40002, 0xa0e40000, 0x0002fffe, 0x454e494c,
	0x00000036, 0x0002fffe, 0x454e494c, 0x00000037,
	0x0002fffe, 0x454e494c, 0x00000038, 0x0002fffe,
	0x454e494c, 0x00000038, 0x00000005, 0x800f0003,
	0x80e40002, 0x80e40002, 0x0002fffe, 0x454e494c,
	0x00000039, 0x0002fffe, 0x454e494c, 0x00000039,
	0x00000005, 0x800f0002, 0x80e40003, 0x80e40003,
	0x0002fffe, 0x454e494c, 0x0000003a, 0x0002fffe,
	0x454e494c, 0x0000003a, 0x00000005, 0x800f0002,
	0x80e40002, 0x80e40002, 0x0002fffe, 0x454e494c,
	0x0000003b, 0x0002fffe, 0x454e494c, 0x0000003b,
	0x00000005, 0x800f0002, 0x80e40002, 0x80e40003,
	0x0002fffe, 0x454e494c, 0x0000003c, 0x0002fffe,
	0x454e494c, 0x0000003d, 0x0002fffe, 0x454e494c,
	0x0000003d, 0x00000002, 0xd00f0000, 0x80e40001,
	0x80e40002, 0x0002fffe, 0x454e494c, 0x0000003e,
	0x0002fffe, 0x454e494c, 0x0000003f, 0x0002fffe,
	0x454e494c, 0x00000040, 0x0002fffe, 0x454e494c,
	0x00000041, 0x0002fffe, 0x454e494c, 0x00000041,
	0x00000001, 0xe0030000, 0x90e40002, 0x0000ffff
	
};

DWORD	DxEffectFishEye::m_dwSavedStateBlock(0x00000000);
DWORD	DxEffectFishEye::m_dwEffectStateBlock(0x00000000);