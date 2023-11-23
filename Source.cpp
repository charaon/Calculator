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

	std::string inputCommand; //Command recieved in console
	std::string subExpression; //
	int sub_pos = -1;
	int precision = 3; //Set the number of signs after dot
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
			std::cout << "\nРазбор свойств функции [в разработке]" << std::endl;
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

				/*
				Замена подвыражения на значение этого выражения
				 
				Replace the subexpression to value of that expression
				*/
				if (sub_pos != 0)
				{
					inputCommand.erase(sub_pos - 1, inputCommand.find(')', sub_pos) - sub_pos + 2);
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
	std::cout << "To get help, enter the command -help" << std::endl;
}

void GetHelp()
{
	std::cout << std::endl << "Available commands:\n"
		"-exit - close program\n"
		"-help - get list of valid commands\n"
		"-сlear - clear the input-output window\n"
		"-p [value] - set the number of decimal places\n\n"
		"For an arithmetic solution, enter an expression. The use of symbols is allowed in the expression \"+\",\"-\",\"*\",\"/\"\n"
		"To separate the fractional part, the use of symbols is allowed \",\" и \".\"\n"
		"Symbols \"(\" и \"[\",\")\" и \"]\" are equivalent and both spellings are allowed.\n"
		"The following spellings are available for trigonometric functions:\n"
		"sin(x), cos(x), tg(x), ctg(x)\n\n";
}

/*
Функция добавления элемента в сущестующий массив
Элемент добавляется в конец массива

Function for adding an element to an existing array
The element is added to the end of the array
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
Функция удаления элемента массива с индексом _pos

Function for deleting an array element with index _pos
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
Поиск наиболее приоритетного выражения, заключенного в скобки
Выполняется последовательный посимвольный разбор массива символов выражения и вычленяется область
заключённая между последней открывающей скобкой и последующей закрывающей скобкой

Search for the highest priority expression enclosed in parentheses
A sequential character-by-character parsing of the array of characters of the expression is performed and the area
enclosed between the last opening parenthesis and the subsequent closing parenthesis is isolated
*/
void FindSubExpression(string &expression, string &sub, int& sub_pos)
{
	int startIndex{ 0 };
	int endIndex{ 0 };

	/*
	Цикл для определения позиции открывающей скобки startIndex и закрывающей endIndex

	A loop for determining the position of the opening parenthesis startIndex and the closing endIndex
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
	В случае, если endIndex (позиция закрывающей скобки) равен нулю, то в качестве подвыражения передаётся само выражение

	If the endIndex (the position of the closing parenthesis) is zero, then self-expression is passed as a subexpression
	*/
	if (endIndex == 0)
		sub = expression;
	else
		sub = expression.substr(startIndex, endIndex - startIndex);

	sub_pos = startIndex;
}

/* 
Посимвольный разбор введённой команды для удаления нечитаемых символов, либо замены их на другие
Приведение скобок и разделителей к одному общему виду

Character-by-character parsing of the entered command to delete unreadable characters, or replace them with others
Bringing brackets and separators to one common form
*/
void FormattingExpression(string &command)
{
	string tempExpression{ "" };
	string validLiteral{ "0123456789!sincostgctg()[].,+-*/" }; //List of allowed literals

	/*
	Цикл, в котором весь массив символов введённого выражения проверятеся на соответствие списку литералов
	В случае, если i-ый символ соответствует списку, он записывается во временную строку

	A loop in which the entire array of characters of the entered expression is checked for compliance with the list of literals
	If the ith character matches the list, it is written to a temporary string
	*/
	for (int i = 0; i < command.size(); i++)
	{
		if (validLiteral.find_first_of(command[i]) != string::npos)
		{
			tempExpression += command[i];
		}
	}

	/*
	Цикл для приведения разделителей и скобок к одному общему виду

	A loop for bringing separators and brackets to one common form
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
Приведение подвыражения в массив числовых элементов и нахождение значения подвыражения

Bringing a subexpression into an array of numeric elements and finding the value of the subexpression
*/
void SolveExpression(string &expression, int &precision)
{
	string result{ "" };
	
	double* value = new double[0]; //Array for recording all subexpression elements
	int valueSize{ 0 };

	int* multipleIndex = new int[0]; //Array for writing indexes of elements with multiplication operation
	int multipleSize{ 0 };

	string strTempValue{ "" };
	double tempValue{ 0 };

	expression.insert(0, 1, '+');

	/*
	Посимвольный разбор подвыражения справа налево
	Числовые значения записываются в переменную string tempValue
	Если i-ый элемент равен одному из знаков операции, то значение хранящееся в tempValue записывается как элемент массива value, tempValue обнуляется

	Character-by-character parsing of a subexpression from right to left
	Numeric values are written to the string temp Value variable
	If the i-th element is equal to one of the operation signs, then the value stored in temp Value is written as an element of the value array, tempValue is reset
	*/
	for (int i = expression.size() - 1; i >= 0; i--)
	{
		if (expression[i] >= 48 && expression[i] <= 57 || expression[i] == 44) // [0,..,9] or ','
		{
			strTempValue.insert(0, 1, expression[i]);
		}
		/*
		В случае знака операции умножения tempValue записывается в массив value
		Индекс, под которым значение было помещено в массив, записывается в массив multipleIndex

		In the case of the sign of the multiplication operation, temp Value is written to the value array
		The index under which the value was placed in the array is written to the multiple Index array
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
		В случае операции деления индекс элемента также помещается в массив multipleIndex
		Но сам элемент помещается в массив value в виде 1/tempValue

		In the case of a division operation, the index of the element is also placed in the array multiple Index
		But the element itself is placed in the value array in the form of 1/temp Value
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
		Если после знака операции вычитания следует число или конец массива, то знак интерпретируется как операция вычитания
		Если следующий элемент представляет знак умножения или деления, то знак вычитания интерпретируется как обозначение отрицательного числа

		If the sign of the subtraction operation is followed by a number or the end of the array, then the sign is interpreted as a subtraction operation
		If the next element represents a multiplication or division sign, then the subtraction sign is interpreted as denoting a negative number
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
	Каждый элемент массива multipleIndex указывает на соответствующий индекс элемента в массиве value, к которому должна быть применена операция умножения
	В результате каждой итерации в k-ый элемент массива value мы записываем рез-т умножения k-ого элемента и k+1 элемента, 
	после чего элемент с индексом k+1 удаляется из массива value.

	Each element of the multiple Index array points to the corresponding index of the element in the value array to which the multiplication operation should be applied
	As a result of each iteration, in the kth element of the value array, we write the multiplication of the kth element and the k+1 element,
	after which the element with index k+1 is removed from the value array.
	*/
	for (int i = multipleSize - 1; i >= 0; i--)
	{
		value[multipleIndex[i]] = value[multipleIndex[i]] * value[multipleIndex[i] + 1];
		DeleteArrayValue(value, valueSize, multipleIndex[i] + 1);
	}

	/*
	После того как мы выполнили все операции умножения, остаётся сложить все положительные и отрицательные числа
	Сложение выполняется в массиве справа налево и поэтому первый элемент массива будет содержать в себе значение всего подвыражения

	After we have performed all the multiplication operations, it remains to add up all the positive and negative numbers
	Addition is performed in an array from right to left and therefore the first element of the array will contain the value of the entire subexpression
	*/
	for (int i = valueSize - 2; i >= 0; i--)
	{
		value[i] = value[i] + value[i + 1];
	}

	/*
	Приведение значения в общий вид с учётом установленного параметра количества знаков после запятой

	Bringing the value into a general view, taking into account the set parameter of the number of decimal places
	*/
	std::ostringstream s;
	s << std::fixed << std::setprecision(precision) << value[0]; 
	expression = s.str();
	size_t end = expression.find_last_not_of('0') + 1;

	expression = expression.erase(end);

	/*
	Если последний элемент массива символов, содержащий значение подвыражения, является точкой, то есть, выражение не имеет дробной части после точки и является целым
	То удаляем последний элемент массива символов, точку

	If the last element of the character array containing the value of the subexpression is a dot, that is, the expression does not have a fractional part after the dot and is an integer
	, then we delete the last element of the character array, the dot
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


