  @brief Protocol to communicate information about movies and seat availability over TCP.
  
  This protocol defines the format of messages exchanged between a client and a server to request and edit information about movies and available seats in a cinema. The messages are sent over a TCP connection.
  
  The protocol consists of two types of messages: requests and responses. A request message is sent by
  the client to request information about a specific movie, to reserve a seat or edit data inside the server . A response message is
  sent by the server to provide the requested information or to confirm the request is done.
  
  Message format:
  
  Client message:
  
  | 1 byte  |2 bytes     | variable |
  |---------|------------|----------|
  |  Type   |Payload size|Payload   |
  
  - Type: 1-byte field indicating the type of message (request or response).
  - Code: 1-byte field indicating the specific request or response code.
  - Payload size : 2bytes unsigned int containing number of byte contained inside the variable
  - Payload: variable-length field containing the request data.
  
  Server message:
  
  | 1 byte   | 1 byte |2 bytes     | variable |
  |----------|--------|------------|----------|
  |  Type    | Status |Payload size|Payload   |
  
  - Type: 1-byte field indicating the type of message (request or response).
  - Code: 1-byte field indicating the specific request or response code.
  - Status: 1-byte field indicating the status of the request (success or failure).
  - Payload size : 2bytes unsigned int containing number of byte contained inside the variable
  - Payload: variable-length field containing the response data.
  
  Request codes:
  
  - 0x01: Get movie list
  - 0x02: Get available shows for a Movie
  - 0x03: Reserve x seats for a movie
  - 0x04: Add a movie
  - 0x05: Add a show
  - 0x06: Login
  - 0x07: Logout
  
  Response codes:

  - 0x81: Movie list
  - 0x02: Get available shows for a Movie
  - 0x83: Reservation confirmation
  - 0x84: Authentification | No Payload
  - 0x85: Add a movie | No Payload
  - 0x86: Add a movie max number of seat | No Payload
  - 0x87: General Error
  - 0x88: Auth login received
  
  Status codes:
  
  - 0x00: Failure
  - 0x01: Success 
  
  Payload Parsing:
  The payload is a serie of string seperated by a "," separator
