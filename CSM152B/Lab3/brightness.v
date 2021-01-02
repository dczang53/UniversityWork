module brightness (
    input wire [7:0] image_input,
    input wire [7:0] bright,
    input wire do_bright,
    input wire enable,
    input wire enable_process,
    input wire clk,

    output reg [7:0] image_output,
    output reg finish
);

parameter width = 410;
parameter depth = 361;

integer i = 0;
integer j = 0;
integer buff = 0;
integer done = 0;
reg [7:0] image_data [0:width - 1] [0:depth - 1];

always @ (posedge clk) begin
    if (enable) begin
        if (i < width) begin
            if (j < depth) begin
                image_data[i][j] = image_input;
                j = j + 1;
            end
            else begin
                i = i + 1;
                j = 0;
            end
        end
    end

    else if (enable_process) begin
        if (!done) begin
            for (i = 0; i < width; i = i + 1) begin
                for (j = 0; j < depth; j = j + 1) begin
                    if (do_bright) begin buff = image_data[i][j] + bright; end
                    else begin buff = image_data[i][j] - bright; end

                    if (buff > 255) begin buff = 255; end
                    else if (buff < 0) begin buff = 0; end

                    image_data[i][j] = buff;
                end
            end

            i = 0;
            j = 0;
            done = 1;
        end

        else begin
            if (i < width) begin
                if (j < depth) begin
                    image_output = image_data[i][j];
                    j = j + 1;
                end
                else begin
                    i = i + 1;
                    j = 0;
                end
            end
            else begin
                finish <= 1;
            end
        end
    end
end

endmodule