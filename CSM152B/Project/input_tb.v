module input_tb;
	reg [7:0] dir_input;
	reg [7:0] lvl_input;
	reg enable;
	reg clk;

	integer fdw, fdl, count, result, pos;

	initial begin
        clk = 0;
		enable = 1;
		fdw = $fopen("./test.text", "w+");
		if (fdw == -1) begin
			$display("Failed to open input file");
			$stop;
		end
		/*
		fdl = $fopen("./level.text", "r");
		if (fdl == -1) begin
			$display("Failed to open input file");
			$stop;
		end
		*/
	end

	always begin
		#1 clk = ~clk;
	end

	always @ (posedge clk) begin
		if (enable) begin
			count = $fscanf(fdw, "%c,", dir_input);
			if (count > 0) begin
				// if we picked up an input, display it and clost the file
				$display("%c", dir_input);
				$fclose(fdw);

				// re-opening the file should clear it for new input
				fdw = $fopen("./test.text", "w+");
				if (fdw == -1) begin
					$display("Failed to open input file");
					$stop;
				end
			end else begin
				// seek to the beginning of the file and try to scan again
				pos = $fseek(fdw, 0, 0);
			end
		end
		/*
		else begin
			result = $fscanf(fdl, "%d,", lvl_input);
			if ($feof(fdr) && enable) begin
				enable = 1;
				$fclose(fdl);
			end
		end
		*/
	end

endmodule
