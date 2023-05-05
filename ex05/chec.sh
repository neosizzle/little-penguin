
output_value="$(echo asdf)"
if [[ "$output_value" == *"asd"* ]]
	then
	echo Hey that\'s a large number.
	pwd
fi


# STR='GNU/Linux is an operating system'
# SUB='Linux'
# if [[ "$STR" == *"$SUB"* ]]; then
#   echo "It's there."
# fi