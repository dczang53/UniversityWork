`timescale 1ns / 100ps
module traffic_TB;
	reg clk, rst, walkRequest, trafficSensor;
	wire [2:0] mainLights, sideLights;
	wire walkLamp;

	traffic trafficLight (
		.clk(clk),
		.rst(rst),
		.walkRequest(walkRequest),
		.trafficSensor(trafficSensor),
		.mainLights(mainLights),
		.sideLights(sideLights),
		.walkLamp(walkLamp)
	);

	always begin
		#1 clk = ~clk;
	end

	initial begin
        ///*
        // entire cycle(s) with no walkRequest nor trafficSensor (12+2+6+2) (begin 0, end 44)
		clk = 1;
		rst = 0;
		walkRequest = 0;
		trafficSensor = 0;

        // brief reset
		#45 rst = 1;
		#2 rst = 0;

        // entire cycle with no walkRequest but with trafficSensor for main only (12-3+2+6+2) (begin 48, end 86)
		#12 trafficSensor = 1;  // (59) set trafficSensor 1/2 clock cycle before 6 sec check
        #2 trafficSensor = 0;   // (61) unset after 6 sec check

        // brief reset
		#26 rst = 1;
		#2 rst = 0;

        // entire cycle with no walkRequest but with trafficSensor for side only (12+2+6+3+2) (begin 90, end 140)
		#40 trafficSensor = 1;  // (129)
        #2 trafficSensor = 0;   // (131)

        // brief reset
		#10 rst = 1;
		#2 rst = 0;

        // entire cycle with no walkRequest but with trafficSensor for both main and side (12-3+2+6+3+2) (begin 144, end 188)
		#12 trafficSensor = 1;  // (155)
        #2 trafficSensor = 0;   // (157)
		#20 trafficSensor = 1;  // (177)
        #2 trafficSensor = 0;   // (179)

        // brief reset
		#10 rst = 1;
		#2 rst = 0;





        // entire cycle(s) with walkRequest but no trafficSensor (12+2+3+6+2) (begin 192, end 242)
		walkRequest = 1;

        // brief reset
		#52 rst = 1;
		#2 rst = 0;

        // entire cycle with walkRequest and trafficSensor for main only (12-3+2+3+6+2) (begin 246, end 290)
		#12 trafficSensor = 1;  // (257)
        #2 trafficSensor = 0;   // (259)

        // brief reset
		#32 rst = 1;
		#2 rst = 0;

        // entire cycle with walkRequest and trafficSensor for side only (12+2+3+6+3+2) (begin 294, end 350)
		#46 trafficSensor = 1;  // (339)
        #2 trafficSensor = 0;   // (341)

        // brief reset
		#10 rst = 1;
		#2 rst = 0;

        // entire cycle with walkRequest and trafficSensor for both main and side (12-3+2+3+6+3+2) (begin 354, end 404)
		#12 trafficSensor = 1;  // (365)
        #2 trafficSensor = 0;   // (367)
		#26 trafficSensor = 1;  // (393)
        #2 trafficSensor = 0;   // (395)

		#10 $stop;              // (405)
        //*/
	end


endmodule