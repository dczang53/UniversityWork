// takes: 2 bits and a potential carry in
// gives: result and a potential carry out
module add2bit(
    input wire b0,
    input wire b1,
    input wire cin,
    output wire out,
    output wire cout
);

    // calculate the result and the carry out
	assign out = b0 ^ b1 ^ cin;
	assign cout = (b0 & b1) | (b0 & cin) | (b1 & cin);

endmodule


// takes: 2 4-bit inputs and a potential carry in value
// gives: a 5-bit output
module add4bit(
    input wire [3:0] b0,
    input wire [3:0] b1,
    input wire cin,
    output wire [4:0] out
);

    // the inputs/outputs for the various add2bit calls
    wire r0, c0;
    wire r1, c1;
    wire r2, c2;
    wire r3, c3;

    // call add2bit for each bit position with carry in/out
    add2bit add0 (b0[0], b1[0], cin, r0, c0);    
    add2bit add1 (b0[1], b1[1], c0, r1, c1);
    add2bit add2 (b0[2], b1[2], c1, r2, c2);
    add2bit add3 (b0[3], b1[3], c2, r3, c3);

    // combine the results and carry out to get final result
    assign out = {c3, r3, r2, r1, r0};

endmodule


// takes: 2 5-bit inputs
// gives: a 6-bit output
module add5bit(
    input wire [4:0] b0,
    input wire [4:0] b1,
    output wire [5:0] out
);

    // the inputs/outputs for the various add2bit calls
    wire [4:0] r0;
    wire r1, c1;

    // call add4bit for the intial result and add2bit for the final bits
    add4bit add0 (b0[3:0], b1[3:0], 1'b0, r0);    
    add2bit add1 (b0[4], b1[4], r0[4], r1, c1);

    // combine the results and carry out to get final result
    assign out = {c1, r1, r0[3:0]};

endmodule


// borrowed from my partner
module isequal(
    input wire [3:0] b0, 
    input wire [3:0] b1, 
    output wire out
);

	wire [3:0] aXNORb;
	assign aXNORb = b0 ~^ b1;
	assign out = (aXNORb[3] & aXNORb[2] & aXNORb[1] & aXNORb[0]);
endmodule


// borrowed from partner
module ismultiple(
    input wire [3:0] b0, 
    output wire out
);

    // test for a small number of possible values
    wire t0, t1;
    isequal neg11 (b0, 4'b1011, t0);
    isequal zero0 (b0, 4'b0000, t1);

    assign out = t0 | t1;

endmodule


// test module with binary operators
module DivBy11(
    input wire [15:0] bcd,
    output wire out
);

    wire [3:0] inv0; 
    wire [3:0] inv1;
    wire [3:0] inv2;
    wire [3:0] inv3;
    wire [4:0] sub0;
    wire [4:0] sub1;
    wire [4:0] sub2;
    wire [4:0] sub3;
    wire [5:0] add0;
    wire [5:0] add1;
    assign inv0 = {~bcd[3], ~bcd[2], ~bcd[1], ~bcd[0]};
    assign inv1 = {~bcd[11], ~bcd[10], ~bcd[9], ~bcd[8]};

    add4bit add40 (bcd[7:4], inv0, 1'b1, sub0);
    add4bit add41 (bcd[15:12], inv1, 1'b1, sub1);

    assign inv2 = {~sub0[3], ~sub0[2], ~sub0[1], ~sub0[0]};
    assign inv3 = {~sub1[3], ~sub1[2], ~sub1[1], ~sub1[0]};

    add4bit add42 (inv2, 1, 1'b0, sub2);
    add4bit add43 (inv3, 1, 1'b0, sub3);

    add5bit add50 (sub0, sub1, add0);
    add5bit add51 (sub2, sub3, add1);

    wire o0;
    wire o1;

    ismultiple ismult0 (add0[3:0], o0);
    ismultiple ismult1 (add1[3:0], o1);

    assign out = ((sub0[4] & sub1[4]) & o0) | (!(sub0[4] & sub1[4]) & o1);

endmodule