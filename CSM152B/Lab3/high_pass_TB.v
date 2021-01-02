`timescale 1ns / 100ps
module High_Pass_TB;
	reg [7:0] image_input;
	reg enable, enable_process, clk;
	wire [7:0] image_output;
	wire finish;

	highpass highPassFilter (
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
		fdw = $fopen("./highPassImage.text", "w");
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