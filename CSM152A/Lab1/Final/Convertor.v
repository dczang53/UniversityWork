module Convertor(D, O);

input [11:0] D;
output reg [7:0] O;

wire S;
wire [10:0] m;
wire [2:0] exp;
wire [3:0] sig;
wire round;
wire [2:0] E;
wire [3:0] F;

SignedMagnitude m_signedMagnitude(.d(D), .S(S), .m(m));
Separator m_separator(.m(m), .exp(exp), .sig(sig), .round(round));
Rounder m_rounder(.exp(exp), .sig(sig), .round(round), .E(E), .F(F));

always @ *
begin
O[7] = S;
O[6:4] = E;
O[3:0] = F;
end

endmodule