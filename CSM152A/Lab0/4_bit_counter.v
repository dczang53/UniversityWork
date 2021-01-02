module four_bit_counter (
		rst,clk
);

input rst, clk;

reg [3:0] a;

always @ (posedge clk)
begin
	if (rst)
		a <= 4'b0000;
	else
		a <= a + 1'b1;
end
endmodule