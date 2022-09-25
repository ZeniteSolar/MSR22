#make avr-can-lib
make -C lib/avr-can-lib/src clean
make -C lib/avr-can-lib/src lib

#make oled-display
make -C lib/oled-display clean
make -C lib/oled-display
wget https://raw.githubusercontent.com/ZeniteSolar/CAN_IDS/master/can_ids.h -O src/can_ids.h
