#include <iostream>
#include <string>
#include <windows.h>
#include <sstream>
#include <iomanip>
#include <cmath>

using std::string;

void ClearScreen();
void GetInfo();
void GetHelp();

void SetPrecision(string& command, int& precision);
void FormattingExpression(string& command);
void FindSubExpression(string& expression, std::string& sub, int& sub_pos);
void SolveExpression(string& expression, int& precision);

template<typename T>
void AddArrayValue(T*& array, T value, int& _size);

template<typename T>
void DeleteArrayValue(T*& array, int& _size, int _pos);

int main()
{
	setlocale(LC_ALL, "ru");

	std::string inputCommand; //������� ���������� �� �������
	std::string subExpression; //
	int sub_pos = -1;
	int precision = 3; //����� ���������� ������ ����� ������� ��� ������ ������
	bool isExit = false;

	GetInfo();

	do
	{
		inputCommand = "";
		getline(std::cin, inputCommand);

		if (inputCommand.find("-exit") != string::npos)
		{
			isExit = true;
		}
		else if (inputCommand.find("-help") != string::npos)
		{
			GetHelp();
		}
		else if (inputCommand.find("-clear") != string::npos)
		{
			ClearScreen();
			GetInfo();
		}
		else if (inputCommand.find("-f y(x)=") != string::npos)
		{
			std::cout << "\n������ ������� ������� [� ����������]" << std::endl;
		}
		else if (inputCommand == "")
		{
			continue;
		}
		else if (inputCommand.find("-p") != string::npos)
		{
			SetPrecision(inputCommand, precision);
		}
		else
		{
			

			do
			{
				FormattingExpression(inputCommand);
				FindSubExpression(inputCommand, subExpression, sub_pos);
				SolveExpression(subExpression, precision);


				if (sub_pos != 0)
				{
					inputCommand.erase(sub_pos - 1, inputCommand.find(')', sub_pos) - sub_pos + 2); //�������� ���������, ������������ ��� ��������
					inputCommand.insert(sub_pos - 1, subExpression);
				}

			} while (sub_pos != 0);
			
			std::cout << subExpression << std::endl;
		}
	} while (!isExit);

	return 0;
}

void ClearScreen()
{
	COORD topleft = { 0,0 };
	CONSOLE_SCREEN_BUFFER_INFO screen;
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(console, &screen);
	DWORD written, cells = screen.dwSize.X * screen.dwSize.Y;
	FillConsoleOutputCharacter(console, ' ', cells, topleft, &written);
	FillConsoleOutputAttribute(console, screen.wAttributes, cells, topleft, &written);
	SetConsoleCursorPosition(console, topleft);
}

void GetInfo()
{
	std::cout << "��� ��������� ������� ������� ������� -help" << std::endl;
}

void GetHelp()
{
	std::cout << std::endl << "��������� �������:\n"
		"-exit - ���������� ������\n"
		"-help - ������ ��������� ������ � ��������� ������� �� ������ ���������\n"
		"-�lear - �������� ���� �����/������\n"
		"-p [value] - ����� ���������� ��������� ������ ����� ������� \n\n"
		"��� ��������������� ������� ������� ���������. � ��������� ����������� ������������� �������� \"+\",\"-\",\"*\",\"/\"\n"
		"��� ��������� ������� ����� ����������� ������������� �������� \",\" � \".\"\n"
		"������� \"(\" � \"[\",\")\" � \"]\" �������� ������������� � ����������� ��� �������� ���������.\n"
		"��� ������������������ ������� �������� ��������� �������� ���������:\n"
		"sin(x), cos(x), tg(x), ctg(x)\n\n";
}

/*
������� ���������� �������� � ����������� ������
������� ����������� � ����� �������
*/
template<typename T>
void AddArrayValue(T*& array, T value, int &_size)
{
	_size++;

	T* tempArray = new T[_size];

	for (int i = 0; i < _size; i++)
	{
		tempArray[i] = array[i];
	}

	tempArray[_size - 1] = value;

	delete[] array;
	array = tempArray;
}

/*
������� �������� �������� ������� � �������� _pos
*/
template<typename T>
void DeleteArrayValue(T*& array, int& _size, int _pos)
{
	T* tempArray = new T[_size - 1];

	for (int i = 0; i < _size - 1; i++)
	{
		if (i < _pos)
		{
			tempArray[i] = array[i];
		}
		else
		{
			tempArray[i] = array[i + 1];
		}
	}

	_size--;

	delete[] array;
	array = tempArray;
}

/*
����� �������� ������������� ���������, ������������ � ������
����������� ���������������� ������������ ������ ������� �������� ��������� � ����������� �������
����������� ����� ��������� ����������� ������� � ����������� ����������� �������
*/
void FindSubExpression(string &expression, string &sub, int& sub_pos)
{
	int startIndex{ 0 };
	int endIndex{ 0 };

	/*
	���� ��� ����������� ������� ����������� ������ startIndex � ����������� endIndex
	*/
	for (int i = 0; i < expression.size(); i++)
	{
		if (expression[i] == '(')
		{
			startIndex = i + 1;
			endIndex = expression.find(')', startIndex);
		}
	}

	/*
	� ������, ���� endIndex (������� ����������� ������) ����� ����, �� � �������� ������������ ��������� ���� ���������
	*/
	if (endIndex == 0)
		sub = expression;
	else
		sub = expression.substr(startIndex, endIndex - startIndex);

	sub_pos = startIndex;
}

/* 
������������ ������ �������� ������� ��� �������� ���������� ��������, ���� ������ �� �� ������
���������� ������ � ������������ � ������ ������ ����
*/
void FormattingExpression(string &command)
{
	string tempExpression{ "" };
	string validLiteral{ "0123456789!sincostgctg()[].,+-*/" }; //������ ����������� ���������

	/*
	����, � ������� ���� ������ �������� ��������� ��������� ����������� �� ������������ ������ ���������
	� ������, ���� i-�� ������ ������������� ������, �� ������������ �� ��������� ������
	*/
	for (int i = 0; i < command.size(); i++)
	{
		if (validLiteral.find_first_of(command[i]) != string::npos)
		{
			tempExpression += command[i];
		}
	}

	/*
	���� ��� ���������� ������������ � ������ � ������ ������ ����
	*/
	for (int i = 0; i < tempExpression.size(); i++)
	{
		if (tempExpression[i] == 46) //change '.' to ','
			tempExpression[i] = 44;
		else if (tempExpression[i] == 91) //change '[' to '('
			tempExpression[i] = 40;
		else if (tempExpression[i] == 93) //change ']' to ')'
			tempExpression[i] = 41;
	}

	command = tempExpression;
}

/*
���������� ������������ � ������ �������� ��������� � ���������� �������� ������������
*/
void SolveExpression(string &expression, int &precision)
{
	string result{ "" };
	
	double* value = new double[0]; //������ ��� ������ ���� ��������� ������������
	int valueSize{ 0 };

	int* multipleIndex = new int[0]; //������ ��� ������ �������� ��������� � ��������� ���������
	int multipleSize{ 0 };

	string strTempValue{ "" };
	double tempValue{ 0 };

	expression.insert(0, 1, '+');

	/*
	������������ ������ ������������ ������ ������
	�������� �������� ������������ � ���������� string tempValue
	���� i-�� ������� ����� ������ �� ������ ��������, �� �������� ���������� � tempValue ������������ ��� ������� ������� value, tempValue ����������
	*/
	for (int i = expression.size() - 1; i >= 0; i--)
	{
		if (expression[i] >= 48 && expression[i] <= 57 || expression[i] == 44) // [0,..,9] or ','
		{
			strTempValue.insert(0, 1, expression[i]);
		}
		/*
		� ������ ����� �������� ��������� tempValue ������������ � ������ value
		������, ��� ������� �������� ���� �������� � ������, ������������ � ������ multipleIndex
		*/
		else if (expression[i] == 42 && strTempValue != "") // '*'
		{
			tempValue = stod(strTempValue);

			AddArrayValue(value, tempValue, valueSize);
			AddArrayValue(multipleIndex, valueSize - 1, multipleSize);

			tempValue = 0;
			strTempValue = "";

		}
		/*
		� ������ �������� ������� ������ �������� ����� ���������� � ������ multipleIndex
		�� ��� ������� ���������� � ������ value � ���� 1/tempValue
		*/
		else if (expression[i] == 47 && strTempValue != "") // '/'
		{
			tempValue = stod(strTempValue);
			tempValue = 1 / tempValue;

			AddArrayValue(value, tempValue, valueSize);
			AddArrayValue(multipleIndex, valueSize - 1, multipleSize);

			tempValue = 0;
			strTempValue = "";
		}
		else if (expression[i] == 43 && strTempValue != "") // '+'
		{
			tempValue = stod(strTempValue);

			AddArrayValue(value, tempValue, valueSize);

			tempValue = 0;
			strTempValue = "";
		}
		/*
		���� ����� ����� �������� ��������� ������� ����� ��� ����� �������, �� ���� ���������������� ��� �������� ���������
		���� ��������� ������� ������������ ���� ��������� ��� �������, �� ���� ��������� ���������������� ��� ����������� �������������� �����
		*/
		else if (expression[i] == 45 && strTempValue != "") // '-'
		{
			tempValue = stod(strTempValue);
			tempValue *= -1;

			if (i > 1 && expression[i - 1] == 42)
			{
				AddArrayValue(value, tempValue, valueSize);
				AddArrayValue(multipleIndex, valueSize - 1, multipleSize);
			}
			else if (i > 1 && expression[i - 1] == 47)
			{
				tempValue = 1 / tempValue;
				AddArrayValue(value, tempValue, valueSize);
				AddArrayValue(multipleIndex, valueSize - 1, multipleSize);
			}
			else
			{
				AddArrayValue(value, tempValue, valueSize);
			}

			tempValue = 0;
			strTempValue = "";
		}
	}

	/*
	������ ������� ������� multipleIndex ��������� �� ��������������� ������ �������� � ������� value, � �������� ������ ���� ��������� �������� ���������
	� ���������� ������ �������� � k-�� ������� ������� value �� ���������� ���-� ��������� k-��� �������� � k+1 ��������, 
	����� ���� ������� � �������� k+1 ��������� �� ������� value.
	*/
	for (int i = multipleSize - 1; i >= 0; i--)
	{
		value[multipleIndex[i]] = value[multipleIndex[i]] * value[multipleIndex[i] + 1];
		DeleteArrayValue(value, valueSize, multipleIndex[i] + 1);
	}

	/*
	����� ���� ��� �� ��������� ��� �������� ���������, ������� ������� ��� ������������� � ������������� �����
	�������� ����������� � ������� ������ ������ � ������� ������ ������� ������� ����� ��������� � ���� �������� ����� ������������
	*/
	for (int i = valueSize - 2; i >= 0; i--)
	{
		value[i] = value[i] + value[i + 1];
	}

	/*
	���������� �������� � ����� ��� � ������ �������������� ��������� ���������� ������ ����� �������
	*/
	std::ostringstream s;
	s << std::fixed << std::setprecision(precision) << value[0]; 
	expression = s.str();
	size_t end = expression.find_last_not_of('0') + 1;

	expression = expression.erase(end);

	/*
	���� ��������� ������� ������� ��������, ���������� �������� ������������, �������� ������, �� ����, ��������� �� ����� ������� ����� ����� ����� � �������� �����
	�� ������� ��������� ������� ������� ��������, �����
	*/
	if (expression[expression.size() - 1] == '.')
		expression.erase(expression.size() - 1);

	delete[] value;
	delete[] multipleIndex;
}

void SetPrecision(string& command, int &precision)
{
	string validLiteral = "1234567890";

	for (int i = command.size() - 1; i >= 0; i--)
	{
		if (validLiteral.find_first_of(command[i]) == string::npos)
		{
			command.erase(i, 1);
		}
	}

	precision = stoi(command);
}
