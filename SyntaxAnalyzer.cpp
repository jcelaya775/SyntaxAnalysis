#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

class SyntaxAnalyzer {
private:
  vector<string> lexemes;
  vector<string> tokens;
  vector<string>::iterator lexitr;
  vector<string>::iterator tokitr;

  // other private methods
  bool vdec();
  int vars();
  bool stmtlist();
  int stmt();
  bool ifstmt();
  bool elsepart();
  bool whilestmt();
  bool assignstmt();
  bool inputstmt();
  bool outputstmt();
  bool expr();
  bool simpleexpr();
  bool term();
  bool logicop();
  bool arithop();
  bool relop();
  std::istream &getline_safe(std::istream &input, std::string &output);

public:
  SyntaxAnalyzer(istream &infile);
  // pre: 1st parameter consists of an open file containing a source code's
  //	valid scanner output.  This data must be in the form
  //			token : lexeme
  // post: the vectors have been populated

  bool parse();
  // pre: none
  // post: The lexemes/tokens have been parsed.
  // If an error occurs, a message prints indicating the token/lexeme pair
  // that caused the error.  If no error, data is loaded in vectors
};

SyntaxAnalyzer::SyntaxAnalyzer(istream &infile) {
  string line, tok, lex;
  int pos;
  getline_safe(infile, line);
  bool valid = true;
  while (!infile.eof() && (valid)) {
    pos = line.find(":");
    tok = line.substr(0, pos);
    lex = line.substr(pos + 1, line.length());
    cout << pos << " " << tok << " " << lex << endl;
    tokens.push_back(tok);
    lexemes.push_back(lex);
    getline_safe(infile, line);
  }
  tokitr = tokens.begin();
  lexitr = lexemes.begin();
}

bool SyntaxAnalyzer::parse() { // checks for valid PROG
  if (vdec()) {
    if (tokitr != tokens.end() && *tokitr == "t_main") {
      tokitr++;
      lexitr++;
      if (tokitr != tokens.end() && stmtlist()) {
        if (tokitr != tokens.end()) // should be at end token
          if (*tokitr == "t_end") {
            tokitr++;
            lexitr++;
            if (tokitr == tokens.end()) { // end was last thing in file
              cout << "Valid source code file" << endl;
              return true;
            } else {
              cout << "end came too early" << endl;
            }
          } else {
            cout << "invalid statement ending code" << endl;
          }
        else {
          cout << "no end" << endl;
        }
      } else {
        cout << "bad/no stmtlist" << endl;
      }
    } else {
      cout << "no main" << endl;
    }
  } else {
    cout << "bad var list" << endl;
  }
  return false;
}

bool SyntaxAnalyzer::vdec() {
  // may contian errors

  if (*tokitr != "t_var")
    return true;
  else {
    tokitr++;
    lexitr++;
    int result = 0; // 0 - valid, 1 - done, 2 - error
    result = vars();
    if (result == 2)
      return false;
    while (result == 0) {
      if (tokitr != tokens.end())
        result = vars(); // parse vars
    }

    if (result == 1)
      return true;
    else
      return false;
  }
}

int SyntaxAnalyzer::vars() {
  int result = 0; // 0 - valid, 1 - done, 2 - error
  string temp;
  if (*tokitr == "t_integer") {
    temp = "t_integer";
    tokitr++;
    lexitr++;
  } else if (*tokitr == "t_string") {
    temp = "t_string";
    tokitr++;
    lexitr++;
  } else
    return 1;

  bool semihit = false;
  while (tokitr != tokens.end() && result == 0 && !semihit) {
    if (*tokitr == "t_id") {
      tokitr++;
      lexitr++;
      if (tokitr != tokens.end() && *tokitr == "s_comma") {
        tokitr++;
        lexitr++;
      } else if (tokitr != tokens.end() && *tokitr == "s_semi") {
        semihit = true;
        tokitr++;
        lexitr++;
      } else
        result = 2;
    } else {
      result = 2;
    }
  }

  return result;
}

bool SyntaxAnalyzer::stmtlist() {
  int result = stmt();

  while (result == 1) {
    result = stmt();
  }
  if (result == 0)
    return false;
  else
    return true;
}
int SyntaxAnalyzer::stmt() { // returns 1 or 2 if valid, 0 if invalid
  if (*tokitr == "t_if") {
    tokitr++;
    lexitr++;
    if (ifstmt())
      return 1;
    else
      return 0;
  } else if (*tokitr == "t_while") {
    tokitr++;
    lexitr++;
    if (whilestmt())
      return 1;
    else
      return 0;
  } else if (*tokitr == "t_id") { // assignment starts with identifier
    tokitr++;
    lexitr++;
    cout << "t_id" << endl;
    if (assignstmt())
      return 1;
    else
      return 0;
  } else if (*tokitr == "t_input") {
    tokitr++;
    lexitr++;
    if (inputstmt())
      return 1;
    else
      return 0;
  } else if (*tokitr == "t_output") {
    tokitr++;
    lexitr++;
    cout << "t_output" << endl;
    if (outputstmt())
      return 1;
    else
      return 0;
  }
  return 2; // stmtlist can be null
}

bool SyntaxAnalyzer::ifstmt() {
  return true;
  // we will write this together in class
}

bool SyntaxAnalyzer::elsepart() {
  if (*tokitr == "t_else") {
    tokitr++;
    lexitr++;
    if (stmtlist())
      return true;
    else
      return false;
  }
  return true; // elsepart can be null
}

bool SyntaxAnalyzer::whilestmt() {
  return true;
  // write this function
}

bool SyntaxAnalyzer::assignstmt() {
  return true;
  // write this function
}
bool SyntaxAnalyzer::inputstmt() {
  if (*tokitr == "s_lparen") {
    tokitr++;
    lexitr++;
    if (*tokitr == "t_id") {
      tokitr++;
      lexitr++;
      if (*tokitr == "s_rparen") {
        tokitr++;
        lexitr++;
        return true;
      }
    }
  }
  return false;
}

bool SyntaxAnalyzer::outputstmt() {
  return true;
  // write this function
}

bool SyntaxAnalyzer::expr() {
  if (simpleexpr()) {
    if (logicop()) {
      if (simpleexpr())
        return true;
      else
        return false;
    } else
      return true;
  } else {
    return false;
  }
}

bool SyntaxAnalyzer::simpleexpr() {
  return true;
  // write this function
}

bool SyntaxAnalyzer::term() {
  if ((*tokitr == "t_int") || (*tokitr == "t_str") || (*tokitr == "t_id")) {
    tokitr++;
    lexitr++;
    return true;
  } else if (*tokitr == "s_lparen") {
    tokitr++;
    lexitr++;
    if (expr())
      if (*tokitr == "s_rparen") {
        tokitr++;
        lexitr++;
        return true;
      }
  }
  return false;
}

bool SyntaxAnalyzer::logicop() {
  if ((*tokitr == "s_and") || (*tokitr == "s_or")) {
    tokitr++;
    lexitr++;
    return true;
  } else
    return false;
}

bool SyntaxAnalyzer::arithop() {
  if ((*tokitr == "s_mult") || (*tokitr == "s_plus") ||
      (*tokitr == "s_minus") || (*tokitr == "s_div") || (*tokitr == "s_mod")) {
    tokitr++;
    lexitr++;
    return true;
  } else
    return false;
}

bool SyntaxAnalyzer::relop() {
  if ((*tokitr == "s_lt") || (*tokitr == "s_gt") || (*tokitr == "s_ge") ||
      (*tokitr == "s_eq") || (*tokitr == "s_ne") || (*tokitr == "s_le")) {
    tokitr++;
    lexitr++;
    return true;
  } else
    return false;
}
std::istream &SyntaxAnalyzer::getline_safe(std::istream &input,
                                           std::string &output) {
  char c;
  output.clear();

  input.get(c);
  while (input && c != '\n') {
    if (c != '\r' || input.peek() != '\n') {
      output += c;
    }
    input.get(c);
  }

  return input;
}

int main() {
  ifstream infile("codelexemes.txt");
  if (!infile) {
    cout << "error opening lexemes.txt file" << endl;
    exit(-1);
  }
  SyntaxAnalyzer sa(infile);
  sa.parse();
  return 1;
}