#include <iostream>
#include <string>
#include <windows.h>
#include <sstream>
#include <iomanip>
#include <cmath>

void ClearScreen();
void GetInfo();
void GetHelp();

std::string FormattingExpression(std::string command);
std::string FindSubExpression(std::string expression, int& sub_pos);
std::string SolveExpression(std::string expression);

template<typename T>
void AddArrayValue(T*& array, T value, int& _size);

template<typename T>
void DeleteArrayValue(T*& array, int& _size, int _pos);

int main()
{
	setlocale(LC_ALL, "ru");

	std::string command;
	std::string sub;
	int sub_pos = -1;
	bool isExit = false;

	GetInfo();

	do
	{
		command = "";
		getline(std::cin, command);

		if (command.find("-exit") != std::string::npos)
		{
			isExit = true;
		}
		else if (command.find("-help") != std::string::npos)
		{
			GetHelp();
		}
		else if (command.find("-clear") != std::string::npos)
		{
			ClearScreen();
			GetInfo();
		}
		else if (command.find("-f y(x)=") != std::string::npos)
		{
			std::cout << "\nРазбор свойств функции [в разработке]" << std::endl;
		}
		else if (command == "")
		{
			continue;
		}
		else
		{
			

			do
			{
				command = FormattingExpression(command);
				sub = FindSubExpression(command, sub_pos);
				sub = SolveExpression(sub);


				if (sub_pos != 0)
				{
					command.erase(sub_pos - 1, command.find(')', sub_pos) - sub_pos + 2); //Удаление выражения, находящегося под скобками
					command.insert(sub_pos - 1, sub);
				}

			} while (sub_pos != 0);
			
			std::cout << sub << std::endl;
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
	std::cout << "Для получения справки введите команду -help" << std::endl;
}

void GetHelp()
{
	std::cout << "Доступные команды:\n"
		"-exit - завершение работы\n"
		"-help - список доступных команд и получение справки по работе программы\n"
		"-сlear - очищение окна ввода/вывода\n\n"
		"Для арифметического решения введите выражение. В выражении допускается использование символов \"+\",\"-\",\"*\",\"/\"\n"
		"в случае, если между двумя членами выражения не стоит знака оператора, автоматически применяется операция умножения.\n"
		"Для отделения целой части числа допускается использование символов \",\",\".\"\n"
		"Символы \"(\" и \"[\",\")\" и \"]\" являются равнозначными и допускаются оба варианта написания.\n"
		"Для тригонометрических функций доступны следующие варианты написания:\n"
		"sin(x), cos(x), tg(x), ctg(x)\n\n"
		"Для разбора функции введите команду -f y(x)=[function body]\n\n";
}

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

std::string FindSubExpression(std::string expression, int& sub_pos)
{
	int startIndex{ 0 };
	int endIndex{ 0 };

	std::string sub{ "" };

	for (int i = 0; i < expression.size(); i++)
	{
		if (expression[i] == '(')
		{
			startIndex = i + 1;
			endIndex = expression.find(')', startIndex);
		}
	}

	if (endIndex == 0)
		sub = expression;
	else
		sub = expression.substr(startIndex, endIndex - startIndex);

	sub_pos = startIndex;

	return sub;
}

std::string FormattingExpression(std::string command)
{
	std::string expression{ "" };
	std::string validLiteral{ "0123456789!sincostgctg()[].,+-*/" };

	for (int i = 0; i < command.size(); i++)
	{
		if (validLiteral.find_first_of(command[i]) != std::string::npos)
		{
			expression += command[i];
		}
	}

	for (int i = 0; i < expression.size(); i++)
	{
		if (expression[i] == 46) //change '.' to ','
			expression[i] = 44;
		else if (expression[i] == 91) //change '[' to '('
			expression[i] = 40;
		else if (expression[i] == 93) //change ']' to ')'
			expression[i] = 41; 
	}

	return expression;
}

std::string SolveExpression(std::string expression)
{
	std::string result{ "" };
	
	double* value = new double[0];
	int valueSize{ 0 };

	int* multipleIndex = new int[0];
	int multipleSize{ 0 };

	int* addIndex = new int[0];
	int addSize{ 0 };

	std::string strTempValue{ "" };
	double tempValue{ 0 };

	expression.insert(0, 1, '+');

	for (int i = expression.size() - 1; i >= 0; i--)
	{
		if (expression[i] >= 48 && expression[i] <= 57 || expression[i] == 44) // [0,..,9] or ','
		{
			strTempValue.insert(0, 1, expression[i]);
		}
		else if (expression[i] == 42 && strTempValue != "") // '*'
		{
			tempValue = stod(strTempValue);

			AddArrayValue(value, tempValue, valueSize);
			AddArrayValue(multipleIndex, valueSize - 1, multipleSize);

			tempValue = 0;
			strTempValue = "";

		}
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

	for (int i = multipleSize - 1; i >= 0; i--)
	{
		value[multipleIndex[i]] = value[multipleIndex[i]] * value[multipleIndex[i] + 1];
		DeleteArrayValue(value, valueSize, multipleIndex[i] + 1);
	}

	for (int i = valueSize - 2; i >= 0; i--)
	{
		value[i] = value[i] + value[i + 1];
	}

	std::ostringstream s;
	s << std::fixed << std::setprecision(6) << value[0]; // some magic from stackoverflow about double output without sci notation, example 10e38 etc
	result = s.str();
	size_t end = result.find_last_not_of('0') + 1;

	delete[] value;
	delete[] multipleIndex;
	delete[] addIndex;
	
	return result.erase(end);
}

//std::string FindFunction(std::string expression)
//{
//	if (expression.find("sin") != std::string::npos)
//	{
//
//	}
//}
