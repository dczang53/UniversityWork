module tutorial_manager(btn_pressed, rst, clk_100hz, note1, note2, note3, note4);
`include "top_definitions.v"

input [NOTE_WIDTH:0] btn_pressed;
input rst;
input clk_100hz;

output reg [NOTE_WIDTH:0] note1, note2, note3, note4;

reg [NOTE_WIDTH:0] tutorial [TUTORIAL_LENGTH:0]; 


reg [TUTORIAL_LENGTH:0] tutorial_state;
reg [TUTORIAL_LENGTH:0] next_state;

reg should_mutate;

reg correct_press;

initial begin
	/*
    // C C D C F E
    // 2 2 3 2 5 4
    tutorial[0] <= 2;
    tutorial[1] <= 2;
    tutorial[2] <= 3;
    tutorial[3] <= 2;
    tutorial[4] <= 5;
    tutorial[5] <= 4;
	 
	// C C D C G F
    // 2 2 3 2 6 5
	tutorial[6]  <= 2;
    tutorial[7]  <= 2;
    tutorial[8]  <= 3;
    tutorial[9]  <= 2;
    tutorial[10] <= 6;
    tutorial[11] <= 5;
	*/
	
	
	// A B C D E F G A
	// 0 1 2 3 4 5 6 7
	
	// C G E E D C
	tutorial[0] <= 2;
    tutorial[1] <= 6;
    tutorial[2] <= 4;
    tutorial[3] <= 4;
    tutorial[4] <= 3;
    tutorial[5] <= 2;
	
	// C F-E E D C
	tutorial[6]  <= 2;
    tutorial[7]  <= 5;
    tutorial[8]  <= 4;
    tutorial[9]  <= 4;
    tutorial[10] <= 3;
    tutorial[11] <= 2;
	
	// C G E E D D C D .A
	tutorial[12] <= 2;
    tutorial[13] <= 6;
    tutorial[14] <= 4;
    tutorial[15] <= 4;
    tutorial[16] <= 3;
    tutorial[17] <= 3;
	tutorial[18] <= 2;
	tutorial[19] <= 3;
	tutorial[20] <= 0;
	
	
	
	
	// C C G-E E D D
	tutorial[21] <= 2;
	tutorial[22] <= 2;
    tutorial[23] <= 6;
    tutorial[24] <= 4;
    tutorial[25] <= 4;
    tutorial[26] <= 3;
    tutorial[27] <= 3;
	
	// C C F-E E D D
	tutorial[28] <= 2;
	tutorial[29] <= 2;
    tutorial[30] <= 5;
    tutorial[31] <= 4;
    tutorial[32] <= 4;
    tutorial[33] <= 3;
    tutorial[34] <= 3;
	
	// C C G E D D C D .A
	tutorial[35] <= 2;
	tutorial[36] <= 2;
    tutorial[37] <= 6;
    tutorial[38] <= 4;
    tutorial[39] <= 4;
    tutorial[40] <= 3;
    tutorial[41] <= 3;
	tutorial[42] <= 2;
	tutorial[43] <= 3;
	tutorial[44] <= 0;
	
	
	// HEY NOW 
	
	// E C C .A C C
	tutorial[45] <= 4;
	tutorial[46] <= 2;
	tutorial[47] <= 2;
	tutorial[48] <= 0;
	tutorial[49] <= 2;
	tutorial[50] <= 2;
	
	// C .A C C C E
	tutorial[51] <= 2;
	tutorial[52] <= 0;
	tutorial[53] <= 2;
	tutorial[54] <= 2;
	tutorial[55] <= 2;
	tutorial[56] <= 4;
	
	// E C C .A C C
	tutorial[57] <= 4;
	tutorial[58] <= 2;
	tutorial[59] <= 2;
	tutorial[60] <= 0;
	tutorial[61] <= 2;
	tutorial[62] <= 2;
	
	// C .A C C C E
	tutorial[63] <= 2;
	tutorial[64] <= 0;
	tutorial[65] <= 2;
	tutorial[66] <= 2;
	tutorial[67] <= 2;
	tutorial[68] <= 4;
	
	/*
	
	// E G F E D D C
	tutorial[39] <= 4;
	tutorial[40] <= 6;
	tutorial[41] <= 5;
	tutorial[42] <= 4;
	tutorial[43] <= 3;
	tutorial[44] <= 3;
	tutorial[44] <= 2;
	
	// C C D C E D D C D A
	tutorial[45] <= 2;
	tutorial[46] <= 2;
	tutorial[47] <= 3;
	tutorial[48] <= 2;
	tutorial[49] <= 4;
	tutorial[50] <= 3;
	tutorial[51] <= 3;
	tutorial[52] <= 2;
	tutorial[53] <= 3;
	tutorial[54] <= 0;
	*/
end

/*always @ (rst) begin
	if (rst)
		tutorial_state <= 0;
end*/

always @ (btn_pressed, rst) begin
	if (rst)
		correct_press <= 0;
   if (btn_pressed == tutorial[tutorial_state])
		correct_press <= 1;
	else
		correct_press <= 0;
end

always @ (posedge clk_100hz or posedge rst) begin
	if (rst)
		next_state <= 0;
	else if (correct_press && should_mutate)
		next_state <= tutorial_state + 1;
end

always @ (posedge clk_100hz or posedge rst) begin
	if (rst) begin
		tutorial_state <= 0;
		should_mutate <= 1;
	end
	else if (correct_press) begin
		tutorial_state <= next_state ;
		should_mutate <= 0;
	end
	else begin
		tutorial_state <= tutorial_state;
		should_mutate <= 1;
	end
end

always @ (tutorial_state, rst) begin
	 if (rst) begin
		note1 <= 8;
		note2 <= 8;
		note3 <= 8;
		note4 <= 8;
	 end
	 else begin
    if (tutorial_state <= TUTORIAL_LENGTH)
        note1 <= tutorial[tutorial_state];
    else
        note1 <= 8;
        
    if (tutorial_state <= TUTORIAL_LENGTH-1)
        note2 <= tutorial[tutorial_state+1];
    else
        note2 <= 8;
        
    if (tutorial_state <= TUTORIAL_LENGTH-2)
        note3 <= tutorial[tutorial_state+2];
    else
        note3 <= 8;
        
    if (tutorial_state <= TUTORIAL_LENGTH-3)
        note4 <= tutorial[tutorial_state+3];
    else
        note4 <= 8;
	 end
end

endmodule