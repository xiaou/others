#! /bin/sh


echo 'count /etc/passwd lines('$(cat /etc/passwd | wc -l)'):'
cat /etc/passwd | awk 'BEGIN{ sum = 0 }; { sum += 1 }; END{ print sum }'
awk 'BEGIN{i=0;j=1}{i+=1;j+=1}END{print i;print j}' /etc/passwd

awk '
BEGIN{
	i=0
}{
	i+=1
}
END{
	print i
}' /etc/passwd

echo 'see net state which is LISTEN:'
netstat -apn | awk '$6 == "LISTEN" { print $0 }'

echo 'see net state of tcp:'
netstat -apn | awk '$1 ~/^tcp/ {print $0}'









