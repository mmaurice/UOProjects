# DuckInterest PubSub

The goal of Part 1 is to develop the first version of the DuckInterest (DI) Server that makes it possible for
a publisher or subscriber to connect to the service. The idea is that a publisher or subscriber first needs
to contact the DI server to register themselves and indicate what topics they are interested in. This will
be done through interprocess communication (IPC) using pipes. To keep things simple for testing, Part
1 implements the following:
• Write a program that forks a DI server process, n publisher processes (Pi
, 1 ≤ i ≤ n), and m
subscriber processes (Sj , 1 ≤ j ≤ n).
• The DI server creates a pipe with each publisher and each subscriber process. It then listens for
information on each pipe.
• For each publisher, Pi
, send the following to the DI server:
“pub connect”
“pub topic k” (1 ≤ k ≤ t, for each topic of interest)
“end”
After each string, wait for a response from the DI server before sending the next string. If the
response is “accept” then the next string can be sent. If the response is “reject” then something
went wrong and the publisher should try again or terminate. A separate “pub topic k” command
string is required for each topic of interest.
• For each subscriber, Si
, send the following to the DI server:
“sub connect”
“sub topic k” (1 ≤ k ≤ n, for each topic of interest)
“end”
After each string, wait for a response from the DI server before sending the next string. If the
response is “accept” then the next string can be sent. If the response is “reject” then something
went wrong and the publisher should try again or terminate. A separate “pub topic k” command
string is required for each topic of interest.
• As discussed above, the DI server talks to the publisher and subscriber processes over the pipes
and responds to their commands with “accept” or “reject” accordingly. If the connection protocol
is done successfully, the DI server will create a connection record indicating whether the connection
is a publisher or subscriber, what specific publisher or subscribed it is, what topics were selected,
and the pipe associated with the connected process.
• After all of the connections have been made, the DI server prints out the records and begins the
termination protocol. Here, it waits for each publisher and subscriber to send the command:
“terminate”
The DI server responds with “terminate” and then closes the pipe. It prints out a message indicating
that which publisher or subscrber was terminated.
2
Part 2 – DuckInterest Server Multithreading
Part 1 implements a basic single-threaded DI server for establishing publisher and subscriber connections.
If you continued to have only a single thread, it would have to service all of these connections by itself, in
addition to the rest of the DI server pub/sub functions. Part 2 improves on the Part 1 implementation by
creating a thread for each connected publisher and subscriber that will operate as a server-side “proxy”
for their actions.
Part 2 does the following right after a publisher or subscriber is connected:
• The DI server process creates a thread and provides the thread with the connection record. If all
of the publishers and subscribers are successful in connecting, there should be n publisher threads,
T
p
i
, 1 ≤ i ≤ n, and there should be m subscriber threads, T
s
j
, 1 ≤ j ≤ m.
• For each T
p
i
thread, wait for the associated Pi publisher to send a “terminate” command and then
respond with “terminate” and closes the pipe, printing out a message to that affect.
Pthreads will be used to implement the server-side threading. Be careful to make the code you develop
thread safe.
