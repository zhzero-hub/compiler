#!/bin/bash

filenamein=$1
index=`expr index "$1" .`

if [ $index == 0 ]  #输入不带文件后缀名
then
    filename=$filenamein
    flexfilename="${filenamein}.lex"
    suffix="lex"

else                #输入带文件后缀，判断后缀类型
    filename=${filenamein:0:index-1}
    suffix=${filenamein:index:${#filenamein}}
    if [ "$suffix"x = "lex"x ]
    then
        flexfilename=$1
    
    else
        echo "Error file name!"
        exit
    fi
    
    
fi

flexoutfilename="${filename}.yy.c"

echo "Filename in: $filenamein"
echo "Flex file name: $flexfilename"
echo "Suffix: $suffix"
echo "Flex output file name: $flexoutfilename"
echo "Executable file name: $filename"
echo ""

flex -o $flexoutfilename $flexfilename
clang $flexoutfilename -o $filename
./$filename
