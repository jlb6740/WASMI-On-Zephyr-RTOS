/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <stdio.h>
#include <kernel.h>
#include <rust_wasmi/wasmi_cdriver.h>

/*
void testprint(int rust) {
	s32_t sleep_time = 2000;
	printf("Called me from rust %d\n\n", rust);
	k_sleep(sleep_time);
}

*/
void main(void)
{
	int i = 0;
	int return_value = 0; 

	s32_t sleep_time = 1000;
	while (true) {
		printf("Hello World here (%d)! %s\n", i, CONFIG_BOARD);
		i++;
		k_sleep(sleep_time);
		return_value = hi_rust(i);
		k_sleep(sleep_time);
		printf("Hello Rust returned (%d)! %s\n", return_value, CONFIG_BOARD);
		//testprint(3);
	}
}


// Pulled from here: https://opensource.apple.com/source/Libm/Libm-315/Source/ARM/fmod.c.auto.html
double fmod( double x, double y )
{
	union{ double d; uint64_t u;}ux = {x};
	union{ double d; uint64_t u;}uy = {y};
	
	uint64_t absx = ux.u & ~0x8000000000000000ULL;
	uint64_t absy = uy.u & ~0x8000000000000000ULL;

	if( absx-1ULL >= 0x7fefffffffffffffULL || absy-1ULL >= 0x7fefffffffffffffULL )
	{
		double fabsx = __builtin_fabs(x);
		double fabsy = __builtin_fabs(y);
		
		// deal with NaN
		if( x != x || y != y )
			return x + y;

		//x = Inf or y = 0, return Invalid per IEEE-754
		if( fabsx == __builtin_inf() || 0.0 == y )
		{
			return __builtin_nan("");
		}

		//handle trivial case
		if( fabsy == __builtin_inf() || 0.0 == x )
		{
			return x;
		}
	}
 
	if( absy >= absx )
	{
		if( absy == absx )
		{
			ux.u ^= absx;
			return ux.d;
		}
		
		return x;
	}
 
	int32_t expx = absx >> 52;
	int32_t expy = absy >> 52;
	int64_t sx = absx & 0x000fffffffffffff;
	int64_t sy = absy & 0x000fffffffffffff;

	if( 0 == expx )
	{
		uint32_t shift = __builtin_clzll( absx ) - (64-53);
		sx <<= shift;
		expx = 1 - shift;
	}

	if( 0 == expy )
	{
		uint32_t shift = __builtin_clzll( absy ) - (64-53);
		sy <<= shift;
		expy = 1 - shift;
	}
	sx |= 0x0010000000000000ULL;
	sy |= 0x0010000000000000ULL;


	int32_t idiff = expx - expy;
	int32_t shift = 0;
	int64_t mask;
	
	do
	{
		sx <<= shift;
		idiff += ~shift;
		sx -= sy;
		mask = sx >> 63;
		sx += sx;
		sx += sy & mask;
		shift = __builtin_clzll( sx ) - (64-53);
	}
	while( idiff >= shift && sx != 0LL );

	if( idiff < 0 )
	{
		sx += sy & mask;
		sx >>= 1;
		idiff = 0;
	}
	
	sx <<= idiff;
	
	if( 0 == sx )
	{
		ux.u &= 0x8000000000000000;
		return ux.d;
	}
	
	shift = __builtin_clzll( sx ) - (64-53);
	sx <<= shift;
	expy -= shift;
	sx &= 0x000fffffffffffffULL;
	sx |= ux.u & 0x8000000000000000ULL;
	if( expy > 0 )
	{
		ux.u = sx | ((int64_t) expy << 52);
		return ux.d;
	}
	
	expy += 1022;
	ux.u = sx | ((int64_t) expy << 52);
	return ux.d * 0x1.0p-1022;

}


// Pulled from here: https://opensource.apple.com/source/Libm/Libm-315/Source/ARM/fmod.c.auto.html
float fmodf( float x, float y )
{
	
	union{ float f; uint32_t u; }ux = {x};
	union{ float f; uint32_t u; }uy = {y};

	uint32_t absx = ux.u & 0x7fffffffU;
	uint32_t absy = uy.u & 0x7fffffffU;
	if( absx - 1U >= 0x7f7fffffU || absy - 1U >= 0x7f7fffffU )
	{
		float fabsx = __builtin_fabsf(x);	
		float fabsy = __builtin_fabsf(y);

		// deal with NaN
		if( x != x || y != y )
			return x + y;

		//x = Inf or y = 0, return Invalid per IEEE-754
		if( fabsx == __builtin_inff() || 0.0f == y )
		{
#ifdef ARMLIBM_SET_FLAGS
			return required_add_float( __builtin_inff(), -__builtin_inff() );	//set invalid
#else
			return __builtin_nan("");
#endif
		}

		//handle trivial case
		if( fabsy == __builtin_inff() || 0.0f == x )
		{
#ifdef ARMLIBM_SET_FLAGS
			required_add_float( fabsx, 0.0f );	// signal underflow (that is, no flag set,
												// but exception occurs if unmasked) if x is denorm
#endif
			return x;
		}
	}
 
	if( absy >= absx )
	{
		if( absy == absx )
		{
			ux.u &= 0x80000000;
			return ux.f;
		}
		return x;
	}
 
	//extract exponent, mantissa
	int32_t expx = absx >> 23;
	int32_t expy = absy >> 23;
	int32_t sx = absx & 0x007fffff;			// x significand
	int32_t sy = absy & 0x007fffff;			// y significand
	if( 0 == expx )		//denormal x
	{
		//calculate shift to move leading bit to exponents field
		uint32_t shift = __builtin_clzl( absx ) - (8*sizeof(long) - 24);
		sx <<= shift;	//do the shift
		expx = 1-shift;	//adjust the biased exponent accordingly, -1 here for the implicit shift to make implicit denorm leading bit explicit
	}
	if( 0 == expy )		//denormal y
	{
		//calculate shift to move leading bit to exponents field
		uint32_t shift = __builtin_clzl( absy ) - (8*sizeof(long) - 24);
		sy <<= shift;	//do the shift
		expy = 1-shift;	//adjust the biased exponent accordingly, -1 here for the implicit shift to make implicit denorm leading bit explicit
	}
	//make implicit bits explicit
	sx |= 0x00800000;
	sy |= 0x00800000;
	
		
	// The idea here is to iterate until the exponent of x is the same as y
	// Calculate the number of bits we can safely shave off before we reach parity
	int32_t idiff = expx - expy;	
	int32_t shift = 0;
	int32_t mask;	

	//since idiff is always >= 0 here, it is safe to enter
	//We always shift by shift+1 here, so in the first iteration, the worst that can happen
	do
	{
		// move the leading bit of x to the 23rd bit
		sx <<= shift;

		//Keep track that we did that
		idiff += ~shift;			// idiff -= shift + 1, +1 for the shift below
	
		//The two values both have the 23rd bit set as the leading bit  (24 bit unsigned number)
		//subtract one from the other. This gives us a signed 23 bit number in the range { -0x00ffffff ... 0x00ffffff }
		sx -= sy;		

		//record the sign
		mask = sx >> 31;

		//shift x left 1 to restore a 24 bit quantity
		sx += sx;		//this is potentially 1 shift too far, which we patch up later

		//if negative, we add back sy to restore to postiveness. This is the same as x - y + 0.5y = - 0.5y 
		// instead of x-y. We've effectively hoisted the subtract that would have appeared in the next loop
		// iteration here, and saved a test+branch in exchange for a shift and and. (The add happens either way.)
		sx += sy & mask;	

		//figure out how far we need to shift sx to get the leading bit into the 23rd position
		shift = __builtin_clzl( sx ) - (8*sizeof(long) - 24);
	}
	while( idiff >= shift && sx != 0);
	
	//We may have gone too far 
	if( idiff < 0 )	
	{
		//If so, rewind a little.
		// if sx - sy succeeded, it was the right thing to do, the only thing we did wrong was the shift
		// if sx - sy yielded a negative number, then we shouldn't have done that either
		sx += sy & mask;
		sx >>= 1;
//debug code to make sure we didn't do something dumb here
		idiff = 0;
	}

	//We may have some bits left over to shift in. 
	sx <<= idiff;
	
//convert back to float
	if( 0 == sx )
	{
		ux.u &= 0x80000000;
		return ux.f;
	}
			
	//figure out how far we need to shift in order to move leading bit into exponent field
	shift = __builtin_clzl( sx ) - (8*sizeof(long) - 24);
	sx <<= shift;				// move leading bit into exponent field
	expy -= shift;				// account for the shift in the exponent
	sx &= 0x007fffff;			// remove leading bit
	sx |= ux.u & 0x80000000;	//or in sign
	if( expy > 0 )
	{
		ux.u = sx | (expy << 23);
		return ux.f;
	}
	
	//denormal
	expy += 126;
	ux.u = sx | (expy << 23);
	return ux.f * 0x1.0p-126f;
	
}
