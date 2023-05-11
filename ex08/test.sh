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
make && insmod reverse.ko
# TEST 1
echo -n asdf > /dev/reverse
output_value=$((cat /dev/reverse) 2>&1)


string_to_search="fdsa"
printf "[TEST 1] Comparing for $Cyan '$string_to_search' $Color_Off in output.. (cat /sys/kernel/debug/fortytwo/foo).."

if [[ "$output_value" == "$string_to_search" ]]
	then
		printf "$BGreen OK $Color_Off\n"
	else
		printf "$BRed KO $Color_Off\n"
fi

# TEST 2
echo -n racecar > /dev/reverse
output_value=$((cat /dev/reverse) 2>&1)


string_to_search="racecar"
printf "[TEST 2] Comparing for $Cyan '$string_to_search' $Color_Off in output.. (cat /sys/kernel/debug/fortytwo/foo).."

if [[ "$output_value" == "$string_to_search" ]]
	then
		printf "$BGreen OK $Color_Off\n"
	else
		printf "$BRed KO $Color_Off\n"
fi

# TEST 3
echo -n "the quick brown fox jumps over the lazy dog" > /dev/reverse
output_value=$((cat /dev/reverse) 2>&1)


string_to_search="god yzal eht revo spmuj xof nworb kciuq eht"
printf "[TEST 3] Comparing for $Cyan '$string_to_search' $Color_Off in output.. (cat /sys/kernel/debug/fortytwo/foo).."

if [[ "$output_value" == "$string_to_search" ]]
	then
		printf "$BGreen OK $Color_Off\n"
	else
		printf "$BRed KO $Color_Off\n"
fi

rmmod reverse.ko