#include <iostream>
#include <fstream>
#include <cstring>

int main(void)
{
  std::cout << "Opening scores.dat for writing...\n";
  std::ofstream file("scores.dat", std::ios::out | std::ios::binary);
  std::cout << "Writing blank score data...\n";
  for (int i = 0; i < 15; i++)
  {
    file << "__________";
    file.write("\0\0\0\0", 4);
  }
  std::cout << "Closing file...\n";
  file.close();
  std::cout << "Score data were reset successfully.\n";
  return 0;
}
