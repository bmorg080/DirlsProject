//
//  dirls.cpp
//  
//
//  Created by Brian Morgan on 12/12/20.
//

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h> //used for MAX_PATH variable
#include <string.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <string>
#include <limits.h>
using namespace std;


int ListDir(std::string path, int aflag, int fflag, int dflag, int lflag) { //This function based off function from https://www.systutorials.com/how-to-iterate-all-dirs-and-files-in-a-dir-in-c/
  struct dirent *entry; 
  struct dirent *linkEntry;
  DIR *dp; 
  struct stat fileInfo;
  struct stat linkInfo;
  dp = ::opendir(path.c_str());
  if ((dflag) && (fflag)) 	//handling if dflag and fflag are both set
		  fflag = 0;

  if (dp == NULL) {
    perror("opendir: Path does not exist or could not be read.");
    return -1;
  }
    
    while ((entry = ::readdir(dp))) {    
        if (entry->d_name[0] == '.' && aflag == 0) //ignore dot files if aflag not set
            continue;

	string filePath = path + "/" + entry->d_name;
        lstat(filePath.c_str(), &fileInfo);
        if (lflag) {
	    struct group *grp;
	    struct passwd *pwd;
	    time_t timeID; 
	    grp = getgrgid(fileInfo.st_gid);
	    pwd = getpwuid(fileInfo.st_uid);
	    
            //Printing file permissions string. Example: -rw--wxrwx
	    if (S_ISDIR(fileInfo.st_mode))
	        cout << "d";
	    else if (S_ISLNK(fileInfo.st_mode))
		cout << "l";
	    else 
	    	cout << "-";
            printf( (fileInfo.st_mode & S_IRUSR) ? "r" : "-");
            printf( (fileInfo.st_mode & S_IWUSR) ? "w" : "-");
            printf( (fileInfo.st_mode & S_IXUSR) ? "x" : "-");
            printf( (fileInfo.st_mode & S_IRGRP) ? "r" : "-");
            printf( (fileInfo.st_mode & S_IWGRP) ? "w" : "-");
            printf( (fileInfo.st_mode & S_IXGRP) ? "x" : "-");
            printf( (fileInfo.st_mode & S_IROTH) ? "r" : "-");
            printf( (fileInfo.st_mode & S_IWOTH) ? "w" : "-");
            printf( (fileInfo.st_mode & S_IXOTH) ? "x" : "-");
            printf("\t");
            //Printing size of file(bytes)
            cout << to_string(fileInfo.st_size) << "\t";
	    //Printing username   
	    cout << pwd->pw_name << "\t" << grp->gr_name << "\t";
	    //Printing time
	    timeID = fileInfo.st_mtime;
	    char *time = strtok(ctime(&timeID), "\n");
	    cout << time << "\t";
       }

      // if fflag, then also call ListDir recursively
      // check to see if realpath to the resolved link is a directory
      // DT_DIR
      if (entry->d_type == DT_DIR) { //If entry is a directory
	  cout << entry->d_name << endl;;
        
          if ((dflag == 0) && ((strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0)))       {
               
 	      string filePath; //used for recursive function call
	      filePath = path + "/" + entry->d_name;
              ListDir(filePath, aflag, fflag, dflag, lflag); //recursive call
          }
      }
      // DT_REG
      if (entry->d_type == DT_REG) { //if item is a regular file
          cout << entry->d_name << endl;
      }
      // DT_LNK
      if (entry->d_type == DT_LNK) { //if item is a symbolic link
          cout << entry->d_name;
	  //If lflag enabled
	  char linkPath[PATH_MAX + 1];
	  char *result = realpath(filePath.c_str(), linkPath);
	  if (lflag) {
		  cout << " -> " << linkPath;
	  }
	  if (fflag) {
		   lstat(linkPath, &linkInfo);
	    	   if (S_ISDIR(linkInfo.st_mode)) {
			   ListDir(linkPath, aflag, fflag, dflag, lflag);
		   }
	  }
	  cout << endl;
      }
  }
  
  ::closedir(dp);
  return 0;
}


int main (int argc, char **argv) //https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html#Example-of-Getopt
{
    int aflag = 0;
    int fflag = 0;
    int dflag = 0;
    int lflag = 0;
    int hflag = 0;
    char *cvalue = NULL;
    int index;
    int c;
    
    string startingDir = ".";
    string hOutput = "Usage: dirls [(-[adflh]+) (dir)]*\n\t-a: include dot files\n\t-f: follow symbolic links\n\t-d: only this directory\n\t-l: long form\n\t-h: prints this message\n";
    opterr = 0;
    
    while ((c = getopt (argc, argv, "afdlh")) != -1) //from GNU
        switch (c)
        {
            case 'a':
                aflag = 1;
                break;
            case 'f':
                fflag = 1;
                cout << "got f\n";
                break;
            case 'd':
                dflag = 1;
                cout << "got d";
                break;
            case 'l':
                lflag = 1;
                cout << "got l\n";
                break;
            case 'h':
                hflag = 1;
                cout << hOutput;
                return 0;
            case '?':
                if (optopt == 'c')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr,
                                "Unknown option character `\\x%x'.\n",
                                optopt);
                    return 1;
            default:
                abort ();
        }

    //cout << "optind: " << optind << endl; //testing what is value of optind
	for (index = optind; index < argc; index++)
    {
		printf ("Non-option argument %s\n", argv[index]);
        startingDir = argv[index];
    }

    ListDir(startingDir, aflag, fflag, dflag, lflag);
    
	return 0;
}

//systutorials.com
//codewiki.wikidot.com



