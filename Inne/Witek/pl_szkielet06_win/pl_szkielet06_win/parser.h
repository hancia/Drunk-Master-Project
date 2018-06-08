#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>

using namespace std;

class RokkoParser
{

public:
   static void objToTxt(const string aInFilename, 
                        const string aOutFilename, 
                        bool aVerbose = false);
   static vector<string> explode(string aStr, char aDelim);
};