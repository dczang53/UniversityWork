`timescale 1ns / 100ps

module bcdCounterTB;
    reg clk;
    reg enable;
    reg load;
    reg up;
    reg clr;
    reg [3:0] d;
    wire [3:0] q;
    wire co;

    bcdCounter UUT (.CLK(clk),
                    .ENABLE(enable),
                    .LOAD(load),
                    .UP(up),
                    .CLR(clr),
                    .D(d),
                    .Q(q),
                    .CO(co));

    always
        #5 clk = ~clk;

    initial
        begin
        clk = 1;            // note: clk will have posedge trigger every 10ns
        enable = 0;
        load = 0;
        up = 1;
        clr = 1;
        d = 5;
        // test case #1: load into counter at 20ns, and hold value for 1 clock cycle (until beginning of 40ns)
        #11 enable = 1;     // 11
        load = 1;
        #20 load = 0;       // 31
        // test case #2: counter increment for 10 clock cycles (40ns, 50ns, 60ns, 70ns, 80ns, 90ns, 100ns, 110ns, 120ns, 130ns)
        // test case #3: disable increment for 3 clock cycles (140ns, 150ns, 160ns)
        #100 enable = 0;    // 131
        // test case #4: counter decrement for 10 clock cycles (170ns, 180ns, 190ns, 200ns, 210ns, 220ns, 230ns, 240ns, 250ns, 260ns)
        #30 up = 0;         // 161
        enable = 1;         // 161
        // test case #5: disable decrement for 3 clock cycles (270ns, 280ns, 290ns)
        #100 enable = 0;    // 261
        // re-enable and increment for 2 clock cycles (300ns, 310ns)
        #30 up = 1;         // 291
        enable = 1;
        // test case #6: async reset for 3 clock cycles ("311ns", 320ns, 330ns, 340ns)
        #20 clr = 0;        // 311
        // load into counter, and hold value for 2 clock cycles (350ns, 360ns)
        #30 clr = 1;        // 341
        d = 8;
        load = 1;
        #20 load = 0;       // 361
        // counter increment for 2 clock cycles (370ns, 380ns)
        // test case #7a: async reset for 3 clock cycles ("381ns", 390ns, 400ns, 410ns)
        #20 clr = 0;        // 381
        // load into counter, and hold value for 2 clock cycles (420ns, 430ns)
        #30 clr = 1;        // 411
        d = 1;
        load = 1;
        #20 load = 0;       // 431
        up = 0;             // 431
        // counter decrement for 2 clock cycles (440ns, 450ns)
        // test case #7b: async reset for 3 clock cycles ("451ns", 460ns, 470ns, 480ns)
        #20 clr = 0;        // 451
        // end test cases and pause for waveform
        #30 $stop;          // 481
        // #200 $finish;    // don't use this; this will terminate the simulation
        end
endmodule

// expected values:
// time (ns):   10  20  30  40  50  60  70  80  90  100 110 120 130 140 150 160 170 180 190 200 210 220 230 240 250 260 270 280 290 300 310 320 330 340 350 360 370 380 390 400 410 420 430 440 450 460 470 480
// q:           x   5   5   6   7   8   9   0   1   2   3   4   5   5   5   5   4   3   2   1   0   9   8   7   6   5   5   5   5   6   7   0   0   0   8   8   9   0   0   0   0   1   1   0   9   0   0   0
// co:          x   0   0   0   0   0   0   1   0   0   0   0   0   0   0   0   0   0   0   0   0   1   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   1   0   0   0   0   0   0   1   0   0   0


