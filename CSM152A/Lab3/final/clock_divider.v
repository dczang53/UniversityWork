module clock_divider(inp_clk, rst, out_clk_1hz, out_clk_100hz);

input inp_clk, rst;

output reg out_clk_1hz;
output reg out_clk_100hz;

reg [26:0] cnt_1hz;
reg [26:0] cnt_100hz;
 
always @ (posedge inp_clk)
begin
	if (rst)
	begin
		cnt_1hz <= 0;
		cnt_100hz <= 0;
		out_clk_1hz <= 0;
		out_clk_100hz <= 0;
	end
	else
	begin
		if (cnt_1hz == 50000000)
		begin
			cnt_1hz <= 0;
			out_clk_1hz <= !out_clk_1hz;
		end
		else
			cnt_1hz <= cnt_1hz + 1'b1;
		if (cnt_100hz == 50000)
		begin
			cnt_100hz <= 0;
			out_clk_100hz <= !out_clk_100hz;
		end
		else
			cnt_100hz <= cnt_100hz + 1'b1;
	end
end

/* always @ (posedge inp_clk)
begin
	if (cnt_1hz == 50000000)
	begin
		cnt_1hz <= 0;
		out_clk_1hz <= !out_clk_1hz;
	end
end

always @ (posedge inp_clk)
begin
	if (cnt_100hz == 500000)
	begin
		cnt_100hz <= 0;
		out_clk_100hz <= !out_clk_100hz;
	end
end */


endmodule