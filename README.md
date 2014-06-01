#			     C-Store Database System


##Table of Contents		
===================
* Compile&link
* Load data to system
* Retrieve data in system
* Compress data
* Join
* Count
* TODO

##Compile&Link
============
To get the executable of C-Store Database System application, you just need to startup terminal and CD to this folder and run 'make' command. This will compile the source files in /src and link the object to leave the executable to /bin. 
  

##Load data to system
===================
In this version, you should copy the data file(orders.tbl, e.g.) to this folder and startup a terminal and CD to /bin and run "./db load <datafile>"( ./db load orders, e.g. ) command. This will load the data into the system.


##Retrieve data in system
=======================
After you have load data to the system, you can query the tuples in the system with corresponding primary keys. Run "./db retrieve <tablename>" will start the retrieve mode of the system. And you can insert several primary keys to get the tuple. For example, you can run follow command in terminal to retrieve orders table with order_key:
~$ ./db retrieve orders
1
2
3
105892
^+D(EOF)

##Compress data
=============
After you have load data to the system, you can compress it. Run "./db compress " will start extern sort the data in the system and compress them, it may takes a few time to sort and compress a large amount of data. When it has done, compress rate will be return and log to screen.

##Join
=====
After you have compressed orders table and load customer, you can join two tables. Run “./db join” will join orders table and customer table and print the custkey and orderkey in the terminal.

##Count
======
After you have compressed orders table, you can count the lines number of the orders table quickly. Run “./db count” will count the lines and log to the screen.

TODO
====
   * Clear the switch code in the load data operation to get a general method to load 
     other tables to the system.

   * Implement an algorithm to sort general page with comparable entries
   
   * Join general two tables in the system

 Copyright (c) 2014 - 2014. All rights reserved

