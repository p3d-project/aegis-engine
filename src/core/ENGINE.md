Contains the core of the game engine

## ECS
We don't want to expose EnTT directly. We want to have a layer to interface in between for a few reasons
- We call an aegis engine namespace. It is very clear where the functionality is coming from
- We may want to modify/change ECS in the future ourselves.
Essentially, you can think of it as an abstraction layer seperating the ECS API from the ECS implementation, similar to how a HAL works

## Loop
<ecs - Scene>  - does ECS things. “Scene” is basically what our “Views” are
 	|
    V
<ecs2ui - Bridge> - mapping what we have from scene into what ui wants
 	|
    V
<ui - UIManager w/ UIWidgets> - does UI things. UIWidgets are the individual UI objects
composed together to create a UI. Composition.
 	|
    V
<ecs - SceneBuilder> - mapping for HAL (pulls data from entities)
<ui - UIBuilder> - mapping for HAL (pulls data from UIWidgets)
 	|
    V
<HAL - Device>  - HAL layer
