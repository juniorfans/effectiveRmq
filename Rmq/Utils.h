#pragma once
void parseIntStream(char *buf,int bufLen,int *numbers,int maxNumLen,int *realNumLen);

void fileIntsEx(const string& fileName,int *ints,int readMaxInts,int *realIntsCounts);

void intsToFile(int *ints,int len,const string &fileName);

int radix(int num);