module top_module(sw, clk, JC, seg, an, JA);
`include "top_definitions.v"

input sw;
input clk;
inout [7:0] JC;
output [7:0] seg;
output [3:0] an;
output [3:0] JA; // PMOD output

// JA[0] -> AIN
// JA[1] -> GAIN
// JA[3] -> SHUTDOWN

wire clk_1hz;
wire clk_100hz;

wire clk_440hz;

wire [NOTE_WIDTH:0] btn_pressed;

wire [NOTE_WIDTH:0] note1;
wire [NOTE_WIDTH:0] note2;
wire [NOTE_WIDTH:0] note3;
wire [NOTE_WIDTH:0] note4;

/*
assign note1 = 3'b001;
assign note2 = 3'b111;
assign note3 = 3'b101;
assign note4 = 3'b010;
*/


clock_divider clock_divider_ (
	// INPUTS:
	.inp_clk(clk),
	.rst(sw),
	// OUTPUTS:
	.out_clk_1hz(clk_1hz),
	.out_clk_100hz(clk_100hz),
	.out_clk_440hz(clk_440hz)
);

seven_seg_display seven_seg_display_(
	// INPUTS:
	.clock_100hz(clk_100hz),
	.rst(sw),
    .note1(note1),
    .note2(note2),
    .note3(note3),
    .note4(note4),
	// OUTPUTS:
	.seg(seg),
	.an(an)
);

/*
button_handler button_handler_ (
    // INPUTS:
    .sw(sw),
	.clk_100hz(clk_100hz),
    // OUTPUTS:
    .btn_pressed(btn_pressed)
);
*/

button_handler button_handler_ (
    // INPUTS:
	 .clk(clk),
	 .Row(JC[7:4]),
    // OUTPUTS:
	 .Col(JC[3:0]),
    .btn_pressed(btn_pressed)
);

tutorial_manager tutorial_manager_(
    //INPUTS:
    .btn_pressed(btn_pressed),
    .rst(sw),
	 .clk_100hz(clk_100hz),
    //OUTPUTS:
    .note1(note1),
    .note2(note2),
    .note3(note3),
    .note4(note4)
);

audio_generator audio_generator_ (
	// INPUTS:
    .clk(clk),
	.clk_440hz(clk_440hz),
	.btn_pressed(btn_pressed),
	.rst(sw),
	// OUTPUS:
	.audio_signal(JA[0]),
    .gain(JA[1]),
    .shutdown(JA[3])
);

endmodule
