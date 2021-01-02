`timescale 1ns / 100ps
module AverageTB;
	reg clk;
	reg [7:0] n0, n1, n2, n3, n4, n5, n6, n7, n8;
	wire [7:0] s;

	Average avg (
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

always begin
		#1 clk = ~clk;
	end

	initial begin 
		clk = 0;
		n0 = 100;
		n1 = 110;
		n2 = 120;
		n3 = 130;
		n4 = 140;
		n5 = 150;
		n6 = 160; 
		n7 = 170;
		n8 = 180;
		#5 n0 = 150;
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

module LowPassTB;
	reg [7:0] image_input;
	reg enable, enable_process, clk;
	wire [7:0] image_output;
	wire finish;

	LowPass lowPassFilter (
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
		fdw = $fopen("./lowPassImage.text", "w");
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