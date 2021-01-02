#include <iostream>
#include <string>
#include <stack>
#include <cassert>
using namespace std;

bool translate(string& n);

int evaluate(string infix, const bool values[], string& postfix, bool& result)
{
	string out = infix;
	if (!translate(out))
		return 1;
	stack<bool> operands;
	bool op1;
	bool op2;
	string integers = "0123456789";
	for (int x = 0; x < out.size(); x++)
	{
		if (isdigit(out[x]))
		{
			for (int y = 0; y <= 9; y++)
				if (out[x] == integers[y])
					operands.push(values[y]);
		}
		else if (out[x] == '!')
		{
			if (operands.size() < 1)
				return 1;
			op1 = operands.top();
			operands.pop();
			operands.push(!op1);
		}
		else if (out[x] == '&')
		{
			if (operands.size() < 2)
				return 1;
			op2 = operands.top();
			operands.pop();
			op1 = operands.top();
			operands.pop();
			operands.push(op1 & op2);
		}
		else if (out[x] == '|')
		{
			if (operands.size() < 2)
				return 1;
			op2 = operands.top();
			operands.pop();
			op1 = operands.top();
			operands.pop();
			operands.push(op1 || op2);
		}
	}
	if (operands.size() == 1)
	{
		postfix = out;
		result = operands.top();
		return 0;
	}
	return 1;
}

bool translate(string& n)
{
	string translation;
	stack<char> operators;
	int c = 0;
	for (int x = 0; x < n.size(); x++)
	{
		switch (n[x])
		{
		case ('0'):
		case ('1'):
		case ('2'):
		case ('3'):
		case ('4'):
		case ('5'):
		case ('6'):
		case ('7'):
		case ('8'):
		case ('9'):
			if (x != 0 && isdigit(n[x - 1]))
				return false;
			translation += n[x];
			break;
		case ('('):
			operators.push(n[x]);
			break;
		case (')'):
			if (x == 0 || (x != 0 && (n[x - 1] == '!' || n[x - 1] == '&' || n[x - 1] == '|')))
				return false;
			while (operators.top() != '(' && !operators.empty())
			{
				translation += operators.top();
				operators.pop();
			}
			if (!operators.empty())
				operators.pop();
			else
				return false;
			break;
		case ('!'):
			if (x == n.size() - 1 || (x != 0 && (n[x - 1] == ')')))
				return false;
			operators.push(n[x]);
			break;
		case ('&'):
			if (x == 0 || x == n.size() - 1 || (x != 0 && (n[x - 1] == '(' || n[x - 1] == '!' || n[x - 1] == '&' || n[x - 1] == '|')))
				return false;
			while (operators.size() != 0 && operators.top() == '!')
			{
				translation += operators.top();
				operators.pop();
			}
			operators.push(n[x]);
			break;
		case ('|'):
			if (x == 0 || x == n.size() - 1 || (x != 0 && (n[x - 1] == '(' || n[x - 1] == '!' || n[x - 1] == '&' || n[x - 1] == '|')))
				return false;
			while (operators.size() != 0 && (operators.top() == '!' || operators.top() == '&'))
			{
				translation += operators.top();
				operators.pop();
			}
			operators.push(n[x]);
			break;
		case (' '):
			break;
		default:
			return false;
		}
	}
	while (!operators.empty())
	{
		if (operators.top() == '(' || operators.top() == ')')
			return false;
		translation += operators.top();
		operators.pop();
	}

	for (int x = 0; x < translation.size(); x++)
	{
		if (isdigit(translation[x]))
			c++;
		if (translation[x] == '!')
			if (c < 1)
				return false;
		if (translation[x] == '&' || translation[x] == '|')
		{
			if (c < 2)
				return false;
			c--;
		}
	}
	if (c == 1)
	{
		n = translation;
		return true;
	}
	else
		return false;
}

int main()
{
	bool ba[10] = {
		//  0      1      2      3      4      5      6      7      8      9
		true,  true,  true,  false, false, false, true,  false, true,  false
	};
	string pf;
	bool answer;
	assert(evaluate("2| 3", ba, pf, answer) == 0 && pf == "23|" &&  answer);
	assert(evaluate("8|", ba, pf, answer) == 1);
	assert(evaluate("4 5", ba, pf, answer) == 1);
	assert(evaluate("01", ba, pf, answer) == 1);
	assert(evaluate("()", ba, pf, answer) == 1);
	assert(evaluate("2(9|8)", ba, pf, answer) == 1);
	assert(evaluate("2(&8)", ba, pf, answer) == 1);
	assert(evaluate("(6&(7|7)", ba, pf, answer) == 1);
	assert(evaluate("", ba, pf, answer) == 1);
	assert(evaluate("4  |  !3 & (0&3) ", ba, pf, answer) == 0
		&& pf == "43!03&&|" && !answer);
	assert(evaluate(" 9  ", ba, pf, answer) == 0 && pf == "9" && !answer);
	ba[2] = false;
	ba[9] = true;
	assert(evaluate("((9))", ba, pf, answer) == 0 && pf == "9"  &&  answer);
	assert(evaluate("2| 3", ba, pf, answer) == 0 && pf == "23|" && !answer);
	cout << "Passed all tests" << endl;
}