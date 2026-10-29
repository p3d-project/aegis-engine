Contains the mapping files from the ecs/ui layer to the device (HAL) layer.

Builders require the ecs registry, and the ui manager. For the ecs, we need to pull the data we want to submit from entities. For the ui manager, we want to pull the data we want to submit from UIWidgets.

Builders submit to a shared queue, and the items in the queue is what gets submitted to the HAL
