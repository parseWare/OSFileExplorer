#include "config.h"
using namespace std;
void op(string path);
struct termios old_tio, new_tio;
   DIR *d;
struct dirent *de;
struct stat buf;
int posn = 30;
int uppos;
int downpos;
string path;
string root;
stack<string> currs;
stack<string> prevs;
vector<string> vec;
vector<string> val;
stack<string> stc1;
stack<string> stc2;
int cpos=0;
int prepos=0;

int getch(void)
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}
void parsestr(string code, vector<string> &tokens){

     stringstream check1(code); 
      
    string intermediate; 
      
    // Tokenizing w.r.t. space ' ' 
    while(getline(check1, intermediate, ' ')) 
    { 
        tokens.push_back(intermediate); 
    } 
      
}
int isDirectory(const char* path){
    DIR* dir=opendir(path);
    if(dir!=NULL){
        closedir(dir);
        return 0;
    }
    return -1;
}
string getfilename(string path){
    int sz=path.size()-1; string res="";
    while(path[sz]!='/'){
        res=path[sz]+res;
        sz--;
    }
    return res;
}
void copyfile(string sor,string des){
    char temp[1024];
    int inp,outp,fread;
    char *src=new char[sor.length()+1];
    strcpy(src,sor.c_str());
    string file=getfilename(string(src));
    des=des+"/"+file;
    cout<<des<<endl;
    char *fin=new char[des.size()+1];
    strcpy(fin,des.c_str());
    inp=open(src,O_RDONLY);
    outp=open(fin,O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
    while((fread=read(inp,temp,sizeof(temp)))>0)
        write(outp,temp,fread);
    cout<<"success"<<endl;
}
void copydir(char *sor,char *des){
    string desf=string(des)+"/"+getfilename(string(sor));
    char *desti=new char[desf.length()+1];
    strcpy(desti,desf.c_str());
    int st=mkdir(desti,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if(st==-1)
        cout<<"error"<<endl;
    DIR *direc;
    struct dirent *dir;
    direc=opendir(sor);
    if(direc){
        while((dir=readdir(direc))!=NULL){
            if((string(dir->d_name)=="..")|| (string(dir->d_name)==".")){ }
            else{
                string fname=string(sor)+"/"+string(dir->d_name);
                cout<<fname<<endl;
                char *newp=new char[fname.length()+1];
                strcpy(newp,fname.c_str());

                
                struct stat sb;
                if(stat(newp,&sb)==-1){}
                else{
                if((S_ISDIR(sb.st_mode))){
                    copydir(newp,desti);
                }
                else{
                    copyfile(fname,desf);
                }
                }
            }
        }
    }
}

void delete_file(string path){
    char *src=new char[path.length()+1];
    strcpy(src,path.c_str());
    int sts=remove(src);
    if(sts!=0){
        cout<<"error occurred while deleting"<<endl;
    }
}
void delete_dir(string path){
    char *src=new char[path.length()+1];
    strcpy(src,path.c_str());
    DIR *d;
    struct dirent *dir;
    d=opendir(src);
    if(d){
        while((dir=readdir(d))!=NULL){
            if((string(dir->d_name)=="..") || (string(dir->d_name)==".")){ }
            else{
                string fpath=string(path)+"/"+string(dir->d_name);
                char *fsrc=new char[fpath.length()+1];
                strcpy(fsrc,fpath.c_str());
                struct stat sb;
                if(stat(fsrc,&sb)==-1){ }
                else{
                    if((S_ISDIR(sb.st_mode))){
                        delete_dir(fpath);
                    }
                    else{
                        delete_file(fpath);
                    }
                }
            }
        }
        closedir(d);
        int sts=rmdir(src);
    }
}
bool searchdirandfile(string path,string file){
    DIR *d;
    struct dirent *dir;
    char *src=new char[path.length()+1];
    strcpy(src,path.c_str());
    d=opendir(src);
    if(d){
        while((dir=readdir(d))!=NULL){
           if((string(dir->d_name)=="..") || (string(dir->d_name)==".")){ }
           else{
               string name=dir->d_name;
               //cout<<name<<" ";
               string fpath=string(src)+"/"+name;
               char *newp=new char[fpath.length()+1];
               strcpy(newp,fpath.c_str());

               struct stat sb;
        if (stat(newp, &sb) == -1)
        {
          perror("lstat");
        }
                else{
                    if((S_ISDIR(sb.st_mode))){
                        if(name==file){
                            return true;
                        }
                        else{
                               
                                string mystr = path + "/" + name;
                               //cout<<mystr<<endl;
                               bool nw = searchdirandfile(mystr,file);
                               if(nw)
                                return true;
                    }
                  }
                    else{
                        if(name==file){
                            return true;
                        }
                    }
                }
           } 
        }
      // cout<<"False"<<endl;
        return false;
    }
    return false;
}

void commandmode(string path){

   //cout<<"\u001b[J";
   // op(path);
     struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        cout<<"\u001b["<<w.ws_row-8<<"B";
        //cout<<"\033["<<50<<"B";

        cout<<"\u001b[33m";
        cout<<"Enter Command:";
        cout<<"\u001b[0m";

        // printf("\033[6n");

    char ch;int n;
    string code="";
    vector<string> token;
    while(ch=getch()){
        //  cout<<GetCursorPos();
          if(ch == 27){
           // op(path);
           // break;
            return;
          }
          if(ch == 10){
            cout<<ch;
            break;
          }
          if(ch == 127){
            //cout<<"\033[1C";
            if(w.ws_col > 0){
            cout<<"\b \b";
            code = code.substr(0,code.length()-1);
          }
           // cout<<code;
          }
          else{
            cout<<ch;
            code += (char(ch));
          }
        }
    parsestr(code,token);
    if(token[0]=="copy"){
     /* system("clear");
      for(int i=0; i<token.size(); ++i)
        cout<<token[i]<<endl;
      while(1)
        cin>>n;*/
      string dest = token[token.size()-1];
      char *des = new char[dest.length()+1];
      strcpy(des,dest.c_str());
      string file,nxtdest;
      char space[256];
     
      for(int i=1; i<token.size()-1; ++i){
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        file = string(cwd) + "/" + token[i];
        char *src = new char[file.length()+1];
        strcpy(src, file.c_str());
        if(isDirectory(src)==0)
          copydir(src,des);
        else
          copyfile(file,des);
      }

    }
    else if(token[0]=="delete_file"){
        delete_file(token[1]);
       
       /* printf("\033[0;0H");
        for(int k=uppos; k<=downpos; ++k)
          cout<<left<<setw(30)<<vec[k]<<val[k]<<endl;
          printf("\033[0;0H");*/


    }
    else if(token[0] == "delete_dir"){
        delete_dir(token[1]);
        
        /*for(int k=uppos; k<=downpos; ++k)
          cout<<left<<setw(30)<<vec[k]<<val[k]<<endl;
          printf("\033[0;0H");*/
    }
    else if(token[0] == "move"){

        string dest = token[token.size()-1];
      char *des = new char[dest.length()+1];
      strcpy(des,dest.c_str());
      string file,nxtdest;
      char space[256];
     
      for(int i=1; i<token.size()-1; ++i){
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        file = string(cwd) + "/" + token[i];
        char *src = new char[file.length()+1];
        strcpy(src, file.c_str());
        if(isDirectory(src)==0){
          copydir(src,des);
          delete_dir(file);
        }
        else{
          copyfile(file,des);
         delete_file(file);
        }

      }
    }
    else if(token[0]=="rename"){
      string mystr;
       char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        mystr = string(cwd) + "/" + token[1];      
        char *source = new char[mystr.length() + 1];
        strcpy(source, mystr.c_str());
        mystr = string(cwd) + "/" + token[2];
        char *destination = new char[mystr.length() + 1];
        strcpy(destination, mystr.c_str());
        rename(source,destination);
        

    }
    else if(token[0]=="create_file"){

      string mystr = token[2] + "/" + token[1];
        char *source = new char[mystr.length() + 1];
        strcpy(source, mystr.c_str());

        ofstream out1(source);
      
    }
    else if(token[0]=="create_dir"){

      string mystr = token[2] + "/" + token[1];
      char *source = new char[mystr.length() + 1];
      strcpy(source, mystr.c_str());
      int stat= mkdir(source, S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IXOTH);
      if(stat!=0)
      {
          cout<<endl<<"Invalid path"<<endl;
      }
     
    }
    else if(token[0]=="goto"){

          string mystr = token[1];
          char *source = new char[mystr.length() + 1];
          strcpy(source, mystr.c_str());
          chdir(source);
          path = source;
         

    }
    else if(token[0]=="search"){
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        string mystr = string(cwd);
        //cout<<mystr<<" ";
        bool flag = searchdirandfile(mystr,token[1]);
        if(flag)
          cout<<"True"<<endl;
        else
          cout<<"False"<<endl;
    }
    if(token[0]!="search")
        op(path);
    commandmode(path);
}


void movecur(vector<string> &vec, string path){
  int ccrp=0;//vec.size();
  //cout<<endl<<ccrp<<" "<<vec[ccrp-1];
    printf("\033[0;0H");
  uppos = 0;
  downpos = 22>val.size()-1?val.size()-1:22;
   char ch;
   
    struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        //cout<<"\u001b[J";
        //cout<<"\u001b["<<0;
 
  while(1)
    {
      ch = getch();
    
    if(ch==27){  
     ch=getchar();
     ch = getchar();
      if(ch==65){
         if(ccrp>0){
            ccrp--;
      if(ccrp>=uppos && ccrp<downpos && ccrp<val.size())
          cout<<"\033[1A";
        else{
          downpos = uppos -1;
          uppos = (downpos-22)<=0?0:(downpos-22);
          system("clear");
           for(int k=uppos; k<=downpos; ++k)
          cout<<left<<setw(30)<<vec[k]<<val[k]<<endl;
          printf("\033[23;0H");
        }
   }
  }
      else if(ch==66){
        if(ccrp < val.size()-1){
        if(ccrp>=uppos && ccrp<downpos && ccrp<val.size())
        cout<<"\033[1B";
        else{
          uppos = downpos+1;
          downpos = (22+uppos)>val.size()-1?val.size()-1:(22+uppos);
          system("clear");
          for(int k=uppos; k<=downpos; ++k)
          cout<<left<<setw(30)<<vec[k]<<val[k]<<endl;
          printf("\033[0;0H");
        }
          ccrp++;
        }
      }
      else if(ch==68){
        if(currs.size()>1){
       string nw = currs.top();
       currs.pop();
       prevs.push(nw);
       path = currs.top();
        currs.pop();
       op(path);
       movecur(vec,path);
}
      }
      else if(ch == 67){
        if(!prevs.empty()){
        path = prevs.top();
        prevs.pop();
        
        op(path);
        movecur(vec,path);
      }
      }
      //else
      //  break;
       
        }
    
    else if(ch == 127){
      string nw = path;//currs.top();
      int ln = nw.size()-1;
      while(nw[ln]!='/')
        ln--;
      path = nw.substr(0,ln);
      
        op(path);
       movecur(vec,path);
    }
    else if(ch == 10){
      path = currs.top() + "/" + vec[ccrp];
      string nw = vec[ccrp];
      int n = path.length();  
    char pth[n + 1]; 
    strcpy(pth, path.c_str()); 
      stat(pth, &buf);
                 if (S_ISREG(buf.st_mode)) {
                  char *args[3];
                  string fle = "vi";
                  string fles = vec[ccrp];
                  args[1] = (char *)fles.c_str();
                  args[0] = (char *)fle.c_str();
                  args[2]=NULL;
                   pid_t pid = fork();
                   if(pid == 0){
                    execl("/usr/bin/xdg-open","xdg-open",pth,NULL);
                      op(path);
                       movecur(vec,path);
                  }
                   
                     // perror("fork error");
                 }
          else{
            
            op(path);
             movecur(vec,path);
          }
      }
    else if(ch == 104){
      path = "/home";
      op(path);
       movecur(vec,path);
    }
    else if(ch == 113){
      exit;
    }
    else if(ch == 107){
      if(downpos+1<val.size()){
          uppos = downpos+1;
          downpos = (22+uppos)>val.size()-1?val.size()-1:(22+uppos);
          system("clear");
          for(int k=uppos; k<=downpos; ++k)
          cout<<left<<setw(30)<<vec[k]<<val[k]<<endl;
          printf("\033[0;0H");
          ccrp=uppos;
        }
    }
    else if(ch == 108){
      if(uppos-23>=0){
          downpos = uppos -1;
          uppos = (downpos-22)<=0?0:(downpos-22);
          system("clear");
           for(int k=uppos; k<=downpos; ++k)
          cout<<left<<setw(30)<<vec[k]<<val[k]<<endl;
          printf("\033[0;0H");
          ccrp=uppos;
      }
    }
    else if(ch == 58){
       commandmode(path);   
        op(path);
         movecur(vec,path);
        break;
    }
    
}
}

void op(string pths){
  //printf("\033[H\033[J");
  path = pths;
  system("clear");
      struct winsize w; // To get terminal property
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  currs.push(path);
 int i=0,j=0;
 char P[10]="rwxrwxrwx",AP[10]=" ";
 // printf("\033[%d;%dH", 0,0);
 struct tm *t;
 char time[26];
 //cout<<path<<endl;
 char *fname = new char[path.length()+1];
 strcpy(fname,path.c_str());
  d=opendir(fname);
 readdir(d);
 readdir(d);
  vec.clear();
  val.clear();
 
 while( (de=readdir(d))!=NULL)
 {
 //cout<<++i<<" ";

  stat(de->d_name,&buf);
  string currval="";
  //currval += string(de->d_name)+"\t";
 //  cout<<de->d_name;
   
  // cout<<setw(25)<<buf.st_size<<setw(25);
   int sze = buf.st_size;
   

  if(S_ISDIR(buf.st_mode)){
    currval += "d";
  // cout<<"d";
  }
  else if(S_ISREG(buf.st_mode)){
      currval += "-";
 // cout<<"-";
}
  else if(S_ISCHR(buf.st_mode)){
      currval += "c";
  cout<<"c";}
  else if(S_ISBLK(buf.st_mode)){
      currval += "b";
 //  cout<<"b";
  }
  else if(S_ISLNK(buf.st_mode)){
      currval += "l";
  // cout<<"l";
  }
  else if(S_ISFIFO(buf.st_mode)){
      currval += "p";
 //  cout<<"p";
  }
  else if(S_ISSOCK(buf.st_mode)){
      currval += "s";
 //  cout<<"s";
 }
    for(i=0,j=(1<<8);i<9;i++,j>>=1)
   AP[i]= (buf.st_mode & j ) ? P[i] : '-' ;
  currval += string(AP) + "\t";
  currval += to_string(sze) + "\t";
  t=localtime(&buf.st_mtime);
  strftime(time,sizeof(time),"%b %d %H:%M",t);
 // cout<<setw(25)<<time<<endl;
    currval += string(time) + "\t";

  
  string nw = de->d_name;
  vec.push_back(nw);
  if(nw == ".")
    cpos = vec.size();
  if(nw== "..")
    cpos = vec.size();
 
  //File Name
  val.push_back(currval);

}
  for(int k=0; k<23 && k<val.size(); k++)
    cout<<left<<setw(30)<<vec[k]<<val[k]<<endl;

  printf("\033[0;0H");
  
  //closedir(d);
}

int main(int argc,char **argv)
{
  printf("\033[H\033[J");

  // string nw = home;
  /* tcgetattr(STDIN_FILENO, &old_tio);
    atexit(trm);
    new_tio = old_tio;
    new_tio.c_lflag &= (~ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);*/

  op(".");
  movecur(vec,path);
 return 0;
}
// /home/paras/Projects/OS/Assignment1