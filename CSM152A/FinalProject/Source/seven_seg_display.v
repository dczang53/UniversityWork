module seven_seg_display(clock_100hz, rst, note1, note2, note3, note4, seg, an);
`include "top_definitions.v"

input clock_100hz, rst;
input [NOTE_WIDTH:0] note1, note2, note3, note4;

output reg [7:0] seg;
output reg [3:0] an;
 
reg [2:0] state;

parameter stRst  = 0;
parameter stNote1 = 1;
parameter stNote2 = 2;
parameter stNote3 = 3;
parameter stNote4 = 4;

function [7:0] numberTo7Seg;
	input [NOTE_WIDTH:0] din;
	begin
		case (din)
			0: numberTo7Seg = 8'b10001000; // 0 -> A
			1: numberTo7Seg = 8'b10000000; // 1 -> B
			2: numberTo7Seg = 8'b11000110; // 2 -> C
			3: numberTo7Seg = 8'b11000000; // 3 -> D
			4: numberTo7Seg = 8'b10000110; // 4 -> E
			5: numberTo7Seg = 8'b10001110; // 5 -> F
			6: numberTo7Seg = 8'b11000010; // 6 -> G
			7: numberTo7Seg = 8'b10001000; // 7 -> A
			// DISPLAY_ONLY STATES - NOTHING TO DO WITH NOTE MAPPING
			8: numberTo7Seg = 8'b11111111; // 8 -> NO NOTE DISPLAYED
			default: numberTo7Seg = 8'b00000000;
		endcase
	end
endfunction

function [3:0] stateToAn;
	input [2:0] din;
	begin
		case (din)
			stRst:   stateToAn = 4'b0000;
			stNote1:  stateToAn = 4'b0111;
			stNote2:  stateToAn = 4'b1011;
			stNote3:  stateToAn = 4'b1101;
			stNote4:  stateToAn = 4'b1110;
			default: stateToAn = 4'b1111;
		endcase
	end
endfunction

always @ (posedge clock_100hz or posedge rst)
begin
	if (rst)
	begin
		an <= stateToAn(state);
		seg <= numberTo7Seg(3'b000);
	end
	else if (state == stNote1)
	begin
		an <= stateToAn(state);
        seg <= numberTo7Seg(note1);
	end
	else if (state == stNote2)
	begin
		an <= stateToAn(state);
		seg <= numberTo7Seg(note2);
	end
	else if (state == stNote3)
	begin
		an <= stateToAn(state);
		seg <= numberTo7Seg(note3);
	end
	else if (state == stNote4)
	begin
		an <= stateToAn(state);
		seg <= numberTo7Seg(note4);
	end
end

always @ (posedge clock_100hz, posedge rst)
begin
	if (rst)
		state <= stRst;
	else if (state == stRst)
		state <= stNote1;
	else if (state == stNote1)
		state <= stNote2;
	else if (state == stNote2)
		state <= stNote3;
	else if (state == stNote3)
		state <= stNote4;
	else if (state == stNote4)
		state <= stNote1;
end

endmodule