#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 2)
    {
        cerr << "Incorrect number of arguments\n";
        return -1;
    }
    
    ifstream in;
    ofstream out;
    in.open(argv[1]);
    out.open("formatted_lvl.txt", ios::trunc);
    
    if (!in.is_open())
    {
        cerr << "Problem opening input file\n";
        return -1;
    }
    if (!out.is_open())
    {
        cerr << "Problem opening output file\n";
        return -1;
    }
    
    string line;
    while (getline(in,line))
    {
      out << line << ',';
    }
    
    in.close();
    out.close();
    
    return 0;
}
