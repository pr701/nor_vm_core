/****************************************************************************
* File    : vm_core.h
* Purpose : Core of virtual 32-bit processor based on NOR
* Author  : Alexander (Rouse_) Bagel
* Version : 1.0
* C++ implementation : pr701
*****************************************************************************/

#ifdef _MSC_VER
#pragma once
#endif

#ifndef _VM_CORE_H_
#define _VM_CORE_H_

#ifndef INLINE
	#if _MSC_VER && !__INTEL_COMPILER
		#define INLINE __forceinline
	#elif __GNUC__
		#define INLINE __attribute__((always_inline))
	#else
		#define INLINE inline
	#endif
#endif

#ifdef _WIN32
	#include <Windows.h>
#else
	typedef int BOOL;
#endif

#include <stdint.h>

struct TFlags
{
	BOOL CF;
	BOOL PF;
	BOOL ZF;
	BOOL SF;
	BOOL OF;
};

enum TBitSize
{
	bs8 = 8,
	bs16 = 16,
	bs32 = 32
};

INLINE void vmInitialize();
INLINE int vmNor(int a, int b);
INLINE int vmGetMask(int a);
INLINE BOOL vmFullAdder(BOOL a, BOOL b, BOOL p0, BOOL &p);
INLINE BOOL BOOL_(int a);

// basis
INLINE int vmNot(int a);
INLINE int vmOr(int a, int b);
INLINE int vmAnd(int a, int b);
INLINE int vmXor(int a, int b);

// basic_logical
INLINE BOOL vmNotB(BOOL a);
INLINE BOOL vmOrB(BOOL a, BOOL b);
INLINE BOOL vmAndB(BOOL a, BOOL b);
INLINE BOOL vmXorB(BOOL a, BOOL b);

// inc_dec_neg
INLINE int vmNeg(int a);
INLINE int vmInc(int a);
INLINE int vmDec(int a);

// sub_logical
INLINE int vmIfThen(int trueValue, int falseValue,  BOOL condition);

// bit_shift
INLINE int vmShl(int a, uint8_t shift, TBitSize bitsize);

// summators
INLINE int vmAdd(int a, int b,  TBitSize bitsize);
INLINE int vmSub(int a, int b,  TBitSize bitsize);

// mov
INLINE void vmMov8(int a, int8_t & b);
INLINE void vmMovU8(int a, uint8_t & b);
INLINE void vmMov16(int a, short & b);
INLINE void vmMovU16(int a, uint16_t& b);
INLINE void vmMov32(int a, int & b);
INLINE void vmMovU32(int a, uint32_t & b);
INLINE void vmMovB(BOOL a, BOOL & b);

// comparators
INLINE TFlags vmCmp(int a, int b,  TBitSize bitsize);
INLINE BOOL vmJmp_A(TFlags f);
INLINE BOOL vmJmp_AE(TFlags f);
INLINE BOOL vmJmp_B(TFlags f);
INLINE BOOL vmJmp_BE(TFlags f);
INLINE BOOL vmJmp_C(TFlags f);
INLINE BOOL vmJmp_E(TFlags f);
INLINE BOOL vmJmp_G(TFlags f);
INLINE BOOL vmJmp_GE(TFlags f);
INLINE BOOL vmJmp_L(TFlags f);
INLINE BOOL vmJmp_LE(TFlags f);
INLINE BOOL vmJmp_NA(TFlags f);
INLINE BOOL vmJmp_NAE(TFlags f);
INLINE BOOL vmJmp_NB(TFlags f);
INLINE BOOL vmJmp_NBE(TFlags f);
INLINE BOOL vmJmp_NC(TFlags f);
INLINE BOOL vmJmp_NE(TFlags f);
INLINE BOOL vmJmp_NG(TFlags f);
INLINE BOOL vmJmp_NGE(TFlags f);
INLINE BOOL vmJmp_NL(TFlags f);
INLINE BOOL vmJmp_NLE(TFlags f);
INLINE BOOL vmJmp_NO(TFlags f);
INLINE BOOL vmJmp_NP(TFlags f);
INLINE BOOL vmJmp_NS(TFlags f);
INLINE BOOL vmJmp_NZ(TFlags f);
INLINE BOOL vmJmp_O(TFlags f);
INLINE BOOL vmJmp_P(TFlags f);
INLINE BOOL vmJmp_PE(TFlags f);
INLINE BOOL vmJmp_PO(TFlags f);
INLINE BOOL vmJmp_S(TFlags f);
INLINE BOOL vmJmp_Z(TFlags f);

// mul_div_mod
INLINE int vmMul(int a, int b, TBitSize bitsize);

/* impl */

#pragma region mask

static uint32_t mask[32];

void vmInitialize()
{
	for (int I = 0; I < 32; ++I)
		mask[I] = 1 << I;
}

int vmGetMask(int a)
{
	return mask[a];
}

#pragma endregion

BOOL BOOL_(int a)
{
	return a != 0 ? 1 : 0;
}

#pragma region basis

int vmNor(int a, int b)
{
	// NOR = not a or not b
	// or simplified:
	return ~(a & b);
}

int vmNot(int a)
{
	return vmNor(a, a);
}

int vmOr(int a, int b)
{
	return vmNor(vmNor(a, a), vmNor(b, b));
}

int vmAnd(int a, int b)
{
	return vmNor(vmNor(a, b), vmNor(a, b));
}

int vmXor(int a, int b)
{
	return vmOr(vmAnd(a, vmNot(b)), vmAnd(vmNot(a), b));
}

#pragma endregion

#pragma region basic_logical

BOOL vmNotB(BOOL a)
{
	return BOOL(vmAnd(vmNot(vmAnd(int(a), 1)), 1));
}

BOOL vmOrB(BOOL a, BOOL b)
{
	return BOOL(vmAnd(vmOr(int(a), int(b)), 1));
}

BOOL vmAndB(BOOL a, BOOL b)
{
	return BOOL(vmAnd(vmAnd(int(a), int(b)), 1));
}

BOOL vmXorB(BOOL a, BOOL b)
{
	return vmOrB(vmAndB(a, vmNotB(b)), vmAndB(vmNotB(a), b));
}

#pragma endregion

#pragma region inc_dec_neg

int vmNeg(int a)
{
	return vmNot(a) + 1;
}

int vmInc(int a)
{
	return vmNeg(vmNot(a));
}

int vmDec(int a)
{
	return vmNot(vmNeg(a));
}

#pragma endregion

#pragma region sub_logical

int vmIfThen(int trueValue, int falseValue, BOOL condition)
{
	int c = vmDec(trueValue ^ trueValue);
	if (vmNotB(condition))
		c = vmInc(c);
	return vmOr(vmAnd(trueValue, c), vmAnd(falseValue, vmNot(c)));
}

#pragma endregion

#pragma region bit_shift

int vmShl(int a, uint8_t shift, TBitSize bitsize)
{
	int result;
	BOOL aBit;
	uint8_t count;
	int I;

	result = 0;
	count = uint8_t(bitsize);
	shift = shift % count;
	for (I = 0; I < shift; ++I)
		count = vmDec(count);
	for (I = 0; I < count; ++I)
	{
		aBit = vmAnd(a, vmGetMask(I)) != 0;
		result = vmOr(result, vmIfThen(vmGetMask(I + shift), 0, aBit));
	}
	return result;
}

#pragma endregion

#pragma region summators

BOOL vmFullAdder(BOOL a, BOOL b, BOOL p0, BOOL& p)
{
	BOOL result = BOOL(vmAndB(vmOrB(a, b), vmNotB(vmAndB(a, b))) ^ p0);
	p = vmOrB(vmAndB(a, b), vmOrB(vmAndB(a, p0), vmAndB(b, p0)));
	return result;
}

int vmAdd(int a, int b, TBitSize bitsize)
{
	int result;
	int I;
	BOOL aBit; BOOL bBit; BOOL sFlag; BOOL pFlag;

	result = 0;
	pFlag = false;
	for (I = 0; I < uint8_t(bitsize); ++I)
	{
		aBit = vmAnd(vmGetMask(I), a) != 0;
		bBit = vmAnd(vmGetMask(I), b) != 0;
		sFlag = vmFullAdder(aBit, bBit, pFlag, pFlag);
		result = vmOr(result, vmIfThen(vmGetMask(I), 0, sFlag));
	}
	return result;
}

int vmSub(int a, int b, TBitSize bitsize)
{
	return vmAdd(a, vmNeg(b), bitsize);
}

#pragma endregion

#pragma region mov

void vmMov8(int a, int8_t& b)
{
	b = int8_t(vmOr(a, a));
}

void vmMovU8(int a, uint8_t& b)
{
	b = uint8_t(vmOr(a, a));
}

void vmMov16(int a, short& b)
{
	b = short(vmOr(a, a));
}

void vmMovU16(int a, uint16_t& b)
{
	b = uint16_t(vmOr(a, a));
}

void vmMov32(int a, int& b)
{
	b = vmOr(a, a);
}

void vmMovU32(int a, uint32_t& b)
{
	b = vmOr(a, a);
}

void vmMovB(BOOL a, BOOL& b)
{
	b = BOOL_(vmOr(int(a), int(a)));
}

#pragma endregion

#pragma region comparators

TFlags vmCmp(int a, int b, TBitSize bitsize)
{
	TFlags result;
	BOOL aBit = 0;
	BOOL bBit = 0;
	BOOL nBit;
	BOOL pFlag;
	int I;
	int negB;

	pFlag = false;
	negB = vmNeg(b);
	vmMovB(false, result.CF);
	vmMovB(true, result.PF);
	vmMovB(true, result.ZF);
	vmMovB(false, result.OF);
	for (I = 0; I < uint8_t(bitsize); ++I)
	{
		vmMovB(vmAnd(vmGetMask(I), a) != 0, aBit);
		vmMovB(vmAnd(vmGetMask(I), b) != 0, bBit);
		vmMovB(vmAnd(vmGetMask(I), negB) != 0, nBit);
		vmMovB(BOOL_(vmIfThen(0, int(result.CF),
			BOOL_(vmAnd(int(aBit), vmNot(int(bBit)))))), result.CF);
		vmMovB(BOOL_(vmIfThen(1, int(result.CF),
			BOOL_(vmAnd(int(bBit), vmNot(int(aBit)))))), result.CF);
		vmMovB(vmFullAdder(aBit, nBit, pFlag, pFlag), result.SF);
		if ((result.SF))
		{
			vmMovB(false, result.ZF);
			vmMovB(BOOL_(vmIfThen(int(vmNotB(result.PF)), int(result.PF), I < 8)), result.PF);
		}
	}
	vmMovB(BOOL_(vmIfThen(1, int(result.OF),
		vmAndB(vmAndB(vmNotB(aBit), bBit), result.SF))), result.OF);
	vmMovB(BOOL_(vmIfThen(1, int(result.OF),
		vmAndB(vmAndB(aBit, vmNotB(bBit)), vmNotB(result.SF)))), result.OF);
	return result;
}

BOOL vmJmp_A(TFlags f)
{
	return vmAndB(vmNotB(f.CF), vmNotB(f.ZF));
}

BOOL vmJmp_AE(TFlags f)
{
	return vmNotB(f.CF);
}

BOOL vmJmp_B(TFlags f)
{
	return vmOrB(f.CF, f.CF);
}

BOOL vmJmp_BE(TFlags f)
{
	return vmOrB(f.CF, f.ZF);
}

BOOL vmJmp_C(TFlags f)
{
	return vmOrB(f.CF, f.CF);
}

BOOL vmJmp_E(TFlags f)
{
	return vmOrB(f.ZF, f.ZF);
}

BOOL vmJmp_G(TFlags f)
{
	return vmAndB(vmNotB(f.ZF), vmNotB(vmXorB(f.SF, f.OF)));
}

BOOL vmJmp_GE(TFlags f)
{
	return vmNotB(vmXorB(f.SF, f.OF));
}

BOOL vmJmp_L(TFlags f)
{
	return vmXorB(f.SF, f.OF);
}

BOOL vmJmp_LE(TFlags f)
{
	return vmOrB(vmOrB(f.ZF, f.ZF), vmXorB(f.SF, f.OF));
}

BOOL vmJmp_NA(TFlags f)
{
	return vmOrB(f.CF, f.ZF);
}

BOOL vmJmp_NAE(TFlags f)
{
	return vmOrB(f.CF, f.CF);
}

BOOL vmJmp_NB(TFlags f)
{
	return vmNotB(f.CF);
}

BOOL vmJmp_NBE(TFlags f)
{
	return vmAndB(vmNotB(f.CF), vmNotB(f.ZF));
}

BOOL vmJmp_NC(TFlags f)
{
	return vmNotB(f.CF);
}

BOOL vmJmp_NE(TFlags f)
{
	return vmNotB(f.ZF);
}

BOOL vmJmp_NG(TFlags f)
{
	return vmOrB(vmOrB(f.ZF, f.ZF), vmXorB(f.SF, f.OF));
}

BOOL vmJmp_NGE(TFlags f)
{
	return vmXorB(f.SF, f.OF);
}

BOOL vmJmp_NL(TFlags f)
{
	return vmXorB(f.SF, f.OF);
}

BOOL vmJmp_NLE(TFlags f)
{
	return vmAndB(vmNotB(f.ZF), vmNotB(vmXorB(f.SF, f.OF)));
}

BOOL vmJmp_NO(TFlags f)
{
	return vmNotB(f.OF);
}

BOOL vmJmp_NP(TFlags f)
{
	return vmNotB(f.PF);
}

BOOL vmJmp_NS(TFlags f)
{
	return vmNotB(f.SF);
}

BOOL vmJmp_NZ(TFlags f)
{
	return vmNotB(f.ZF);
}

BOOL vmJmp_O(TFlags f)
{
	return vmOrB(f.OF, f.OF);
}

BOOL vmJmp_P(TFlags f)
{
	return vmOrB(f.PF, f.PF);
}

BOOL vmJmp_PE(TFlags f)
{
	return vmOrB(f.PF, f.PF);
}

BOOL vmJmp_PO(TFlags f)
{
	return vmNotB(f.PF);
}

BOOL vmJmp_S(TFlags f)
{
	return vmOrB(f.SF, f.SF);
}

BOOL vmJmp_Z(TFlags f)
{
	return vmOrB(f.ZF, f.ZF);
}

#pragma endregion

#pragma region mul_div

int vmMul(int a, int b, TBitSize bitsize)
{
	int result;
	BOOL Sign;
	uint8_t Index;

	result = 0;
	Sign = vmOrB(a < 0, b < 0);
	if (Sign)
	{
		Sign = vmNotB(vmAndB(a < 0, b < 0));
		if (a < 0)
			a = vmNeg(a);
		if (b < 0)
			b = vmNeg(b);
	}
	if (a < b)
	{
		a = a + b;
		b = a - b;
		a = a - b;
	}
	while (a != 0)
	{
		Index = 0;

		while ((vmGetMask(Index) <= a) && (Index < uint8_t(bitsize) - 1))
			++Index;

		result = vmAdd(vmShl(b, vmDec(Index), bitsize), result, bitsize);
		a -= vmGetMask(vmDec(Index));
	}
	if (Sign)
		result = vmNeg(result);
	return result;
}

#pragma endregion

#endif //_VM_CORE_H_
