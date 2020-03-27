module top_module ();

    reg clk, reset, t, q;
    tff is1 (clk, reset, t, q);


    initial begin
        clk = 0;
        reset = 1;
        #10
        reset = 0;
        t = 1;
    end
    

    always begin 
        #5
        clk = ~clk;
    end
endmodule
