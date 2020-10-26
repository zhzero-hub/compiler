#!/bin/bash

#@author: zhzero

if [ "$1"x=x  ]   	                    #没有输入文件名
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

if [ $index == 0 ]                          #输入不带文件后缀名
then
    filename=$filenamein
    flexfilename="${filenamein}.l"
    suffix="l"

else                                        #输入带文件后缀，判断后缀类型
    filename=${filenamein:0:index-1}
    suffix=${filenamein:index:${#filenamein}}
    lexConvert="x"
    if [ "$suffix"x = "lex"x ]              #输入文件后缀为.lex，自动转换成.l
    then
        flexfilename=$1
        lexConvert="xx"
    
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

if [ lexConvert = "xx" ]
then
    echo "Convert the file $flexfilename to ${filename}.l"
    mv $flexfilename ${filename}.l
    $flexfilename="${filename}.l"

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
