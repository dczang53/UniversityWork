module Separator(m, exp, sig, round);

input [10:0] m;
output reg [2:0] exp;
output reg [3:0] sig;
output reg round;

integer i;
integer cnt;
integer digit;

always @ (m)
begin
	i = 10;
	cnt = 1;
	digit = 3;

	if (m[10] == 1)
	begin
		cnt = 1;
		i = 10;
	end
	else if (m[9] == 1)
	begin
		cnt = 2;
		i = 9;
	end
	else if (m[8] == 1)
	begin
		cnt = 3;
		i = 8;
	end
	else if (m[7] == 1)
	begin
		cnt = 4;
		i = 7;
	end
	else if (m[6] == 1)
	begin
		cnt = 5;
		i = 6;
	end
	else if (m[5] == 1)
	begin
		cnt = 6;
		i = 5;
	end
	else if (m[4] == 1)
	begin
		cnt = 7;
		i = 4;
	end
	else if (m[3] == 1)
	begin
		cnt = 8;
		i = 3;
	end
	else if (m[2] == 1)
	begin
		cnt = 9;
		i = 2;
	end
	else if (m[1] == 1)
	begin
		cnt = 10;
		i = 1;
	end
	else if (m[0] == 1)
	begin
		cnt = 11;
		i = 0;
	end
	else
	begin
		cnt = 12;
		i = -1;
	end
	
	if (cnt > 8)
		exp = 0;
	else
		exp = 8 - cnt;

	while((digit >= 0) && (i >= 0))
	begin
		sig[digit] = m[i];
		digit = digit - 1;
		i = i - 1;
	end

	if(digit >= 0)
		sig = (sig >> (digit + 1));

	if(i >= 0)
		round = m[i];
	else
		round = 0;
end

endmodule