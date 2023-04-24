# Command for mosquitto 
mosquitto -v -c confNetwork.conf
moquitto_pub -h 192.168.88.108 -t test -m "message to send" -r -i client -d
mosquito_sub -h 192.168.88.108 -t topic -d

# Test
data frame: "2400,600,1200,600,1200,600,1200,650,550,650,1150,650,550,650,600,550,1250,550,650,550,650,550,650,600,600"