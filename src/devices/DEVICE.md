Contains the device implementations acting as the HAL layer.

Devices need to implement a clear, well-defined API that the rest of the game engine publically interfaces with. Then internally, it takes this data and calls HW functions to perform the desired functionality.

Still very uncertain/iffy on how to actually build out the HAL. *Do* we want to do it this way??
