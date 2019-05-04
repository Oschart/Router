# Router
Project 2 (Digital Design II) supervised by Dr. Shalan
By:
- Fadi Dawoud
- Eslam Soliman
- Mohamed Abdel Hamed
***********************************************************************************************
-> Description:
*****************
Presented is an application that performs routing (i.e. wiring) of standard cells 
on a hardware design after placement, developed using C++. The program does that 
on two phases, namely Global Routing using coarse-grained GCells, followed by 
Detailed Routing, after which we obtain an almost complete (almost ready for fabrication) 
functional model for the design specified by the input DEF (Design Exchange Format) 
and LEF (Library Exchange Format) files. 
***********************************************************************************************
-> Algorithm:
***************
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
-> How to use:
****************
The application is to be run and provided with input in the manner described below:
1- 
2- 
3- 
***********************************************************************************************
-> Progress Report:
*********************
>> What's been done:
    - Complete, functional mechanisms and infrastructure for Global Routing using 
    GCells
    - Implemented mechanism for ripping and re-routing failed nets
    - Designed model for performing Detailed Routing
>> What's left to do:
    - LEF and DEF parsing
    - Developing a complete implementation for the Detailed Routing model 
    - Realizing the abstracted nets using actual wires and vias
    - Writing back the routed nets to the DEF file 
***********************************************************************************************
-> References:
*****************
- DEF, L. (2003). Language Reference Version 54. USA: Cadence Design System Inc, 56(5), 57-62.
- Kahng, A. B., Lienig, J., Markov, I. L., & Hu, J. (2011). VLSI physical design: from graph 
  partitioning to timing closure. Springer Science & Business Media.
- Soukup, J. I. R. I. (1978, June). Fast maze router. In Proceedings of the 15th Design Automation 
  Conference (pp. 100-102). IEEE Press.
***********************************************************************************************
