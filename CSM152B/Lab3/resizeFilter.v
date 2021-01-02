// Constants for maximum register and filter sizes
`define MAX_FILTER_OFFSET 50    // maximum window size of ((MAX_FILTER_OFFSET * 2) + 1)^2
`define MAX_IMG_WIDTH 500       // maximum image width
`define MAX_IMG_HEIGHT 500      // maximum image height
`define MAX_SCALE 10            // maximum scaling (increasing and decreasing)

module average (enable, clk, averageWindow, windowSize, finish, avg);
    parameter maxWindowElements = `MAX_SCALE * `MAX_SCALE;

    input enable;
    input clk;
    input [0:(maxWindowElements * 8) - 1] averageWindow;
    input [31:0] windowSize;
    output reg finish;
    output [7:0] avg;

    integer i, j;

    wire [31:0] windowElements;
    assign windowElements = windowSize * windowSize;

    reg [31:0] total;

    assign avg = (finish)? (total / windowElements) : 8'b00000000;

    always @ (posedge clk)
        begin
        if (enable == 1)
            begin
            if (finish == 0)
                begin
                finish <= 1;
                for (i = 0; i < windowSize; i = i + 1)
                    begin
                        for (j = 0; j < windowSize; j = j + 1)
                            begin
                            total = total + averageWindow[(i *`MAX_SCALE * 8) + (j * 8)+:8];
                            end
                    end
                end
            end
        else
            begin
            finish <= 0;
            total <= 0;
            end
        end
endmodule

module resizeFilter (image_input, enable, enable_process, clk, image_output, finish, Width, Depth, resize_size, enlargen);
    parameter maxWindowElements = `MAX_SCALE * `MAX_SCALE;
    parameter maxElements = (`MAX_IMG_HEIGHT + `MAX_SCALE - 1) * (`MAX_IMG_WIDTH + `MAX_SCALE - 1);

    input [7:0] image_input;
    input enable;
    input enable_process;
    input clk;
    output reg [7:0] image_output;
    output reg finish;

    input [31:0] Width;
    input [31:0] Depth;
    input [31:0] resize_size;
    input enlargen;

    reg [0:(maxElements * 8) - 1] inputImg;
    reg [31:0] inputX, inputY;
    wire [31:0] inputPosition;
    assign inputPosition = (inputX * (`MAX_IMG_WIDTH + `MAX_SCALE - 1) * 8) + (inputY * 8);

    reg avgEnable;
    wire avgFinish;
    reg [7:0] allAverages [0:`MAX_IMG_HEIGHT - 1] [0:`MAX_IMG_WIDTH - 1];                               // used only for shrinking image
    reg [0:(maxWindowElements * 8) - 1] windowInput;
    wire [7:0] averageBuffer;
    reg [31:0] averageX, averageY;
    integer inputRow;

    average av(.enable(avgEnable), .clk(clk), .averageWindow(windowInput), .windowSize(resize_size), .finish(avgFinish), .avg(averageBuffer));

    reg [31:0] outputX, outputY;
    wire [31:0] originX, originY, finalHeight, finalWidth, originOutputPosition;
    assign originX = (!enlargen)? (outputX * resize_size) : (outputX / resize_size);
    assign originY = (!enlargen)? (outputY * resize_size) : (outputY / resize_size);
    assign finalHeight = (!enlargen)? (Depth / resize_size) : (Depth * resize_size);
    assign finalWidth = (!enlargen)? (Width / resize_size) : (Width * resize_size);
    assign originOutputPosition = (originX * (`MAX_IMG_WIDTH + `MAX_SCALE - 1) * 8) + (originY * 8);    // for accessing original value in inputImg when expanding only

    reg outputState;    // 0 for processing, 1 for outputing

    always @ (posedge clk)
        begin
        if (enable == 1)
            begin
            image_output <= 8'b00000000;
            finish <= 0;
            windowInput <= {(maxWindowElements){8'b00000000}};
            avgEnable <= 0;
            averageX <= 0;
            averageY <= 0;
            outputX <= 0;
            outputY <= 0;
            outputState <= 0;
            if (inputX < Depth && inputY < Width)
                begin
                inputImg[inputPosition+:8] <= image_input;
                if ((inputY + 1) < Width)
                    begin
                    inputY <= inputY + 1;
                    end
                else
                    begin
                    inputX <= inputX + 1;
                    inputY <= 0;
                    end
                end
            else
                finish <= 1;
            end
        else if (enable == 0 && enable_process == 1)
            begin
            inputX <= 0;
            inputY <= 0;
            if (outputState == 0)
                begin
                image_output <= 8'b00000000;
                if (averageX < Depth && averageY < Width)
                    begin
                    finish <= 0;
                    if (avgEnable == 0)
                        begin
                        for(inputRow = 0; inputRow < `MAX_SCALE; inputRow = inputRow + 1)
                            begin
                            windowInput[(inputRow * `MAX_SCALE * 8)+:(`MAX_SCALE * 8)] <= inputImg[((averageX * (`MAX_IMG_WIDTH + `MAX_SCALE - 1) * 8) + (averageY * 8))+:(`MAX_SCALE * 8)];
                            end
                        avgEnable <= 1;
                        end
                    else if (avgFinish == 1)
                        begin
                        allAverages[averageX][averageY] <= averageBuffer;
                        avgEnable <= 0;
                        if ((averageY + 1) < Width)
                            begin
                            averageY <= averageY + 1;
                            end
                        else
                            begin
                            averageX <= averageX + 1;
                            averageY <= 0;
                            end
                        end
                    end
                else
                    begin
                    finish <= 1;
                    outputState <= 1;
                    end
                end
            else
                begin
                finish <= 1;
                if (outputX < finalHeight && outputY < finalWidth)
                    begin
                    if (enlargen == 0)
                        begin
                        image_output <= allAverages[originX][originY];
                        end
                    else
                        begin
                        image_output <= inputImg[originOutputPosition+:8];
                        end
                    if ((outputY + 1) < finalWidth)
                        begin
                        outputY <= outputY + 1;
                        end
                    else
                        begin
                        outputX <= outputX + 1;
                        outputY <= 0;
                        end
                    end
                else
                    begin
                    image_output <= 8'b00000000;
                    end
                end
            end
        else
            begin
            image_output <= 8'b00000000;
            finish <= 0;
            inputImg <= {(maxElements){8'b00000000}};
            inputX <= 0;
            inputY <= 0;
            windowInput <= {(maxWindowElements){8'b00000000}};
            avgEnable <= 0;
            averageX <= 0;
            averageY <= 0;
            outputX <= 0;
            outputY <= 0;
            outputState <= 0;
            end
        end
endmodule


