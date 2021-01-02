/* 
 * CS:APP Data Lab 
 * 
 * Dennis Zang / #704766877
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
/* 
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int bang(int x) {
  /*Consider each pair of x and and (~x + 1). Each pair would end up with a positive and negative value, except for 0 and INT_MIN. So, we compare the signs of the two. To distinguish between 0 and INT_MIN, we would implement ((x ^ ((~x) + 1)) | x) in order to check the extra condition that x is not negative.*/
  return (((x ^ ((~x) + 1)) | x) >> 31) + 1;
  //return ~((x ^ (((~x) + 1))&(~x)) >> 31) + 1;
}
/*
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 4
 */
int bitCount(int x) {
  /*To count '1' bits, we create multiple masks to count those bits in each segment of 2, 4, 8, and 16 (in that order). The sum of the previous are summed up by shifting the number accordingly to create each subsequenct new number of 'segments'.*/
  int mask2 = ((((((0x55) << 8) + (0x55)) << 8) + (0x55)) << 8) + (0x55);
  int mask4 = ((((((0x33) << 8) + (0x33)) << 8) + (0x33)) << 8) + (0x33);
  int mask8 = ((((((0x0F) << 8) + (0x0F)) << 8) + (0x0F)) << 8) + (0x0F);
  int a = (x & mask2) + ((x >> 1) & mask2);
  a = (a & mask4) + ((a >> 2) & mask4);
  a = (a & mask8) + ((a >> 4) & mask8);
  return (a + (a >> 8) + (a >> 16) + (a >> 24)) & 0x3F;
}
/* 
 * bitOr - x|y using only ~ and & 
 *   Example: bitOr(6, 5) = 7
 *   Legal ops: ~ &
 *   Max ops: 8
 *   Rating: 1
 */
int bitOr(int x, int y) {
  /*This can be done by noting that the 0's digits are where it is 0 in both x and y. To find these spots, one can find the complement of x and y, then apply & to see where these digits are. Find the complement of the result would be the desired result.*/
  return ~((~x) & (~y));
}
/*
 * bitRepeat - repeat x's low-order n bits until word is full.
 *   Can assume that 1 <= n <= 32.
 *   Examples: bitRepeat(1, 1) = -1
 *             bitRepeat(7, 4) = 0x77777777
 *             bitRepeat(0x13f, 8) = 0x3f3f3f3f
 *             bitRepeat(0xfffe02, 9) = 0x10080402
 *             bitRepeat(-559038737, 31) = -559038737
 *             bitRepeat(-559038737, 32) = -559038737
 *   Legal ops: int and unsigned ! ~ & ^ | + - * / % << >>
 *             (This is more general than the usual integer coding rules.)
 *   Max ops: 40
 *   Rating: 4
 */
int bitRepeat(int x, int n) {
  /*This is done by masking x for the first n digits of x, and then repeatedly doubling the stackby shifting it repeatedly (at most 5 times in the case n = 1) (the "stack" doubles with each iteration). Masking is also done if the size becomes greater than 32, in which case the size doubling stops.*/
  int b = n - 1;
  int mask = ~((~1) << b);
  int a = x & mask;
  int bmask;

  a = ((a << b) << 1) + a;
  b = (n * 2);
  bmask = ((b - 32) >> 31);

  a = (a << (b & bmask)) + (a & bmask);
  b = (b * 2);
  bmask = ((b - 32) >> 31);

  a = (a << (b & bmask)) + (a & bmask);
  b = (b * 2);
  bmask = ((b - 32) >> 31);

  a = (a << (b & bmask)) + (a & bmask);
  b = (b * 2);
  bmask = ((b - 32) >> 31);

  a = (a << (b & bmask)) + (a & bmask);
  return a;
}
/* 
 * fitsBits - return 1 if x can be represented as an 
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int fitsBits(int x, int n) {
  /*This can be checked by checking that all the digits besides the last n-1 digits are all zeros or ones (depending if the number is positive (all 0's) or negative (all 1's)).*/
  int neg = x >> 31;
  int c = x >> (n + (~0));
  return !(neg ^ c);
}
/* 
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int getByte(int x, int n) {
  /*This can be done by creating as mask to get those specific bits (number of shifts is determined by shifting as well)*/
  int shift = n << 3;
  return (x >> shift) & (0xFF);
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
  /*This can be checked by seeing if the terms are positive or negative, and then checking if the first is smaller/equal to the second by adding ~y (since x + ~x = -1 and a number would have it's complement be nearly proportional to its absolute value) if and only if the signs match.*/
  int neg1 = x >> 31;
  int neg2 = y >> 31;
  int state = neg1 & ~neg2;
  return !!(state | ((((x + (~y)) >> 31)) & (!(neg1 ^ neg2))));
}
/* 
 * isPositive - return 1 if x > 0, return 0 otherwise 
 *   Example: isPositive(-1) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 3
 */
int isPositive(int x) {
  /*This is done by first checking if x in negative (by shifting), then checking if x is 0 (by masking).*/
  return !((x >> 31) | !(x & (~0)));
}
/* 
 * logicalShift - shift x to the right by n, using a logical shift
 *   Can assume that 0 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3 
 */
int logicalShift(int x, int n) {
  /*This is done by first creating a mask to omit the first n digits, then applying it after the shift.*/
  int mask = ~(((1 << 31) >> n) << 1);
  return ((x >> n) & mask);
}
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
  /*This is done by simply shifting a present 1's digit all the way to the left.*/
  return (1 << 31);
}
