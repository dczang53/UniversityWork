module Puzzle_TB;
	reg [7:0] grid_input;
	reg enable, enable_process, clk, rst;
	wire result;

	Puzzle puzzle_solver (
		.grid_input(grid_input),
		.enable(enable),
		.enable_process(enable_process),
		.clk(clk),
		.rst(rst),
		.result(result)
	);

	reg stop_sim;
	integer fdr1, fdr2;
	integer result1, result2;
	initial begin
		stop_sim = 0;
		clk = 0;
		rst = 0;
		enable = 1;
		enable_process = 0;
		fdr1 = $fopen("./p3.txt", "r");
		if (fdr1 == -1) begin
			$display("Failed to open read file");
			$stop;
		end
		fdr2 = $fopen("./p3_sol.txt", "r");
		if (fdr2 == -1) begin
			$display("Failed to open read file");
			$stop;
		end
	end

	always begin
		#1 clk = ~clk;
	end

	always @ (posedge clk) begin
		if (stop_sim)
			$stop;
		else if (!$feof(fdr1) && enable)
			result1 = $fscanf(fdr1, "%d,", grid_input);
		else if ($feof(fdr1) && enable)
			enable = 0;
		else if (enable == 0 && enable_process == 0) begin
			enable_process = 1;
			result2 = $fscanf(fdr2, "%d,", grid_input);
		end else if (enable_process) begin
			if ($feof(fdr2)) begin
				$fclose(fdr1);
				$fclose(fdr2);
				stop_sim = 1;
			end else
				result2 = $fscanf(fdr2, "%d,", grid_input);
		end
	end
endmodule