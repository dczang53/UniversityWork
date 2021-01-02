module Rounder(exp, sig, round, E, F);

input [2:0] exp;
input [3:0] sig;
input round;
output reg [2:0] E;
output reg [3:0] F;

always @ (exp, sig, round)
begin
	E = exp;
	F = sig;
	if (round == 1)
	begin
		if (F < 15)
			F = F + 1;
		else
			if (E < 7)
			begin
				E = E + 1;
				F = 4'b1000;
			end
	end
end

endmodule