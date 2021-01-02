module addbit (cin, a, b, s, cout);
    input cin;
    input a;
    input b;
    output s;
    output cout;

    wire notA;
    wire notB;
    wire notCi;
    not(notA, a);
    not(notB, b);
    not(notCi, cin);
    wire s1;
    wire s2;
    wire s3;
    wire s4;
    and(s1, a, notB, notCi);
    and(s2, notA, b, notCi);
    and(s3, notA, notB, cin);
    and(s4, a, b, cin);
    or(s, s1, s2, s3, s4);

    wire cout1;
    wire cout2;
    wire cout3;
    and(cout1, a, b);
    and(cout2, b, cin);
    and(cout3, a, cin);
    or(cout, cout1, cout2, cout3);
endmodule
