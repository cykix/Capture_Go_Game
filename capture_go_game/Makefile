# Compiler
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pthread

# Executables
SERVER_EXEC = server
CLIENT_EXEC = client

# Source Files
SERVER_SRC = server.cpp game.cpp protocol.cpp
CLIENT_SRC = client.cpp game.cpp protocol.cpp  # ✅ Include game.cpp for the client as well

# Object Files
SERVER_OBJ = $(SERVER_SRC:.cpp=.o)
CLIENT_OBJ = $(CLIENT_SRC:.cpp=.o)

# Build the server executable
$(SERVER_EXEC): $(SERVER_OBJ)
	$(CXX) $(CXXFLAGS) -o $(SERVER_EXEC) $(SERVER_OBJ)

# Build the client executable
$(CLIENT_EXEC): $(CLIENT_OBJ)
	$(CXX) $(CXXFLAGS) -o $(CLIENT_EXEC) $(CLIENT_OBJ)

# Compile .cpp files into .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -f $(SERVER_EXEC) $(CLIENT_EXEC) $(SERVER_OBJ) $(CLIENT_OBJ)

# Phony targets
.PHONY: clean
