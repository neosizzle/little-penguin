# Reset
Color_Off='\033[0m'       # Text Reset

# Regular Colors
Black='\033[0;30m'        # Black
Red='\033[0;31m'          # Red
Green='\033[0;32m'        # Green
Yellow='\033[0;33m'       # Yellow
Blue='\033[0;34m'         # Blue
Purple='\033[0;35m'       # Purple
Cyan='\033[0;36m'         # Cyan
White='\033[0;37m'        # White

# Bold
BBlack='\033[1;30m'       # Black
BRed='\033[1;31m'         # Red
BGreen='\033[1;32m'       # Green
BYellow='\033[1;33m'      # Yellow
BBlue='\033[1;34m'        # Blue
BPurple='\033[1;35m'      # Purple
BCyan='\033[1;36m'        # Cyan
BWhite='\033[1;37m'       # White

clear
make
insmod fortytwo.ko
# echo "read cat /dev/fortytwo"
# cat /dev/fortytwo
# echo ""

# TEST 1
output_value=$((echo -n 'jng123' > /dev/fortytwo) 2>&1)


string_to_search="Invalid argument"
printf "[TEST 1] Searching for $Cyan $string_to_search $Color_Off in output.. (echo -n 'jng123' > /dev/fortytwo)\n"

if [[ "$output_value" == *"$string_to_search"* ]]
	then
		printf "$BGreen OK $Color_Off\n"
	else
		printf "$BRed KO $Color_Off\n"
fi


#TEST 2
output_value=$((echo -n 'jn' > /dev/fortytwo) 2>&1)


string_to_search="Invalid argument"
printf "[TEST 2] Searching for $Cyan $string_to_search $Color_Off in output.. (echo -n 'jn' > /dev/fortytwo)\n"

if [[ "$output_value" == *"$string_to_search"* ]]
	then
		printf "$BGreen OK $Color_Off\n"
	else
		printf "$BRed KO $Color_Off\n"
fi

# TEST 3
output_value=$((echo -n 'dabb' > /dev/fortytwo) 2>&1)


string_to_search="Invalid argument"
printf "[TEST 3] Searching for $Cyan $string_to_search $Color_Off in output.. (echo -n 'dabb' > /dev/fortytwo)\n"

if [[ "$output_value" == *"$string_to_search"* ]]
	then
		printf "$BGreen OK $Color_Off\n"
	else
		printf "$BRed KO $Color_Off\n"
fi

# TEST 4
output_value=$((echo -n 'jng' > /dev/fortytwo) 2>&1)


val_to_cmp=$?
printf "[TEST 4] Return value $Cyan 0 $Color_Off in output.. (echo -n 'jng' > /dev/fortytwo)\n"

if [[ $val_to_cmp == 0 ]]
	then
		printf "$BGreen OK $Color_Off\n"
	else
		printf "$BRed KO $Color_Off\n"
fi

# TEST 5
output_value=$((cat /dev/fortytwo) 2>&1)


string_to_search="jng"
printf "[TEST 5] Searching for $Cyan $string_to_search $Color_Off in output.. (cat /dev/fortytwo)\n"

if [[ "$output_value" == *"$string_to_search"* ]]
	then
		printf "$BGreen OK $Color_Off\n"
	else
		printf "$BRed KO $Color_Off\n"
fi

rmmod fortytwo.ko