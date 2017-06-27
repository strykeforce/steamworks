# Steamworks Autonomous Routines

These sequence numbers correspond to the auton switch position on the robot.

## BLUE

- **01**: BLUE fuel hopper
- **06**: BLUE center gear
- **08**: BLUE left gear (boiler), shoot
- **09**: BLUE right gear (retrieval zone), shoot
- **0A**: BLUE right gear (retrieval zone), drive down field
- **0D**: BLUE right gear (retrieval zone), dump two hoppers

## RED

- **02**: RED fuel hopper
- **07**: RED center gear
- **0B**: RED left gear (retrieval zone), drive down field
- **0C**: RED right gear (boiler), shoot
- **0E**: RED left gear (retrieval zone), shoot

## Segment Programming

Segments are entered with an angle relative to the robot's starting orientation.

```
  driver station
_______________
    fuel intake
       ⤹0⤵︎
    -90    +90
```
