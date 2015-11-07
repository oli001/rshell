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
    string commander;     //string to hold the user command
 
    bool restart = true; //keep looping until user enters exit
    while(restart)
    {
        cout << "$ ";       //prompt the user
        getline(cin, commander);      //takes the entire line of prompts
        vector<string> inter = tokenize(commander); 
	//calls the tokenize function
	inter.push_back("\0");

        vector<char*> tokens;
        char* cmd;
        for(unsigned it = 0; it < inter.size(); it++) 
	//strings to char*
        {
            cmd = stringToCstring(inter.at(it));    
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
		return 0;
        	break;   
            }
		
	    else //remake the command array
	    {
		command[counter] = new char[inter[i].size() + 1];
		copy(inter[i].begin(), inter[i].end(), command[counter]);
                command[counter][inter[i].size()] = '\0';
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
    // seprates by spaces

    vector<string> tokenVectorAnd;
    string delimiter = "&&";
    size_t pos = 0;
    string oldString;
    string newString;
    for(unsigned i = 0; i < tokenVector.size(); i++)
    {
        oldString = tokenVector.at(i);
	if((pos = oldString.find(delimiter)) != string::npos
	    && oldString != delimiter)
	{ 
            while ((pos = oldString.find(delimiter)) != string::npos) 
            {
	        newString = oldString.substr(0, pos);
	        tokenVectorAnd.push_back(newString);
		tokenVectorAnd.push_back(delimiter);
	        oldString.erase(0, pos + delimiter.length());
            }
	    tokenVectorAnd.push_back(oldString);
	}

	else
	{
	    tokenVectorAnd.push_back(oldString);
	}
    }
    //separates by &&

    vector<string> tokenVectorOr;
    delimiter = "||";
    pos = 0;
    for(unsigned j = 0; j < tokenVectorAnd.size(); j++)
    {
        oldString = tokenVectorAnd.at(j);
	if((pos = oldString.find(delimiter)) != string::npos
	    && oldString != delimiter)
	{ 
            while ((pos = oldString.find(delimiter)) != string::npos) 
            {
	        newString = oldString.substr(0, pos);
	        tokenVectorOr.push_back(newString);
		tokenVectorOr.push_back(delimiter);
	        oldString.erase(0, pos + delimiter.length());
            }
	    tokenVectorOr.push_back(oldString);
	}

	else
	{
	    tokenVectorOr.push_back(oldString);
	}
    }
    //separates by ||

    vector<string> tokenVectorHash;
    delimiter = "#";
    pos = 0;
    for(unsigned k = 0; k < tokenVectorOr.size(); k++)
    {
        oldString = tokenVectorOr.at(k);
	if((pos = oldString.find(delimiter)) != string::npos
	    && oldString != delimiter)
	{ 
            while ((pos = oldString.find(delimiter)) != string::npos) 
            {
	        newString = oldString.substr(0, pos);
	        tokenVectorHash.push_back(newString);
		tokenVectorHash.push_back(delimiter);
	        oldString.erase(0, pos + delimiter.length());
            }
	    tokenVectorHash.push_back(oldString);
	}

	else
	{
	    tokenVectorHash.push_back(oldString);
	}
    }
    //separates by #
 
    vector<string> tokenVectorFinal;
    delimiter = ";";
    pos = 0;
    for(unsigned l = 0; l < tokenVectorHash.size(); l++)
    {
        oldString = tokenVectorHash.at(l);
	if((pos = oldString.find(delimiter)) != string::npos
	    && oldString != delimiter)
	{ 
            while ((pos = oldString.find(delimiter)) != string::npos) 
            {
	        newString = oldString.substr(0, pos);
	        tokenVectorFinal.push_back(newString);
		tokenVectorFinal.push_back(delimiter);
	        oldString.erase(0, pos + delimiter.length());
            }
	    tokenVectorFinal.push_back(oldString);
	}

	else
	{
	    tokenVectorFinal.push_back(oldString);
	}
    }
    //separates by ; 
    return tokenVectorFinal;
}

char *stringToCstring(string z) //makes a char* from a string
{
    char *cstr = new char[z.length() + 1];
    strcpy(cstr, z.c_str());
    return cstr;
} 
