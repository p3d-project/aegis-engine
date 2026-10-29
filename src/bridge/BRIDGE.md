Contains the mapping files from the ecs to the ui.

Bridges require the ecs registry, and the uiManager. We use the uiManager to fetch the UIWidgets that we want to manipulate. We use the ecs registry to fetch the data we want to give to UIWidgets. We only talk to the ui via UiWidget function calls.

Well defined interface for bridge
- Registry
- UIManager
