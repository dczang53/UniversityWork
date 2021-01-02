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
    out.open("formatted_in.txt", ios::trunc);
    
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
        for (auto it = line.cbegin() ; it != line.cend(); it++) {
            if (*it == 'N')
                out << "78";
            else if (*it == 'S')
                out << "83";
            else if (*it == 'E')
                out << "69";
            else if (*it == 'W')
                out << "87";
            else
                out << *it;
        }
    }
    
    in.close();
    out.close();
    
    return 0;
}
