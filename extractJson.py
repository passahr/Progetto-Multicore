'''
Author: Gianfranco "kanema" Passanisi
Description: Simple python code that extract every TCP/UDP packet data stream and saves the one per row in a new txt file called "processed.txt"
'''
import json

with open('test.json', 'r') as f:
    packets_json = json.load(f)
file = open("processed.txt","w")

packets = []

for packet in packets_json:
    stringa_pacchetto = json.dumps(packet)+"\n"
    packets.append(stringa_pacchetto)
    

file.writelines(packets)    

print("File created!")