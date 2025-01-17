# BouncingMarble

Compile the program by specifiying the curses library. For example: `gcc marblec.c -o marble -lcurses`. To run the program type `./marble` followed by the number of desired rows, the number of desired columns, a number between -2 and 2, inclusive (the speed in the x direction), and a number between -2 and 2, inclusive (the speed in the y direction). For example, `./marble 4 4 -1 2` would split the terminal into boxes with 4 rows and 4 columns. The cursor would be sent slowly to the left and quickly downwards (y increases as the cursor moves down in the terminal). As the cursor bounces around the terminal, you will be able to see the number of times it has entered each box. Press *Ctrl-C* to end the program.

### Update (7/11/2022)

I realize that making sure the 'curses' library is installed properly can be quite the hassle, especially on a Windows machine. Therefore I have made changes that should make it so that this program can run on any machine. HOWEVER, YOU MUST HAVE DOCKER INSTALLED ON YOUR SYSTEM. If you meet this requirement, clone the repo as normal. Make sure Docker is running. Then, go to the root of the directory in your command line and enter `docker build -t bouncingmarble .`. This will create the Docker image. Then run `docker run bouncingmarble` and the program should run as intended. 

If you would like to change the speed/number of boxes in the terminal go to `Dockerfile` in the root of the directory and change the last line to your specifications. For example, `CMD ["./marble", "4", "4", "-1", "-2"]` would split the terminal into boxes with 4 rows and 4 columns. The cursor would be sent slowly to the left and quickly downwards (y increases as the cursor moves down in the terminal). This will require you to rebuild the image and run the container again.

It is also possible to change the speed/number of boxes without completely rebuilding the image and running the container again. This can be done by running new commands within the container. To do so run a command similar to the following (while the Docker container is running): <code>docker exec -it *container_id* /bin/sh</code> (the container id of the running container can be found by running `docker ps`). This will allow you to enter new commands within the running container. Then you can run a command such as `./marble 4 4 -1 -2`. This will run the program with the same effects as described in the paragraph above.

I am trying to get practice using Docker, and I remembered how annoying it was to run this program on Windows. Therefore, this was a great way to see if I could get the program to run on my Windows machine using Docker.

