module test_tb;
	reg [7:0] dir_input;
	reg clk;

	integer fdw, count, pos;

	initial begin
        clk = 0;
		fdw = $fopen("./test.text", "w+");
		if (fdw == -1) begin
			$display("Failed to open input file");
			$stop;
		end
	end

	always begin
		#1 clk = ~clk;
	end

	always @ (posedge clk) begin
		count = $fscanf(fdw, "%c,", dir_input);

		if (count > 0) begin
            // if we picked up an input, display it and clost the file
			$display("%c", dir_input);
            $fclose(fdw);

            // re-opening the file should create a new one
            // this will be used for the next input
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

endmodule
