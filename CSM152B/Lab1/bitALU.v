module bitALU (A, B, CIN, CTRL, ANS, COUT);
    input A;
    input B;
    input CIN;
    input CTRL;
    output ANS;
    output COUT;

    wire notB;
    not(notB, B);
    
    wire B2;
    mux2_1 muxAddOrSub(.a(B),
                       .b(notB),
                       .sel(CTRL),
                       .o(B2));
    
    addbit adder(.cin(CIN),
                 .a(A),
                 .b(B2),
                 .s(ANS),
                 .cout(COUT));
endmodule
