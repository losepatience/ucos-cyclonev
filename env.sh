#! /bin/bash

tags(){
	rm -f cscope.*
	find `pwd` -name "*.S" -o -name "*.h" -o -name "*.c" -o -name "*.lds" -o -name "*.mk" -o -name "Makefile" > tags.file
	cscope -bqk -i tags.file
	rm -f tags.file

}

env(){
	/home/tyro/altera/14.0/embedded/embedded_command_shell.sh
}

case "$1" in
	tags)
		tags
		;;
	*)
		#echo "Usage: $0 [tags|env]"
		env
		;;
esac

