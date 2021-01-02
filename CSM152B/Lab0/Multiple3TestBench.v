module add3_1bitTB;
	reg a, b, c;
	reg pass;
	wire d, e;

	Add3_1bit AddTestBench (
		.a(a),
		.b(b),
		.c(c),
		.d(d),
		.e(e)
	);

	initial
		begin
			assign a = 0;
			assign b = 0;
			assign c = 0;
			assign pass = (d == 0) & (e == 0);
			#10 assign a = 1;
			assign b = 0;
			assign c = 0;
			assign pass = (d == 1) & (e == 0);
			#10 assign a = 0;
			assign b = 1;
			assign c = 0;
			assign pass = (d == 1) & (e == 0);
			#10 assign a = 0;
			assign b = 0;
			assign c = 1;
			assign pass = (d == 1) & (e == 0);
			#10 assign a = 1;
			assign b = 1;
			assign c = 0;
			assign pass = (d == 0) & (e == 1);
			#10 assign a = 1;
			assign b = 0;
			assign c = 1;
			assign pass = (d == 0) & (e == 1);
			#10 assign a = 0;
			assign b = 1;
			assign c = 1;
			assign pass = (d == 0) & (e == 1);
			#10 assign a = 1;
			assign b = 1;
			assign c = 1;
			assign pass = (d == 1) & (e == 1);
		end
endmodule

module add2_4bitTB;
	reg [3:0] a, b;
	reg pass;
	wire [4:0] c;

	Add2_4bit AddTestBench (
		.a(a),
		.b(b),
		.c(c)
	);

	initial
		begin
			assign a = 8;
			assign b = 7;
			assign pass = (c == 15);
			#20 assign a = 3;
			assign pass = (c == 10);
			#20 assign b = 4;
			assign pass = (c == 7);
			#20 assign a = 0;
			assign pass = (c == 4);
			#20 assign b = 1;
			assign pass = (c == 1);
			#20 assign a = 5;
			assign pass = (c == 6);
			#20 assign b = 7;
			assign pass = (c == 12);
		end
endmodule

module add2_5bitTB;
	reg [4:0] a, b;
	reg pass;
	wire [5:0] c;

	Add2_5bit AddTestBench (
		.a(a),
		.b(b),
		.c(c)
	);

	initial
		begin
			assign a = 8;
			assign b = 7;
			assign pass = (c == 15);
			#20 assign a = 3;
			assign pass = (c == 10);
			#20 assign b = 4;
			assign pass = (c == 7);
			#20 assign a = 0;
			assign pass = (c == 4);
			#20 assign b = 1;
			assign pass = (c == 1);
			#20 assign a = 5;
			assign pass = (c == 6);
			#20 assign b = 7;
			assign pass = (c == 12);
			#20 assign a = 12;
			assign pass = (c == 19);
			#20 assign b = 10;
			assign pass = (c == 22);
			#20 assign a = 18;
			assign pass = (c == 28);
			#20 assign b = 13;
			assign pass = (c == 31);
			#20 assign a = 23;
			assign pass = (c == 36);
		end
endmodule

module CheckEqualTB;
	reg [5:0] a, b;
	reg pass;
	wire c;

	CheckEqual EqualityTestBench (
		.a(a),
		.b(b),
		.c(c)
	);

	initial
		begin
			assign a = 8;
			assign b = 7;
			assign pass = ~c;
			#20 assign a = 7;
			assign pass = c;
			#20 assign b = 4;
			assign pass = ~c;
			#20 assign a = 4;
			assign pass = c;
			#20 assign b = 1;
			assign pass = ~c;
			#20 assign a = 5;
			assign pass = ~c;
			#20 assign b = 5;
			assign pass = c;
			#20 assign a = 0;
			assign b = 0;
			assign pass = c;
			#20 assign a = 63;
			assign b = 62;
			assign pass = ~c;
			#20 assign b = 63;
			assign pass = c;
		end
endmodule

module Multiple3SmallTB;
	reg [5:0] a;
	reg pass;
	wire b;

	CheckMultiple3Small MultipleTest (
		.a(a),
		.b(b)
	);

	initial	
		begin
			assign a = 0;
			assign pass = b;
			#10 assign a = 1;
			assign pass = ~b;
			#10 assign a = 2;
			assign pass = ~b;
			#10 assign a = 3;
			assign pass = b;
			#10 assign a = 4;
			assign pass = ~b;
			#10 assign a = 5;
			assign pass = ~b;
			#10 assign a = 6;
			assign pass = b;
			#10 assign a = 7;
			assign pass = ~b;
			#10 assign a = 8;
			assign pass = ~b;
			#10 assign a = 9;
			assign pass = b;
			#10 assign a = 10;
			assign pass = ~b;
			#10 assign a = 11;
			assign pass = ~b;
			#10 assign a = 12;
			assign pass = b;
			#10 assign a = 13;
			assign pass = ~b;
			#10 assign a = 14;
			assign pass = ~b;
			#10 assign a = 15;
			assign pass = b;
			#10 assign a = 16;
			assign pass = ~b;
			#10 assign a = 17;
			assign pass = ~b;
			#10 assign a = 18;
			assign pass = b;
			#10 assign a = 19;
			assign pass = ~b;
			#10 assign a = 20;
			assign pass = ~b;
			#10 assign a = 21;
			assign pass = b;
			#10 assign a = 22;
			assign pass = ~b;
			#10 assign a = 23;
			assign pass = ~b;
			#10 assign a = 24;
			assign pass = b;
			#10 assign a = 25;
			assign pass = ~b;
			#10 assign a = 26;
			assign pass = ~b;
			#10 assign a = 27;
			assign pass = b;
			#10 assign a = 28;
			assign pass = ~b;
			#10 assign a = 29;
			assign pass = ~b;
			#10 assign a = 30;
			assign pass = b;
			#10 assign a = 31;
			assign pass = ~b;
			#10 assign a = 32;
			assign pass = ~b;
			#10 assign a = 33;
			assign pass = b;
			#10 assign a = 34;
			assign pass = ~b;
			#10 assign a = 35;
			assign pass = ~b;
			#10 assign a = 36;
			assign pass = b;
		end
endmodule

module Multiple3TB;
	reg [15:0] a;
	reg pass;
	wire b;

	CheckMultiple3 ProjectCheck (
		.a(a),
		.b(b)
	);

	initial
		begin
			assign a = 0;
			assign pass = b;
			#10 assign a = 3;
			assign pass = b;
			#10 assign a = 6;
			assign pass = b;
			#10 assign a = 9;
			assign pass = b;
			#10 assign a = 8;
			assign pass = ~b;
			#10 assign a = 25;	// BCD value is 19
			assign pass = ~b;
			#10 assign a = 24;	// BCD value is 18
			assign pass = b;
			#10 assign a = 39;	// BCD value is 27
			assign pass = b;
			#10 assign a = 68;	// BCD value is 44
			assign pass = ~b;
			#10 assign a = 258;	// BCD value is 102
			assign pass = b;
			#10 assign a = 865;	// BCD value is 361
			assign pass = ~b;
			#10 assign a = 2436;	// BCD value is 984
			assign pass = b;
			#10 assign a = 4369;	// BCD value is 1111
			assign pass = ~b;
			#10 assign a = 14230;	// BCD value is 3796
			assign pass = ~b;
			#10 assign a = 26994;	// BCD value is 6972
			assign pass = b;
			#10 assign a = 34952; 	// BCD value is 8888
			assign pass = ~b;
			#10 assign a = 39321;	// BCD value is 9999
			assign pass = b;
		end
endmodule