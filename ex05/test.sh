make
insmod fortytwo.ko
echo "bad write echo -n "jng123" > /dev/fortytwo"
echo -n "jng123" > /dev/fortytwo
echo "bad write2 echo -n "jn" > /dev/fortytwo"
echo -n "jn" > /dev/fortytwo
echo "bad write3 echo -n "dabb" > /dev/fortytwo"
echo -n "dabb" > /dev/fortytwo
echo "read cat /dev/fortytwo"
cat /dev/fortytwo
echo ""
rmmod fortytwo.ko