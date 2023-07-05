#!/bin/bash

while read command
	do
		path=$(find $(echo $MANPATH | sed "s/:/ /g") -name "man3")
		file=$(find $path -name "$command.*")

		if [[ $file =~ .*gz ]]; then
			echo $(zgrep '#include' $file | sed 's/.*#include <//' | sed 's/>.*//' | head -n 1)
		elif [[ -f $file ]]; then
			echo $(grep "#include" $file | sed 's/.*#include <//' | sed 's/>.*//' | head -n 1)
		else
			echo ---
		fi
	done
