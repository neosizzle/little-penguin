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

make
insmod fortytwo.ko
# echo "bad write echo -n "jng123" > /dev/fortytwo"
# echo -n "jng123" > /dev/fortytwo
# echo "bad write2 echo -n "jn" > /dev/fortytwo"
# echo -n "jn" > /dev/fortytwo
# echo "bad write3 echo -n "dabb" > /dev/fortytwo"
# echo -n "dabb" > /dev/fortytwo
# echo "read cat /dev/fortytwo"
# cat /dev/fortytwo
# echo ""

output_value=$((echo -n 'jng123' > /dev/fortytwo) 2>&1)

echo "$BPurple OUTPUT IS $output_value $Color_Off"
# if [[ "$output_value" == *"asd"* ]]
# 	then
# 	echo Hey that\'s a large number.
# 	pwd
# fi


rmmod fortytwo.ko