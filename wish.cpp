#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <utility>
#include <sys/wait.h>

#include <fcntl.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h> 

#include <string>
#include <string.h>

#include <fstream>

using namespace std;

pair<string, vector<string>> makepairs(vector<string> &ref);
string char_tostring(char *text, int size);
bool containsAND(vector<string> &ref);
int amountofANDS(vector<string> &ref);

int main(int argc, char *argv[])
{   
    string auto_path = "/bin/";
    string auto_path2 = "/usr/bin/";
    string commands;
    string temp;
    string input;
    if (argc == 2)
    {
        fstream command_file(argv[1]);
        if (command_file.fail())
        {
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit (1);
        }
        //reads in single line
        while (getline(command_file, temp, '\n'))
        {   
            stringstream command_stream(temp);
            string intermediate;
            vector<string> tokens;
            pair<string, vector<string>> pair1;
            //SINGLE COMMAND WITH ARGUMENT
            while (getline(command_stream, intermediate, ' '))
            {
                //adding words into vector
                tokens.push_back(intermediate);
            }
            pair1 = makepairs(tokens);
            if (containsAND(pair1.second))
            {
                if (amountofANDS(pair1.second) == 1)
                {
                    continue;
                }
            }

            //searching whether the commands are built in commands
            if ((pair1.first == "cd") || (pair1.first == "path") || (pair1.first == "exit"))
            {
                if (pair1.first == "exit")
                {
                    if (pair1.second.size() > 1)
                    {
                        char error_message[30] = "An error has occurred\n";
                        write(STDERR_FILENO, error_message, strlen(error_message));
                    }
                    else
                    {
                        exit(0);
                    }
                }
                else if (pair1.first == "cd")
                {
                    pair1.second.erase(pair1.second.begin());
                    if (pair1.second.size() == 0 || pair1.second.size() > 1)
                    {
                        char error_message[30] = "An error has occurred\n";
                        write(STDERR_FILENO, error_message, strlen(error_message));
                    }
                    else
                    {
                        //const char *path = pointer to a constant char
                        string temp;
                        for (unsigned int i = 0; i < pair1.second.size(); i++)
                        {
                            temp += pair1.second[i];
                        }
                        char *cstr = new char[temp.length() + 1];
                        strcpy(cstr, temp.c_str());
                        chdir(cstr);
                    }
                }
                else if (pair1.first == "path")
                {
                    pair1.second.erase(pair1.second.begin());
                    if (pair1.second.size() >= 1)
                    {
                        char buffer[100];
                        if (getcwd(buffer, sizeof(buffer)) != NULL)
                        {
                            string temp(buffer);
                            temp += "/" + pair1.second[0] + "/";
                            auto_path = temp;
                            auto_path2 = temp;
                        }
                    }
                    else
                    {
                        auto_path2 = auto_path = string();
                    }
                }
            }
            else if (pair1.first.empty())
            {
                vector<string> temp;
                string file_name{auto_path + pair1.first};
                string file_name2{auto_path2 + pair1.first};
                for (unsigned int i = 0; i < pair1.second.size(); i++)
                {
                    if (pair1.second[i]!= "")
                    {
                        temp.push_back(pair1.second[i]);
                    }
                }
                if (temp.size() != 0)
                {   
                    vector<char *> c_arguments;
                    file_name = auto_path + temp[0];
                    file_name2 = auto_path2 + temp[0];
                    pair1.second = temp;
                    for (auto c_container = pair1.second.begin(); c_container != pair1.second.end(); c_container++)
                    {
                        c_arguments.push_back(&(*c_container)[0]);//stackoverflow support
                    }
                    c_arguments.push_back(NULL);
                    execv(file_name.c_str(), &c_arguments[0]);
                }
            }
            else
            {   
                if (auto_path.empty())
                {
                    char error_message[30] = "An error has occurred\n";
                    write(STDERR_FILENO, error_message, strlen(error_message));
                }
                else
                {   
                    int rc = fork();

                    if (rc == 0)
                    {   
                        int counter = 0;
                        int check = 0;
                        int check2 = 0;
                        int counter4 = 0;
                        int magic_num = 0;
                        string new_path ="";
                        string new_path2 = "";
                        string filename_path = "";
                        string filename_path2 = "";
                        string path_name = "";
                        string real_path = "";
                        string file_name{auto_path + pair1.first};
                        string file_name2{auto_path2 + pair1.first};

                        if (access(file_name.c_str(), X_OK) != 0)
                        {
                            if (access(file_name2.c_str(), X_OK) != 0)
                            {
                                char error_message[30] = "An error has occurred\n";
                                write(STDERR_FILENO, error_message, strlen(error_message));
                            }
                            // the first try didn't do it, use the alternative
                            file_name = file_name2;
                        }
                        for (unsigned int i = 1; i < pair1.second.size(); i++)
                        {
                            for (unsigned int j = 0; j < pair1.second[i].length(); j++)
                            {
                                if (pair1.second[i][j] != '>')
                                {
                                    new_path += pair1.second[i][j];
                                    path_name +=  pair1.second[i][j];
                                }
                                if (pair1.second[i][j] == '>')
                                {   
                                    if (magic_num == 0)
                                    {
                                        real_path = path_name;
                                    }
                                    magic_num++;
                                    new_path2 += new_path;
                                    for (unsigned int k = j+1; k < pair1.second[i].length(); k++)
                                    {  
                                        filename_path += pair1.second[i][k];
                                    }
                                    counter4++;
                                    if (counter4 == 1)
                                    {
                                        filename_path2 = filename_path; 
                                    }
                                    counter++;
                                    if (counter > 1)
                                    {   
                                    
                                        char error_message[30] = "An error has occurred\n";
                                        write(STDERR_FILENO, error_message, strlen(error_message));
                                        check = 1;
                                    }
                                    if(((pair1.second.size() - 1) == 1 ) && pair1.second[i][j+1] != '/')
                                    {   
                                        char error_message[30] = "An error has occurred\n";
                                        write(STDERR_FILENO, error_message, strlen(error_message));
                                        check = 1;
                                        
                                    }
                                }
                            }
                            if (counter == 1)
                            {
                                check2++;
                            }
                        }  
                        if (check2 - 1 == 2)
                        {   
                            char error_message[30] = "An error has occurred\n";
                            write(STDERR_FILENO, error_message, strlen(error_message));
                            check = 1;
                            
                        }
                        bool results = false;
                         for (unsigned int i = 1; i < pair1.second.size(); i++)
                        {
                            for (unsigned int j = 0; j < pair1.second[i].length(); j++)
                            {
                               if ((pair1.second[i][j] == '>') && (check != 1))
                               {
                                   results = true;
                                   break;  
                               } 
                            }
                            break;
                        }
                        if (results) 
                        {
                        
                        ofstream myfile(filename_path2);
                        char c_path[filename_path2.size()+1];
                        strcpy(c_path,filename_path2.c_str());
                        

                        int fd_redirect = open(c_path, O_CREAT | O_TRUNC | O_WRONLY);
                        dup2(fd_redirect,STDOUT_FILENO);
                        close(fd_redirect);
                        vector<string> temp;
                        temp.push_back(pair1.first);
                        temp.push_back(real_path);
                        pair1.second = temp;
                        }
                        if (check != 1)
                        {
                        vector<char *> c_arguments;

                        for (auto c_container = pair1.second.begin(); c_container != pair1.second.end(); c_container++)
                        {
                        c_arguments.push_back(&(*c_container)[0]); //stackoverflow helped me with this line
                        }
                        c_arguments.push_back(NULL);
                        execv(file_name.c_str(), &c_arguments[0]);
                        }
                    }
                    else
                    {
                        int rc_wait = wait(NULL);
                        if (rc_wait != -1)
                        {
                            continue;
                        }
                    }
                }
            }
        }
        exit(0);
    }
    else 
    {   
        char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
    }
    return 0;
}


pair<string, vector<string>> makepairs(vector<string> &ref)
{
    vector<string> arguments;
    string f_word = ref[0];
    string temp;
    for (unsigned i = 0; i < ref.size(); i++)
    {
        arguments.push_back(ref[i]);
    }

    return make_pair(f_word, arguments);
}


bool containsAND(vector<string> &ref)
{
    for(unsigned int i = 0; i < ref.size(); i++)
    {   
        for (unsigned int j = 0; j < ref[i].length(); j++)
        {
            if(ref[i][j] == '&')
            {
            return true;
            }
        }
    }
    return false;
}

int amountofANDS(vector<string> &ref)
{   
    int amount = 0;
    for(unsigned int i = 0; i < ref.size(); i++)
    {
        for (unsigned int j = 0; j < ref[i].length(); j++)
        {
            if(ref[i][j] == '&')
            {
                amount++;
            }
        }
    }
    return amount;
}
