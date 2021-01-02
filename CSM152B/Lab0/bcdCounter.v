module bcdCounter (CLK, ENABLE, LOAD, UP, CLR, D, Q, CO);
    input CLK;
    input ENABLE;
    input LOAD;
    input UP;
    input CLR;
    input [3:0] D;
    output reg [3:0] Q;
    output reg CO;
    
    wire [3:0] q;

    tick TICKER(.A(Q),
                .SIGN(UP),
                .B(q));

    always @ (posedge CLK)
        begin
        if (CLR & ENABLE)
            begin
            if (LOAD)
                begin
                Q <= D;
                CO <= 0;
                end
            else
                begin
                CO <= (UP && (Q == 9)) || (!UP && (Q == 0));
                Q <= q;
                end
            end
        end
    
    always @ (CLR)
        begin
        if (~CLR)
            begin
            Q <= 4'b0000;
            CO <= 0;
            end
        end

endmodule