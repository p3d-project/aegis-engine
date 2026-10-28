# Aegis Engine 2.0.0

See core/README.md for a quick overview on how the game engine should work.

## Overall idea
*Actually* build properly this time.
- Uses a *real* ECS
- Data driven from the ground up, not an afterthought. Proper modelling/schema design at the start.
- Proper seperation of concerns and decoupling. ECS handles ECS, UI handles UI, HAL handles HAL.
- Optimization as a focus from the ground up
- Mocking/testing as a core focus from the beginning. As we build the game engine, we will also create tests, and measure performance
- Migrate over game engine features from the persona-3-dual repo into the aegis-engine repo. Develop aegis-engine *as* a fully-fledged game engine, and develop persona-3-dual *as* a customer of aegis-engine. Engine remains with engine, game remains with game!
- Add actual, proof-of-concept SDL3 support within the HAL layer. The sooner we can mess around with the HAL layer/break it, the faster we can figure out how to fix it.
---
- Built for the team, with the team
- No AI, pure skill.
- No superficial design design document. Pure coding, architecture, documentation, notes (in md files) and PR reviews will be the basis of how we develop this game engine.

## TODO
- [ ] Deprecate aegis engine v1.0.0


