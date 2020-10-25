#!/bin/bash

#@author: zhzero

if [ "$1x"="x"  ]   #没有输入文件名
then
    read -t 30 -p "Please input filename: " filenamein

else
    filenamein=$1
fi

if [  "${filenamein}x"="x"  ]
then
    echo "No input file name!"
    exit
fi

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
    
    elif [ "$suffix"x = "l"x ]
    then
        flexfilename=$1

    else
        echo "Error file name ${filenamein}!"
        exit
    fi
    
    
fi

flexoutfilename="${filename}.yy.c"

if [ ! -f "$flexfilename" ]
then
    echo "Error file name ${filenamein}!"
    exit
fi

echo "Filename in: $filenamein"
echo "Flex file name: $flexfilename"
echo "Suffix: $suffix"
echo "Flex output file name: $flexoutfilename"
echo "Executable file name: $filename"
echo "-------------------------------"

flexCompile()
{  
    flex -o $3 $2
    clang $3 -o $1
    ./$1
    return true
}

flexCompile $filename $flexfilename $flexoutfilename
