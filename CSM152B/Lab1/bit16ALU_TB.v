`timescale 1ns / 100ps

module bit16ALU_TB;
    reg [15:0]  a;
    reg [15:0] b;
    reg [3:0] aluCtrl;
    wire overflow;
    wire zero;
    wire [15:0] s;

    bit16ALU UUT(.A(a),
                 .B(b),
                 .AluCtrl(aluCtrl),
                 .Overflow(overflow),
                 .Zero(zero),
                 .S(s));

    initial
        begin
        // subtraction
        #10 aluCtrl = 4'b0000;
        a = 16'b0101001111011011;       // (21,467 and 5,981)
        b = 16'b0001011101011101;       // two + w/ + result

        #10 a = 16'b0010101010011011;   // (10,907 and 21,958)
        b = 16'b0101010111000110;       // two + w/ - result

        #10 a = 16'b1001011101011101;   // (-26,787 and -27,685)
        b = 16'b1001001111011011;       // two - w/ + result

        #10 a = 16'b1001110101010110;   // (-25,258 and -13,419)
        b = 16'b1100101110010101;       // two - w/ - result

        #10 a = 16'b0001011101011101;   // (5,981 and -13,419)
        b = 16'b1100101110010101;       // a + and a - w/ + result

        #10 a = 16'b1100101110010101;   // (-13,419 and 10,907)
        b = 16'b0010101010011011;       // a - and a + w/ - result

        #10 a = 16'b0001011101011101;   // (5,981)
        b = 16'b0001011101011101;       // result should be zero (+)

        #10 a = 16'b1100101110010101;   // (-13,419)
        b = 16'b1100101110010101;       // result should be zero (-)

        #10 a = 16'b0101001111011011;   // (21,467 and -25,258)
        b = 16'b1001110101010110;       // + and - overflow
        
        #10 a = 16'b1001001111011011;   // (-27,685 and 22,365)
        b = 16'b0101011101011101;       // - and + overflow

        // addition
        #10 aluCtrl = 4'b0001;
        a = 16'b0101001111011011;       // (21,467 and 5,981)
        b = 16'b0001011101011101;       // two + w/ + result

        #10 a = 16'b1110101110011101;   // (-5,219 and -13,419)
        b = 16'b1100101110010101;       // two - w/ - result

        #10 a = 16'b0101010111000110;   // (21,958 and -13,419)
        b = 16'b1100101110010101;       // a + and a - w/ + result

        #10 a = 16'b0001011101011101;   // (5,981 and -13,419)
        b = 16'b1100101110010101;       // a + and a - w/ - result

        #10 a = 16'b1100101110010101;   // (-13,419 and 21,467)
        b = 16'b0101001111011011;       // a - and a + w/ + result

        #10 a = 16'b1001110101010110;   // (-25,258 and 10,907)
        b = 16'b0010101010011011;       // a - and a + w/ - result

        #10 a = 16'b0001011101011101;   // (5,981 and -5,981)
        b = 16'b1110100010100011;       // result should be zero (+)

        #10 a = 16'b1100101110010101;   // (-13,419 and 13,419)
        b = 16'b0011010001101011;       // result should be zero (-)

        #10 a = 16'b0101001111011011;   // (21,467 and 21,958)
        b = 16'b0101010111000110;       // two + overflow

        #10 a = 16'b1001011101011101;   // (-26,787 and -27,685)
        b = 16'b1001001111011011;       // two - overflow

        // bitwise or
        #10 aluCtrl = 4'b0010;
        a = 16'b1100101110010101;
        b = 16'b0101001111011011;

        // bitwise and
        #10 aluCtrl = 4'b0011;
        a = 16'b1100101110010101;
        b = 16'b0101001111011011;

        // decrement
        #10 aluCtrl = 4'b0100;
        a = 16'b0101001111011011;       // + example (21,467)

        #10 a = 16'b1100101110010101;   // - example (-13,419)

        #10 a = 16'b1000000000000000;   // - overflow (-32,768)

        // increment
        #10 aluCtrl = 4'b0101;
        a = 16'b0101001111011011;       // + example (21,467)

        #10 a = 16'b1100101110010101;   // - example (-13,419)

        #10 a = 16'b0111111111111111;   // + overflow (32,767)

        // invert
        #10 aluCtrl = 4'b0110;
        a = 16'b1001011101011101;       // (-26,787)

        // arithmetic shift left
        #10 aluCtrl = 1100;
        a = 16'b1001011101011101;       // (-26,787)

        // arithmetic shift right
        #10 aluCtrl = 1110;
        a = 16'b0101001111011011;       // + example (21,467)

        #10 a = 16'b1001011101011101;   // - example (-26,787) (should preserve sign)

        // logical shift left
        #10 aluCtrl = 1000;
        a = 16'b1001011101011101;       // (-26,787) (should be the same as arithmetic shift left)

        // logical shift right
        #10 aluCtrl = 1010;
        a = 16'b0101001111011011;       // + example (21,467)

        #10 a = 16'b1001011101011101;   // - example (-26,787) (shouldn't preserve sign)

        // set on less than or equal
        #10 aluCtrl = 1001;
        a = 16'b0101010111000110;   // (21,958 and 5,981)
        b = 16'b0001011101011101;   // 2 + and SLTE not set

        #10 a = 16'b0001011101011101;   // (5,981 and 21,958)
        b = 16'b0101010111000110;       // 2 + and SLTE set

        #10 a = 16'b1100101110010101;   // (-13,419 and -26,787)
        b = 16'b1001011101011101;       // 2 - and SLTE not set

        #10 a = 16'b1001011101011101;   // (-26,787 and -13,419)
        b = 16'b1100101110010101;       // 2 - and SLTE set

        #10 a = 16'b0001011101011101;   // (5,981 and -27,685)
        b = 16'b1001001111011011;       // a + and a - and SLTE not set

        #10 a = 16'b1110101110011101;   // (-5,219 and 21,958)
        b = 16'b0101010111000110;       // a - and a + and SLTE set

        #10 a = 16'b0101010111000110;   // (21,958)
        b = 16'b0101010111000110;       // 2 equal + and SLTE set

        #10 a = 16'b1100101110010101;   // (-13,419)
        b = 16'b1100101110010101;       // 2 equal - and SLTE set

        #10 $stop;
        end
endmodule

// NOTE: decimal range from -32,768 to 32,767
/*
16'b0001011101011101    // 5,981
16'b0010101010011011    // 10,907
16'b0101001111011011    // 21,467
16'b0101010111000110    // 21,958

16'b1110101110011101    // -5,219
16'b1100101110010101    // -13,419
16'b1001110101010110    // -25,258
16'b1001011101011101    // -26,787
16'b1001001111011011    // -27,685
*/
/*
check numbers using:
https://www.omnicalculator.com/math/twos-complement
https://www.mathsisfun.com/binary-decimal-hexadecimal-converter.html
*/




