# save thermodata to csv
# args: tty port, loginterval in sec, log lengt in records
# example: python3 thermometer.py /dev/ttyACM0 10, 100 ## will record every 10 secs for 100 times, so 1000/60 = 15mins


import serial
import time
import sys
import datetime
import csv

serport = sys.argv[1]
interval = int(sys.argv[2])
records = int(sys.argv[3])

port = serial.Serial(serport, timeout=3)

filename = "logs/" #dir
filename += datetime.datetime.now().strftime("%Y-%m-%d_%H:%M:%S")
filename += "_themocoupleReadout.csv"
print("Writing to file: ", end='')
print(filename)


if port.is_open == False:
    port.open()


firstrow = ["Date", "time", "Thermo1", "Thermo2", "Thermo3", "Thermo4"]

with open(filename, 'w', newline='') as file:
    writer = csv.writer(file)
    writer.writerow(firstrow)
    file.close()


for i in range(records):
    starttime = time.time()
    if port.is_open == False:
        port.open()
    port.write(b'temp?\n')
    port.flush()
    # reads a line, decodes, to string, strips of newline
    line = port.readline().decode("utf-8").strip()
    if (line[-1] == ','):  # strip last comma
        line = line[:len(line)-1]
    port.close()
    line = line.split(',')  # split in dict
    # set date and time in first cells
    row = []
    row.append(datetime.datetime.now().strftime("%Y-%m-%d"))
    row.append(datetime.datetime.now().strftime("%H:%M:%S"))
    #  sets the values
    c = 1
    for j in line:
        val = j.split(':')
        try:
            row.append(val[1].strip())
        except:
            row.append("error")
        # print("T", end='')
        # print(c, end='')
        # print(" = ", end='')
        # print(row[-1], end='')
        # print(" ", end='')
        # c+=1

    with open(filename, 'a') as file:
        writer = csv.writer(file)
        writer.writerow(row)
        file.close()
    print(int(records) - i, end='')
    print(' measurements to go, aprox ', end='')
    timtogo = (int(records) - i)*interval
    if (timtogo > 60):
        print(round(timtogo/60, 1), end='')
        print(' minutes         ', end='\r')
    else:
        print(timtogo, end='')
        print(' seconds         ', end='\r')

    while time.time()-starttime < interval:
        time.sleep(0.5)
