# Triplet Tester
## A tool for quantatatively testing the solvability of systems of equilateral triangles from randomly chosen and distributed points
Essentially stolen from <a href="https://community.wolfram.com/groups/-/m/t/615401">this Wolfram Community post</a> <br> <br>
Consider a plane with n&ge;3 points. At the start, each point p<sub>i</sub> chooses two other points, p<sub>j</sub> ,p<sub>j</sub>, where i&ne;k,i&ne;j,j&ne;k. The points are placed randomly on this plane.
Then, any number of timesteps are ran, where all of the points, simultaneously, take a step towards their individual "goal points". A point's goal is defined as a point where the distance between the point and each of the two points it observes is the same as the distance between those two points.
<br>
<br>
This means that, if the system is solvable and with a bit of luck for a given set of starting conditions, the points will align themselves in a number of equilateral triangles. The system is considered solved if all points are at one of their two goal points and no two points are at the same location.
The purpose of this program is to evaluate to solvability of systems using various stepping algorithms, as well as evaulating the relative effectiveness of the algorithms themselves.
