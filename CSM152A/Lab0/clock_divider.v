module clock_divider (sw, clk, Led);

input sw;
input clk;
output reg Led;

reg [26:0] a;

/*
initial begin
	Led = 0;
end
*/

always @ (posedge clk)
begin
	if (sw)
	begin
		a <= 0;
		Led <= 0;
	end
	else
		a <= a + 1'b1;
end

always @ (posedge clk)
begin
	if (a == 50000000)
	begin
		a <= 0;
		Led <= !Led;
	end
end

endmodule

//can only assign registers in always blocks
//values of registers can be accessed in always blocks