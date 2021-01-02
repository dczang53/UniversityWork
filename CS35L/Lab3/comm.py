import locale, sys
from optparse import OptionParser

class comm:
    def __init__(self, file1, file2, orient, parser):
        try:
            if (file1 == "-"):
                if (file2 != "-"):
                    self.file1lines = sys.stdin.readlines()
                    f = open(file2, 'r')
                    self.file2lines = f.readlines()
                    f.close()
                else:
                    parser.error("cannot have two inputs from stdin")
            elif (file2 == "-"):
                if (file1 != "-"):
                    self.file2lines = sys.stdin.readlines()
                    f = open(file1, 'r')
                    self.file1lines = f.readlines()
                    f.close()
                else:
                    parser.error("cannot have two inputs from stdin")
            else:
                f = open(file1, 'r')
                self.file1lines = f.readlines()
                f.close()
                f = open(file2, 'r')
                self.file2lines = f.readlines()
                f.close()
        except IOError as e:
            parser.error("File cannot be read {0}".format(e.errno, e.strerror))
        self.nA = len(self.file1lines)
        self.nB = len(self.file2lines)
        self.orderset = orient
        if (orient):
            A = 1
            while (A < self.nA):
                if (locale.strcoll(self.file1lines[A], self.file1lines[A - 1]) < 0):
                    parser.error("FILE 1 is not sorted")
                A += 1
            B = 1
            while (B < self.nB):
                if (locale.strcoll(self.file2lines[B], self.file2lines[B - 1]) < 0):
                    parser.error("FILE 2 is not sorted")
                B += 1
        if (self.nA != 0):
            self.file1lines[self.nA - 1] += "\n"
        if (self.nB != 0):
            self.file2lines[self.nB - 1] += "\n"
        self.array1 = list(range(1, self.nA + 1))
        self.array2 = list(range(1, self.nB + 1))
        lineA = 0
        lineB = 0
        while (lineA < self.nA):
            while (lineB < self.nB):
                if (self.file1lines[lineA] == self.file2lines[lineB] and self.array2[lineB] > 0):
                    if (lineA > lineB):
                        self.array1[lineA] = -1
                        self.array2[lineB] = 0
                    else:
                        self.array1[lineA] = 0
                        self.array2[lineB] = -1
                    break
                lineB += 1
            lineB = 0
            lineA += 1

    def display(self, first, second, third):
        C = 0
        if (not(self.orderset)):
            n = 0
            while (n < self.nA):
                if (self.array1[n] < 0):
                    self.array1[n] = 0
                n += 1
        if (first):
            n = 0
            while (n < self.nA):
                if (self.array1[n] > 0):
                    self.array1[n] = -1
                n += 1
        if (second):
            n = 0
            while (n < self.nB):
                if (self.array2[n] > 0):
                    self.array2[n] = -1
                n += 1
        if (third):
            n = 0
            while (n < self.nA):
                if (self.array1[n] == 0):
                    self.array1[n] = -1
                n += 1
            n = 0
            while (n < self.nB):
                if (self.array2[n] == 0):
                    self.array2[n] = -1
                n += 1
        if (self.orderset):
            if(self.nA > self.nB):
                nC = self.nB
                w = self.file1lines
                x = self.array1
                y = self.nA
                z = "\t"
            else:
                nC = self.nA
                w = self.file2lines
                x = self.array2
                y = self.nB
                z = "\t\t"
            while (C < nC):
                if (locale.strcoll(self.file1lines[C], self.file2lines[C]) < 0):
                    if (self.array1[C] >= 0):
                        if (self.array1[C]):
                            sys.stdout.write(self.file1lines[C])
                        else:
                            sys.stdout.write("\t\t" + self.file1lines[C])
                    if (self.array2[C] >= 0):
                        if (self.array2[C]):
                            sys.stdout.write("\t" + self.file2lines[C])
                        else:
                            sys.stdout.write("\t\t" + self.file2lines[C])
                else:
                    if (self.array2[C] >= 0):
                        if (self.array2[C]):
                            sys.stdout.write("\t" + self.file2lines[C])
                        else:
                            sys.stdout.write("\t\t" + self.file2lines[C])
                    if (self.array1[C] >= 0):
                        if (self.array1[C]):
                            sys.stdout.write(self.file1lines[C])
                        else:
                            sys.stdout.write("\t\t" + self.file1lines[C])
                C += 1
            while (C < y):
                if (x[C] > 0):
                    sys.stdout.write(z + w[C])
                C += 1
        else:
            for A in self.array1:
                if (A > 0):
                    sys.stdout.write(self.file1lines[C])
                elif (A == 0):
                    sys.stdout.write("\t\t" + self.file1lines[C])
                C += 1
            C = 0
            for B in self.array2:
                if (B > 0):
                    sys.stdout.write("\t" + self.file2lines[C])
                C += 1

def main():
    locale.setlocale(locale.LC_ALL, "C")
    version_msg = "%prog 2.0"
    usage_msg = """%prog [OPTION] FILE1 FILE2

    Select or reject lines common to two files."""
    parser = OptionParser(version=version_msg, usage=usage_msg)
    parser.add_option("-1", action="store_true", dest="set1", default=False, help="Suppress the output column of lines unique to FILE1.")
    parser.add_option("-2", action="store_true", dest="set2", default=False, help="Suppress the output column of lines unique to FILE2.")
    parser.add_option("-3", action="store_true", dest="set3", default=False, help="Suppress the output column of lines unique to FILE3.")
    parser.add_option("-u", action="store_false", dest="order", default=True, help="Run command with unsorted FILE1 and FILE2.")
    (options, args) = parser.parse_args(sys.argv[1:])
    
    if len(args) != 2:
        parser.error("wrong number of operands")
    
    compared = comm(args[0], args[1], options.order, parser)
    compared.display(options.set1, options.set2, options.set3)
    

if __name__ == "__main__":
    main()




#dos2unix