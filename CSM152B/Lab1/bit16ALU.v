// -----------------------------------------------
// parallel port mux's
module pportMux2_1 (a, b, sel, out);
    input [15:0] a;
    input [15:0] b;
    input sel;
    output [15:0] out;

    mux2_1 mux0(.a(a[0]),
                .b(b[0]),
                .sel(sel),
                .o(out[0]));
    mux2_1 mux1(.a(a[1]),
                .b(b[1]),
                .sel(sel),
                .o(out[1]));
    mux2_1 mux2(.a(a[2]),
                .b(b[2]),
                .sel(sel),
                .o(out[2]));
    mux2_1 mux3(.a(a[3]),
                .b(b[3]),
                .sel(sel),
                .o(out[3]));
    mux2_1 mux4(.a(a[4]),
                .b(b[4]),
                .sel(sel),
                .o(out[4]));
    mux2_1 mux5(.a(a[5]),
                .b(b[5]),
                .sel(sel),
                .o(out[5]));
    mux2_1 mux6(.a(a[6]),
                .b(b[6]),
                .sel(sel),
                .o(out[6]));
    mux2_1 mux7(.a(a[7]),
                .b(b[7]),
                .sel(sel),
                .o(out[7]));
    mux2_1 mux8(.a(a[8]),
                .b(b[8]),
                .sel(sel),
                .o(out[8]));
    mux2_1 mux9(.a(a[9]),
                .b(b[9]),
                .sel(sel),
                .o(out[9]));
    mux2_1 mux10(.a(a[10]),
                 .b(b[10]),
                 .sel(sel),
                 .o(out[10]));
    mux2_1 mux11(.a(a[11]),
                 .b(b[11]),
                 .sel(sel),
                 .o(out[11]));
    mux2_1 mux12(.a(a[12]),
                 .b(b[12]),
                 .sel(sel),
                 .o(out[12]));
    mux2_1 mux13(.a(a[13]),
                 .b(b[13]),
                 .sel(sel),
                 .o(out[13]));
    mux2_1 mux14(.a(a[14]),
                 .b(b[14]),
                 .sel(sel),
                 .o(out[14]));
    mux2_1 mux15(.a(a[15]),
                 .b(b[15]),
                 .sel(sel),
                 .o(out[15]));
endmodule

module pportMux16_1 (a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, sel, out);
    input [15:0] a;
    input [15:0] b;
    input [15:0] c;
    input [15:0] d;
    input [15:0] e;
    input [15:0] f;
    input [15:0] g;
    input [15:0] h;
    input [15:0] i;
    input [15:0] j;
    input [15:0] k;
    input [15:0] l;
    input [15:0] m;
    input [15:0] n;
    input [15:0] o;
    input [15:0] p;
    input [3:0] sel;
    output [15:0] out;

    wire [15:0] outA0;
    wire [15:0] outA1;
    wire [15:0] outA2;
    wire [15:0] outA3;
    wire [15:0] outA4;
    wire [15:0] outA5;
    wire [15:0] outA6;
    wire [15:0] outA7;
    pportMux2_1 mux00(.a(a), .b(b), .sel(sel[0]), .out(outA0));
    pportMux2_1 mux01(.a(c), .b(d), .sel(sel[0]), .out(outA1));
    pportMux2_1 mux02(.a(e), .b(f), .sel(sel[0]), .out(outA2));
    pportMux2_1 mux03(.a(g), .b(h), .sel(sel[0]), .out(outA3));
    pportMux2_1 mux04(.a(i), .b(j), .sel(sel[0]), .out(outA4));
    pportMux2_1 mux05(.a(k), .b(l), .sel(sel[0]), .out(outA5));
    pportMux2_1 mux06(.a(m), .b(n), .sel(sel[0]), .out(outA6));
    pportMux2_1 mux07(.a(o), .b(p), .sel(sel[0]), .out(outA7));

    wire [15:0] outB0;
    wire [15:0] outB1;
    wire [15:0] outB2;
    wire [15:0] outB3;
    pportMux2_1 mux10(.a(outA0), .b(outA1), .sel(sel[1]), .out(outB0));
    pportMux2_1 mux11(.a(outA2), .b(outA3), .sel(sel[1]), .out(outB1));
    pportMux2_1 mux12(.a(outA4), .b(outA5), .sel(sel[1]), .out(outB2));
    pportMux2_1 mux13(.a(outA6), .b(outA7), .sel(sel[1]), .out(outB3));

    wire [15:0] outC0;
    wire [15:0] outC1;
    pportMux2_1 mux20(.a(outB0), .b(outB1), .sel(sel[2]), .out(outC0));
    pportMux2_1 mux21(.a(outB2), .b(outB3), .sel(sel[2]), .out(outC1));

    pportMux2_1 mux3(.a(outC0), .b(outC1), .sel(sel[3]), .out(out));
endmodule

module mux16_1 (a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, sel, out);
    input a;
    input b;
    input c;
    input d;
    input e;
    input f;
    input g;
    input h;
    input i;
    input j;
    input k;
    input l;
    input m;
    input n;
    input o;
    input p;
    input [3:0] sel;
    output out;

    wire outA0;
    wire outA1;
    wire outA2;
    wire outA3;
    wire outA4;
    wire outA5;
    wire outA6;
    wire outA7;
    mux2_1 mux00(.a(a), .b(b), .sel(sel[0]), .o(outA0));
    mux2_1 mux01(.a(c), .b(d), .sel(sel[0]), .o(outA1));
    mux2_1 mux02(.a(e), .b(f), .sel(sel[0]), .o(outA2));
    mux2_1 mux03(.a(g), .b(h), .sel(sel[0]), .o(outA3));
    mux2_1 mux04(.a(i), .b(j), .sel(sel[0]), .o(outA4));
    mux2_1 mux05(.a(k), .b(l), .sel(sel[0]), .o(outA5));
    mux2_1 mux06(.a(m), .b(n), .sel(sel[0]), .o(outA6));
    mux2_1 mux07(.a(o), .b(p), .sel(sel[0]), .o(outA7));

    wire outB0;
    wire outB1;
    wire outB2;
    wire outB3;
    mux2_1 mux10(.a(outA0), .b(outA1), .sel(sel[1]), .o(outB0));
    mux2_1 mux11(.a(outA2), .b(outA3), .sel(sel[1]), .o(outB1));
    mux2_1 mux12(.a(outA4), .b(outA5), .sel(sel[1]), .o(outB2));
    mux2_1 mux13(.a(outA6), .b(outA7), .sel(sel[1]), .o(outB3));

    wire outC0;
    wire outC1;
    mux2_1 mux20(.a(outB0), .b(outB1), .sel(sel[2]), .o(outC0));
    mux2_1 mux21(.a(outB2), .b(outB3), .sel(sel[2]), .o(outC1));

    mux2_1 mux3(.a(outC0), .b(outC1), .sel(sel[3]), .o(out));
endmodule
// -----------------------------------------------
// Helper Submodules
// Ripple Adder
module bit16AddSub (a, b, ctrl, ans, cout);
    input [15:0] a;
    input [15:0] b;
    input ctrl;
    output [15:0] ans;
    output cout;

    wire cout1;
    bitALU calc0(.A(a[0]),
                 .B(b[0]),
                 .CIN(ctrl),
                 .CTRL(ctrl),
                 .ANS(ans[0]),
                 .COUT(cout1));

    wire cout2;
    bitALU calc1(.A(a[1]),
                 .B(b[1]),
                 .CIN(cout1),
                 .CTRL(ctrl),
                 .ANS(ans[1]),
                 .COUT(cout2));

    wire cout3;
    bitALU calc2(.A(a[2]),
                 .B(b[2]),
                 .CIN(cout2),
                 .CTRL(ctrl),
                 .ANS(ans[2]),
                 .COUT(cout3));

    wire cout4;
    bitALU calc3(.A(a[3]),
                 .B(b[3]),
                 .CIN(cout3),
                 .CTRL(ctrl),
                 .ANS(ans[3]),
                 .COUT(cout4));

    wire cout5;
    bitALU calc4(.A(a[4]),
                 .B(b[4]),
                 .CIN(cout4),
                 .CTRL(ctrl),
                 .ANS(ans[4]),
                 .COUT(cout5));

    wire cout6;
    bitALU calc5(.A(a[5]),
                 .B(b[5]),
                 .CIN(cout5),
                 .CTRL(ctrl),
                 .ANS(ans[5]),
                 .COUT(cout6));

    wire cout7;
    bitALU calc6(.A(a[6]),
                 .B(b[6]),
                 .CIN(cout6),
                 .CTRL(ctrl),
                 .ANS(ans[6]),
                 .COUT(cout7));

    wire cout8;
    bitALU calc7(.A(a[7]),
                 .B(b[7]),
                 .CIN(cout7),
                 .CTRL(ctrl),
                 .ANS(ans[7]),
                 .COUT(cout8));

    wire cout9;
    bitALU calc8(.A(a[8]),
                 .B(b[8]),
                 .CIN(cout8),
                 .CTRL(ctrl),
                 .ANS(ans[8]),
                 .COUT(cout9));

    wire cout10;
    bitALU calc9(.A(a[9]),
                 .B(b[9]),
                 .CIN(cout9),
                 .CTRL(ctrl),
                 .ANS(ans[9]),
                 .COUT(cout10));

    wire cout11;
    bitALU calc10(.A(a[10]),
                  .B(b[10]),
                  .CIN(cout10),
                  .CTRL(ctrl),
                  .ANS(ans[10]),
                  .COUT(cout11));

    wire cout12;
    bitALU calc11(.A(a[11]),
                  .B(b[11]),
                  .CIN(cout11),
                  .CTRL(ctrl),
                  .ANS(ans[11]),
                  .COUT(cout12));

    wire cout13;
    bitALU calc12(.A(a[12]),
                  .B(b[12]),
                  .CIN(cout12),
                  .CTRL(ctrl),
                  .ANS(ans[12]),
                  .COUT(cout13));

    wire cout14;
    bitALU calc13(.A(a[13]),
                  .B(b[13]),
                  .CIN(cout13),
                  .CTRL(ctrl),
                  .ANS(ans[13]),
                  .COUT(cout14));

    wire cout15;
    bitALU calc14(.A(a[14]),
                  .B(b[14]),
                  .CIN(cout14),
                  .CTRL(ctrl),
                  .ANS(ans[14]),
                  .COUT(cout15));

    bitALU calc15(.A(a[15]),
                  .B(b[15]),
                  .CIN(cout15),
                  .CTRL(ctrl),
                  .ANS(ans[15]),
                  .COUT(cout));
endmodule

// -----------------------------------------------
// Top-Level Module
module bit16ALU (A, B, AluCtrl, Overflow, Zero, S);
    input [15:0] A;
    input [15:0] B;
    input [3:0] AluCtrl;
    output Overflow;
    output Zero;
    output [15:0] S;

    wire invSignA;
    wire invSignB;
    not(invSignA, A[15]);
    not(invSignB, B[15]);

    // 0000 (subtraction)
    wire [15:0] _sub;
    bit16AddSub subtraction(.a(A),
                            .b(B),
                            .ctrl(1'b1),
                            .ans(_sub),
                            .cout());
    wire invSignSub;
    not(invSignSub, _sub[15]);
    wire plusMinusMinus;
    wire minusPlusPlus;
    and(plusMinusMinus, invSignA, B[15], _sub[15]);
    and(minusPlusPlus, A[15], invSignB, invSignSub);
    wire _subOverflow;
    or(_subOverflow, plusMinusMinus, minusPlusPlus);

    // 0001 (addition)
    wire [15:0] _add;
    bit16AddSub addition(.a(A),
                         .b(B),
                         .ctrl(1'b0),
                         .ans(_add),
                         .cout());
    wire invSignAdd;
    not(invSignAdd, _add[15]);
    wire plusPlusMinus;
    wire minusMinusPlus;
    and(plusPlusMinus, invSignA, invSignB, _add[15]);
    and(minusMinusPlus, A[15], B[15], invSignAdd);
    wire _addOverflow;
    or(_addOverflow, plusPlusMinus, minusMinusPlus);

    // 0010 (bitwise or)
    wire [15:0] _or;
    or(_or[0], A[0], B[0]);
    or(_or[1], A[1], B[1]);
    or(_or[2], A[2], B[2]);
    or(_or[3], A[3], B[3]);
    or(_or[4], A[4], B[4]);
    or(_or[5], A[5], B[5]);
    or(_or[6], A[6], B[6]);
    or(_or[7], A[7], B[7]);
    or(_or[8], A[8], B[8]);
    or(_or[9], A[9], B[9]);
    or(_or[10], A[10], B[10]);
    or(_or[11], A[11], B[11]);
    or(_or[12], A[12], B[12]);
    or(_or[13], A[13], B[13]);
    or(_or[14], A[14], B[14]);
    or(_or[15], A[15], B[15]);

    // 0011 (bitwise and)
    wire [15:0] _and;
    and(_and[0], A[0], B[0]);
    and(_and[1], A[1], B[1]);
    and(_and[2], A[2], B[2]);
    and(_and[3], A[3], B[3]);
    and(_and[4], A[4], B[4]);
    and(_and[5], A[5], B[5]);
    and(_and[6], A[6], B[6]);
    and(_and[7], A[7], B[7]);
    and(_and[8], A[8], B[8]);
    and(_and[9], A[9], B[9]);
    and(_and[10], A[10], B[10]);
    and(_and[11], A[11], B[11]);
    and(_and[12], A[12], B[12]);
    and(_and[13], A[13], B[13]);
    and(_and[14], A[14], B[14]);
    and(_and[15], A[15], B[15]);

    // 0100 (decrement)
    wire [15:0] _decr;
    bit16AddSub decrement(.a(A),
                          .b(16'b0000000000000001),
                          .ctrl(1'b1),
                          .ans(_decr),
                          .cout());
    wire invSignDecr;
    not(invSignDecr, _decr[15]);
    wire decrPlusMinus;
    wire decrMinusPlus;
    and(decrPlusMinus, invSignA, _decr[15]);
    and(decrMinusPlus, A[15], invSignDecr);
    wire _decrOverflow;
    or(_decrOverflow, decrPlusMinus, decrMinusPlus);

    // 0101 (increment)
    wire [15:0] _incr;
    bit16AddSub increment(.a(A),
                          .b(16'b0000000000000001),
                          .ctrl(1'b0),
                          .ans(_incr),
                          .cout());
    wire invSignIncr;
    not(invSignIncr, _incr[15]);
    wire incrPlusMinus;
    wire incrMinusPlus;
    and(incrPlusMinus, invSignA, _incr[15]);
    and(incrMinusPlus, A[15], invSignIncr);
    wire _incrOverflow;
    or(_incrOverflow, incrPlusMinus, incrMinusPlus);

    // 0110 (invert)
    wire [15:0] _inv;
    not(_inv[0], A[0]);
    not(_inv[1], A[1]);
    not(_inv[2], A[2]);
    not(_inv[3], A[3]);
    not(_inv[4], A[4]);
    not(_inv[5], A[5]);
    not(_inv[6], A[6]);
    not(_inv[7], A[7]);
    not(_inv[8], A[8]);
    not(_inv[9], A[9]);
    not(_inv[10], A[10]);
    not(_inv[11], A[11]);
    not(_inv[12], A[12]);
    not(_inv[13], A[13]);
    not(_inv[14], A[14]);
    not(_inv[15], A[15]);

    // 1100 (arithmetic left shift)
    // doesn't preserves sign; same as logShiftL (use _logShiftL)
    wire _arithShiftLOverflow;
    assign _arithShiftLOverflow = A[15] ^ A[14];

    // 1110 (arithmetic right shift)
    wire [15:0] _arithShiftR;   // preserves sign (https://open4tech.com/logical-vs-arithmetic-shift/)
    assign _arithShiftR[0] = A[1];
    assign _arithShiftR[1] = A[2];
    assign _arithShiftR[2] = A[3];
    assign _arithShiftR[3] = A[4];
    assign _arithShiftR[4] = A[5];
    assign _arithShiftR[5] = A[6];
    assign _arithShiftR[6] = A[7];
    assign _arithShiftR[7] = A[8];
    assign _arithShiftR[8] = A[9];
    assign _arithShiftR[9] = A[10];
    assign _arithShiftR[10] = A[11];
    assign _arithShiftR[11] = A[12];
    assign _arithShiftR[12] = A[13];
    assign _arithShiftR[13] = A[14];
    assign _arithShiftR[14] = A[15];
    assign _arithShiftR[15] = A[15];

    // 1000 (logical left shift)
    wire [15:0] _logShiftL;
    assign _logShiftL[0] = 0;
    assign _logShiftL[1] = A[0];
    assign _logShiftL[2] = A[1];
    assign _logShiftL[3] = A[2];
    assign _logShiftL[4] = A[3];
    assign _logShiftL[5] = A[4];
    assign _logShiftL[6] = A[5];
    assign _logShiftL[7] = A[6];
    assign _logShiftL[8] = A[7];
    assign _logShiftL[9] = A[8];
    assign _logShiftL[10] = A[9];
    assign _logShiftL[11] = A[10];
    assign _logShiftL[12] = A[11];
    assign _logShiftL[13] = A[12];
    assign _logShiftL[14] = A[13];
    assign _logShiftL[15] = A[14];

    // 1010 (logical right shift)
    wire [15:0] _logShiftR;
    assign _logShiftR[0] = A[1];
    assign _logShiftR[1] = A[2];
    assign _logShiftR[2] = A[3];
    assign _logShiftR[3] = A[4];
    assign _logShiftR[4] = A[5];
    assign _logShiftR[5] = A[6];
    assign _logShiftR[6] = A[7];
    assign _logShiftR[7] = A[8];
    assign _logShiftR[8] = A[9];
    assign _logShiftR[9] = A[10];
    assign _logShiftR[10] = A[11];
    assign _logShiftR[11] = A[12];
    assign _logShiftR[12] = A[13];
    assign _logShiftR[13] = A[14];
    assign _logShiftR[14] = A[15];
    assign _logShiftR[15] = 0;

    // 1001 (set on less than or equal)
    wire [15:0] _lessThanEqual;
    assign _lessThanEqual[15] = 0;
    assign _lessThanEqual[14] = 0;
    assign _lessThanEqual[13] = 0;
    assign _lessThanEqual[12] = 0;
    assign _lessThanEqual[11] = 0;
    assign _lessThanEqual[10] = 0;
    assign _lessThanEqual[9] = 0;
    assign _lessThanEqual[8] = 0;
    assign _lessThanEqual[7] = 0;
    assign _lessThanEqual[6] = 0;
    assign _lessThanEqual[5] = 0;
    assign _lessThanEqual[4] = 0;
    assign _lessThanEqual[3] = 0;
    assign _lessThanEqual[2] = 0;
    assign _lessThanEqual[1] = 0;
    wire AposBposLTE;
    wire AnegBnegLTE;
    wire AposBneg;
    wire notAposBneg;
    wire AnegBpos;
    wire notA15;
    wire notB15;
    not(notA15, A[15]);
    not(notB15, B[15]);
    and(AposBneg, notA15, B[15]);
    not(notAposBneg, AposBneg);
    and(AnegBpos, A[15], notB15);
    wire [15:0] diff;
    bit16AddSub difference(.a(A),
                           .b(B),
                           .ctrl(1'b1),
                           .ans(diff),
                           .cout());
    wire notDiffZero;
    wire diffZero;
    or(notDiffZero, diff[0], diff[1], diff[2], diff[3], diff[4], diff[5], diff[6], diff[7], diff[8], diff[9], diff[10], diff[11], diff[12], diff[13], diff[14], diff[15]);
    not(diffZero, notDiffZero);
    wire ssDiffNeg;
    wire nonAposBnegLTE;
    or(nonAposBnegLTE, AnegBpos, diff[15], diffZero);
    and(_lessThanEqual[0], notAposBneg, nonAposBnegLTE);

    // Overflow
    mux16_1 overflowMux(.a(_subOverflow),           // 0000
                        .b(_addOverflow),           // 0001
                        .c(1'b0),                   // 0010
                        .d(1'b0),                   // 0011
                        .e(_decrOverflow),          // 0100
                        .f(_incrOverflow),          // 0101
                        .g(1'b0),                   // 0110
                        .h(),                       // 0111
                        .i(1'b0),                   // 1000
                        .j(1'b0),                   // 1001
                        .k(1'b0),                   // 1010
                        .l(),                       // 1011
                        .m(_arithShiftLOverflow),   // 1100
                        .n(),                       // 1101
                        .o(1'b0),                   // 1110
                        .p(),                       // 1111
                        .sel(AluCtrl),
                        .out(Overflow));

    // Zero
    wire anyOnes;
    or(anyOnes, S[0], S[1], S[2], S[3], S[4], S[5], S[6], S[7], S[8], S[9], S[10], S[11], S[12], S[13], S[14], S[15]);
    not(Zero, anyOnes);

    // S
    pportMux16_1 SMux(.a(_sub),              // 0000
                      .b(_add),              // 0001
                      .c(_or),               // 0010
                      .d(_and),              // 0011
                      .e(_decr),             // 0100
                      .f(_incr),             // 0101
                      .g(_inv),              // 0110
                      .h(),                  // 0111
                      .i(_logShiftL),        // 1000
                      .j(_lessThanEqual),    // 1001
                      .k(_logShiftR),        // 1010
                      .l(),                  // 1011
                      .m(_logShiftL),        // 1100
                      .n(),                  // 1101
                      .o(_arithShiftR),      // 1110
                      .p(),                  // 1111
                      .sel(AluCtrl),
                      .out(S));
endmodule




