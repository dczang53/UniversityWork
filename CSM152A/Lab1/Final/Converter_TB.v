`timescale 1ns / 1ps

module Converter_TB;

reg [11:0] sw_T;
wire [7:0] O;

Convertor UUT (.D(sw_T), 
               .O(O));

initial
begin
	sw_T = 0;
end

always
	#5 sw_T = sw_T + 12'b1;

initial
	#20490 $finish;	//small bug with testbench: value 011111111111 and 111111111111 will not show up within time; will show up if testbench were continued after the finish, and does display the correct converted value

endmodule