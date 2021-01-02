module reg_file (
    input wire clk,
    input wire reset,

    input wire WrEn,
    input wire [4:0] Rw,
    input wire [4:0] Ra,
    input wire [4:0] Rb,

    input wire [15:0] busW,
    output reg [15:0] busA,
    output reg [15:0] busB
);

integer index;

reg [15:0] r_list [0:31];

always @ (posedge clk) begin

    if (reset) begin
        for (index = 0; index < 32; index = index + 1) begin
            r_list[index] = 16'h0000;
        end
    end

    else if (WrEn) begin
            r_list[Rw] = busW;
    end

    busA = r_list[Ra];
    busB = r_list[Rb];

end

endmodule