# Router
Project 2 (Digital Design II) supervised by Dr. Shalan
By:
- Fadi Dawoud - 900163212
- Eslam Soliman - 900163257
- Mohamed Abdel Hamed - 900163202
***********************************************************************************************
**Description:**
------------------
Presented is an application that performs routing (i.e. wiring) of standard cells 
on a hardware design after placement, developed using C++. The program does that 
on two phases, namely Global Routing using coarse-grained GCells, followed by 
Detailed Routing, after which we obtain an almost complete (almost ready for fabrication) 
functional model for the design specified by the input DEF (Design Exchange Format) 
and LEF (Library Exchange Format) files. 
***********************************************************************************************
**Phases of Execution:**
-------------------------
1- LEF & DEF Parsing: to extract design and technology information    
2- Mapping and Initializing Extracted Data into their appropriate Data Structures    
3- Global Routing    
4- Detailed Routing    
5- Realizing Nets into Actual Wires and Vias        
6- Writeback Routed Design into DEF file    
***********************************************************************************************
**Algorithm:**
---------------
For this application, the Soukup's Fast Maze Router is the base algorithm used for both 
Global and Detailed Routing with some variations between the two in order to cater for 
their differences in objectives and constraints. The most notable variation made onto 
the one used for Global Routing is introducing the concept of GCell Cost Threshold, which
is the number of nets using (inside) the GCell, as well as the number of cell obstructions,
after which the GCell starts being avoided and possibly blocked (considered an obstacle). This cost threshold is left as a design decision for the user, same as the dimesnions of the global grid. In doing so, we spare a resonable free room to be used in critical situations such as when routing a target GCell that has already exceeded the cost threshold, in which case upper metal layers are utilized to circumvent blockages and congestion. 
***********************************************************************************************
**Technical Details of Routing**
----------------------
The routing process is as follows: 
1. Pin locations of the nets, read from the DEF file as rectangles, are translated into grid cells. 
2. A global netlist is derived from the input netlist
3. Details of the global grid are computed, and the grids are prepared for routing
4. Obstructions are taken into consideration
5. Each net is routed one by one
    - The source is first routed to the nearest cell, globally then in details. 
    - For all other pins other than the source and the first target: one of the cells designating this pin is set as source, and is routed, attempting for any cell on the previously created paths
    - An optimal path is found by Soukup's algorithm for this pin. 
6. Wire segments are deduced while tracing-back the maze-routing algorithm
7. Wire segments are passed on to the DEF-writer, which interprets them into rectangular wires, and writes out a complete DEF file. 
*********************************************************************************************
**Issues and Limitations**
--------------------------
1. The static limit functionality on GCells is very weak. It does not differentiate between how obstructions and pins are arranged within the GCell. So, a GCell could be blocked although it could still have space, and it could be free although it cannot be used for routing. 
2. Congestion cost of each GCell cannot be used to discourage router from using them. Soukup's router is a greedy router that aims to run for the target. It does not minimize over an arbitrary cost function other than the distance to the target. Hence, it was difficult to avoid congestions. This led to our design's inability to process large, heavily congested design. The router just fills whatever space closest to it, in effect choking the first metal layer from all others. 
********************************************************************************************

**How to use:**
----------------
1. Compile and/or run the router.cpp file
    - You can use the command "g++ -o route router.cpp && ./route" from the directory of the source files
2. As prompted, enter the dimensions for the global grid, the limit on each GCell, whether or not you want to consider obstructions, and the def/lef file names, in addition to the output def name. 
***********************************************************************************************
***Progress Report:***
---------------------
* *What's been done:*
    - Complete, functional mechanisms and infrastructure for Global Routing using    
    GCells, supporting netlists with nets having more than two pins using the Soukup maze routing algorithm    
    - Implemented mechanism for ensuring routability and preventing faulty routes, as well as dynamically allocating more metal layers    
    - Implemented testing infrastructure (IO, Netlist generators, etc)    
    - Designed a theoretical model for performing Detailed Routing  
    - Developed a complete implementation for the Detailed Routing model       
	- Parsed the layers and vias info from the LEF    
	- Parsed design info from the DEF file     
	- Parsing checking for some syntax errors   
    - Fully implemented DEF_Writer 

***********************************************************************************************
**References:**
---------------
- DEF, L. (2003). Language Reference Version 54. USA: Cadence Design System Inc, 56(5), 57-62.
- Kahng, A. B., Lienig, J., Markov, I. L., & Hu, J. (2011). VLSI physical design: from graph 
  partitioning to timing closure. Springer Science & Business Media.
- Soukup, J. I. R. I. (1978, June). Fast maze router. In Proceedings of the 15th Design Automation 
  Conference (pp. 100-102). IEEE Press.
***********************************************************************************************