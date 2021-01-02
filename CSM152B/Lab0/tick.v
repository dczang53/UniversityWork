module tick (A, SIGN, B);
    input [3:0] A;
    input SIGN;             // "0" for -1, "1" for 1
    output [3:0] B;

/*
    assign B[3] = ((SIGN && (A == 7 || A == 8)) || (!SIGN && (A == 0 || A == 9)));
    assign B[2] = ((SIGN && ((A == 3) || (A == 4) || (A == 5) || (A == 6))) || (!SIGN && ((A == 5) || (A == 6) || (A == 7) || (A == 8))));
    assign B[1] = ((SIGN && ((A == 1) || (A == 2) || (A == 5) || (A == 6))) || (!SIGN && ((A == 3) || (A == 4) || (A == 7) || (A == 8))));
    assign B[0] = ~A[0];
*/

    // came up with this because I thought there was a bug here (there wasn't lol, it's the same as above, I wasted my time) -_____-
    //(imma just leave this here just in case)
    assign B[0] = ~A[0];
    assign B[1] = (SIGN & (A[0] ^ A[1]) & ~A[3]) | (~SIGN & ((A[0] & A[1] & ~A[3]) | (~(A[0] | A[1]) & (A[2] ^ A[3]))));
    assign B[2] = (SIGN & ((A[0] & (A[1] ^ A[2])) | (~A[0] & A[2])) & ~A[3]) | (~SIGN & (((A[0] | A[1]) & A[2] & ~A[3]) | (~A[0] & ~A[1] & ~A[2] & A[3])));
    assign B[3] = (SIGN & ((A[0] & A[1] & A[2] & ~A[3]) | (~A[0] & ~A[1] & ~A[2] & A[3]))) | (~SIGN & (A[0] ^~ A[3]) & ~A[1] & ~A[2]);
    
endmodule

// assumes no inputs 1010-1111 (behavior not defined here)
