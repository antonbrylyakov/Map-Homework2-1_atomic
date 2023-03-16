#include <iostream>
#ifdef _WIN32
#include "windows.h"
#endif

#include <thread>
#include <atomic>

// Вариант с использованием std::atomic
// Логика работы остается та же самая, что и при использовании мьютекса https://github.com/antonbrylyakov/Map-Homework1
// Но может быть нарушена последовательность вывода сообщений в консоль из разных потоков

std::atomic<int> count = 0;

void simulateClients(int clientCount)
{
	for (auto i = 0; i < clientCount; ++i)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		// https://en.cppreference.com/w/cpp/atomic/atomic/operator_arith
		auto currCnt = ++count;
		std::cout << "Количество клиентов: " << currCnt << " (один пришел)" << std::endl;
	}
}

void simulateOperators()
{
	static bool planningToGo = false;

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::seconds(2));
		auto currCnt = count.load();
		
		if (currCnt <= 0)
		{
			std::cout << "Нет ни одного клиента, операционист уходит" << std::endl;
			break;
		}
		else
		{
			// В данном случае такой подход применим, потому что мы знаем, что оператор только один.
			// Т.е. другой поток может только увеличить значение, но не уменьшить. Т.е. значение счетчика не станет меньше нуля.
			currCnt = --count;
			std::cout << "Количество клиентов: " << currCnt << " (один был обслужен)" << std::endl;
		}
	}
}


int main()
{
	setlocale(LC_ALL, "Russian");
#ifdef _WIN32
	SetConsoleCP(1251);
#endif

	auto clientCount = 10;
	std::cout << "Начало обслуживания клиентов, максимальное количество: " << clientCount << std::endl;

	std::thread clientThread(simulateClients, clientCount);
	std::thread operatorThread(simulateOperators);
	clientThread.join();
	operatorThread.join();
}