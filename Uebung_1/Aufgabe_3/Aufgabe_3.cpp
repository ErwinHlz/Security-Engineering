//
// Created by PC on 17.04.2025.
//
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <list>
#include <algorithm>
#include <unordered_set>




int main() {
 std::srand(static_cast<unsigned int>(std::time(nullptr)));
 std::unordered_set<int> generatedNumbers;
 int min = 1;
int max = 1000;
 int x = 2;
 int randomNumber;
 int randomNumber2;
 int iter_a = 0;
 int iter_b = 0;

while(randomNumber != x) {
 randomNumber = min + std::rand() % (max - min);
 iter_a++;
}

 while(randomNumber2 != x) {
  randomNumber2 = min + std::rand() % (max - min);
  iter_b++;
  generatedNumbers.insert(randomNumber2);




  if (generatedNumbers.count(randomNumber)) {
   std::cout << "Kollision:" << randomNumber  << std::endl;
   break;
  }


 }

std::cout << "Zufallszahl:" << randomNumber << std::endl;
 std::cout << "Iterationen:" << iter_a << std::endl;
  std::cout << "Iterationen bis Kollision:" << iter_b << std::endl;
 std::cout << "alle generierten Numbers:\n";
 for (const int& value : generatedNumbers) {
  std::cout << value << " ";
 }
 std::cout << std::endl;

return 0;
}