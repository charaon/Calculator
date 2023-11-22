// todo : разберись как разбивать файлы на .h и .cpp и погугли почему это важно
//        по сути ты уже разбил функции на объявление и реализацию, просто они в разных файлах будут

#include <iostream>
#include <string>
#include <windows.h>  // todo : в целом если можно без виндовских зависимостей делать, то делай, я просто у себя не смогу это запустить даже
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

	// todo : обычно do while не используют т.к. очень неудобно читать такие циклы из-за того
    //  что условие внизу и его не видно и прочие преколы
    //  в твоем случае я бы сделал просто while (true) и внутри если выдали команду экзит -
    //  я бы выходил через break (но это скорее вкусовщина )

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
            /* todo : такой момент - лучше вынести работу самого калькулятора, подготовки данных и сопроводительные действия
                по разным функциям чтобы не было такой колбасы ифов чтобы это все было примерно так:
                while (true) {
                    auto command = GetCommandFromUser();

                    if (IsExitCommand(command)) {
                        PrintExitMessage();
                        break;
                    }

                    if (IsNoCalculationCommand(command)) {
                        ProcessNoCalculationCommand(command); // вот тут обработали весь хелп, клеар сет пресижн и вышли
                        continue;
                    }

                    ValidateCalculationCommand(command);// тут отвалидировали, что все ок

                    auto calculation_command = PrepareCalculationCommand(command); // тут если нужно что-то изменили

                    auto result = SolveExpression(calculation_command); // тут нужно подумать что делать,
                                                                           если ошибка во время работы,
                                                                           возможно возвращать std::optional или пустую строку
                    PrintResult(result);
                }
                В таком случае у тебя мейн короткий и все логические вещи разнесены по функциям
                и все сходу понятно верхнеуровнево что происходит


                Я саму логику работы не смотрел, но по сути кажется, что чтобы сделать класс ничего не нужно:
                просто делаешь класс калькулятор
                у него поле precision и два публичных метода
                SolveExpression(std::string expression);
                и
                SetPrecision(int prec);

                и несколько приватных: тут уже на сколько фантазии хватит
                типо:
                GetCalculationTokens
                FindSubExpression();
                SolveSubExpression
                типо все что делает идейно какую-то одну часть логики можно упаковывать в отдельную функцию
            */

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
// еба, я такого даже не знаю
	COORD topleft = { 0,0 };
	CONSOLE_SCREEN_BUFFER_INFO screen;
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(console, &screen);
	DWORD written, cells = screen.dwSize.X * screen.dwSize.Y;
	FillConsoleOutputCharacter(console, ' ', cells, topleft, &written);
	FillConsoleOutputAttribute(console, screen.wAttributes, cells, topleft, &written);
	SetConsoleCursorPosition(console, topleft);
}

// todo: лучше давай названия функциям такие чтобы они описывали ровно то что они делают,
//  если это гет, значит она должна возвращать что-то , тут скорее принт ну и со всеми функциями так же
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

// todo: по сути все тоже самое что я написал к SetPrecision - можно просто проверять отдельно
//  и если пользователь ошибся, просто шли его нахер.
//  Еще момент - в данном случае это норм и правильно и все соответствует названию функции,
//  но если выносить это в класс - то я бы может переделал это так, чтобы на вход подавалась константная строка,
//  а внутри создавал новую форматированную и ее бы отдавал назад , просто чтобы было прозрачно и понятно:
//  вот нам пришли входные данные а вот мы их преобразовали в наш формат и это разные сущности а не все в одной строке
//  просто пока не пишешь каакой то ебанутый хайлоад, нет смысла экономить эти 10 байт и 100 наносекунд
//  т.к. если у тебя будет большой пайплайн преобразований входных данныхи
//  и везде по ссылке будешь работать с одним объектом, можно ошибиться и долго искать а на каком этапе говно
//  но это скорее такое ИМХО а не прям руководство к действию - типо тут все правильно сделал это я просто рассказываю
//  что можно не так сильно заебываться
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


// todo: вообще это прям сишный стиль - передавать по ссылке как аргумент и его менять,
//   тут тоже - обычно просто возвращают новое значение через ретурн
//   ну и тут тоже по неймингу: по сути у тебя функция сет пресижн, помимо выставления пресижна
//   еще валидирует входные данные и изменяет их
//   ну и идеологически я бы это разнес и сделал отдельно ValidateInputPrec() который просто проверял,
//   что все значения валидные и если нет просто падал бы или писал в консоль - "ОШИБКА иди нахер давай заново"
//   просто менять входные данные и додумывать за пользователя это место где можно ошибиться
//   и вообще странно, что программа сама за меня что-то додумывает что я сделать хотел -
//   может я просто головой на клаву упал и вбил sdkljfnal123jnfsd;f123, а мне выставят пресижн 123123
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
