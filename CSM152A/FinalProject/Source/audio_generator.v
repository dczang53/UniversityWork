module audio_generator(clk, clk_440hz, rst, btn_pressed, audio_signal, shutdown, gain);
`include "top_definitions.v"

input clk;
input rst;
input clk_440hz;

input [NOTE_WIDTH:0] btn_pressed;

output reg audio_signal, shutdown, gain;

reg [26:0] freq_divider;
reg [26:0] freq_counter;
reg clk_note_freq;

/* 
	A4 -> 440.00 Hz -> 227273
	B4 -> 493.88 Hz -> 202478
	C5 -> 523.25 Hz -> 191113
	D5 -> 587.33 Hz -> 170262
	E5 -> 659.25 Hz -> 151688
	F5 -> 698.46 Hz -> 143172
	G5 -> 783.99 Hz -> 127553
	A5 -> 880.00 Hz -> 113636
*/
function [26:0] btnPressedToFreqDivider;
	input [NOTE_WIDTH:0] din;
	begin
		case (din)
			0: btnPressedToFreqDivider = 227273; // 0 -> A
			1: btnPressedToFreqDivider = 202478; // 1 -> B
			2: btnPressedToFreqDivider = 191113; // 2 -> C
			3: btnPressedToFreqDivider = 170262; // 3 -> D
			4: btnPressedToFreqDivider = 151688; // 4 -> E
			5: btnPressedToFreqDivider = 143172; // 5 -> F
			6: btnPressedToFreqDivider = 127553; // 6 -> G
			7: btnPressedToFreqDivider = 113636; // 7 -> A
			default: btnPressedToFreqDivider = 1;
		endcase
	end
endfunction

always @ (btn_pressed)
	freq_divider <= btnPressedToFreqDivider(btn_pressed);
	
always @ (posedge clk or posedge rst) begin

	if (rst || btn_pressed == 9) begin
		clk_note_freq <= 0;
		freq_counter <= 0;
	end
	else begin 
		if (freq_counter == freq_divider) begin
			clk_note_freq <= !clk_note_freq;
			freq_counter <= 0;
		end
		else
			freq_counter <= freq_counter + 1;
	end

end

always @ (posedge clk or posedge rst) begin
	if (rst)
	begin
		audio_signal <= 1'b0;
		shutdown <= 1'b0;
		gain <= 1'b0;
	end
	else 
	begin 
		audio_signal <= clk_note_freq;
		shutdown <= 1'b1;
		gain <= 1'b0;
	end
end

endmodule