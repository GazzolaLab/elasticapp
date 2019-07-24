#include <iostream>
#include <string>
#include <vector>

#include "tbb/parallel_for.h"

int main(int argc, char* argv[]) {
  std::string a("Hello World!");
  std::cout << a << std::endl;
  return 0;
}
