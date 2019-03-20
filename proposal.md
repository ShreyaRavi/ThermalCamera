
# Proposal

## Wireless Thermal Camera
* Team: Alexa Ramachandran and Shreya Ravi

### Description

The Wireless Thermal Camera is a handheld device that sends thermal data to a monitor that then renders an image to the user. Thermal data is collected using an infrared sensing camera and communicated to the monitor using a WiFi chip. The monitor then reads the thermal data and creates a visual representation of the thermal data for the user. This rendering can then be extended to track areas of high thermal activity.

### Further Details
* Hardware required
  * IR Camera -- Sparkfun Qwiic IR Array MLX90640
  * Qwiic Cable Breadboard Jumper
  * ESP-32S Development Board -- 2
* Funds requested (the amount you will want to be reimbursed, remember you have
  a $20 budget per person)
  * $22 - ESP-32S Development Board
  * $2 - Qwiic Cable Jumper
  * $16 - IR Camera (Camera costs $40, $24 is covered by the team)
  * Total: $40
* Major task breakdown and who is responsible for each task
  * Read 32x24 pixel data from IR Camera -- Shreya
  * Send data to other Raspberry Pi via ESP32s -- Alexa
  * Interpolate on Raspberry Pi to match monitor width and height -- Alexa
  * Render thermal image on monitor by putting data in frame buffer -- Shreya
  * Extended Goal #1: Put bounding box around areas of high thermal activity -- Shreya
  * Extended Goal #2: Take picture of thermal image and store -- Alexa
* Goals for next week
  * Read image from IR Camera into buffer on one Raspberry Pi
  * Send buffer data from one Raspberry Pi to another using UART
  * Read buffer data from UART and interpolate to match monitor size
  * Put pixel data from interpolated buffer in framebuffer and render final image
* References and resources used
  * https://learn.sparkfun.com/tutorials/qwiic-ir-array-mlx90640-hookup-guide/all
  * https://docs.zerynth.com/latest/official/board.zerynth.nodemcu_esp32/docs/index.html
  * Ashwin created and maintains a library for ESP32 communication that allows the user to use UART to communicate between the Raspberry Pis
  * https://cdn.sparkfun.com/assets/3/1/c/6/f/MLX90640-Datasheet.pdf
