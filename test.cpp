#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main() {
  vector<string> tokens;
  tokens.push_back("1");
  tokens.push_back("2");
  tokens.push_back("end");

  vector<string>::iterator tokitr = tokens.begin();

  while (tokitr != tokens.end()) {
    cout << *tokitr << endl;
    tokitr++;
  }

  tokitr--; // point to last element in tokens

  if (tokitr != tokens.end())
    if (*tokitr == "end")
      cout << "valid" << endl;
    else
      cout << "invalid" << endl;

  return 0;
}