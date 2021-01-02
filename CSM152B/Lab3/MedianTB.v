`timescale 1ps / 1ps
module MedianHelperTB;
	reg [7:0] n0, n1, n2, n3, n4, n5, n6, n7, n8;
	reg clk;
	wire [7:0] s;

	MedianHelper medHelp (
		.clk(clk),
		.n0(n0),
		.n1(n1),
		.n2(n2),
		.n3(n3),
		.n4(n4),
		.n5(n5),
		.n6(n6),
		.n7(n7),
		.n8(n8),
		.s(s)
	);

	initial begin
		clk = 0;
	end
	
	always begin
		#1 clk = ~clk;
	end

	initial begin 
		n0 = 0;
		n1 = 1;
		n2 = 2;
		n3 = 3;
		n4 = 4;
		n5 = 5;
		n6 = 6; 
		n7 = 7;
		n8 = 8;
		#5 n0 = 15;
		n1 = 14;
		n2 = 7;
		n3 = 12;
		n4 = 8;
		n5 = 10;
		n6 = 11;
		n7 = 8;
		n8 = 13;
		#5
		$stop;
	end
endmodule

module MedianTB;
	reg [7:0] image_input;
	reg enable, enable_process, clk;
	wire [7:0] image_output;
	wire finish;

	Median medianfilter (
		.image_input(image_input),
		.enable(enable),
		.enable_process(enable_process),
		.clk(clk),
		.image_output(image_output),
		.finish(finish)
	);

	integer fdr, fdw;
	integer result;
	initial begin
		clk = 0;
		enable = 1;
		enable_process = 0;
		fdr = $fopen("./noisy_image.text", "r");
		if (fdr == -1) begin
			$display("Failed to open read file");
			$stop;
		end
		fdw = $fopen("./medianImage.text", "w");
		if (fdw == -1) begin
			$display("Failed to open write file");
			$stop;
		end
	end

	always begin
		#1 clk = ~clk;
	end

	always @ (posedge clk) begin
		result = $fscanf(fdr, "%d,", image_input);

		if ($feof(fdr) && enable) begin
			enable = 0;
			enable_process = 1;
		end else if (finish == 1) begin
			$fclose(fdr);
			$fclose(fdw);
			$stop;
		end else if (enable_process) begin
			if (image_output === 8'hxx)
				$fwrite(fdw, "%d,", 8'b01111111);
			else
				$fwrite(fdw, "%d,", image_output);
		end
	end

endmodule
