#! /bin/sh

#while read -r FUNCTION
#do
#PATH_TO_FILE=$(find $(echo $MANPATH | sed "s/:/ /g") -name "man3")
#LIBFILE=$(find $PATH_TO_FILE -name "$FUNCTION.*")
#echo $FINCTION
#if [[ $LIBFILE =~ .*gz ]];
#then
#gunzip -c $PATH/$FUNCTION.3.gz > ~/"current_functon.txt"
#DONE_FILE=$("~/current_function.txt")
#fi
#echo $(grep '#include' $DONE_FILE  | sed 's/.*#include <//' | sed 's/>.*//' | head -n 1)
#elif [[ -f $LIBFILE ]];
#then
#echo $(grep '#include' $LIBFILE | sed 's/.*#include <//' | sed 's/>.*//' | head -n 1)
#else
#echo —-
#fi
#done

read FUNCTION
path=/usr/share/man/man3
if [ -f $path/$FUNCTION.3.gz ]
then
echo 1
gunzip -c $path/$FUNCTION.3.gz > ~/current_functon.txt

echo $(grep -m 1 '#include' ~/current_functon.txt  | sed 's/.*#include <//' | sed 's/>.*//' | head -n 1)
fi

