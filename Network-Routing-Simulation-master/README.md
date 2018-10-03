# Network-Routing-Simulation
EECS-340 Routing Simulation project, by Prof. Peter Dinda.

Simple event-driven simulator for link-state and distance vector routing.


## QUICK START INSTRUCTIONS:
$ touch .dependencies

$ make depend

$ make TYPE=GENERIC

$ ./routesim demo.topo demo.event

### GENERATING SIMULATION TOPOLOGIES

The generate_simulation.py script can be used to generate random networks for
testing.

$ python generate_simulation.py --help
