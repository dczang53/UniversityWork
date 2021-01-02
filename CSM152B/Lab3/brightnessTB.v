module brightnessTB;
	reg [7:0] image_input;
	reg enable, enable_process, clk;
	
	// For Brightness only
	reg do_bright;
	reg [7:0] bright;

	wire [7:0] image_output;
	wire finish;

	brightness Brightfilter (
		.image_input(image_input),
		.bright(bright),
		.do_bright(do_bright),
		.enable(enable),
		.enable_process(enable_process),
		.clk(clk),
		.image_output(image_output),
		.finish(finish)
	);

	integer fdr, fdw;
	integer result;
	initial begin
		bright = 60;
		do_bright = 0;
		clk = 0;
		enable = 1;
		enable_process = 0;
		fdr = $fopen("./noisy_image.text", "r");
		if (fdr == -1) begin
			$display("Failed to open read file");
			$stop;
		end
		fdw = $fopen("./brightness.text", "w");
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
			if (!(image_output === 8'hxx))
				$fwrite(fdw, "%d,", image_output);
		end
	end

endmodule
