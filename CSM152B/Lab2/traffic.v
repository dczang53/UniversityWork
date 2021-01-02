`timescale 1ns / 100ps
module traffic (clk, rst, walkRequest, trafficSensor, 
			mainLights, sideLights, walkLamp);

	input clk, rst, walkRequest, trafficSensor;
	output [2:0] mainLights, sideLights;
	output walkLamp;

	// Light colors for the traffic light
	parameter red = 3'b100;
	parameter yellow = 3'b010;
	parameter green = 3'b001;
	parameter on = 1'b1;
	parameter off = 1'b0;

	// Reg to hold changes to traffic lights and walk sign
	reg [2:0] mainTempReg, sideTempReg;
	reg walkTempReg;

	// List of all possible states. Green for main implies red for side
	// and vice versa
	reg [2:0] state;
	parameter main = 3'b000; 	// For 6 seconds, mainLights = green
	parameter mainExtended = 3'b001;// For 3 or 6 seconds, mainLights = green
	parameter mainYellow = 3'b010;  // For 2 seconds, mainLights = yellow
	parameter side = 3'b011; 	// For 6 seconds, sideLights = green
	parameter sideExtended = 3'b100;// For 3 seconds, sideLights = green
	parameter sideYellow = 3'b101;  // For 2 seconds, sideLights = yellow
	parameter walk = 3'b110;	// For 3 seconds, mainLights, sideLights = red
					//     Walk sign set to ON
	
	reg [3:0] stateTimer; 		// The time remaining for the current state 
	reg walkReg;			// Walk Register	

	initial begin
		stateTimer = 1;
		walkReg = 0;
		state = sideYellow;	// Starts by transitioning to main
	end
	
	always @ (posedge clk) begin
		// Decrement the Timer
		stateTimer = stateTimer - 1;

		// Handle state transitions
		if (stateTimer == 0) begin
			if (state == main) begin
				state = mainExtended;
				if (trafficSensor == 1)
					stateTimer = 3'b011;
				else
					stateTimer = 3'b110;
			end else if (state == mainExtended) begin
				state = mainYellow;
				stateTimer = 3'b010;
			end else if (state == mainYellow) begin
				if (walkReg == 1) begin	
					state = walk;
					stateTimer = 3'b011;
				end else begin
					state = side;
					stateTimer = 3'b110;
				end
			end else if (state == side) begin
				if (trafficSensor == 1) begin
					state = sideExtended;
					stateTimer = 3'b011;
				end else begin
					state = sideYellow;
					stateTimer = 3'b010;
				end
			end else if (state == sideExtended) begin
				state = sideYellow;
				stateTimer = 3'b010;
			end else if (state == sideYellow) begin
				state = main;
				stateTimer = 3'b110;
			end else if (state == walk) begin
				state = side;
				stateTimer = 3'b110;
				walkReg = 0;
			end
		end
	end

	always @ * begin
	// Set output based on current state
		if (rst == 1) begin
			stateTimer = 1;
			walkReg = 0;
			state = sideYellow;
		end
		else if (walkRequest == 1)
			walkReg = 1;

		if (state == main || state == mainExtended) begin
			assign mainTempReg = green;
			assign sideTempReg = red;
			assign walkTempReg = off;
		end else if (state == mainYellow) begin
			assign mainTempReg = yellow;
			assign sideTempReg = red;
			assign walkTempReg = off;
		end else if (state == side || state == sideExtended) begin
			assign mainTempReg = red;
			assign sideTempReg = green;
			assign walkTempReg = off;
		end else if (state == sideYellow) begin 
			assign mainTempReg = red;
			assign sideTempReg = yellow;	
			assign walkTempReg = off;
		end else if (state == walk) begin
			assign mainTempReg = red;
			assign sideTempReg = red;
			assign walkTempReg = on;
		end
	end
	assign mainLights = mainTempReg;
	assign sideLights = sideTempReg;
	assign walkLamp = walkTempReg;
endmodule
