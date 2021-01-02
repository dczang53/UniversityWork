`timescale 1ns / 100ps

module part1_TB;
    reg m2a;
    reg m2b;
    reg m2sel;
    wire m2o;
    mux2_1 UUT0(.a(m2a),
                .b(m2b),
                .sel(m2sel),
                .o(m2o));

    reg m3a;
    reg m3b;
    reg m3c;
    reg [1:0] m3sel;
    wire m3o;
    mux3_1 UUT1(.a(m3a),
                .b(m3b),
                .c(m3c),
                .sel(m3sel),
                .o(m3o));

    reg addCin;
    reg addA;
    reg addB;
    wire addS;
    wire addCout;
    addbit UUT2(.cin(addCin),
                .a(addA),
                .b(addB),
                .s(addS),
                .cout(addCout));

    reg a;
    reg b;
    reg cin;
    reg ctrl;
    wire ans;
    wire cout;
    bitALU UUT3(.A(a),
                .B(b),
                .CIN(cin),
                .CTRL(ctrl),
                .ANS(ans),
                .COUT(cout));

    initial
        begin
        // mux2_1 functionality
        m2a = 0;
        m2b = 0;
        m2sel = 0;

        #10 m2a = 0;
        m2b = 0;
        m2sel = 1;

        #10 m2a = 1;
        m2b = 0;
        m2sel = 0;

        #10 m2a = 0;
        m2b = 1;
        m2sel = 0;

        #10 m2a = 1;
        m2b = 0;
        m2sel = 1;

        #10 m2a = 0;
        m2b = 1;
        m2sel = 1;
        
        // mux3_1 functionality
        #10 m3a = 0;
        m3b = 0;
        m3c = 0;
        m3sel = 2'b00;

        #10 m3a = 0;
        m3b = 0;
        m3c = 0;
        m3sel = 2'b01;

        #10 m3a = 0;
        m3b = 0;
        m3c = 0;
        m3sel = 2'b10;

        #10 m3a = 1;
        m3b = 0;
        m3c = 0;
        m3sel = 2'b00;

        #10 m3a = 0;
        m3b = 1;
        m3c = 0;
        m3sel = 2'b00;

        #10 m3a = 0;
        m3b = 0;
        m3c = 1;
        m3sel = 2'b00;

        #10 m3a = 1;
        m3b = 0;
        m3c = 0;
        m3sel = 2'b01;

        #10 m3a = 0;
        m3b = 1;
        m3c = 0;
        m3sel = 2'b01;

        #10 m3a = 0;
        m3b = 0;
        m3c = 1;
        m3sel = 2'b01;

        #10 m3a = 1;
        m3b = 0;
        m3c = 0;
        m3sel = 2'b10;

        #10 m3a = 0;
        m3b = 1;
        m3c = 0;
        m3sel = 2'b10;

        #10 m3a = 0;
        m3b = 0;
        m3c = 1;
        m3sel = 2'b10;

        // bitadder addition
        #10 addCin = 0;
        addA = 0;
        addB = 0;

        #10 addA = 1;
        addB = 0;

        #10 addA = 0;
        addB = 1;

        #10 addA = 1;
        addB = 1;

        #10 addCin = 1;
        addA = 0;
        addB = 0;

        #10 addA = 1;
        addB = 0;

        #10 addA = 0;
        addB = 1;

        #10 addA = 1;
        addB = 1;
        
        // bitALU addition
        #10 ctrl = 0;
        cin = 0;
        a = 0;
        b = 0;

        #10 a = 0;
        b = 1;

        #10 a = 1;
        b = 0;

        #10 a = 1;
        b = 1;

        #10 cin = 1;
        a = 0;
        b = 0;

        #10 a = 0;
        b = 1;

        #10 a = 1;
        b = 0;

        #10 a = 1;
        b = 1;

        // bitALU subtraction
        #10 ctrl = 1;
        cin = 0;
        a = 0;
        b = 0;

        #10 a = 0;
        b = 1;

        #10 a = 1;
        b = 0;

        #10 a = 1;
        b = 1;

        #10 cin = 1;
        a = 0;
        b = 0;

        #10 a = 0;
        b = 1;

        #10 a = 1;
        b = 0;

        #10 a = 1;
        b = 1;

        #10 $stop;
        end
endmodule




