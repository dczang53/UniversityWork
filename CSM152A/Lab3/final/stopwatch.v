module stopwatch(sw,clk,an,seg);

input [3:0] sw; // 0 - reset, 1 - pause, 2- adj, 3 - sel
input clk;
output [7:0] seg;
output [3:0] an;

wire clk_1hz;
wire clk_100hz;

wire [5:0] min;
wire [5:0] sec;

clock_divider clock_divider_ (
	// INPUTS:
	.inp_clk(clk),
	.rst(sw[0]),
	// OUTPUTS:
	.out_clk_1hz(clk_1hz), 
	.out_clk_100hz(clk_100hz)
);

counter counter_ (
	// INPUTS:
	.clk(clk_1hz), 
	.rst(sw[0]), 
	.pause(sw[1]), 
	.adj(sw[2]), 
	.sel(sw[3]), 
	// OUTPUTS:
	.min(min), 
	.sec(sec)
);

seven_seg_display seven_seg_display_(
	// INPUTS:
	.clock_100hz(clk_100hz), 
	.clock_1hz(clk_1hz), 
	.min(min), 
	.sec(sec), 
	.adj(sw[2]), 
	.sel(sw[3]),
	.rst(sw[0]),
	// OUTPUTS:
	.seg(seg), 
	.an(an)
);

endmodule