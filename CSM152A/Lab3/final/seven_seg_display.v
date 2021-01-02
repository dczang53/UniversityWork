module seven_seg_display(clock_100hz, clock_1hz, min, sec, adj, sel, rst, seg, an);

input clock_100hz, clock_1hz, adj, sel, rst;
input [5:0] min, sec;

output reg [7:0] seg;
output reg [3:0] an;
 
reg [2:0] state;

parameter stRst  = 0;
parameter stMin0 = 1;
parameter stMin1 = 2;
parameter stSec0 = 3;
parameter stSec1 = 4;

function [7:0] numberTo7Seg;
	input [3:0] din;
	begin
		case (din)
			4'b0000: numberTo7Seg = 8'b11000000; 
			4'b0001: numberTo7Seg = 8'b11111001;
			4'b0010: numberTo7Seg = 8'b10100100;
			4'b0011: numberTo7Seg = 8'b10110000;
			4'b0100: numberTo7Seg = 8'b10011001;
			4'b0101: numberTo7Seg = 8'b10010010;
			4'b0110: numberTo7Seg = 8'b10000010;
			4'b0111: numberTo7Seg = 8'b11111000;
			4'b1000: numberTo7Seg = 8'b10000000;
			4'b1001: numberTo7Seg = 8'b10010000;
			default: numberTo7Seg = 8'b00000000;
		endcase
	end
endfunction

function [3:0] stateToAn;
	input [2:0] din;
	begin
		case (din)
			stRst:   stateToAn = 4'b0000;
			stMin0:  stateToAn = 4'b0111;
			stMin1:  stateToAn = 4'b1011;
			stSec0:  stateToAn = 4'b1101;
			stSec1:  stateToAn = 4'b1110;
			default: stateToAn = 4'b1111;
		endcase
	end
endfunction

always @ (posedge clock_100hz or posedge rst)
begin
	if (rst)
	begin
		an <= stateToAn(state);
		seg <= numberTo7Seg(4'b0000);
	end
	/* else if (adj && clock_1hz)
	begin
		if (sel)
		begin
			an <= stateToAn(state);
			seg <= 8'b11111111;
		end
		else
		begin
			an <= stateToAn(state);
			seg <= 8'b11111111;
		end
	end */
	else if (state == stMin0)
	begin
		an <= stateToAn(state);
		if (adj && clock_1hz && ~sel)
			seg <= 8'b11111111;
		else
			seg <= numberTo7Seg(min / 10);
	end
	else if (state == stMin1)
	begin
		an <= stateToAn(state);
		if (adj && clock_1hz && ~sel)
			seg <= 8'b11111111;
		else
			seg <= numberTo7Seg(min % 10);
	end
	else if (state == stSec0)
	begin
		an <= stateToAn(state);
		if (adj && clock_1hz && sel)
			seg <= 8'b11111111;
		else
			seg <= numberTo7Seg(sec / 10);
	end
	else if (state == stSec1)
	begin
		an <= stateToAn(state);
		if (adj && clock_1hz && sel)
			seg <= 8'b11111111;
		else
			seg <= numberTo7Seg(sec % 10);
	end
end

always @ (posedge clock_100hz, posedge rst)
begin
	if (rst)
		state <= stRst;
	else if (state == stRst)
		state <= stMin0;
	else if (state == stMin0)
		state <= stMin1;
	else if (state == stMin1)
		state <= stSec0;
	else if (state == stSec0)
		state <= stSec1;
	else if (state == stSec1)
		state <= stMin0;
end

/*
always @ (posedge clock_1hz or posedge rst)
begin
	if (rst)
		stBlink <= 0;
	else
		stBlink <= ~stBlink;
end

always @ (posedge clock_1hz)
begin
	if (adj)
	begin
		if (sel)
		begin
			if (stBlink)
				an <= {0,0,an[1],[0]};
		end
		else
		begin
			if (stBlink)
				an <= 
		end
	end
end
*/


endmodule