# Client-Server Book Processing Program

- MAKE IT MULTI-THREADED, CREATE A NEW THREAD PER REQUEST AND CLOSE THAT THREAD WHEN REQUEST IS DONE - SOON COME
- Server that takes requests from Client - 
  - All in an infinite while (true) loop
  - Start listening to the connections
  - When connection is accepted, thread go brrrr (use threads or 'select()' function)
  - Then keep track separately where the start of the book is (pointer in linked list where 1st part of that book appears)
  - Implement NON-BLOCKING reads from each socket from each client
  - __Ensuring the timing of each thread's execution is correct so the nextbook pointer actually points to the next part the book__
  - When a client closes connection to the server, the server then loops through the list to read and write the whole book from that client to a text file, and closes the thread

## Part 2

- Make 2 or more analysis threads to multi-program search through the linked list for a search term
- Keep track of how many times search term appears per book
- Live update to stdout (the screen) every 2-5 seconds the counts of found search term for each book
