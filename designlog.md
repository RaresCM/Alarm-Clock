## Learning points
- Project taught me how TWI communicates and transmits/receives data and how do utilize it with multiple modules
- Allowed me to write my own TWI logic/driver for communcations
- Taught me how a 16/2 display works using 8 bit data transmission and what controls can be analog vs digital
- Allowed me to see how to interface and communicate with a simple clock module without the need for WiFi
- Allowed me to come up with the analog to digital control logic for switches and number conversion from different formats like BCD to decimal or ASCII for displaying on an LCD and doing mathematical operations with the data

## Issues and obstacles 
- Learning and understanding the flow of TWI was difficult and I settled over 8 bit logic instead of 4 bit nibbles with a low and high byte
- Use of 8 bits needs more wires so switching to 4 bit high and low nibbles would be better as well as implementation of error detection and correction
- An unexpected problem I had was the time counting over 59 for minutes and seconds which made me implement wrap around logic to stop at 59 and start again at 0
- The buttons to control the alarm settings were tricky to come up with due to not being trivial in thinking
- While prototyping I accidently reversed the order of the 8 wire TWI connection to the LCD resulting in my LCD showing random characters which required a lot of troubleshooting to realize that was the problem mainly due to some characters looking fine
