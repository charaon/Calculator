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

	std::string inputCommand; //Команда получаемая из консоли
	std::string subExpression; //
	int sub_pos = -1;
	int precision = 3; //Задаёт количество знаков после запятой для вывода ответа
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


				if (sub_pos != 0)
				{
					inputCommand.erase(sub_pos - 1, inputCommand.find(')', sub_pos) - sub_pos + 2); //Удаление выражения, находящегося под скобками
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
	std::cout << "Для получения справки введите команду -help" << std::endl;
}

void GetHelp()
{
	std::cout << std::endl << "Доступные команды:\n"
		"-exit - завершение работы\n"
		"-help - список доступных команд и получение справки по работе программы\n"
		"-сlear - очищение окна ввода/вывода\n"
		"-p [value] - задаёт количество выводимых знаков после запятой \n\n"
		"Для арифметического решения введите выражение. В выражении допускается использование символов \"+\",\"-\",\"*\",\"/\"\n"
		"Для отделения дробной части допускается использование символов \",\" и \".\"\n"
		"Символы \"(\" и \"[\",\")\" и \"]\" являются равнозначными и допускаются оба варианта написания.\n"
		"Для тригонометрических функций доступны следующие варианты написания:\n"
		"sin(x), cos(x), tg(x), ctg(x)\n\n";
}

/*
Функция добавления элемента в сущестующий массив
Элемент добавляется в конец массива
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
*/
void FindSubExpression(string &expression, string &sub, int& sub_pos)
{
	int startIndex{ 0 };
	int endIndex{ 0 };

	/*
	Цикл для определения позиции открывающей скобки startIndex и закрывающей endIndex
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
*/
void FormattingExpression(string &command)
{
	string tempExpression{ "" };
	string validLiteral{ "0123456789!sincostgctg()[].,+-*/" }; //Список разрешённых литералов

	/*
	Цикл, в котором весь массив символов введённого выражения проверятеся на соответствие списку литералов
	В случае, если i-ый символ соответствует списку, он записывается во временную строку
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
*/
void SolveExpression(string &expression, int &precision)
{
	string result{ "" };
	
	double* value = new double[0]; //Массив для записи всех элементов подвыражения
	int valueSize{ 0 };

	int* multipleIndex = new int[0]; //Массив для записи индексов элементов с операцией умножение
	int multipleSize{ 0 };

	string strTempValue{ "" };
	double tempValue{ 0 };

	expression.insert(0, 1, '+');

	/*
	Посимвольный разбор подвыражения справа налево
	Числовые значения записываются в переменную string tempValue
	Если i-ый элемент равен одному из знаков операции, то значение хранящееся в tempValue записывается как элемент массива value, tempValue обнуляется
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
	*/
	for (int i = multipleSize - 1; i >= 0; i--)
	{
		value[multipleIndex[i]] = value[multipleIndex[i]] * value[multipleIndex[i] + 1];
		DeleteArrayValue(value, valueSize, multipleIndex[i] + 1);
	}

	/*
	После того как мы выполнили все операции умножения, остаётся сложить все положительные и отрицательные числа
	Сложение выполняется в массиве справа налево и поэтому первый элемент массива будет содержать в себе значение всего подвыражения
	*/
	for (int i = valueSize - 2; i >= 0; i--)
	{
		value[i] = value[i] + value[i + 1];
	}

	/*
	Приведение значения в общий вид с учётом установленного параметра количества знаков после запятой
	*/
	std::ostringstream s;
	s << std::fixed << std::setprecision(precision) << value[0]; 
	expression = s.str();
	size_t end = expression.find_last_not_of('0') + 1;

	expression = expression.erase(end);

	/*
	Если последний элемент массива символов, содержащий значение подвыражения, является точкой, то есть, выражение не имеет дробной части после точки и является целым
	То удаляем последний элемент массива символов, точку
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
