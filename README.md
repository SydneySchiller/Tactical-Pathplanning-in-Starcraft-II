
# **Project: Tactical Path Planning Using Influence Maps to Identify the Safest Path**

### Site Navigation
[Project Report](master/README.md)
[Deliverables](docs/deliverables.md)


### Group Members
Bayan Mashat, [bmmashat@ucdavis.edu](mailto:bmmashat@ucdavis.edu), 999851616

Ben Larrison, [bllarrison@ucdavis.edu](mailto:bllarrison@ucdavis.edu), 913268599

Jonny Hopkins, [jwhopkins@ucdavis.edu](mailto:jwhopkins@ucdavis.edu), 912794417

Mark Nickerson, [manickerson@ucdavis.edu](mailto:manickerson@ucdavis.edu), 914325295

Sydney Schiller, [ssschiller@ucdavis.edu](mailto:ssschiller@ucdavis.edu), 998352459
### Problem and Contribution Statement
Unit path planning in StarCraft II, while being sophisticated in its use of swarm or flocking techniques, is currently limited to the scope of finding the shortest path. Currently, if deployed units are to encounter enemy units, they would engage in combat whether or not their strengths were equivalent. Most current path planning doesn’t take into consideration the multiple hazards in a given situation (environmental choke points, ranged units positioned atop a bluff, potential unequal unit strength distribution), leading to a greater potential for a loss of units. Porting the influence-map-based path planning techniques developed in this project will provide developers with more intuitive agents in future games.

Consumer-grade influence map based path planning is currently unavailable, with small game developers having to rely on implementing A* and other path planning algorithms for game agents. As A* (and its variants) lack the ability to generate tactically intuitive paths, our solution aims to implement a more responsive path planning system that takes multiple factors into consideration when traversing terrain. This could mean finding a safest-shortest-path or a most concealed-shortest-path (a path that doesn’t trigger enemy ‘alarms’)--a type of consideration not currently being taken advantage of by games AI.

By combining the two techniques of path planning and influence maps, better traversal in a dynamic environment is possible. This safer traversal should lead to an agent that is more adaptive, responsive, and flexible to an environment with different enemies, resources, and building types. More intuitive path planning system involves the implementation and generation of multiple influence maps--by giving certain key characteristics (for buildings or units) a specified amount of influence and influence decay, agents will be able to dynamically assess the safest shortest path. Therefore, multiple influence maps will be able to change the given cost of a node-to-node transition

Potential approaches to the problem of units only considering shortest path largely relies on the path planning algorithm itself. Outside of creating a sophisticated unit-based intelligence (where individual units could assess a given situation and make a responsive decision), influencing path planning via influence maps seems to be the most lightweight (read: practical) solution.

Each map within StarCraft II can be thought of as a graph with many nodes--wherein there are two ways one can approach influencing unit traversal. One could either modify the heuristic value of a map node, or modify the cost value of a map node. Modifying the heuristic value wouldn’t accurately represent collective enemy presence, as heuristic values represent a single cost taken on each transition into that node. Modifying the cost of a node based on enemy influence, however, is better capable of representing collective enemy presence as the values are compound. The proposed approach, therefore, will alter the cost values of a node’s edge based on the aggregate influence exhibited on that node.

*[Provide the transformation of your problem from the original proposal to its current form. If the problem your project addressed and your contribution did not change, add a section stating that the original problem was well-scoped and held for the duration of the project. If it did change, describe the chronology of those changes and include your reasoning and justifications for making each change.]*

**After the end of the project time period, we determined that this problem was well scoped and we did not need to make any adjustments to the problem we wanted to solve.**

### Design and Technical Approach
***Technical Design***
 By implementing tactical path planning using influence maps to find the safest shortest path, this project will involve delving into the area of search, particularly with the manipulation of path planning algorithms and their cost evaluation utility functions. Furthermore, as influence maps provide a way to represent dynamic unit movement and location to an agent, the agent will be able to utilize the data generated from the influence maps to make meaningful responses to the environment. When abstracted as a Unity extension method, this information will be represented as a 2D grid for simplified node traversal.

The generation of influence maps will allow us to produce a more adaptive AI. These influence maps will cover a variety of characteristics (see figure 1) such as unit range, unit health, and attack type (aerial attack, ground attack, or both aerial and ground attack). Splitting up characteristics like this will allow for agents to make responsive tactical movements in a changing environment.

<p align="center">
  <img src="https://lh6.googleusercontent.com/cbMD0r8uLCr6q-RzBZHTBOA5xIfo7rK8IbS5YIj1wp25r_yHw0LZxTw-rIMmab7qfv3uFR64Rok7j_1VD957AoaGzQi9U2kMNjgXaVsBLkOyJu941GIi85a8piHHD_qP8oP9vTmt"><br/>
Figure 1 - Individual unit influence dependent on attack type, health of unit, and range of unit
</p>

An influence map will have a variation of values in the following three features of influence: momentum of spread of influence, decay of influence over time and space, and influence map update frequency. As can be seen in figure 2, the center node represents a tile occupied by a unit with a decay of influence radiating out from the center node.

<p align="center">
  <img src="https://lh4.googleusercontent.com/54pQa7-Zm7-3IVMIE8fBCdKHc4GF0WkIZqawj-P50OpXe39F0M7k8LJS4uuMWbLtn1OK4s_XlAcj5Ow4Qa7tpR47L2zw5L5lExxlGiHQRqJ6RUa9J3aeLzHVLj-xc9ovf--xKAro"><br/>
Figure 2 - Very basic influence map decay (Adapted from Game AI Pro 2, chapter 30: Modular Tactical Influence Maps)
</p>
Lastly, to model the agent more clearly, we will also be creating an environment within Unity. To do this, we will need to implement our own pathfinding algorithm so that each agent in the Unity environment can make use of the influence map-based navigation. For this, we will implement A*, using the utilities developed for StarCraft II.

Path planning algorithms don’t implicitly use influence maps to update the cost of a node based on the influence expressed by the other agents in the map. As can be seen with the commonly implemented algorithm A* (and its variations), the cost of a node transition typically relies on heuristic values varying from node-to-node in order to find the shortest path. The solution presented will use influence maps to dynamically change the cost of a node given the influence exhibited in that area by enemy units.

***Technology Stack***
The technology stack to be used will consist of using GitHub for communal development and version control--with GitHub pages being used for creating the project’s deliverable website. Google documents will be used for general documentation and Slack will be used for inter-team discourse. The StarCraft II artificial intelligence API and Deepmind’s StarCraft II Learning Environment (PySC2) will be used to interact and manipulate StarCraft II’s path planning. All code will adhere to the C# coding conventions as outlined by Microsoft and will be developed using one of the following IDE’s: Sublime, Visual Studio Community, or a version of Atom. Further porting of the project will make use of Unity and the Unity extension methods. This technology stack is deemed appropriate as the final goal of this project is to create something to be used in Unity. Using C# as the main language for this project allows for an easier creation of a Unity extension.

GitHub branches will be used whenever we can modularize a task and will be merged to master once thoroughly tested. Once merged we will do further testing to ensure compatibility with existing codebase. Furthermore, the transparency of commits and version consistency allows for better development.

*[As with the previous section, tell the narrative of the changes to the design and technical approaches to your project since the original proposal. The changes in this section should be motivated by either changes to the previous section or by pragmatic or technical limitations in your design or technical approach.]*

**Given the fact the team was more familiar and comfortable working with C++ (rather than Python), we eventually decided to rule out PySC2. Our original choice of using C# and the .NET core was to easily integrate our solution in Unity (the game engine). However, due to a lack of documentation with the .NET core, we opted for the well documented StarCraft 2 API that used C++. Furthermore, this decision made us decide to use the Unreal Engine instead of Unity. Lastly, we ended up scoping down even further when we simply did not have enough time to integrate our work into an Unreal project, resulting in that portion being cut. At the point of project completion, it would not be difficult to take the extra steps later to integrate into an Unreal project in the future, we just simply ran out of time.**

### Scope and Timeline
Provide a justification for the feasibility and scope of your project. Provide a timeline covering the major deliverables and technical problems.

*Phase 1*

4/29 - 5/5: Familiarization with StarCraft II API and setting up the development environment

-   Identifying how navmeshes are used in SCII
-   Identifying how units move on SCII maps
-   Swarm AI
-   How can we influence this navigation using an influence map
-   SCII Development Environment Setup


*Phase 2*

5/6 - 5/12: Basic implementation

-   Basic Influence Map Implementation
-   Basic Path Planning Utility Function Manipulation
-   SCII Visualization of Influence Maps

*Phase 3*

5/13 - 5/19: Generalization and advancement of implementation

-   Advanced Influence Map Implementation
-   Advanced Path Planning Utility Function Manipulation

*Phase 4*

5/20 - 5/26: Further advancements and initial abstraction to Unity

-   Unity Environment Setup
-   Unity A* Implementation

*Phase 5*

5/27 - 6/9: Abstraction to Unity and deliverable development

-   Unity Extension Method Implementation
-   Unity Influence Map Representation
-   Deliverable Reports

*[Detail justifications for any changes in your scope and timeline. Be sure to reference the appropriate material from the previous two sections that impacted the scope of your project.]*

**One of the biggest challenges we faced when completing this project was sticking to our project timeline. For instance, when we thought that familiarizing ourselves with the Starcraft II API would only take a week of our time, this process actually bled into our third week, as we were forced to switch from the C# API, to PySC2, before finally settling on the C++ StarCraft 2 API. This process ended up halting much of the work we had planned to be completed, resulting in us removing the aspects of influence map visualization and application to a game engine from our project.**

### Documentation and Access
This project will rely on GitHub for code sharing, taking advantage of the repository's README for additional documentation. Furthermore, the project and the models created will be displayed on an associated website.

Repository:  [https://github.com/SydneySchiller/ECS170-Project](https://github.com/SydneySchiller/ECS170-Project)

Website:  [https://sydneyschiller.github.io/ECS170-Project/](https://sydneyschiller.github.io/ECS170-Project/)

### Evaluation
A successful project will be able to demonstrate tactical responsiveness in StarCraft II unit’s path planning--where units respond dynamically to enemy presence on a map. Additionally, successfully porting the project to Unity as an extension--to be used in future games and projects is a mark of success. A successful agent will be able to find the safest shortest path around enemies, which can be compared to the baseline as provided by the base StarCraft II agent.

*[Unless your project change drastically in scope, design, or technical approach, this section should be the bulk of your final report. Provide data and analysis to contextualize your contribution. Additionally, describe any deviations from your original evaluation plan.]*

**As stated in previous sections, we were unable to complete the abstraction to a game engine due to time constraints and early difficulties. However, we are currently able to run our bot in Starcraft, which demonstrates our implementation of influence maps and path planning based on the influence in the area. With the implementation of our influence maps, units no longer enter into areas where enemy influence is greater than the alliance influence, resulting in unneeded combat and loss of units.**

### Plan for Deliverables
The GitHub repository for this project will remain public for the duration of the Spring 2018 quarter. Pending group discussion, the GitHub repository may be made private. Should this project be deemed usable and feasible for further modeling beyond the avoid/fight dynamic inherent to StarCraft II, subsequent project member’s personal games may feature the use of the project. Additionally, pending the success of the project, a paper may be written and submitted to conferences.

Repository:  [https://github.com/SydneySchiller/ECS170-Project](https://github.com/SydneySchiller/ECS170-Project)

Website:  [https://sydneyschiller.github.io/ECS170-Project/](https://sydneyschiller.github.io/ECS170-Project/)

Project Report:  [https://goo.gl/LudjhE](https://goo.gl/LudjhE)

### Separation of Tasks for Team
\[] SCII Development Environment Setup:  Mark<br/>
\[] Basic Influence Map Implementation:  Mark, Sydney, Ben<br/>
\[] Advanced Influence Map Implementation:  Ben, Jonny<br/>
\[] SCII Visualization of Influence Maps:  Ben, Jonny<br/>
\[] Path Planning Utility Function Manipulation:  Mark, Sydney, Bayan<br>
\[] Inline Documentation:  Mark, Sydney, Bayan, Ben, Jonny<br/>
\[] Unity Environment Setup:  Sydney, Bayan<br/>
\[] Unity A* Implementation:  Bayan, Jonny<br/>
\[] Unity Extension Method Implementation:  Bayan, Jonny<br/>
\[] Unity Influence Map Representation:  Mark, Sydney<br/>
\[] Unity Documentation:  Mark, Sydney, Bayan, Ben, Jonny<br/>
\[] Deliverable Reports:  Mark, Sydney, Bayan<br/>

*[Describe how the final delineation of tasks varied from the original separation.]*

**Revised Separation of Tasks:**

SCII Development Environment Setup:  Mark, Ben<br/>
Basic Influence Map Implementation:  Mark, Sydney<br/>
Advanced Influence Map Implementation:  Mark, Sydney<br/>
SCII Visualization of Influence Maps:  CUT<br/>
Path Planning Utility Function Manipulation:  Ben, Jonny<br/>
Inline Documentation:  Mark, Sydney, Ben, Jonny<br/>
Unity Environment Setup:  CUT<br/>
A* Implementation:  Jonny<br/>
Unity Extension Method Implementation:  CUT<br/>
Unity Influence Map Representation:  CUT<br/>
Unity Documentation:  CUT<br/>
Deliverable Reports:  Sydney, Bayan<br/>

**Notes: Two major parts of our project included the extension to Unity/Unreal and visualization of our influence map working. As weeks progressed we separated tasks as follows:**

Influence Map Implementation:  Mark, Sydney<br/>
Path planning integration with Influence Maps:  Ben<br/>
A* Implementation:  Jonny<br/>
Visualization of Influence Maps:  Bayan<br/>
Unreal/Unity Implementation:  Jonny, Bayan<br/>

**However, as we faced setbacks with choosing an environment to work in, and visualization proved to be more difficult that we realized it would be, some team members ended up working weeks on a single aspect of the project that was not utilized in the final project build.**
