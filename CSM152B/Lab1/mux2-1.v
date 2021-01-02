module mux2_1 (a, b, sel, o);
    input a;
    input b;
    input sel;
    output o;

    wire notSel;
    not(notSel, sel);

    wire o1;
    wire o2;
    and(o1, a, notSel);
    and(o2, b, sel);

    or(o, o1, o2);
endmodule
