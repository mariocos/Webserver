i=1

while [ $i -le 20 ];
do
	echo "Request number : $i"
    curl http://localhost:4243/index.html
    ((i++))
done