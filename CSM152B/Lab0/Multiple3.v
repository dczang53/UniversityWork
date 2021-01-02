module Invert2_1bit (a, b);
	input a;
	output b;
	
	assign b = ~ a;
endmodule

module Add3_1bit (a, b, c, d, e);
	input a;
	input b;
	input c;
	output d;
	output e;

	assign d = a ^ b ^ c;
	assign e = (a & b) | (a & c) | (b & c);
endmodule


module Add2_4bit (a, b, c);
	input [3:0] a;
	input [3:0] b;
	output [4:0] c;

	wire addFirstResult;
	wire addFirstCarry;
	wire addSecondResult;
	wire addSecondCarry;
	wire addThirdResult;
	wire addThirdCarry;
	wire addFourthResult;
	wire addFourthCarry;

	Add3_1bit addFirst (a[0], b[0], 1'b0, addFirstResult, addFirstCarry);
	Add3_1bit addSecond (a[1], b[1], addFirstCarry, addSecondResult, addSecondCarry);
	Add3_1bit addThird (a[2], b[2], addSecondCarry, addThirdResult, addThirdCarry);
	Add3_1bit addFourth (a[3], b[3], addThirdCarry, addFourthResult, addFourthCarry);
	
	assign c = {addFourthCarry, addFourthResult, addThirdResult, addSecondResult, addFirstResult};
endmodule

module Add2_5bit (a, b, c);
	input [4:0] a;
	input [4:0] b;
	output [5:0] c;

	wire [4:0] firstFourResult;
	wire fifthResult;
	wire fifthCarry;

	Add2_4bit addFirstFour (a[3:0], b[3:0], firstFourResult);
	Add3_1bit addFifth (a[4], b[4], firstFourResult[4], fifthResult, fifthCarry);
	
	assign c = {fifthCarry, fifthResult, firstFourResult[3:0]};
endmodule

module CheckEqual (a, b, c);
	input [5:0] a;
	input [5:0] b;
	output c;

	wire [5:0] aXNORb;
	assign aXNORb = a ~^ b;
	assign c = (aXNORb[5] & aXNORb[4] & aXNORb[3] & aXNORb[2] & aXNORb[1] & aXNORb[0]);
endmodule

module CheckMultiple3Small (a, b);
// Checks if a is a multiple of 3. 0 is not a multiple of 3
	input [5:0] a;
	output b;

	wire o0, o1, o2, o3, o4, o5, o6, o7, o8, o9, o10, o11, o12;

	CheckEqual eq0 (a, 0, o0);
	CheckEqual eq3 (a, 3, o1);
	CheckEqual eq6 (a, 6, o2);
	CheckEqual eq9 (a, 9, o3);
	CheckEqual eq12 (a, 12, o4);
	CheckEqual eq15 (a, 15, o5);
	CheckEqual eq18 (a, 18, o6);
	CheckEqual eq21 (a, 21, o7);
	CheckEqual eq24 (a, 24, o8);
	CheckEqual eq27 (a, 27, o9);
	CheckEqual eq30 (a, 30, o10);
	CheckEqual eq33 (a, 33, o11);
	CheckEqual eq36 (a, 36, o12);

	assign b = o0 | o1 | o2 | o3 | o4 | o5 | o6 | o7 | o8 | o9 | o10 | o11 | o12;

endmodule

module CheckMultiple3 (a, b);
	input [15:0] a;
	output b;

	wire [4:0] sum1_1;
	wire [4:0] sum1_2;
	wire [5:0] sum2_1;

	Add2_4bit add1_1 (a[15:12], a[11:8], sum1_1);
	Add2_4bit add1_2 (a[7:4], a[3:0], sum1_2);
	Add2_5bit add2_1 (sum1_1, sum1_2, sum2_1);
	CheckMultiple3Small checkMult3 (sum2_1, b);

endmodule