#! /bin/bash

find . -name "*.c" -o -name "*.h" | xargs sed -i '/[a-zA-Z0-9_]\+\.h/{s/\\/\//g}'
find . -name "*.c" -o -name "*.h" | xargs sed -i 's/\(\"\.\{2\}\)\\/\1\//g'
