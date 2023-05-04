make && insmod debugfs.ko
echo "bad write echo -n "jng123" > /sys/kernel/debug/fortytwo/id"
echo -n "jng123" > /sys/kernel/debug/fortytwo/id
echo "bad write2 echo -n "jn" > /sys/kernel/debug/fortytwo/id"
echo -n "jn" > /sys/kernel/debug/fortytwo/id
echo "bad write3 echo -n "dabb" > /sys/kernel/debug/fortytwo/id"
echo -n "dabb" > /sys/kernel/debug/fortytwo/id
echo "read cat /sys/kernel/debug/fortytwo/id"
cat /sys/kernel/debug/fortytwo/id
echo ""
rmmod fortytwo.ko