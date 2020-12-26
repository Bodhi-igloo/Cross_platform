Sensor Data Acquisition and Transfer:

For the implementation of the system in the project, we have used KL25Z arm cortex M0+ on one side where the 3 axial sensor is present on the board. On the other hand, we have used the nrfDK board with Arm® Cortex®-M4 which receives the acquired data from the sensor. The 3 axial sensor is interfaced with I2C communication protocol on KL25z board, hence, the sensor
data is read from I2C bus. After successful reading , packets are formed in order to send it through the UART bus to an external device. In this case , the external device used is nrf DK
board. In nrf DK application, the received information is first evaluated by performing certain checks and then the data is unpacked and displayed on the logger window.