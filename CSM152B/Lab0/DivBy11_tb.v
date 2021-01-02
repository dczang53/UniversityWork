module DivBy11_tb;

    reg [15:0] bcd;
    wire out;

    // testing method learned from
    // https://verilogguide.readthedocs.io/en/latest/verilog/testbench.html
    localparam period = 20;

    DivBy11 UUT (bcd, out);

    initial
        begin
            // test for 1100 (should return 1)
            assign bcd = 16'h6083;
            #period;

            // test for 0011 (should return 1)
            assign bcd = 16'h0539;
            #period;

            // test for 0001 (should return 1)
            assign bcd = 16'h0001;
            #period;

            // test for 1234 (should return 0)
            assign bcd = 16'h1234;
            #period;

            // test for 9999 (should return 1)
            assign bcd = 16'h9999;
            #period;
        end

endmodule

module ismultiple_tb;

    reg [3:0] b;
    wire out;

    // testing method learned from
    // https://verilogguide.readthedocs.io/en/latest/verilog/testbench.html
    localparam period = 20;

    ismultiple UUT (b, out);

    initial
        begin
            assign b = 11;
            #period;

            assign b = 0;
            #period;

            assign b = -1;
            #period;

            assign b = 5;
            #period;

            assign b = 2;
            #period;
        end

endmodule

module isequal_tb;

    reg [3:0] b;
    reg [3:0] b1;
    wire out;

    // testing method learned from
    // https://verilogguide.readthedocs.io/en/latest/verilog/testbench.html
    localparam period = 20;

    isequal UUT (b, b1, out);

    initial
        begin
            assign b = 11;
            assign b1 = 11;
            #period;

            assign b = 0;
            assign b1 = 11;
            #period;
        end

endmodule

module add4bit_tb;

    reg [3:0] b0;
    reg [3:0] b1;
    reg cin;
    wire [4:0] out;

    // testing method learned from
    // https://verilogguide.readthedocs.io/en/latest/verilog/testbench.html
    localparam period = 20;

    add4bit UUT (b0, b1, cin, out);

    initial
        begin
            b0 = 0;
            b1 = 4'b1010;
            cin = 1'b1;
            #period;

            b0 = 3;
            b1 = 4'b0110;
            cin = 1'b1;
            #period;
                        
            b0 = 9;
            b1 = 4'b1111;
            cin = 1'b1;
            #period;       
            
            b0 = 9;
            b1 = 4'b0110;
            cin = 1'b1;
            #period;
        end

endmodule