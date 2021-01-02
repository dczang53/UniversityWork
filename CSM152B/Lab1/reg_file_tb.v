module reg_file_tb;

reg clk;
reg reset;

reg WrEn;
reg [4:0] Rw;
reg [4:0] Ra;
reg [4:0] Rb;

reg [15:0] busW;
wire [15:0] busA;
wire [15:0] busB;

reg_file UUT (clk, reset, WrEn, Rw, Ra, Rb, busW, busA, busB);

initial begin
    
    clk = 1;
    reset = 0;
    WrEn = 0;
    Ra = 5'b00000;
    Rb = 5'b00000;

    #20;
    reset = 1;

    #20;
    reset = 0;
    WrEn = 1;
    Rw = 5'b00000;
    Ra = 5'b00000;
    Rb = 5'b00000;
    busW = 16'h1234;

    #20;
    Rw = 5'b00001;
    Ra = 5'b00000;
    Rb = 5'b00001;
    busW = 16'h4321;

    #20;
    WrEn = 0;
    Rw = 5'b00010;
    Ra = 5'b00001;
    Rb = 5'b00010;
    busW = 16'h9999;

    #20;
    reset = 1;

end

always begin

    #10;
    clk = ~clk;

end 

endmodule
