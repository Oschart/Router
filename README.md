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
is the number of nets using (inside) the GCell, after which the GCell starts being avoided
and possibly blocked (considered an obstacle). This cost threshold is adjusted to be equal 
to half the number of tracks within the GCell with each net being assigned a track within the 
GCell. In doing so, we spare a resonable free room to be used in critical situations such as 
when routing a target GCell that has already exceeded the cost threshold, in which case upper 
metal layers are utilized to circumvent blockages and congestion. 
***********************************************************************************************
**How to use:**
----------------
To use the global router:
1. Compile and/or run the file (you can do it using the provided groute.exe file)
2. You will be prompted to enter details about the global route grid and the netlist
3. The global route cost matrix will be printed out, where each cell value corresponds to the number of nets assigned to this GCell

To run this milestone's code:
1. Insert your test case file in a file called "in.txt"
2. Open cmd (e.g. PowerShell)
3. Navigate to the project directory 
4. Run the command "g++ -o Demo Demo.cpp" to compile
5. Insert "./Demo" to run
6. Routing output appears to the console, Routed DEF is written to the file specified in "in.txt" 
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
* *What's left to do:*    
    - Complete LEF parsing    
    - DEF Writer Optimization  
    - Full mechanism for ripping routes   
***********************************************************************************************
**References:**
---------------
- DEF, L. (2003). Language Reference Version 54. USA: Cadence Design System Inc, 56(5), 57-62.
- Kahng, A. B., Lienig, J., Markov, I. L., & Hu, J. (2011). VLSI physical design: from graph 
  partitioning to timing closure. Springer Science & Business Media.
- Soukup, J. I. R. I. (1978, June). Fast maze router. In Proceedings of the 15th Design Automation 
  Conference (pp. 100-102). IEEE Press.
***********************************************************************************************
