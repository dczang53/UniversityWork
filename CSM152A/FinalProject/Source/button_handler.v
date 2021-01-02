/*
    MODULE THAT COMMUNICATES WITH BUTTON HARDWARE
    inputs: whatever needed
    outputs: btnPressed - a 3 bit number between 0 & 7 representing the note being played. 
                            see `seven_seg_display.v` for mappping between numbers and notes.
*/
module button_handler(clk, Row, Col, btn_pressed);
`include "top_definitions.v"
/*
NOTE: Implementing this module as a placeholder until @dennis replaces this with code that 
talks to the button hardware. Replace the input 'sw' with whatever is needed for the buttons.
Replace the contents of the always block with whatever is needed to determine which button is pressed.
*/

input clk;
input [3:0] Row;
output [3:0] Col;
output reg [NOTE_WIDTH:0] btn_pressed;
//output reg btn_signal;

reg [3:0] Col;
reg [19:0] sclk;

reg [1:0] col_pressed;

always @ (posedge clk) begin
	// 1ms
		if (sclk == 20'b00011000011010100000) begin
			//C1
			Col <= 4'b0111;
			sclk <= sclk + 1'b1;
		end	
		// check row pins
		else if(sclk == 20'b00011000011010101000) begin
			//R1
			if (Row == 4'b0111) begin
				btn_pressed <= 0;		//1
				col_pressed <= 0;
			end
			//R2
			else if(Row == 4'b1011) begin
				btn_pressed <= 4; 		//4
				col_pressed <= 0;
			end
			else if (col_pressed == 0) begin
				btn_pressed <= 9;
			end
			/*
			//R3
			else if(Row == 4'b1101) begin
				btn_pressed <= 9; 		//7
			end
			//R4
			else if(Row == 4'b1110) begin
				btn_pressed <= 9; 		//0
			end
			*/
				sclk <= sclk + 1'b1;
		end
		// 2ms
		else if(sclk == 20'b00110000110101000000) begin
			//C2
			Col<= 4'b1011;
			sclk <= sclk + 1'b1;
		end		
		// check row pins
		else if(sclk == 20'b00110000110101001000) begin
			//R1
			if (Row == 4'b0111) begin
				btn_pressed <= 1; 		//2
				col_pressed <= 1;
			end
			//R2
			else if(Row == 4'b1011) begin
				btn_pressed <= 5; 		//5
				col_pressed <= 1;
			end
			else if (col_pressed == 1) begin
				btn_pressed <= 9;
			end
			/*
			//R3
			else if(Row == 4'b1101) begin
				btn_pressed <= 9; 		//8
			end
			//R4
			else if(Row == 4'b1110) begin
				btn_pressed <= 9; 		//F
			end
			*/
			sclk <= sclk + 1'b1;
		end
		//3ms
		else if(sclk == 20'b01001001001111100000) begin
			//C3
			Col<= 4'b1101;
			sclk <= sclk + 1'b1;
		end	
		// check row pins
		else if(sclk == 20'b01001001001111101000) begin
			//R1
			if(Row == 4'b0111) begin
				btn_pressed <= 2; 		//3
				col_pressed <= 2;
			end
			//R2
			else if(Row == 4'b1011) begin
				btn_pressed <= 6; 		//6
				col_pressed <= 2;
			end
			else if (col_pressed == 2) begin
				btn_pressed <= 9;
			end
			/*
			//R3
			else if(Row == 4'b1101) begin
				btn_pressed <= 9; 		//9
			end
			//R4
			else if(Row == 4'b1110) begin
				btn_pressed <= 9; 		//E
			end
			*/
				sclk <= sclk + 1'b1;
		end
		//4ms
		else if(sclk == 20'b01100001101010000000) begin
			//C4
			Col<= 4'b1110;
			sclk <= sclk + 1'b1;
		end
		// Check row pins
		else if(sclk == 20'b01100001101010001000) begin
			//R1
			if(Row == 4'b0111) begin
				btn_pressed <= 3; //A
				col_pressed <= 3;
			end
			//R2
			else if(Row == 4'b1011) begin
				btn_pressed <= 7; //B
				col_pressed <= 3;
			end
			else if (col_pressed == 3) begin
				btn_pressed <= 9;
			end
			/*
			//R3
			else if(Row == 4'b1101) begin
				btn_pressed <= 9; //C
			end
			//R4
			else if(Row == 4'b1110) begin
				btn_pressed <= 9; //D
			end
			*/
			sclk <= 20'b00000000000000000000;
		end
		// Otherwise increment
		else begin
			sclk <= sclk + 1'b1;
		end
/*
    if (sw[0] == 1)
        btn_pressed = 7;
    else if (sw[1])
        btn_pressed = 6;
    else if (sw[2])
        btn_pressed = 5;
    else if (sw[3])
        btn_pressed = 4;
    else if (sw[4])
        btn_pressed = 3;
    else if (sw[5])
        btn_pressed = 2;
    else if (sw[6])
        btn_pressed = 1;
    else if (sw[7])
        btn_pressed = 0;
*/
end

endmodule