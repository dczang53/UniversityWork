// Constants for maximum register and filter sizes
// `define MAX_FILTER_OFFSET 50    // maximum window size of ((MAX_FILTER_OFFSET * 2) + 1)^2
`define MAX_IMG_WIDTH 500       // maximum image width
`define MAX_IMG_HEIGHT 500      // maximum image height
`define MAX_SCALE 10            // maximum scaling (increasing and decreasing)

// Change these to test on different files and scales
`define TEST_WIDTH 410
`define TEST_HEIGHT 361
`define TEST_FILE "noisy_image.text"
`define TEST_SCALE 3            // testing scaling in testbench

`timescale 1ns / 100ps



module testbench;
	reg [7:0] image_input;
    reg enable, enable_process, clk;
    wire [7:0] image_output;
    wire finish;
    reg [31:0] Width;
    reg [31:0] Depth;
    reg [31:0] resize_size;
    reg enlargen;

    resizeFilter UUT (  .image_input(image_input),
                        .enable(enable),
                        .enable_process(enable_process),
                        .clk(clk),
                        .image_output(image_output),
                        .finish(finish),
                        .Width(Width),
                        .Depth(Depth),
                        .resize_size(resize_size),
                        .enlargen(enlargen));

	always
        begin
		#1 clk = ~clk;
        end

    integer imgCSV, readPos;
    reg [7:0] imgData [0:(`MAX_IMG_WIDTH * `MAX_IMG_HEIGHT) - 1];
    integer buffer, readStatus;
    
    integer imgPosition;

    integer shrinkCSV;
    reg [7:0] shrinkOutput [0:(`MAX_IMG_WIDTH * `MAX_IMG_HEIGHT) - 1];

    integer expandCSV;
    reg [7:0] expandOutput [0:(`MAX_IMG_WIDTH * `MAX_IMG_HEIGHT * `TEST_SCALE * `TEST_SCALE) - 1];

    integer i;

    initial
        begin
        readPos = 0;
        imgCSV = $fopen(`TEST_FILE, "r");
        readStatus = $fscanf(imgCSV, "%d,", buffer);
        while (readStatus > 0)
            begin
                $display ("buffer = %b", buffer);
                readStatus = $fscanf(imgCSV, "%d,", buffer);
                imgData[readPos] = buffer;
                readPos = readPos + 1;
            end
        $fclose(imgCSV);
        for (i = 0; i < readPos; i = i + 1)
            begin
                $display ("imgData[%d] = %b", i, imgData[i]);
            end



        clk = 1;

        // shrinking by scale of 3
        resize_size = `TEST_SCALE;
        enlargen = 0;
        enable = 0;
        enable_process = 0;
        Width = `TEST_WIDTH;
        Depth = `TEST_HEIGHT;
        #1;
        enable = 1;
        imgPosition = 0;
        while (finish != 1)
            begin
            image_input = imgData[imgPosition];
            imgPosition = imgPosition + 1;
            #2;
            end
        #2;
        enable = 0;
        enable_process = 1;
        #2;
        while (finish != 1)
            begin
            #2;
            end
        #2;
        imgPosition = 0;
        while (imgPosition < ((`TEST_WIDTH / `TEST_SCALE) * (`TEST_HEIGHT / `TEST_SCALE)))
            begin
            shrinkOutput[imgPosition] <= image_output;
            imgPosition = imgPosition + 1;
            #2;
            end
        shrinkCSV = $fopen("shrink.text");
        $fwrite(shrinkCSV, "%d", shrinkOutput[0]);
        for (i = 1; i < ((`TEST_WIDTH / `TEST_SCALE) * (`TEST_HEIGHT / `TEST_SCALE)); i = i + 1)
            begin
                $display ("writing shrinkOutput[%d] = %b", i, shrinkOutput[i]);
                $fwrite(shrinkCSV, ",%d", shrinkOutput[i]);
            end
        $fclose(shrinkCSV);
        enable = 0;
        enable_process = 0;





        // expanding by scale of 3
        #1
        enlargen = 1;
        enable = 0;
        enable_process = 0;
        #1;
        enable = 1;
        imgPosition = 0;
        while (finish != 1)
            begin
            image_input = imgData[imgPosition];
            imgPosition = imgPosition + 1;
            #2;
            end
        #2;
        enable = 0;
        enable_process = 1;
        #2;
        while (finish != 1)
            begin
            #2;
            end
        #2;
        imgPosition = 0;
        while (imgPosition < ((`TEST_WIDTH * `TEST_SCALE) * (`TEST_HEIGHT * `TEST_SCALE)))
            begin
            expandOutput[imgPosition] <= image_output;
            imgPosition = imgPosition + 1;
            #2;
            end
        expandCSV = $fopen("expand.text");
        $fwrite(expandCSV, "%d", expandOutput[0]);
        for (i = 1; i < ((`TEST_WIDTH * `TEST_SCALE) * (`TEST_HEIGHT * `TEST_SCALE)); i = i + 1)
            begin
                $display ("writing expandOutput[%d] = %b", i, expandOutput[i]);
                $fwrite(expandCSV, ",%d", expandOutput[i]);
            end
        $fclose(expandCSV);
        enable = 0;
        enable_process = 0;



        $stop;
        end
endmodule


