#ifndef EXPLODE_H_INCLUDED
#define EXPLODE_H_INCLUDED
    /************
    Coded by Motsu35. have fun and play safe :)
    ************/
    #include <string>
    #include <vector>

    ///explode method that returns an array of strings.
    ///WARNING: due to c++ having no way to check the returned length of an array, only use this if you know how many elements it will return
    ///calling explode("bob joe sue"," ")[3] will cause a segfault, as there will not be a 3rd member.
    ///if calling explode on an unknown string, use the explode_v method below!
    std::string *explode (std::string original, std::string exploder=":")
    {
        std::string tmp;
        tmp=original;
        int num, loc;
        num=1;
        while (tmp.find(exploder)!=std::string::npos)
        {
            loc=tmp.find(exploder);
            tmp=tmp.substr(loc+exploder.length());
            num++;
        }
        std::string *result;
        result = new std::string[num];
        num=0;
        tmp=original;
        while (tmp.find(exploder)!=std::string::npos)
        {
            loc=tmp.find(exploder);
            result[num]=tmp.substr(0,loc);
            tmp=tmp.substr(loc+exploder.length());
            num++;
        }
        result[num]=tmp;
        return result;
    }


///same as aboce, but retuens a vector containing strings.
vector<string> explode_v(const string &str, const string &delimiter)
{
    vector<string> arr;

    int strleng = str.length();
    int delleng = delimiter.length();
    if (delleng==0)
        return arr;//no change

    int i=0;
    int k=0;
    while( i<strleng )
    {
        int j=0;
        while (i+j<strleng && j<delleng && str[i+j]==delimiter[j])
            j++;
        if (j==delleng)//found delimiter
        {
            arr.push_back(  str.substr(k, i-k) );
            i+=delleng;
            k=i;
        }
        else
        {
            i++;
        }
    }
    arr.push_back(  str.substr(k, i-k) );
    return arr;
}

///Starts a hidden windwos process by path name. if it is in the path directory, you can call just the exe by name.
void startProcessHidden(string path)
{
    STARTUPINFOA si = {0};
    PROCESS_INFORMATION pi = { 0 };

    CreateProcessA(NULL,
            path,   // command line
            NULL,          // process security attributes
            NULL,          // primary thread security attributes
            false,          // handles are inherited
            HIGH_PRIORITY_CLASS | CREATE_NO_WINDOW,  // creation flags
            NULL,          // use parent's environment
            NULL,          // use parent's current directory
            &si,  // STARTUPINFO pointer
            &pi);  // receives PROCESS_INFORMATION
    return;
}

///Starts a windwos process by path name. if it is in the path directory, you can call just the exe by name.
void startProcess(string path)
{
    STARTUPINFOA si = {0};
    PROCESS_INFORMATION pi = { 0 };

    CreateProcessA(NULL,
            path,   // command line
            NULL,          // process security attributes
            NULL,          // primary thread security attributes
            false,          // handles are inherited
            HIGH_PRIORITY_CLASS | CREATE_NO_WINDOW,  // creation flags
            NULL,          // use parent's environment
            NULL,          // use parent's current directory
            &si,  // STARTUPINFO pointer
            &pi);  // receives PROCESS_INFORMATION
    return;
}


    #ifndef max
        ///takes A and B, returns whatever is higher.
        #define max(a,b) (((a) > (b)) ? (a) : (b))
    #endif
    #ifndef min
        ///takes A and B, returns whatever is lower.
        #define min(a,b) (((a) < (b)) ? (a) : (b))
    #endif
    #ifndef random
        ///better random function, random(lower bound, upper bound)
        #define random(a,b) ((rand()%b)+a)
    #endif

#endif // EXPLODE_H_INCLUDED
