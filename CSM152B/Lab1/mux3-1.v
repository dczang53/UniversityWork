module mux3_1 (a, b, c, sel, o);
    input a;
    input b;
    input c;
    input [1:0] sel;
    output o;

    wire o02;
    mux2_1 mux02(.a(a),
                 .b(c),
                 .sel(sel[1]),
                 .o(o02));

    mux2_1 mux1(.a(o02),
                .b(b),
                .sel(sel[0]),
                .o(o));
endmodule
