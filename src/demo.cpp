#include <iostream>

#include "vm_core.h"

#ifdef _MSC_VER
	#define NOINLINE __declspec(noinline)
#else
	#define NOINLINE __attribute__ ((noinline))
#endif

NOINLINE char* GenerateSerial(const char* Login)
{
	static char result[18];
	const char* H = "0123456789ABCDEF";

	int LoginValue = 0;

	for (size_t I = 0; I < strlen(Login); ++I)
	{
		LoginValue = LoginValue << 2;
		LoginValue = LoginValue + uint8_t(Login[I]);
		LoginValue = LoginValue ^ 0x48ADEEEF;
		LoginValue = LoginValue * uint8_t(Login[I]);
	}
	for (size_t I = 0; I < 8; I += 2)
	{
		uint8_t B = (LoginValue & 0xff000000) >> 24;
		result[I] = H[B >> 4];
		result[I + 1] = H[B & 0x0f];
		LoginValue <<= 8;
	}
	return result;
}

// =============================================================================

uint8_t CharToByte(const char* Value)
{
	uint8_t A = Value[0];
	uint8_t B = Value[1];

	A -= (A > 0x39 ? 0x37 : 0x30);
	B -= (B > 0x39 ? 0x37 : 0x30);

	return (A << 4) + B;
}

int SimpleHexToInt(const char* Value)
{
	return (CharToByte(&Value[0]) << 24) +
		(CharToByte(&Value[2]) << 16) +
		(CharToByte(&Value[4]) << 8) +
		(CharToByte(&Value[6]));
}

NOINLINE bool CheckSerial(const char* Login, const char* Serial)
{
	int LoginValue;
	int SerialIntValue;

	LoginValue = 0;
	for (size_t I = 0; I < strlen(Login); ++I)
	{
		LoginValue = LoginValue << 2;
		LoginValue = LoginValue + uint8_t(Login[I]);
		LoginValue = LoginValue ^ 0x48ADEEEF;
		LoginValue = LoginValue * uint8_t(Login[I]);
	}
	SerialIntValue = SimpleHexToInt(Serial);
	return SerialIntValue == LoginValue ? true : false;
}

// =============================================================================

uint8_t vmCharToByte(const char* Value)
{
	int I = 0;
	uint8_t A, B, R;
	TFlags F;

	vmMovU8(Value[I], A);
	F = vmCmp(A, 0x39, bs8);
	B = vmIfThen(0x37, 0x30, vmJmp_G(F));
	R = vmSub(A, B, bs8);

	R = vmShl(R, 4, bs8);
	I = vmInc(I);
	vmMovU8(Value[I], A);
	F = vmCmp(A, 0x39, bs8);
	B = vmIfThen(0x37, 0x30, vmJmp_G(F));
	B = vmSub(A, B, bs8);

	return vmAdd(R, B, bs8);
}

int vmSimpleHexToInt(const char* Value)
{
	return vmShl(vmCharToByte(&Value[0]), 24, bs32) +
		vmShl(vmCharToByte(&Value[2]), 16, bs32) +
		vmShl(vmCharToByte(&Value[4]), 8, bs32) +
		vmCharToByte(&Value[6]);
}

NOINLINE bool vmCheckSerialEx(const char* Login, const char* Serial)
{
	vmInitialize();

	int LoginValue;
	int SerialIntValue;
	TFlags F;

	vmMov32(0, LoginValue);
	for (size_t I = 0; I < strlen(Login); ++I)
	{
		LoginValue = vmShl(LoginValue, 2, bs32);
		LoginValue = vmAdd(LoginValue, int(Login[I]), bs32);
		LoginValue = vmXor(LoginValue, 0x48ADEEEF);
		LoginValue = vmMul(LoginValue, uint8_t(Login[I]), bs32);
	}
	SerialIntValue = vmSimpleHexToInt(Serial);
	F = vmCmp(LoginValue, SerialIntValue, bs32);
	return vmJmp_E(F);
}

int main()
{
	const char* Login = "Rouse_";
	const char* Serial = GenerateSerial("Rouse_"); // "81E537F8"

	std::cout << CheckSerial(Login, Serial) << std::endl;
	std::cout << vmCheckSerialEx(Login, Serial) << std::endl;

	return 0;
}