#include <iostream>
#include <string>
#include <vector>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;

vector<string> tokenize(string x); //prototype to tokenize the commands  

int main()
{
    string command;     //string to hold the user command

    cout << "$ ";       //prompt the user
    getline(cin, command);      //takes the entire line of prompts
    vector<string> tokens = tokenize(command); //calls the tokenize function 
    return 0;
}

vector<string> tokenize(string x)
{
    vector<string> tokenVector;
    split(tokenVector, x, is_any_of(" "), token_compress_on); //searches for spaces
    return tokenVector;
}
