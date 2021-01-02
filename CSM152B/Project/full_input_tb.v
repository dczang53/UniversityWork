module input_tb;
	reg [7:0] dir_input;
	reg [7:0] lvl_input;
	reg enable;
	reg clk;

	integer fdr, fdl, count, result, pos;

	initial begin
        clk = 0;
		enable = 1;
		fdr = $fopen("./test.text", "r");
		if (fdw == -1) begin
			$display("Failed to open input file");
			$stop;
		end
		fdl = $fopen("./level.text", "r");
		if (fdl == -1) begin
			$display("Failed to open input file");
			$stop;
		end
	end

	always begin
		#1 clk = ~clk;
	end

	always @ (posedge clk) begin
		if (enable) begin
			count = $fscanf(fdr, "%d,", dir_input);
            if ($feof(fdr) && enable) begin
				$fclose(fdl);
			end
		end
		else begin
			result = $fscanf(fdl, "%d,", lvl_input);
			if ($feof(fdr) && enable) begin
				enable = 1;
				$fclose(fdl);
			end
		end
	end

endmodule
