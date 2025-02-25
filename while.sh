i=1

while [ $i -le 200 ];
do
	echo "Request number : $i"
    curl http://localhost:4243
    ((i++))
done