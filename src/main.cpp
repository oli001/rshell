#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;

vector<string> tokenize(string x); //prototype to tokenize the commands
char *stringToCstring(string z); //converts a string to a cstring  

int main()
{
    string command;     //string to hold the user command
 
    bool restart = true; //keep looping until user enters exit
    while(restart)
    {
        cout << "$ ";       //prompt the user
        getline(cin, command);      //takes the entire line of prompts
        vector<string> inter = tokenize(command); //calls the tokenize function
	inter.push_back("\0");

        vector<char*> tokens;
        char* cmd;
        for(auto it = inter.begin(); it != inter.end(); ++it) //strings to char*
        {
            cmd = stringToCstring(*it);    
            tokens.push_back(cmd);   
        }
	char **command = &tokens[0];
	
	int counter = 0;
        for(unsigned i = 0; i < inter.size(); i++)
        {
            if((inter.at(i) == ";") || (inter.at(i) == "||") || 
                (inter.at(i) == "&&") || (inter.at(i) == "#") ||
		(inter.at(i) == "\0"))
            {
                pid_t pid = fork(); //creates child process
                int state = 0;

                if(pid < 0)
                {
                    perror("fork failed\n"); //if forking fails
                    exit(1);
                }

                else if(pid == 0)
                {
		    command[counter] = NULL;
                    int status = execvp(command[0], command);
		    
                    if(status < 0)
                    {
                        perror("execution failed\n"); //if command fails
                        exit(1);
                    }
                }
                
                else
                {
                    waitpid(-1, &state, 0);
                    if(inter.at(i) == "&&" && (state > 0)) 
			//when the command is and
                    {
                        break;
                    }
                    if(inter.at(i) == "||" && (state <= 0)) //using or
                    {
                        break;
                    }

		    if(inter.at(i) == "#")
		    {
			break;
		    }
                }

		for(int j = 0; j < counter; j++) 
			//makes sure commands don't run again
		{ 
		    command[j] = NULL;
		}
		counter = 0;
            }
                
            else if(inter.at(i) == "exit") //user wants to exit
            {
                exit(1);
        	break;   
            }
		
	    else //remake the command array
	    {
		command[counter] = new char[inter[i].size() + 1];
		copy(inter[i].begin(), inter[i].end(), command[counter]);
                command[counter][inter[i].size()] = NULL;
		counter++;
	    }
        }
    } 
    return 0;
}

vector<string> tokenize(string x)
{
    vector<string> tokenVector;
    split(tokenVector, x, is_any_of(" "), token_compress_on); 
    //searches for spaces
    return tokenVector;
}

char *stringToCstring(string z)
{
    char *cstr = new char[z.length() + 1];
    strcpy(cstr, z.c_str());
    return cstr;
} 
