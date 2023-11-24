1) have nice file structure (DONE)

2) actually write some code (need our own header files and binaries and other cool stuff)

3) cool GUI with terminator reference ofc

4) slap on docker and cmake and whatnot (whatever that means)

5) as a bonus, add protocols for encryption, server discovery, DMs, and file transfer within a server


PLANNED FUNCTIONALITES:
server:
name server, give optional password protection
can run server on a network, should be visible to all clients
run stateless database with all messages (hopefully encrypted) and constantly update client based on relevant entries
spawn new thread for every connected client

client:
set nickname
join available server
send message to database
view messages (DM and serverwide)

client ui needs 3 windows:
servers/open DMS on side
one window for writing message
one window fo viewing current chat
