#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h> 


// char* execute(char *command){
//     if(strlen(command)<2) return "$$$$";
//     if(command[0]=='c' && command[1]=='d'){     // execute cd 
//         char *path;
//         if(strlen(command)==2) return "####";
//         else path = command+3;
//         int ch=chdir(path);
//         if(ch<0) return "####";
//         else return ""; 
//     }
    
//     if(strlen(command)<3) return "$$$$";
//     if(command[0]=='p' && command[1]=='w' && command[2]=='d'){  // execute pwd
//         char *dir = (char *)malloc(100*sizeof(char));
//         char *status = getcwd(dir, 100*sizeof(char));

//         if(status==NULL) return "####";
//         else return dir;
//     }

//     if(command[0]=='d' && command[1]=='i' && command[2]=='r'){  // execute dir
//         char *path;
//         if(strlen(command)==3) path = ".";
//         else path = command+4;
//         DIR *dir = opendir(path);
//         if(dir==NULL) return "####";

//         struct dirent *entry;
//         char *output = (char *)malloc(1000*sizeof(char)); output[0]='\0';
//         while((entry = readdir(dir))!=NULL){
//             if(strcmp(entry->d_name, ".")==0 || strcmp(entry->d_name, "..")==0) continue;  
//             strcat(output, entry->d_name);
//             strcat(output, "\n");
//         }
//         return output;

//     }
//     return "$$$$";
    
// }

char *execute(char *command){
    char *output = (char *)malloc(1000*sizeof(char)); output[0]='\0';
    if(command[0]=='c' && command[1]=='d'){     // execute cd 
        if(strlen(command)==2){
            strcpy(output, "####"); ////////
        }
        else{
            char *path;
            path = command+3;
            int ch=chdir(path);
            if(ch<0) strcpy(output, "####");
            else strcpy(output, ""); 
        }
    }
    else if(command[0]=='p' && command[1]=='w' && command[2]=='d'){  // execute pwd
        char *status = getcwd(output, 100*sizeof(char));

        if(status==NULL) strcpy(output, "####");
    }
    else if(command[0]=='d' && command[1]=='i' && command[2]=='r'){  // execute dir
        char *path;
        if(strlen(command)==3) path = ".";
        else path = command+4;
        DIR *dir = opendir(path);
        if(dir==NULL) strcpy(output, "####");
        else{
            struct dirent *entry;
            while((entry = readdir(dir))!=NULL){
                if(strcmp(entry->d_name, ".")==0 || strcmp(entry->d_name, "..")==0) continue;  
                strcat(output, entry->d_name);
                strcat(output, "\n");
            }
            
        }

    }
    else strcpy(output, "$$$$");
    return output;
}

int main(int argc, char *argv[])
{
    char command[100];

    

    // while(1){
    //     printf("Enter a command: "); 
    //     fgets(command, 100, stdin);
    //     command[strlen(command)-1]='\0';
    //     if(strcmp(command, "exit")==0) break;
    //     char *opt = execute(command);
    //     printf("%s\n\n", opt);
    //     free(opt);

    // }
    command[0]='.';
    int ch = chdir(command);
    printf("%d\n", ch);
    char* try = getcwd(NULL, 0);
    printf("%s\n", try);
    free(try);


    return 0;
}