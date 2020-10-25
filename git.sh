#!/bin/bash

#@author: zhzero

git init
git add -A
if [ $1x = x ]    #无commit信息，自动commit时间
then
    git commit -m "`date +"%Y.%m.%d %H:%M:%S"`"

else
    git commit -m "$*"
fi

git push origin main
