module Average (clk, n0, n1, n2, n3, n4, n5, n6, n7, n8, s);
	input clk;
	input [7:0] n0, n1, n2, n3, n4, n5, n6, n7, n8;
	output [7:0] s;

	assign s = (n0 + n1 + n2 + n3 + n4 + n5 + n6 + n7 + n8) / 9;
endmodule
/*
module LowPass (image_input, enable, enable_process, clk,
		image_output, finish);
	input [7:0] image_input;
	input enable, enable_process, clk;
	output [7:0] image_output;
	output finish;

	parameter WIDTH = 361;
	parameter DEPTH = 410;
	parameter FILTER_SIZE = 3;
	parameter SIZE = 8;

	reg [WIDTH * DEPTH * SIZE - 1:0] pic = 0;
	reg [7:0] n0, n1, n2, n3, n4, n5, n6, n7, n8;
	reg fin;
	integer i;
	integer j;
	integer row;
	integer col;
	
	initial begin 
		fin = 0;
		i = 0;
		j = 0;
		row = 0;
		col = 0;
	end
	
	Average avg (clk, n0, n1, n2, n3, n4, n5, n6, n7, n8, image_output);

	always @ (posedge clk) begin
		if (row >= DEPTH && col >= WIDTH)
			fin = 1;
		else if (j >= DEPTH) begin
			j = 0;
			i = i + 1;
		end else if (row >= DEPTH) begin
			row = 0;
			col = col + 1;
		end

		if (enable && i < WIDTH) begin
            	    	pic[(i * DEPTH + j) * SIZE +: 8] = image_input;
			j = j + 1;
			$display("i is %d", i);
			$display("j is %d", j);
		end else if (enable_process && !fin) begin
			if (row < 1 && col < 1) n0 = 0; 
			else n0 = pic[((col - 1) * DEPTH + row - 1) * SIZE +: 8];
			
			if (row < 1) n1 = 0; 
			else n1 = pic[(col * DEPTH + row - 1) * SIZE +: 8];
			
			if (row < 1 && col > WIDTH - 2) n2 = 0; 
			else n2 = pic[((col + 1) * DEPTH + row - 1) * SIZE +: 8];

			if (col < 1) n3 = 0; 
			else n3 = pic[((col - 1) * DEPTH + row) * SIZE +: 8];

			n4 = pic[(col * DEPTH + row) * SIZE +: 8];

			if (col > WIDTH - 2) n5 = 0; 
			else n5 = pic[((col + 1) * DEPTH + row) * SIZE +: 8];

			if (row > DEPTH - 2 && col < 1) n6 = 0; 
			else n6 = pic[((col - 1) * DEPTH + row - 1) * SIZE +: 8];

			if (row > DEPTH - 2) n7 = 0; 
			else n7 = pic[(col * DEPTH + row + 1) * SIZE +: 8];

			if (row > DEPTH - 2 && col > WIDTH - 2) n8 = 0; 
			else n8 = pic[((col + 1) * DEPTH + row + 1) * SIZE +: 8];

			row = row + 1;
			$display("Col is %d", col);
			$display("Row is %d", row);
		end 
	end

	assign finish = fin;
endmodule
*/


module LowPass (image_input, enable, enable_process, clk,
		image_output, finish);
	input [7:0] image_input;
	input enable, enable_process, clk;
	output [7:0] image_output;
	output finish;

	parameter WIDTH = 361;
	parameter DEPTH = 410;
	parameter FILTER_SIZE = 3;
	parameter SIZE = 8;

	// The +2 is to pad edges
	reg [(WIDTH + 2)*(DEPTH + 2)*SIZE - 1:0] pic = 0;
	
	integer i;
	integer j;
	initial begin
	i = 1; 
	j = 1;
	end

	always @ (posedge clk) begin
		if (enable && i < WIDTH + 1) begin
			pic[((DEPTH + 2) * i + j) * SIZE +: 8] = image_input;

			if (j >= DEPTH) begin
				i = i + 1;
				j = 1;
			end else
				j = j + 1;
		end
		//$display("i is %d", i);
		//$display("j is %d", j);
	end

	reg fin;
	reg [7:0] n0, n1, n2, n3, n4, n5, n6, n7, n8;
	reg [31:0] t0, t1, t2, t3, t4, t5, t6, t7, t8;
	integer row, col;

	initial begin
	row = 1;
	col = 1;
	fin = 0;
	end

	always @ (posedge clk) begin
		t0 = ((DEPTH + 2) * col + row) * SIZE;
		t1 = ((DEPTH + 2) * (col - 1) + (row - 1)) * SIZE;
		t2 = ((DEPTH + 2) * col + (row - 1)) * SIZE;
		t3 = ((DEPTH + 2) * (col + 1) + (row - 1)) * SIZE;
		t4 = ((DEPTH + 2) * (col - 1) + row) * SIZE;
		t5 = ((DEPTH + 2) * (col + 1) + row) * SIZE;
		t6 = ((DEPTH + 2) * (col - 1) + (row + 1)) * SIZE;
		t7 = ((DEPTH + 2) * col + (row + 1)) * SIZE;
		t8 = ((DEPTH + 2) * (col + 1) + (row + 1)) * SIZE;
		
		n0 = pic[t0 +: 8];
		n1 = pic[t1 +: 8];
		n2 = pic[t2 +: 8];
		n3 = pic[t3 +: 8];
		n4 = pic[t4 +: 8];
		n5 = pic[t5 +: 8];
		n6 = pic[t6 +: 8];
		n7 = pic[t7 +: 8];
		n8 = pic[t8 +: 8];
	end

	Average avg (clk, n0, n1, n2, n3, n4, n5, n6, n7, n8, image_output);

	always @ (posedge clk) begin
		if (!enable && enable_process && !fin) begin
			if (col >= WIDTH && row >= DEPTH)
				fin = 1;
			else if (row >= DEPTH) begin
				row = 1;
				col = col + 1;
			end else
				row = row + 1;
		end
		//$display("Column is %d", col);
		//$display("Row is %d", row);
	end

	assign finish = fin;

endmodule




