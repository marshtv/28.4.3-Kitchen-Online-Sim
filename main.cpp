#include <iostream>
#include <cassert>
#include <cstdlib>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>

enum class Dish {
	PIZZA,
	SOUP,
	STEAK,
	SALAD,
	SUSHI
};

std::vector<Dish> orders;
std::vector<Dish> cooking;
std::vector<Dish> delivered;
std::mutex orders_mutex;
std::mutex cooking_mutex;
std::mutex delivered_mutex;

void Order() {
	orders_mutex.lock();
	std::srand(std::time(nullptr));
	int delay = std::rand() % 11;
	while (delay < 5)
		delay = std::rand() % 11;
	std::this_thread::sleep_for(std::chrono::seconds(delay));

	Dish dish;
	std::cout << "Order ";
	switch (std::rand() % 5) {
		case 0:
			std::cout << "Pizza";
			dish = Dish::PIZZA;
			break;
		case 1:
			std::cout << "Steak";
			dish = Dish::STEAK;
			break;
		case 2:
			std::cout << "Soup";
			dish = Dish::SOUP;
			break;
		case 3:
			std::cout << "Salad";
			dish = Dish::SALAD;
			break;
		case 4:
			std::cout << "Sushi";
			dish = Dish::SUSHI;
			break;
	}
	std::cout << " received." << std::endl;
	std::cout << "-----------------------------------" << std::endl;
	orders.push_back(dish);
	orders_mutex.unlock();
}

void Cook() {
	Dish dish = orders[0];

	cooking_mutex.lock();
	std::srand(std::time(nullptr));
	int delay = std::rand() % 16;
	while (delay < 5)
		delay = std::rand() % 16;
	std::this_thread::sleep_for(std::chrono::seconds(delay));

	orders_mutex.lock();
	switch (dish) {
		case Dish::PIZZA:
			std::cout << "Pizza";
			break;
		case Dish::STEAK:
			std::cout << "Steak";
			break;
		case Dish::SOUP:
			std::cout << "Soup";
			break;
		case Dish::SALAD:
			std::cout << "Salad";
			break;
		case Dish::SUSHI:
			std::cout << "Sushi";
			break;
	}
	for (int i = 0; i < orders.size() - 1; ++i) {
		Dish temp = orders[i];
		orders[i] = orders[i + 1];
		orders[i + 1] = temp;
	}
	orders.pop_back();

	cooking.push_back(dish);
	std::cout << " complete!" << std::endl;
	std::cout << "-----------------------------------" << std::endl;
	orders_mutex.unlock();
	cooking_mutex.unlock();
}
int deli_count = 0;

void Delivery() {
	delivered_mutex.lock();
	std::this_thread::sleep_for(std::chrono::seconds(30));
	cooking_mutex.lock();
	for (int i = 0; i < cooking.size(); ++i) {
		delivered.push_back(cooking[i]);
	}

	for (int i = 0; i < cooking.size(); ++i) {
		cooking.pop_back();
	}
	cooking_mutex.unlock();
	for (int i = 0; i < delivered.size(); ++i) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		deli_count++;
		std::cout << deli_count << " - ";
		switch (delivered[i]) {
			case Dish::PIZZA:
				std::cout << "Pizza";
				break;
			case Dish::STEAK:
				std::cout << "Steak";
				break;
			case Dish::SOUP:
				std::cout << "Soup";
				break;
			case Dish::SALAD:
				std::cout << "Salad";
				break;
			case Dish::SUSHI:
				std::cout << "Sushi";
				break;
		}
		std::cout << " delivered. " << std::endl;
		std::cout << "-----------------------------------" << std::endl;
	}
	for (int i = 0; i < delivered.size(); ++i) {
		delivered.pop_back();
	}
	delivered_mutex.unlock();
}

int main() {
	std::cout << "Restaurant starts its job!" << std::endl;
	while (deli_count < 10) {
		std::thread ordering(Order);
		if (!orders.empty()) {
			std::thread cooks(Cook);
			if (!cooking.empty()) {
				std::thread deli(Delivery);
				deli.join();
			}
			cooks.join();
		}
		ordering.join();
	}
	std::cout << "10 dishes delivered! Good by!" << std::endl;

	return 0;
}
