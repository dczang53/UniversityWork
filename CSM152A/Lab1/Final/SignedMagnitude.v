module SignedMagnitude (d, S, m);

input [11:0] d;
output reg S;
output reg [10:0] m;

always @ *
begin
S = d[11];
if (d[11] == 0)
	m <= d[10:0];
else
begin
	if (d[10:0] == 0)
		m <= 2047;
	else
		m <= (~d[10:0] + 1);
end
end

endmodule