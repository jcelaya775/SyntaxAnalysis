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
  // valid scanner output.  This data must be in the form
  // token : lexeme
  // post: the vectors have been populated

  bool prog();
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
    tok = line.substr(0, pos - 1);
    lex = line.substr(pos + 2, line.length());
    cout << pos << " " << tok << " : " << lex << endl;
    tokens.push_back(tok);
    lexemes.push_back(lex);
    getline_safe(infile, line);
  }

  tokitr = tokens.begin();
  lexitr = lexemes.begin();
}

// TO DO: SOME METHODS DO NOT CHECK FOR END OF LIST
bool SyntaxAnalyzer::prog() {
  if (vdec()) {
    if (tokitr != tokens.end() && *tokitr == "t_main") {
      tokitr++;
      lexitr++;
      if (stmtlist()) {
        if (tokitr != tokens.end()) { // should be at last token
          if (*tokitr == "t_end") {
            tokitr++;
            lexitr++;
            if (tokitr == tokens.end()) { // end was last thing in file
              cout << "Valid source code file" << endl;
              return true;
            } else {
              cout << "end came too late" << endl;
            }
          } else {
            cout << "invalid statement ending code" << endl;
          }
        } else {
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
  if (tokitr != tokens.end() && *tokitr == "t_var") {
    tokitr++;
    lexitr++;
  } else
    return false;

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

int SyntaxAnalyzer::vars() {
  if (tokitr == tokens.end())
    return 2;

  int result = 0; // 0 - valid, 1 - done, 2 - error
  if (*tokitr == "t_integer") {
    tokitr++;
    lexitr++;
  } else if (*tokitr == "t_string") {
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
  int result = stmt(); //  0 - invalid, 1 - valid, 2 - done

  while (result == 1) {
    result = stmt();
  }
  if (result == 0)
    return false;
  else
    return true;
}

int SyntaxAnalyzer::stmt() { // returns 1 or 2 if valid, 0 if invalid
  // TO DO: CHECK FOR END OF LIST
  if (tokitr != tokens.end() && *tokitr == "t_if") {
    tokitr++;
    lexitr++;
    if (ifstmt())
      return 1;
    else
      return 0;
  } else if (tokitr != tokens.end() && *tokitr == "t_while") {
    tokitr++;
    lexitr++;
    if (whilestmt())
      return 1;
    else
      return 0;
  } else if (tokitr != tokens.end() &&
             *tokitr == "t_id") { // assignment starts with identifier
    tokitr++;
    lexitr++;
    cout << "t_id" << endl;
    if (assignstmt())
      return 1;
    else
      return 0;
  } else if (tokitr != tokens.end() && *tokitr == "t_input") {
    tokitr++;
    lexitr++;
    if (inputstmt())
      return 1;
    else
      return 0;
  } else if (tokitr != tokens.end() && *tokitr == "t_output") {
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
  if (tokitr == tokens.end() || *tokitr != "s_lparen")
    return false;
  tokitr++;
  lexitr++;
  if (!expr())
    return false;
  tokitr++;
  lexitr++;
  if (tokitr == tokens.end() || *tokitr != "s_rparen")
    return false;
  tokitr++;
  lexitr++;
  if (tokitr == tokens.end() || *tokitr != "t_then")
    return false;
  if (!stmtlist())
    return false;
  if (!elsepart())
    return false;
  if (tokitr == tokens.end() || *tokitr != "t_end")
    return false;
  tokitr++;
  lexitr++;
  if (tokitr == tokens.end() || *tokitr != "t_if")
    return false;
  tokitr++;
  lexitr++;

  return true; // if passed all tests -> return true;
}

bool SyntaxAnalyzer::elsepart() {
  if (tokitr != tokens.end() && *tokitr == "t_else") {
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
  if (tokitr == tokens.end() || *tokitr != "t_while")
    return false;
  tokitr++;
  lexitr++;
  if (tokitr == tokens.end() || *tokitr != "s_lparen")
    return false;
  tokitr++;
  lexitr++;
  if (!expr())
    return false;
  if (tokitr == tokens.end() || *tokitr != "s_rparen")
    return false;
  tokitr++;
  lexitr++;
  if (tokitr == tokens.end() || *tokitr != "t_loop")
    return false;
  tokitr++;
  lexitr++;
  if (!stmtlist())
    return false;
  if (tokitr == tokens.end() || *tokitr != "t_end")
    return false;
  tokitr++;
  lexitr++;
  if (tokitr == tokens.end() || *tokitr != "t_loop")
    return false;
  tokitr++;
  lexitr++;

  return true;
}

bool SyntaxAnalyzer::assignstmt() {
  if (tokitr == tokens.end() || *tokitr != "t_id")
    return false;
  tokitr++;
  lexitr++;
  if (tokitr == tokens.end() || *tokitr != "s_assign")
    return false;
  tokitr++;
  lexitr++;
  if (!expr())
    return false;
  if (tokitr == tokens.end() || *tokitr != "s_semi")
    return false;
  tokitr++;
  lexitr++;

  return true;
  // write this function
}

bool SyntaxAnalyzer::inputstmt() {
  if (tokitr != tokens.end() && *tokitr == "s_lparen") {
    tokitr++;
    lexitr++;
    if (tokitr != tokens.end() && *tokitr == "t_id") {
      tokitr++;
      lexitr++;
      if (tokitr != tokens.end() && *tokitr == "s_rparen") {
        tokitr++;
        lexitr++;
        return true;
      }
    }
  }
  return false;
}

bool SyntaxAnalyzer::outputstmt() {
  if (tokitr == tokens.end() || *tokitr != "t_output")
    return false;
  tokitr++;
  lexitr++;
  if (tokitr == tokens.end() || *tokitr != "s_lparen")
    return false;
  tokitr++;
  lexitr++;
  if (*tokitr != "t_string" && !expr())
    return false;
  if (tokitr == tokens.end() || *tokitr != "s_rparen")
    return false;
  tokitr++;
  lexitr++;

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
  if (tokitr != tokens.end() && !term()) {
    if (arithop() || relop()) {
      if (term())
        return true;
      else
        return false;
    } else
      return true;
  } else
    return false;
}

bool SyntaxAnalyzer::term() {
  if (tokitr != tokens.end() &&
      ((*tokitr == "t_int") || (*tokitr == "t_str") || (*tokitr == "t_id"))) {
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
  if (tokitr != tokens.end() && ((*tokitr == "s_and") || (*tokitr == "s_or"))) {
    tokitr++;
    lexitr++;
    return true;
  } else
    return false;
}

bool SyntaxAnalyzer::arithop() {
  if (tokitr != tokens.end() &&
      ((*tokitr == "s_mult") || (*tokitr == "s_plus") ||
       (*tokitr == "s_minus") || (*tokitr == "s_div") ||
       (*tokitr == "s_mod"))) {
    tokitr++;
    lexitr++;
    return true;
  } else
    return false;
}

bool SyntaxAnalyzer::relop() {
  if (tokitr != tokens.end() &&
      ((*tokitr == "s_lt") || (*tokitr == "s_gt") || (*tokitr == "s_ge") ||
       (*tokitr == "s_eq") || (*tokitr == "s_ne") || (*tokitr == "s_le"))) {
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
  sa.prog();

  return 1;
}