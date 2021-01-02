module counter(clk, rst, pause, adj, sel, min, sec);

input clk;
input rst;
input pause;
input adj;
input sel;

output reg [5:0] min; // count up to 60, which is 6 bits large
output reg [5:0] sec;

always @ (posedge clk, posedge rst)
begin
	if (rst)
	begin
		min <= 6'b000000;
		sec <= 6'b000000; 
	end
	else 
	begin
		if (~pause)
		begin
			if (adj) // in adj case we only increment one of the counters based on sel
			begin
				if (sel) // sel == 1 -> sec is being adjusted
				begin
					if (sec < 58)
						sec <= sec + 2;
					else
						sec <= 6'b000000;
				end
				else // else min is being adjusted
				begin
					if (min < 58)
						min <= min + 2;
					else
						min <= 6'b000000;
				end
			end
			else
			begin
				if (sec == 59)
				begin
					if (min != 59)
					begin
						sec <= 6'b000000;
						min <= min + 1;
					end
				end
				else
					sec <= sec + 1;
			end
		end
	end
end


endmodule