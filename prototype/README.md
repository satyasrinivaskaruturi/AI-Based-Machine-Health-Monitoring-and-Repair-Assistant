# AI-Based Machine Health Monitoring & Repair Assistant
## Wokwi online simulation

This prototype follows the user's supplied ESP32 circuit diagram as closely as practical in Wokwi.

### Reference-to-simulation mapping
| Reference circuit | Wokwi implementation |
|---|---|
| ESP32 | ESP32 DevKitC V4 |
| SW-420 vibration sensor | MPU6050 acceleration used as a vibration input |
| DS18B20 | DS18B20 |
| ACS712 current sensor | Potentiometer used to emulate analog current-sensor output |
| Sound/noise sensor | Potentiometer used to emulate analog sound-sensor output |
| OLED I2C | SSD1306 I2C OLED |
| Red LED | Wokwi red LED |
| Buzzer | Wokwi buzzer |
| Relay module | Wokwi relay module |
| LM358 signal conditioning | Represented at the controller input level; Wokwi is not being used here as a detailed analog op-amp simulation |
| 12V adapter / buck / regulators | Not electrically modeled; Wokwi powers the virtual modules directly |
| microSD | Omitted from this first working version because it is optional in the reference diagram |

### GPIO map
- GPIO 21 -> I2C SDA (OLED + MPU6050)
- GPIO 22 -> I2C SCL (OLED + MPU6050)
- GPIO 33 -> DS18B20 data
- GPIO 34 -> virtual ACS712/current input
- GPIO 32 -> virtual noise input
- GPIO 19 -> red fault LED
- GPIO 18 -> buzzer
- GPIO 17 -> relay input

### Fault thresholds
- Temperature > 75 °C
- Current > 8 A
- Noise > 85 dB
- Vibration magnitude > 1.50 g

Any fault causes:
FAULT -> red LED ON -> buzzer ON -> relay ON

### How to use in Wokwi
1. Open Wokwi and create an ESP32 Arduino project.
2. Replace its diagram.json with this project's diagram.json.
3. Replace its sketch.ino with this project's sketch.ino.
4. Add the libraries listed in libraries.txt using Wokwi Library Manager.
5. Start the simulation.
6. Change the DS18B20 temperature to test overheating.
7. Turn the CURRENT potentiometer to test high-current conditions.
8. Turn the NOISE potentiometer to test high-noise conditions.
9. Change MPU6050 acceleration values to test vibration.

This is an online simulation. It must not be presented as a physical prototype containing the actual SW-420, ACS712 and sound sensor hardware.
