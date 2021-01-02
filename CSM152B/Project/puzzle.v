module Puzzle (grid_input, enable, enable_process, clk, rst, result);
	input [7:0] grid_input;
	input enable, enable_process, clk, rst;
	output result;

	parameter HEIGHT = 10;
	parameter WIDTH = 10;
	parameter SIZE = 8;

	// Loaded Grid
	reg [(HEIGHT * WIDTH * SIZE) - 1:0] grid;

	// Result saved in register
	reg res;

	// Temp register
	reg [7:0] temp;

	// Loading Position
	integer i, j;
	initial begin
		i = 0;
		j = 0;
	end

	// Player position
	integer y, x, goal_y, goal_x, loading_finished;
	initial begin
		grid = 0;
		loading_finished = 0;
		y = -1;
		x = -1;
		goal_y = -1;
		goal_x = -1;
		temp = 0;
	end

	// Fatal Error
	integer lost, won;
	initial begin
		lost = 0;
		won = 0;
	end

	// Loading game into register
	always @ (posedge clk) begin
		// Reset resets the register and some variables
		if (enable && rst) begin
			grid = 0;
			i = 0;
			j = 0;
			loading_finished = 0;
			y = -1;
			x = -1;
			goal_y = -1;
			goal_x = -1;
			lost = 0;
			won = 0;
			temp = 0;
		end
		// First set up the starting and goal position
		else if (enable && loading_finished == 0) begin
			if (grid_input === 8'hxx)
				$display("Detected an X value");
			else if (x == -1)
				x = grid_input;
			else if (y == -1)
				y = grid_input;
			else if (goal_x == -1)
				goal_x = grid_input;
			else begin
				goal_y = grid_input;
				loading_finished = 1;
			end

			//$display("X,Y,GX,GY are %d,%d,%d,%d", x, y, goal_x, goal_y);
		end
		// Now start filling in the grid in the register
		else if (enable && j < HEIGHT) begin
			grid[(WIDTH * j + i) * SIZE +: 8] = grid_input;

			$display("Position %d,%d is assigned value %d", i, j, grid_input);
			$display("Grid has value %d", grid);

			if (i + 1 >= WIDTH) begin
				j = j + 1;
				i = 0;
			end else
				i = i + 1;

			//$display("i is %d", i);
			//$display("j is %d", j);
		end
	end

	// Moving player through grid
	always @ (posedge clk) begin
		// Ignore x values
		if (grid_input === 8'hxx)
			$display("Detected an X value");
		else if (!enable && enable_process) begin
			// Check N = 78
			if (grid_input == 78)
				y = y - 1;
			// Check S = 83
			else if (grid_input == 83)
				y = y + 1;
			// Check E = 69
			else if (grid_input == 69)
				x = x + 1;
			// Check W = 87
			else if (grid_input == 87)
				x = x - 1;
			// If illegal character, then game lost
			else begin
				lost = 1;
				$display("Detected unknown input, %d. Game lost", grid_input);
			end
			
			// If out of bounds, then game lost
			// Otherwise, decrement the legal position
			if (x >= WIDTH || x < 0)
				lost = 1;
			else if (y >= HEIGHT || y < 0)
				lost = 1;
			else begin
				temp = grid[(WIDTH * y + x) * SIZE +: 8];
				$display("Temp is %d", temp);
				temp = temp - 1;
				if (temp == 255 || temp < 0) begin
					lost = 1;
				end
				grid[(WIDTH * y + x) * SIZE +: 8] = temp;

				if (y == goal_y && x == goal_x && grid == 0)
					won = 1;
				else
					won = 0;
			end
			$display("X is %d", x);
			$display("Y is %d", y);
			//$display("Grid is %d", grid);
		end

		if (lost == 1)
			res = 0;
		else if (won == 1)
			res = 1;
		else
			res = 0;
	end

	assign result = res;
endmodule